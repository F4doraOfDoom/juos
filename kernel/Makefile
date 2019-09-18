ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
INCLUDE:=${INCLUDE}
LIBC:=${LIBC}

KERNEL_FLAGS=-ffreestanding -fno-exceptions -fno-rtti -nostdlib -lgcc

KERNEL_OBJS=$(wildcard obj/*.o)
LIBC_OBJS=$(wildcard $(LIBC)/lib/*.libk.a)

PROJECT_NAME:=${PROJECT_NAME}.kernel

build: .o .S
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-T arch/$(ARCH)/linker.ld \
	$(KERNEL_OBJS) $(LIBC_OBJS) \
	-o $(PROJECT_NAME)
	mv $(PROJECT_NAME) ${ROOT}

.S:
	$(AS) arch/$(ARCH)/boot.S -o boot.o
	mv boot.o obj/

.o: 
	mkdir obj
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-Iinclude -I$(INCLUDE) -MD \
	-c kernel/*.cpp arch/$(ARCH)/*.cpp 
	mv *.o obj/
	
clean:
	rm -rf obj/ 
