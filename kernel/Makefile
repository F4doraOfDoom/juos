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

KERNEL_BUILD_ARGS:=$(patsubst %, -D%, $(KERNEL_BUILD_ARGS))
KERNEL_FLAGS=-ffreestanding -fno-exceptions -fno-rtti -nostdlib -lgcc
KERNEL_C=$(wildcard kernel/*.c kernel/*/.c) 
KERNEL_CPP=$(wildcard kernel/*.cpp kernel/*/.cpp)
KERNEL_ASM=$(wildcard kernel/*.S kernel/*/.S)
DRIVER_C=$(wildcard drivers/*.c drivers/*/.c) 
DRIVER_CPP=$(wildcard drivers/*.cpp drivers/*/.cpp)
DRIVER_ASM=$(wildcard drivers/*.S drivers/*/.S)
KERNEL_OBJS= \
$(KERNEL_C:.c=.o) \
$(KERNEL_CPP:.cpp=.o) \
$(KERNEL_ASM:.S=.o) \
$(DRIVER_C:.c=.o) \
$(DRIVER_CPP:.cpp=.o) \
$(DRIVER_ASM:.S=.o) 

ARCH_DIR=arch/$(ARCH)
ARCH_C=$(wildcard $(ARCH_DIR)/*.c $(ARCH_DIR)/*/*.c) 
ARCH_CPP=$(wildcard $(ARCH_DIR)/*.cpp $(ARCH_DIR)/*/*.cpp)
ARCH_ASM=$(wildcard $(ARCH_DIR)/*.S $(ARCH_DIR)/*/*.S)
ARCH_OBJS= \
$(ARCH_C:.c=.o) \
$(ARCH_CPP:.cpp=.o) \
$(ARCH_ASM:.S=.o)

LIBC_OBJS=$(wildcard $(LIBC)/lib/*.libk.a)
LIBCXX_OBJS=$(wildcard $(LIBC)/lib/*.libk.a)

PROJECT:=${PROJECT_NAME}.kernel

.SUFFIXES: .o .cpp .S

build: $(KERNEL_OBJS) $(ARCH_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) \
	-T $(ARCH_DIR)/linker.ld \
	$(KERNEL_OBJS) $(ARCH_OBJS) $(LIBC_OBJS) $(LIBCXX_OBJS) \
	-o $(PROJECT)
	mv $(PROJECT) ${ROOT}

$(KERNEL_OBJS): $(KERNEL_C) $(KERNEL_CPP) $(KERNEL_ASM)

$(ARCH_OBJS): $(ARCH_C) $(ARCH_CPP) $(ARCH_ASM)

.S.o:
	$(AS) $< -o $@

.c.o .cpp.o:  
	echo $(CXXINCLUDE)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(KERNEL_FLAGS) $(KERNEL_BUILD_ARGS) \
	-I$(KINCLUDE) -I$(INCLUDE) -I$(CXXINCLUDE) -I$(ARCH_DIR) \
	-c $< -o $@
	
clean:
	rm -f */*.o */*/*.o */*/*/*.o
