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
    [Parameter(Mandatory=$false)][string]$BuildPath = $null
)

Import-Module -Name "$PSScriptRoot\modules\WriteLog.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\InvokeCommand.psm1" -Force
Import-Module -Name "$PSScriptRoot\modules\GetConfiguration.psm1" -Force

try
{
    $CMakeBuildPath = Get-Configuration CMAKE_BUILD_PATH -DefaultValue .build -ProvidedValue $BuildPath -Required
    Push-Location $CMakeBuildPath
    try
    {
        Invoke-Command { ctest -VV }
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
