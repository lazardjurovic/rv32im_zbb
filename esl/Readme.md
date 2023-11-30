# Readme
## Required tools setup
### Install dependencies

    sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev

### Install riscv-gnu-toolchain
	git clone --recursive https://github.com/riscv/riscv-gnu-toolchain 
	cd riscv-gnu-toolchain
	export RISCV=/path/to/installation/directory 
	./configure --prefix=$RISCV --with-arch=rv32i --with-abi=ilp32
	make
	export PATH=$PATH:$RISCV/bin
### Verify toolchain instalation

    riscv32-unknown-elf-gcc --version
## Running virtual platform
First thing to do is generate instruction and data memory files to be loaded into emulator.

    make