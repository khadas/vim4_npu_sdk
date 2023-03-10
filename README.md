
# Run in Docker

```shell
$ docker pull yanwyb/npu:v1
$ docker run -it --name vim4-npu1 -v $(pwd):/home/khadas/npu \
				-v /etc/localtime:/etc/localtime:ro \
				-v /etc/timezone:/etc/timezone:ro \
				yanwyb/npu:v1
```

# Run

```shell
$ cd demo
$ bash convert_adla.sh
```
