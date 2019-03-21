obj-m+= input.o output.o

all:
 make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

clean:
 make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean

rmmod:
 sudo rmmod input && sudo rmmod output

install:
 sudo insmod input.ko && sudo insmod input.ko && sudo chmod a+w+r /dev/FIFOchar

 test:
 	echo "abc" > /dev/FIFOchar && cat /dev/FIFOchar && dmesg | tail
