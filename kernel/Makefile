ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
LIBC:=${INCLUDE}

KERNEL_FLAGS=-ffreestanding -fno-exceptions -fno-rtti -nostdlib -lgcc
KERNEL_OBJECTS=$(shell echo kernel/`ls kernel | grep cpp`)

build: .o .S
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-T arch/$(ARCH)/linker.ld \
	obj/*.o \

.S:
	$(AS) arch/$(ARCH)/boot.S -o boot.o
	mv boot.o obj/

.o: 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-Iinclude -I$(LIBC) \
	-c $(KERNEL_OBJECTS) arch/$(ARCH)/*.cpp 
	mv *.o obj/
	

clean:
	rm obj/*.o

