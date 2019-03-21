obj-m += input.o output.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

rmmod:
	sudo rmmod input && output

install:
	sudo insmod input.ko && sudo insmod output.ko && sudo chmod a+w+r /dev/FIFOchari && sudo chmod a+w+r /dev/FIFOcharo

test:
	echo "abc" > /dev/FIFOchari && cat /dev/FIFOcharo && dmesg | tail
