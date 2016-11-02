obj-m += taskInfo.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	rm -f *.o *.order *symvers *.mod.c
#make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
