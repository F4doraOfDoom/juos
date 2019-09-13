ARCH:=${ARCH}
HOST:=${HOST}
CC:=${CC}
CFLAGS:=${CFLAGS}
CPPFLAGS:=${CFLAGS}

KERNEL_FLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
KERNEL_OBJECTS=$(shell echo kernel/`ls kernel | grep cpp`)

build: 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-Iinclude \
	-T arch/$(ARCH)/linker.ld \
	-c $(KERNEL_OBJECTS)

clean:
	rm *.o

