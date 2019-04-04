#!/bin/bash

set -e

DEB_TAG="registry.gitlab.com/dreyri/matter/debian:v4"
ALP_TAG="registry.gitlab.com/dreyri/matter/alpine:v1"

CURRENT_TAG=""
CURRENT_FILE=""

get_dockertag() {
    if [ "$1" = "debian" ]; then
	CURRENT_TAG="$DEB_TAG"
	return 0
    elif [ "$1" = "alpine" ]; then
	CURRENT_TAG="$ALP_TAG"
	return 0
    fi

    echo "Unknown image name \"$1\""
    exit 1
}

get_dockerfile() {
    get_dockertag "$1"
    local tag="$CURRENT_TAG"

    if [ "$tag" = "$DEB_TAG" ]; then
	CURRENT_FILE="Dockerfile.deb"
	return 0
    elif [ "$tag" = "$ALP_TAG" ]; then
	CURRENT_FILE="Dockerfile.alp"
	return 0
    fi

    echo "Unknown image tag \"$tag\""
    exit 2
}

build_image () {
    docker build --tag "$1" \
        --file "$2" .
}

run_image() {
    docker run --rm \
           --user $(id -u):$(id -g) \
	   --volume "$(pwd)/..:/home/user/app" \
	   --workdir "/home/user/app" \
	   --tty --interactive "$1" bash
}

cd "$(dirname "$0")"

if [ "$1" = "build" ]; then
    get_dockertag "$2"
    get_dockerfile "$2"

    build_image "$CURRENT_TAG" "$CURRENT_FILE"

elif [ "$1" = "push" ]; then
    images=()

    for name in ${@:2}
    do
	# verify the provided name is valid
	get_dockertag "$name"
	images+=" $name"
    done

    # build all images if required
    for image in $images
    do
	get_dockertag "$image"
	tag="$CURRENT_TAG"
	get_dockerfile "$image"
	file="$CURRENT_FILE"
	build_image "$tag" "$file"
    done
    
    # login and push all images
    docker login registry.gitlab.com

    for image in $images
    do
	get_dockertag "$image"
	tag="$CURRENT_TAG"
	docker push "$tag"
    done
elif [ "$1" = "run" ]; then
    get_dockertag "$2"
    tag="$CURRENT_TAG"

    run_image "$tag"
fi
