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
    [Parameter(Mandatory=$false)][string]$SourcePath = $null,
    [Parameter(Mandatory=$false)][string]$BuildPath = $null,
    [Parameter(Mandatory=$false)][string]$InstallPrefix = $null
)

Import-Module -Name "$PSScriptRoot\modules\WriteLog.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\InvokeCommand.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetResolvePath.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetConfiguration.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetGenerator.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetBuildEnvironment.psm1" -Force

try 
{
    $BuildEnvironment = Get-BuildEnvironment

    $CMakeSourcePath = Get-Configuration CMAKE_SOURCE_PATH -DefaultValue $(Get-Location) -Required
    $CMakeBuildPath = Get-Configuration CMAKE_BUILD_PATH -DefaultValue .build -ProvidedValue $BuildPath -Required
    $CMakeInstallPrefix = Get-Configuration CMAKE_INSTALL_PATH -DefaultValue ".install" -ProvidedValue $InstallPrefix
    $CMakeInstallPath = "$CMakeInstallPrefix\bin"
    $CMakeSymbolsPath = Get-Configuration CMAKE_SYMBOLS_PATH -DefaultValue "$CMakeInstallPrefix\symbols"

    $CMakeSourcePath = Get-Resolve-Path $CMakeSourcePath
    $CMakeBuildPath = Get-Resolve-Path $CMakeBuildPath
    $CMakeInstallPath = Get-Resolve-Path $CMakeInstallPath
    $CMakeSymbolsPath = Get-Resolve-Path $CMakeSymbolsPath

    if($IsWindows)
    {
        Push-Location "$CMakeBuildPath\src\Release"
    }
    else 
    {
        Push-Location "$CMakeInstallPath\bin"
    }

    if(-Not (Test-Path $CMakeSymbolsPath))
    {
        New-Item -ItemType Directory -Path $CMakeSymbolsPath | Out-Null
    }

    $FilesToDump = @(
        "xi-daemon",
        "xi-wallet",
        "xi-miner",
        "xi-pgservice"
    )
    
    try 
    {
        foreach ($FileToDump in $FilesToDump) 
        {
            Write-Log "Dumping symbols of $FileToDump..."
            if($IsWindows)
            {
                Invoke-Command { 
                    $(& "$CMakeSourcePath\external\google-breakpad\src\tools\windows\binaries\dump_syms.exe" `
                        @("$FileToDump.pdb")) > "$CMakeSymbolsPath\$FileToDump.sym"
                }
            }
            else
            {
                Invoke-Command { $(dump_syms "./$FileToDump") > "$CMakeSymbolsPath\$FileToDump.sym" }
                Invoke-Command { strip "./$FileToDump" }
            }
        }
    }
    finally 
    {
        Pop-Location
    }
}
catch 
{
    $Host.SetShouldExit(-1)
    throw
}