##
#
#   Copyright (c) 2003, 2006 Bryan Bullard.  All rights reserved.
#
# Permission is hereby granted, free of charge, to any person 
# obtaining a copy of this software and associated documentation 
# files (the "Software"), to deal in the Software without 
# restriction, including without limitation the rights to use, 
# copy, modify, merge, publish, distribute, sublicense, and/or 
# sell copies of the Software, and to permit persons to whom 
# the Software is furnished to do so, subject to the following 
# conditions:
#
#   The above copyright notice and this permission notice 
#   shall be included in all copies or substantial portions 
#   of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
# OTHER DEALINGS IN THE SOFTWARE.
#
##


# This is the bash build script for elf32-i386, win32 host
# 7.15.2003 bb	
#
#
#


BUILD_ROOT=$(pwd)

echo "Assembler invoked ..."
cd $BUILD_ROOT/src/arch/ia32/
nasm -f win32 --prefix _ -o 000start.o start.s
nasm -f win32 --prefix _ -o arch.o arch.s
nasm -f win32 --prefix _ -o dispatch.o dispatch.s
nasm -f win32 --prefix _ -o macros.o macros.s
nasm -f win32 -o atomic.o atomic.s
nasm -f win32 --prefix _ -o portio.o portio.s
nasm -f win32 --prefix _ -o syscall.o syscall.s
ld -r -o $BUILD_ROOT/bin/000src_arch_ia32_s.o *.o
rm *.o

echo "Compiler invoked ..."
cd $BUILD_ROOT/src/
gcc -ansi -O -I $BUILD_ROOT/incclude -I. -Wall -nostdinc -ffreestanding -c *.c
ld -r -o $BUILD_ROOT/bin/src_c.o *.o
rm *.o

cd $BUILD_ROOT/src/arch/ia32/
gcc -ansi -O -I $BUILD_ROOT/include -I. -Wall -nostdinc -ffreestanding -c *.c
ld -r -o $BUILD_ROOT/bin/src_arch_ia32_c.o *.o
rm *.o

cd $BUILD_ROOT

echo "Linker invoked ..."
ld -T./build.ld -dN -o ./bin/oskernel.pe ./bin/*.o
rm ./bin/*.o

# convert the pe file output from cygwin ld to elf
objcopy -O elf32-i386 ./bin/oskernel.pe ./bin/oskernel.elf
rm ./bin/*.pe

# dump the excore file as asmembler listing
objdump --disassemble-all --all-headers ./bin/oskernel.elf > ./bin/oskernel.lst

mv ./bin/oskernel.elf ./bin/oskernel


echo "Build complete."
