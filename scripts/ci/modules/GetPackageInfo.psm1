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

#Requires -Modules WriteLog, GetConfiguration, GetPackageHashCollection, GetBuildEnvironment

class PackageInfo {
    [object]$Environment
    [string]$TargetOS
    [string]$VersionPath
    [string]$ArchivePath
    [string]$LatestPath
    [string]$Timestamp
    [object]$Hashes
}

function Get-PackageInfo
{
    $reval = [PackageInfo]::new()
    $reval.Environment = Get-BuildEnvironment
    $reval.TargetOS = Get-Configuration CI_TARGET_OS -Required
    $reval.VersionPath = "$($reval.Environment.Channel)\v$($reval.Environment.Version)"
    $reval.ArchivePath = "$($reval.VersionPath)\$($reval.TargetOS)"
    $reval.LatestPath = "$($reval.Environment.Channel)\latest\$($reval.TargetOS)"
    $reval.Timestamp = $((Get-Date).ToUniversalTime().ToString("MMddyy HH:mm:ss"))
    $reval.Hashes = {}
    return $reval
}

Export-ModuleMember -Function * -Alias *
