obj-m := TearGame.o
TearGame-objs := entry.o memory.o process.o

ccflags-y := -mbranch-protection=bti
ldflags-y := -z force-bti

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.ko *.o *.mod *.mod.c *.order *.symvers
