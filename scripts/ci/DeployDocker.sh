#!/bin/sh

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
