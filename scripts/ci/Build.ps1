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
    [Parameter(Mandatory=$false)][string]$InstallPrefix = $null,
    [Parameter(Mandatory=$false)][string]$Generator = $null
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
    $CMakeGenerator = Get-Configuration CMAKE_GENERATOR -DefaultValue $(Get-Generator)
    $CMakeBuildType = Get-Configuration CMAKE_BUILD_TYPE -DefaultValue Release

    $CMakeSourcePath = Get-Resolve-Path $CMakeSourcePath
    $CMakeBuildPath = Get-Resolve-Path $CMakeBuildPath
    $CMakeInstallPath = Get-Resolve-Path $CMakeInstallPath

    $CMakeCacheFile = "$CMakeBuildPath\CMakeCache.txt"
    if(Test-Path -Path $CMakeCacheFile)
    {
        Write-Log "Cached build directory detected. Clearing cmake caches..."
        Get-ChildItem -Path $CMakeBuildPath -Recurse -Filter CMakeCache.txt | Remove-Item
    }

    if(!(Test-Path -Path $CMakeBuildPath))
    {
        New-Item -ItemType Directory -Path $CMakeBuildPath | Out-Null
    }

    Write-Log @"
Build Configuration
`t Source Path         : $CMakeSourcePath   
`t Build Path          : $CMakeBuildPath
`t Install Path        : $CMakeInstallPath
`t Generator           : $CMakeGenerator
`t Build Type          : $CMakeBuildType

Build Environment
$(ConvertTo-Json $BuildEnvironment)
"@

    if(-Not (Test-Path $CMakeInstallPath))
    {
        New-Item -ItemType Directory -Path $CMakeInstallPath | Out-Null
    }

    Push-Location $CMakeBuildPath
    try 
    {
        Invoke-Command { 
            cmake `
                -G $CMakeGenerator `
                -DCMAKE_BUILD_TYPE=$CMakeBuildType `
                -DCMAKE_INSTALL_PREFIX:PATH=$CMakeInstallPath `
                -DXI_BUILD_CHANNEL="$($BuildEnvironment.Channel)" `
                -DXI_BUILD_BREAKPAD=ON `
                -DXI_BUILD_EXECUTABLES=ON `
                -DXI_BUILD_TESTSUITE=ON `
                -DXI_BUILD_TOOLS=ON `
                $CMakeSourcePath 
        }
        Invoke-Command {
            cmake --build . --target install --config $CMakeBuildType
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