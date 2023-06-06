WIN_GPP32=i686-w64-mingw32-g++
WIN_GPP64=x86_64-w64-mingw32-g++
LINUX_GPP=g++

COMPILE_FLAGS=-Wall -Wextra -I../include -O2 -std=c++17
LINK_FLAGS=-O2 -static

SOURCE_FILES=$(shell find ../source -name *.cpp)
LINUX_OBJECT_FILES32=$(addsuffix l32.o, $(SOURCE_FILES))
LINUX_OBJECT_FILES64=$(addsuffix l64.o, $(SOURCE_FILES))
WIN_OBJECT_FILES32=$(addsuffix w32.o, $(SOURCE_FILES))
WIN_OBJECT_FILES64=$(addsuffix w64.o, $(SOURCE_FILES))

WIN_GPP32_INSTALLED:=$(shell command -v $(WIN_GPP32) 2> /dev/null)
WIN_GPP64_INSTALLED:=$(shell command -v $(WIN_GPP64) 2> /dev/null)
LINUX_GPP_INSTALLED:=$(shell command -v g++ 2> /dev/null)

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

clear-binaries:
	rm -f  ./bin/*

build-gear-linux32:
ifndef LINUX_GPP_INSTALLED
	$(error "$(LINUX_GPP) is not installed")
endif
	$(MAKE) $(LINUX_OBJECT_FILES32)
	$(LINUX_GPP) $(LINUX_OBJECT_FILES32) -m32 $(LINK_FLAGS) -o ./bin/gear32.elf

build-gear-linux64:
ifndef LINUX_GPP_INSTALLED
	$(error "$(LINUX_GPP) is not installed")
endif
	$(MAKE) $(LINUX_OBJECT_FILES64)
	$(LINUX_GPP) $(LINUX_OBJECT_FILES64) -m64 $(LINK_FLAGS) -o ./bin/gear64.elf

build-gear-win32:
ifndef WIN_GPP32_INSTALLED
	$(error "$(WIN_GPP32) is not installed")
endif
	$(MAKE) $(WIN_OBJECT_FILES32)
	$(WIN_GPP32) $(WIN_OBJECT_FILES32) -m32 $(LINK_FLAGS) -o ./bin/gear32.exe

build-gear-win64:
ifndef WIN_GPP64_INSTALLED
	$(error "$(WIN_GPP64) is not installed")
endif
	$(MAKE) $(WIN_OBJECT_FILES64)
	$(WIN_GPP64) $(WIN_OBJECT_FILES64) -m64 $(LINK_FLAGS) -o ./bin/gear64.exe

%.cppl32.o:	%.cpp
	$(LINUX_GPP) -c $< -m32 $(COMPILE_FLAGS) -o $@

%.cppl64.o:	%.cpp
	$(LINUX_GPP) -c $< -m64 $(COMPILE_FLAGS) -o $@

%.cppw32.o:	%.cpp
	$(WIN_GPP32) -c $< -m32 $(COMPILE_FLAGS) -o $@

%.cppw64.o:	%.cpp
	$(WIN_GPP64) -c $< -m64 $(COMPILE_FLAGS) -o $@