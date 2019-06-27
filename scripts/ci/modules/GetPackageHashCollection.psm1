# ============================================================================================== #
#                                                                                                #
#                                     Galaxia Blockchain                                         #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Xi framework.                                                         #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018-2019 Xi Project Developers <support.xiproject.io>                               #
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

class PackageHashCollection
{
    [string]$md5
    [string]$sha1
    [string]$sha256
    [string]$sha384
    [string]$sha512
}

function Get-PackageHashCollection
{
    param(
        [Parameter(Mandatory=$true)][string]$Path
    )

    $reval = [PackageHashCollection]::new()
    $reval.md5 = $(Get-FileHash -Path $Path -Algorithm MD5).Hash
    $reval.sha1 = $(Get-FileHash -Path $Path -Algorithm SHA1).Hash
    $reval.sha256 = $(Get-FileHash -Path $Path -Algorithm SHA256).Hash
    $reval.sha384 = $(Get-FileHash -Path $Path -Algorithm SHA384).Hash
    $reval.sha512 = $(Get-FileHash -Path $Path -Algorithm SHA512).Hash
    return $reval
}