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

set -e

mkdir -p ./.docker || $true
unzip -oq "./.packages/beta/latest/Ubuntu Bionic/binaries.zip" -d ./.docker

FULL_VERSION=$(ls ./.docker/Xi | head -1)

mkdir -p ./.docker/context || $true
for i in $(find ./.docker -type f -name "xi-*"); do
    APP=$(basename $i)
    APP_SHORT=${APP/xi-/}
    VERSION=$(echo $FULL_VERSION | sed -e 's/-.*//g' -e 's/v//g')
    CHANNEL=$(echo $FULL_VERSION | sed -e 's/.*-//g')

    if [ -z "$CHANNEL" ]; then
        CHANNEL=stable
    fi

    BASE_TAG=registry.gitlab.com/galaxia-project/blockchain/${CHANNEL}/${APP_SHORT}
    echo "Building $BASE_TAG:${VERSION}"

    cp "$i" ./.docker/context

    docker pull $BASE_TAG:latest || true
    docker build \
        -f ./scripts/ci/Dockerfile \
        -t $BASE_TAG:${VERSION} \
        -t $BASE_TAG:latest \
        --build-arg XI_APP=${APP} \
        ./.docker/context
    docker push $BASE_TAG:${VERSION}
    docker push $BASE_TAG:latest
    rm -rf ./.docker/context/*
done

set +e
