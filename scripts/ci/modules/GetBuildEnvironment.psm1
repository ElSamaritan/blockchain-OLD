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

class BuildEnvironment {
    [string]$Version
    [string]$Channel
    [string]$HostOS
    [string]$HostPlatform
    [string]$GitBranch
    [string]$GitCommitSha
    [string]$GitCommitShortSha
    [string]$CiPipeline
}

#Requires -Modules WriteLog

function Get-BuildEnvironment
{
    $reval = [BuildEnvironment]::new()
    $reval.Version = $(Get-Content .\VERSION)
    $reval.HostOS = $PSVersionTable.OS
    $reval.HostPlatform = $PSVersionTable.Platform

    if(Test-Path env:CI)
    {
        Write-Log "Detected CI environment. Using predefined CI environment variables for build info."
        $reval.GitBranch = $env:CI_COMMIT_REF_NAME
        $reval.GitCommitSha = $env:CI_COMMIT_SHA
        $reval.GitCommitShortSha = $env:CI_COMMIT_SHORT_SHA
        $reval.CiPipeline = $env:CI_PIPELINE_URL
    }
    else 
    {
        $reval.GitBranch = $(git rev-parse --abbrev-ref HEAD)
        $reval.GitCommitSha = $(git rev-parse --verify HEAD)
        $reval.GitCommitShortSha = $(git rev-parse --short HEAD)
        $reval.CiPipeline = $null    
    }

    if($reval.GitBranch -like "master")
    {
        $reval.Channel = "stable"
    }
    elseif($reval.GitBranch -like "release-candidate")
    {
        $reval.Channel = "beta"
    }
    elseif($reval.GitBranch -like "develop")
    {
        $reval.Channel = "edge"
    }
    else
    {
        $reval.Channel = "clutter"
    }

    return $reval
}