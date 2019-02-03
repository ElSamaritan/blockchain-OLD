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

$PackagesPath = $(Get-Resolve-Path ".packages")
$PackagesInfoPath = "$PackagesPath\packages.json"

if((Test-Path -Path $PackagesInfoPath))
{
    Write-Log "Deleteing previous packages info..."
}

Write-Log "Downloading current packages info..."
Invoke-WebRequest -Uri "https://releases.xiproject.io/packages.json" -OutFile $PackagesInfoPath
$PackageInfo = Get-Content $PackagesInfoPath | Out-String | ConvertFrom-Json

$NewPackages = (Get-ChildItem -Recurse -Path "$PackagesPath\$($PackageInfo.VersionPath)\*" -Filter info.json)
foreach ($NewPackage in $NewPackages) 
{
    Write-Log "Adding package $NewPackage"
    $NewPackageInfo = Get-Content $NewPackage | Out-String | ConvertFrom-Json
    $PackageInfo += $NewPackageInfo.ArchivePath -replace "\\", "/"
    $PackageInfo += $NewPackageInfo.LatestPath -replace "\\", "/"
}
$PackageInfo = $PackageInfo | Select-Object -Unique

Write-Log "Updating packages info..."
ConvertTo-Json $PackageInfo | Set-Content -Path $PackagesInfoPath

$FtpHost = $env:FTP_HOST
$FtpUri = "ftp://$FtpHost"
$FtpUser = $env:FTP_USERNAME
$FtpPassword = $env:FTP_PASSWORD

$WebClient = New-Object System.Net.WebClient 
$WebClient.Credentials = New-Object System.Net.NetworkCredential($FtpUser, $FtpPassword)  
 
foreach($Upload in Get-ChildItem -Path $PackagesPath -Recurse)
{ 
    $RelativeUpload = [System.IO.Path]::GetFullPath($Upload.FullName).SubString([System.IO.Path]::GetFullPath($PackagesPath).Length + 1)
    if($Upload.Attributes -eq "Directory")
    {
        try 
        {
            Write-Log "Creating remote directory $RelativeUpload"
            [System.Net.FtpWebRequest]$CreateArchiveRequest = [System.Net.FtpWebRequest]::Create("$FtpUri/$RelativeUpload")
            $CreateArchiveRequest.Method = [System.Net.WebRequestMethods+Ftp]::MakeDirectory
            $CreateArchiveRequest.Credentials = New-Object System.Net.NetworkCredential($FtpUser, $FtpPassword)
            $CreateArchiveRequest.GetResponse() | Out-Null
        }
        catch [System.Net.WebException]
        {
            Write-Log "Error creating remote directory, may already exists."
        }
    }
    else
    {
        Write-Log "Uploading file $RelativeUpload"
        $UploadUri = New-Object System.Uri("$FtpUri/$RelativeUpload")
        $WebClient.UploadFile($UploadUri, $Upload.FullName)
    }
} 