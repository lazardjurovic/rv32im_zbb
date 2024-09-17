ifneq ($(KERNELRELEASE),)
	obj-m :=_bram_driver.o
else

KERNELDIR ?= /lib/modules/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc app.c -o app -lpthread
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f *~
	rm app
endif
