#!/bin/bash

set -e

DEB_TAG="registry.gitlab.com/dreyri/matter/debian:v4"
ALP_TAG="registry.gitlab.com/dreyri/matter/alpine:v1"

cd "$(dirname "$0")"
docker build --tag "${DEB_TAG}" \
    --file "Dockerfile.deb" .

docker build --tag "${ALP_TAG}" \
    --file "Dockerfile.alp" .

if [ "$1" = "--push" ]; then
    docker login registry.gitlab.com
    docker push $DEB_TAG
    docker push $ALP_TAG
else
    docker run --rm \
	--user $(id -u):$(id -g) \
        --volume "$(pwd)/..:/home/user/app" \
        --workdir "/home/user/app" \
        --tty --interactive "${DEB_TAG}" bash
fi
