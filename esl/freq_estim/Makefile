VIVADO = /tools/Xilinx/Vivado/2022.2/bin/vivado

vivado: execute_phase.v
	$(VIVADO) -mode tcl -source setup.tcl

yosys_xil: execute_phase.v yosys_xil.ys
	yosys yosys_xil.ys

yosys_cmos: yosys_cmos.ys
	yosys yosys_cmos.ys

clean: 
	rm -rf projects
	rm -rf .Xil/
	rm vivado*