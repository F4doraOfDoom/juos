FILTER_OUT = $(foreach v,$(2),$(if $(findstring $(1),$(v)),,$(v)))

ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
INCLUDE:=${INCLUDE}
CXXINCLUDE:=${CXXINCLUDE}
KINCLUDE:=${KINCLUDE}
AINCLUDE:=${AINCLUDE}
LIBC:=${LIBC}
LIBCXX:=${LIBCXX}
KERNEL_BUILD_ARGS:=${KERNEL_BUILD_ARGS}

KERNEL_DEST = $(ROOT)/bin/kernel

KERNEL_BUILD_ARGS:=$(patsubst %, -D%, $(KERNEL_BUILD_ARGS))
KERNEL_FLAGS=-ffreestanding -fno-exceptions -fno-rtti -nostdlib -lgcc

KERNEL_C=$(wildcard kernel/*.c kernel/*/.c) 
KERNEL_CPP=$(wildcard kernel/*.cpp kernel/*/.cpp)
KERNEL_ASM=$(wildcard kernel/*.S kernel/*/.S)
KERNEL_OBJS= \
$(KERNEL_C:.c=.ko) \
$(KERNEL_CPP:.cpp=.ko) \
$(KERNEL_ASM:.S=.ko) 
KERNEL_OUT=$(foreach file, $(KERNEL_OBJS), $(KERNEL_DEST)/$(shell basename $(file)))

DRIVER_C=$(wildcard drivers/*.c drivers/*/.c) 
DRIVER_CPP=$(wildcard drivers/*.cpp drivers/*/.cpp)
DRIVER_ASM=$(wildcard drivers/*.S drivers/*/.S)
DRIVER_OBJS= \
$(DRIVER_C:.c=.do) \
$(DRIVER_CPP:.cpp=.do) \
$(DRIVER_ASM:.S=.do)
DRIVER_OUT=$(foreach file, $(DRIVER_OBJS), $(ROOT)/bin/drivers/$(shell basename $(file)))


ARCH_DIR=arch/$(ARCH)
ARCH_C=$(wildcard $(ARCH_DIR)/*.c $(ARCH_DIR)/*/*.c) 
ARCH_CPP=$(wildcard $(ARCH_DIR)/*.cpp $(ARCH_DIR)/*/*.cpp)
ARCH_ASM=$(wildcard $(ARCH_DIR)/*.S $(ARCH_DIR)/*/*.S)
ARCH_OBJS= \
$(ARCH_C:.c=.ao) \
$(ARCH_CPP:.cpp=.ao) \
$(ARCH_ASM:.S=.ao)
ARCH_OUT=$(foreach file, $(ARCH_OBJS), $(ROOT)/bin/arch/$(shell basename $(file)))

LIBC_OBJS=$(wildcard $(ROOT)/bin/libc/*.libk.a)
#LIBCXX_OBJS=$(wildcard $(KERNEL_DEST)/libc/lib/*.libk.a)

PROJECT:=${PROJECT_NAME}.kernel

.SUFFIXES: .ko .ao .do .cpp .S

build: $(KERNEL_OBJS) $(ARCH_OBJS) $(DRIVER_OBJS)
	echo $(ARCH_CPP)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-T $(ARCH_DIR)/linker.ld \
	$(KERNEL_OUT) $(ARCH_OUT) $(DRIVER_OUT) $(LIBC_OBJS) \
	-o $(PROJECT)
	mv $(PROJECT) ${ROOT}

$(KERNEL_OBJS): $(KERNEL_C) $(KERNEL_CPP) $(KERNEL_ASM)

$(ARCH_OBJS): $(ARCH_C) $(ARCH_CPP) $(ARCH_ASM)

$(DRIVER_OBJS): $(DRIVER_C) $(DRIVER_CPP) $(DRIVER_ASM)

.S.do:
	$(AS) $< -o $(ROOT)/bin/drivers/$$(basename $@)

.S.ko:
	$(AS) $< -o $(ROOT)/bin/kernel/$$(basename $@)

.S.ao:
	$(AS) $< -o $(ROOT)/bin/arch/$$(basename $@)

.c.do .cpp.do:
	echo $(CXXINCLUDE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) $(KERNEL_BUILD_ARGS) \
	-I$(KINCLUDE) -I$(INCLUDE) -I$(CXXINCLUDE) -I$(ARCH_DIR) \
	-c $< -o $(ROOT)/bin/drivers/$$(basename $@)

.c.ko .cpp.ko:
	echo $(CXXINCLUDE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) $(KERNEL_BUILD_ARGS) \
	-I$(KINCLUDE) -I$(INCLUDE) -I$(CXXINCLUDE) -I$(ARCH_DIR) \
	-c $< -o $(ROOT)/bin/kernel/$$(basename $@)

.c.ao .cpp.ao:
	echo AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa $(CXXINCLUDE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) $(KERNEL_BUILD_ARGS) \
	-I$(KINCLUDE) -I$(INCLUDE) -I$(CXXINCLUDE) -I$(ARCH_DIR) \
	-c $< -o $(ROOT)/bin/arch/$$(basename $@)

clean:
	rm -f */*.o */*/*.o */*/*/*.o
