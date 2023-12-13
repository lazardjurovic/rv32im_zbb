
# Readme

## Maximum frequency estimation

Regarding longest combinational path in implementation our hypothesis is that it's execute phase of pipeline. For that reason file execute_phase.v is given. It models  all muxes on ALU inputs and also all combinational logic inside of it. For measuring longest propagation time Vivado project is build by Tcl script and runs elaboration, synthesys and implementation for Zybo board. Synthesys is done in -mode out_of_context which views design as not connected to I/O pins of FPGA thus not including long routing lines which increase signal propagation time. 

## Structure
-	setup.tcl - Tcl script to create and build Vivado project ( running performance optimization flow in synthesys step )
-	Makefile
-	execute_phase.v - Verilog implementation of combinational circuit of execute phase
-	constraints.xdc - Design constraints file ( trying to force tool to implement design with less than 7ns max propagation time)

## Usage
From project's root directory /esl run:
``` make freq ```
As a result a folder /freq_estim/projects should be created and it will contain all Vivado files. After Vivado finishes all steps select ```View Reports``` option to see timing summary. 
After finishing analisys, form /esl run:
```
cd freq_estim 
make clean 
```

## Results
After finished implementation results can be seen. Tool managed to pack design in a way that longest combinational path is less than 7ns. Worst Negative Slack (WNS) on setup is 0.173ns (my be different from run to run since tool uses heuristic algorithms). Longest time of 7ns corresponds to frequency of roughly 143MHz. Having in mind that a lot of logic will be build around our ALU we propose frequency of 100MHz for full design.
