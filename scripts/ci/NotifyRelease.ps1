# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018-2019 Galaxia Project Developers                                                 #
#                                                                                                #
# This program is free software: you can redistribute it and/or modify it under the terms of the #
# GNU General Public License as published by the Free Software Foundation, either version 3 of   #
# the License, or (at your option) any later version.                                            #
#                                                                                                #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;      #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      #
# See the GNU General Public License for more details.                                           #
#                                                                                                #
# You should have received a copy of the GNU General Public License along with this program.     #
# If not, see <https://www.gnu.org/licenses/>.                                                   #
#                                                                                                #
# ============================================================================================== #

Import-Module -Name "$PSScriptRoot\modules\WriteLog.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\InvokeCommand.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetResolvePath.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetBuildEnvironment.psm1" -Force

$BuildEnvironment = Get-BuildEnvironment

if((-not ($BuildEnvironment.Channel -like "stable")) -and (-not ($BuildEnvironment.Channel -like "beta")))
{
    Write-Log "Notifications are only sent on stable and beta releases, skipping..."
    return
}
else 
{
    Write-Log "Sending discord notification..."
}

$Icon = "https://releases.galaxia-project.com/_404/images/ci-$($BuildEnvironment.Channel).png"
$Url = "https://releases.galaxia-project.com/$($BuildEnvironment.Channel)/v$($BuildEnvironment.Version)"
$Timestamp = "$(Get-Date -format s)Z"
$TextInfo = (Get-Culture).TextInfo

Invoke-WebRequest -Uri "https://releases.galaxia-project.com/packages.json" -OutFile packages.json
$Packages = Get-Content packages.json | Out-String | ConvertFrom-Json
$Packages = $Packages | Where-Object { $_ -like "$($BuildEnvironment.Channel)/latest/*" }
$Packages = $Packages -replace "$($BuildEnvironment.Channel)/latest/",""
$Packages = $Packages | ForEach-Object { "       {
            ""name"": ""$_"",
            ""value"": ""[``Info``]($Url/$($_ -replace " ","%20")/info.json) [``Binaries``]($Url/$($_ -replace " ","%20")/binaries.zip) [``Symbols``]($Url/$($_ -replace " ","%20")/symbols.zip)"",
            ""inline"": false
        }
" }

Remove-Item packages.json

$Payload="{
  ""username"": """",
  ""avatar_url"": ""$Icon"",
  ""embeds"": [ {
    ""color"": 4388052,
    ""author"": {
      ""name"": ""Galaxia - Releases"",
      ""url"": ""https://releases.galaxia-project.com""
    },
    ""title"": ""New $($TextInfo.ToTitleCase($BuildEnvironment.Channel)) Release v$($BuildEnvironment.Version)"",
    ""url"": ""$Url"",
    ""description"": """",
    ""fields"": [
        $([String]::Join(",", $Packages))
    ],
    ""timestamp"": ""$Timestamp""
  } ]
}"

Invoke-RestMethod -Uri "$($env:DISCORD_WEBHOOK)" -Method "POST" -UserAgent "GitlabCI-Webhook" `
  -ContentType "application/json" -Header @{"X-Author"="gitlab-ci"} `
  -Body $Payload

Write-Log "Notification sent."
