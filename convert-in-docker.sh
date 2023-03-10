#!/bin/bash

set -e

if [ $# -lt 1 ]; then
    echo "Error, please add args normal or ksnn !"
    exit 1
fi

if [ $1 != "normal" ] && [ $1 != "ksnn" ]; then
    echo "Error, please choose args between normal and ksnn !"
    exit 1
fi

CONTAINER_NAME="npu-vim4"
REPOSITORY_NAME="numbqq/npu-vim4"

if [ `docker inspect $CONTAINER_NAME &>>/dev/null &&  echo 0 || echo 1` -eq 0 ];then
    echo "The container $CONTAINER_NAME is exist!, stop and rm it at first"

    docker stop $CONTAINER_NAME
    docker rm -f $CONTAINER_NAME
fi

#Environment required for configuring containers
DOCKER_RUN="docker run -it --name $CONTAINER_NAME \
	--rm \
	-v $(pwd):/home/khadas/npu \
	-v /etc/localtime:/etc/localtime:ro \
	-v /etc/timezone:/etc/timezone:ro \
	-v /home/$(whoami):/home/$(whoami) \
	$REPOSITORY_NAME \
    "

echo "DOCKER_RUN:"$DOCKER_RUN

file="ksnn_args.txt"

content=$(cat "$file" | xargs)
# echo -e "$content" > example.txt

if [ $1 == "normal" ]; then
eval $DOCKER_RUN bash -c '"
	cd ~/npu/adla-toolkit-binary/demo
	bash convert_adla.sh
	"'
fi

if [ $1 == "ksnn" ]; then
eval $DOCKER_RUN bash -c '"
	cd ~/npu/adla-toolkit-binary/python
	./convert $content
	"'
fi

# Remove the container
if [ `docker inspect $CONTAINER_NAME &>>/dev/null &&  echo 0 || echo 1` -eq 0 ];then
    echo "The container $CONTAINER_NAME is exist!, stop and rm it at last"

    docker stop $CONTAINER_NAME
    docker rm -f $CONTAINER_NAME
fi

echo "Convert in Docker Done!!!"

