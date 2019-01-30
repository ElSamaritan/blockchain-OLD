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

#Requires -Modules WriteLog

function Get-Configuration
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true)][string]$Name,
        [Parameter(Mandatory=$false)]$DefaultValue = $null,
        [Parameter(Mandatory=$false)]$ProvidedValue = $null,
        [Parameter(Mandatory=$false)][switch]$Required = $false
    )
    
    if(!$ProvidedValue)
    {
        if(Test-Path env:$Name)
        {
            $EnvironmentValue = $(Get-Item env:$Name).Value
            Write-Log "$Name was provided as environment variable: $EnvironmentValue"
            return $EnvironmentValue
        }
        elseif($DefaultValue) 
        {
            Write-Log "$Name was not provided applying default value: $DefaultValue"
            return $DefaultValue
        }
        else
        {
            if($Required)
            {
                throw ("Get-Configuration: Could not determine required configuration '$Name'")
            }
            else
            {
                Write-Log "$Name was not provided but is not required, returning null."
            }
        }
    }
    else 
    {
        Write-Log "$Name was provided as parameter: $ProvidedValue"
        return $ProvidedValue
    }
}