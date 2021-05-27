# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
# lib.name = externals.howto.example1

cflags := -I/usr/include/python3.7m -I/usr/include/x86_64-linux-gnu/python3.7m 
ldlibs := -lpython3.7m

# input source file (class name == source file basename)
class.sources = tastomarray.c

# all extra files to be included in binary distribution of the library
datafiles =

objectsdir=./build

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=../
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder