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

param(
    [Parameter(Mandatory=$false)][string]$InstallPrefix = $null
)

Import-Module -Name "$PSScriptRoot\modules\WriteLog.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetResolvePath.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetConfiguration.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetBuildEnvironment.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetPackageHashCollection.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetPackageInfo.psm1" -Force

$CMakeInstallPrefix = Get-Configuration CMAKE_INSTALL_PATH -DefaultValue ".install" -ProvidedValue $InstallPrefix
$CMakeInstallBinaries = Get-Resolve-Path "$CMakeInstallPrefix\bin"
$CMakeInstallSymbols = Get-Resolve-Path "$CMakeInstallPrefix\symbols"

$PackageInfo = Get-PackageInfo

$VersionInstallBinaries = "$CMakeInstallPrefix\Xi\v$($PackageInfo.Environment.Version)"
$VersionInstallSymbols  = "$CMakeInstallPrefix\symbols\v$($PackageInfo.Environment.Version)"
if(-Not ($PackageInfo.Environment.Channel -Like "stable"))
{
    Write-Log "Non stable channel create postfix for version directory"
    $VersionInstallBinaries = "$VersionInstallBinaries-$($PackageInfo.Environment.Channel)"
    $VersionInstallSymbols = "$VersionInstallSymbols-$($PackageInfo.Environment.Channel)"   
}

Write-Log "Moving install to version named directory..."
New-Item $VersionInstallBinaries -ItemType Directory -Force -ErrorAction SilentlyContinue | Out-Null
Move-Item -Path "$CMakeInstallBinaries\*" -Destination $VersionInstallBinaries -ErrorAction SilentlyContinue
Remove-Item -Path $CMakeInstallBinaries -Recurse -Force -ErrorAction SilentlyContinue | Out-Null

Write-Log "Building package..."

$CMakePackagesOut = Get-Resolve-Path ".packages"
$ArchivePath = "$CMakePackagesOut\$($PackageInfo.ArchivePath)"
$LatestPath = "$CMakePackagesOut\$($PackageInfo.LatestPath)"

Remove-Item -Recurse -Force -Path $ArchivePath -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path $ArchivePath | Out-Null

Remove-Item -Recurse -Force -Path $LatestPath -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path $LatestPath | Out-Null

$CMakeBinariesPath = $(Get-Resolve-Path "$CMakeInstallPrefix\Xi")
$CMakeSymbolsPath = $(Get-Resolve-Path "$CMakeInstallPrefix\symbols")

Write-Log "Packaging binaries..."
Compress-Archive -Path $CMakeBinariesPath -DestinationPath "$ArchivePath\binaries.zip"

Write-Log "Packaging symbols..."
Compress-Archive -Path $CMakeSymbolsPath -DestinationPath "$ArchivePath\symbols.zip"

Write-Log "Writing package info..."
$PackageInfo.Hashes = Get-PackageHashCollection -Path "$ArchivePath\binaries.zip"
ConvertTo-Json $PackageInfo | Set-Content -Path "$ArchivePath\info.json"

Write-Log "Copying latest package..."
Copy-Item -Path $ArchivePath/* -Destination $LatestPath -Recurse

Write-Log "Finished, package info:`n$(ConvertTo-Json $PackageInfo)"