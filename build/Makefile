WIN_GCC32=i686-w64-mingw32-gcc
WIN_GCC64=x86_64-w64-mingw32-gcc
LINUX_GCC=gcc

COMPILE_FLAGS=-Wall -Wextra -I../include -O2 -std=c17
LINK_FLAGS=-O2

SOURCE_FILES=$(shell find ../source -name *.c)
LINUX_OBJECT_FILES32=$(addsuffix l32.o, $(SOURCE_FILES))
LINUX_OBJECT_FILES64=$(addsuffix l64.o, $(SOURCE_FILES))
WIN_OBJECT_FILES32=$(addsuffix w32.o, $(SOURCE_FILES))
WIN_OBJECT_FILES64=$(addsuffix w64.o, $(SOURCE_FILES))

WIN_GCC32_INSTALLED:=$(shell command -v $(WIN_GCC32) 2> /dev/null)
WIN_GCC64_INSTALLED:=$(shell command -v $(WIN_GCC64) 2> /dev/null)
LINUX_GCC_INSTALLED:=$(shell command -v gcc 2> /dev/null)

all:
	$(MAKE) build-gear
	$(MAKE) clear

build-gear:
	$(MAKE) build-gear-linux32
	$(MAKE) build-gear-linux64
	$(MAKE) build-gear-win32
	$(MAKE) build-gear-win64

clear:
	rm -f $(LINUX_OBJECT_FILES32) $(LINUX_OBJECT_FILES64) $(WIN_OBJECT_FILES32) $(WIN_OBJECT_FILES64)

build-gear-linux32:
ifndef LINUX_GCC_INSTALLED
	$(error "$(LINUX_GCC) is not installed")
endif
	$(MAKE) $(LINUX_OBJECT_FILES32)
	$(LINUX_GCC) $(LINUX_OBJECT_FILES32) -m32 $(LINK_FLAGS) -o ./bin/gear32

build-gear-linux64:
ifndef LINUX_GCC_INSTALLED
	$(error "$(LINUX_GCC) is not installed")
endif
	$(MAKE) $(LINUX_OBJECT_FILES64)
	$(LINUX_GCC) $(LINUX_OBJECT_FILES64) -m64 $(LINK_FLAGS) -o ./bin/gear64

build-gear-win32:
ifndef WIN_GCC32_INSTALLED
	$(error "$(WIN_GCC32) is not installed")
endif
	$(MAKE) $(WIN_OBJECT_FILES32)
	$(WIN_GCC32) $(WIN_OBJECT_FILES32) -m32 $(LINK_FLAGS) -o ./bin/gear32.exe

build-gear-win64:
ifndef WIN_GCC64_INSTALLED
	$(error "$(WIN_GCC64) is not installed")
endif
	$(MAKE) $(WIN_OBJECT_FILES64)
	$(WIN_GCC64) $(WIN_OBJECT_FILES64) -m64 $(LINK_FLAGS) -o ./bin/gear64.exe

%.cl32.o:	%.c
	$(LINUX_GCC) -c $< -m32 $(COMPILE_FLAGS) -o $@

%.cl64.o:	%.c
	$(LINUX_GCC) -c $< -m64 $(COMPILE_FLAGS) -o $@

%.cw32.o:	%.c
	$(WIN_GCC32) -c $< -m32 $(COMPILE_FLAGS) -o $@

%.cw64.o:	%.c
	$(WIN_GCC64) -c $< -m64 $(COMPILE_FLAGS) -o $@