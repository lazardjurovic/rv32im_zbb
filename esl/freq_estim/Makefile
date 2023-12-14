VIVADO = /tools/Xilinx/Vivado/2022.2/bin/vivado

vivado: execute_phase.v
	$(VIVADO) -mode tcl -source setup.tcl

yosys_cmos: yosys_cmos.ys
	yosys yosys_cmos.ys

yosys_xil: yosys_xil.ys
	yosys yosys_xil.ys

clean: 
	rm netlist.v
	rm netlist.json
	rm -rf projects
	rm -rf .Xil/
	rm vivado*
