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

param(
    [Parameter(Mandatory = $true)][string]$Container,
    [Parameter(Mandatory = $true)][string]$Name,
    $JumpIn = $false
)

$RootDir = $(git rev-parse --show-toplevel)
$BuildDir = "$RootDir\.build\$Name"

Write-Host "[--XI--] Container: $Container"
Write-Host "[--XI--] Root Dir : $RootDir"
Write-Host "[--XI--] Build Dir: $BuildDir"

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

Push-Location $RootDir

docker pull $Container
docker run --rm -it -e HUNTER_ACCESS_TOKEN=$Env:HUNTER_ACCESS_TOKEN -v "${PWD}:/source" -w "/build" $Container bash /source/scripts/dev/Build.sh
if ($JumpIn) {
    docker run --rm -it -e HUNTER_ACCESS_TOKEN=$Env:HUNTER_ACCESS_TOKEN -v "${PWD}:/source" -v "${PWD}/.build/$($Name):/build" -w "/build" $Container bash
}

Pop-Location