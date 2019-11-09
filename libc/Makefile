ARCH:=${ARCH}
HOST:=${HOST}

CC:=${CC}
AS:=${AS}
ROOT:=${ROOT}

CFLAGS:=${CFLAGS}
CPPFLAGS:=${CPPFLAGS}
LIBC_HEADERS:=${INCLUDE}
KINCLUDE:=${KINCLUDE}

LIBK_FLAGS=-D__is_libk -I$(KINCLUDE)

SUPPORTED=string stdio

REQUIRED=$(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.cpp))

LIBC_OBJ=$(REQUIRED:.cpp=.libc.o)
LIBK_OBJ=$(REQUIRED:.cpp=.libk.o)

TO_BUILD=$(LIBK_OBJ) $(LIBC_OBJ)

.SUFFIXES: .libc.o libk.o .cpp

build: $(TO_BUILD) 
	$(ROOT)/scripts/mkdir.sh lib
	$(AR) rcs $@.libc.a $(LIBC_OBJ)
	$(AR) rcs $@.libk.a $(LIBK_OBJ)
	mv *.libc.a lib/
	mv *.libk.a lib/

$(LIBC_OBJ): $(REQUIRED_LIBC)

$(LIBK_OBJ): $(REQUIRED_LIBK)

%.libc.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) \
	-I$(LIBC_HEADERS) \
	$< -o $@

%.libk.o: %.cpp
	echo $(LIBC_HEADERS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(LIBK_FLAGS) \
	-I$(LIBC_HEADERS) \
	$< -o $@

clean:
	rm $(foreach dir, $(SUPPORTED), $(wildcard $(dir)/*.o)) 
	rm -rf lib/
