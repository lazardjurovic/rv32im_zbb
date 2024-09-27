
# Readme

## Required tools setup

### Install dependencies
```
sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
```
### Install riscv-gnu-toolchain
```
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
export RISCV=/path/to/installation/directory
./configure --prefix=$RISCV --with-arch=rv32i --with-abi=ilp32
make
export PATH=$PATH:$RISCV/bin
```
### Verify toolchain installation
Make sure that installed gcc has version >12 since in that version BitManip instructions were included by default.
```riscv32-unknown-elf-gcc --version```
After this make sure that gcc works with I,M and Zbb instrucions.
```make test```
This should output elf dump of test program and in main label you should be able to see Zbb assembly instructions.

## Running virtual platform
First thing to do is generate instruction and data memory files to be loaded into emulator.
```
cd vp
make
./main
```

## Running frequency estimation
``` cd freq_estim```
In that folder you should be able to find Readme file with guide how to run different frequency estimations. You can view it with ```nano Readme.md```