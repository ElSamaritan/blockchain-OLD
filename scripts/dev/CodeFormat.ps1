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

$HeaderFiles = Get-ChildItem -Path .\src -Recurse -Filter *.h
$CPPSourceFiles = Get-ChildItem -Path .\src -Recurse -Filter *.cpp
$CSourceFiles = Get-ChildItem -Path .\src -Recurse -Filter *.c

$Files = $HeaderFiles + $CPPSourceFiles + $CSourceFiles

$Encoding = New-Object System.Text.UTF8Encoding $True
foreach ($file in $Files) {
    Write-Host "Processing '$($file.FullName)'"
    clang-format -i --style=file $($file.FullName)
    $content = [IO.File]::ReadAllText($file.FullName);
    Write-Host "Speichere Datei in UTF-8(BOM)..."
    [IO.File]::WriteAllText($file.Fullname, $content, $Encoding)
}
