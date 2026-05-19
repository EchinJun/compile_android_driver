# 模块名称（最终会生成 TearGame.ko）
obj-m := TearGame.o
# 组成模块的源文件（注意，这里是 .o 后缀，但指的是你自己的 .c 文件）
TearGame-objs := entry.o memory.o process.o

# 内核源码目录（通常由在线环境自动提供，固定为 /home/runner/work/_temp/... 或类似）
# 这个 KDIR 变量必须保留，但值一般会自动设置，我们写成 ?= 给个默认值就行
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# 默认编译目标
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.ko *.o *.mod *.mod.c *.order *.symvers
