#!/bin/sh

# ============================================================================================== #
#                                                                                                #
#                                       Xi Blockchain                                            #
#                                                                                                #
# ---------------------------------------------------------------------------------------------- #
# This file is part of the Galaxia Project - Xi Blockchain                                       #
# ---------------------------------------------------------------------------------------------- #
#                                                                                                #
# Copyright 2018 Galaxia Project Developers                                                      #
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

TAG="v$(head -n 1 VERSION)"

if [ "$CI_COMMIT_REF_NAME" == "master" ]; then
    EXTRA=""
elif [ "$CI_COMMIT_REF_NAME" == "develop" ]; then
    EXTRA="dev-"
else
    echo "Docker will be only deployed for master and develop branch."
    exit 1
fi

docker build -t "registry.gitlab.com/galaxia-project/blockchain/xi:$EXTRA$TAG" --file ./scripts/ci/Dockerfile .
docker tag "registry.gitlab.com/galaxia-project/blockchain/xi:$EXTRA$TAG" "registry.gitlab.com/galaxia-project/blockchain/xi:"$EXTRA"latest"
docker push "registry.gitlab.com/galaxia-project/blockchain/xi:"$EXTRA"latest"
docker push "registry.gitlab.com/galaxia-project/blockchain/xi:$EXTRA$TAG" 
