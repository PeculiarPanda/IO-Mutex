obj-m+=input_device.o

module-objs+= output_device.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
