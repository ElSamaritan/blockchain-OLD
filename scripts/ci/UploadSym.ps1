# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018 Galaxia Project Developers                                                      #
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

param(
    [Parameter(Mandatory=$true)][string]$SymFile
)

$Uri = "http://$($env:BREAKPAD_HOST)/symfiles"
$Encoding = New-Object System.Text.UTF8Encoding $False
$Boundary = [System.Guid]::NewGuid().ToString()
$LF = "`r`n"

$Content = [IO.File]::ReadAllText($SymFile)
[IO.File]::WriteAllText($SymFile, $Content, $Encoding)
$Content = [IO.File]::ReadAllText($SymFile)

$BodyLines = (
    "--$Boundary",
    "Content-Disposition: form-data; name=`"symfile`"; filename=`"$SymFile`"",
    "Content-Type: application/octet-stream$LF",
    $Content,
    "--$Boundary--$LF"
) -join $LF

Invoke-RestMethod -Uri $Uri -Method Post -ContentType "multipart/form-data; boundary=`"$Boundary`"" -Body $BodyLines
