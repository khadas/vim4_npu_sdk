
# Run in Docker

```shell
$ docker pull numbqq/npu-vim4
$ docker run -it --name npu-vim4 -v $(pwd):/home/khadas/npu \
				-v /etc/localtime:/etc/localtime:ro \
				-v /etc/timezone:/etc/timezone:ro \
				numbqq/npu-vim4
```

# Run

```shell
$ cd demo
$ bash convert_adla.sh
```
