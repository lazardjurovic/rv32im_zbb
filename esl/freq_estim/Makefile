VIVADO = /tools/Xilinx/Vivado/2022.2/bin/vivado

all:
	$(VIVADO) -mode tcl -source setup.tcl

clean: vivado.jou
	rm -rf projects
	rm -rf .Xil/
	rm vivado*