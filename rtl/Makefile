
all:
	yosys build.ys

test: test_wave
	vvp test_wave -lxt2
	gtkwave control.vcd

test_wave:
	iverilog -o test_wave testbench/control_path_tb.v alu_decoder.v control_decoder.v  forwarding_unit.v hazard_unit.v control_path.v


clean:
	rm control.vcd
	rm test_wave
	rm hardware.json