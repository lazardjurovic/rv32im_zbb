
# Readme

  

## Maximum frequency estimation

Regarding longest combinational path in implementation our hypothesis is that it's execute phase of pipeline. For that reason file execute_phase.v is given. It models all muxes on ALU inputs and also all combinational logic inside of it. For measuring longest propagation time Vivado project is build by Tcl script and runs elaboration, synthesys and implementation for Zybo board. Synthesys is done in -mode out_of_context which views design as not connected to I/O pins of FPGA thus not including long routing lines which increase signal propagation time.

## Structure

- setup.tcl - Tcl script to create and build Vivado project ( running performance optimization flow in synthesys step )
- Makefile
- execute_phase.v - Verilog implementation of combinational circuit of execute phase
- constraints.xdc - Xilinx Design constraints file ( trying to force tool to implement design with less than 7ns max propagation time)
- constraints.sdc - Synopsys Design constraints file ( needed for ASIC flow)
- yosys_xil.ys - Build script for Xilinx 7 Series FPGAs and yosys
- yosys_cmos.ys - Build script for sky130 ( not so neede when using ASIC flow)
- sky.lib - Skywater sky130 nm library

## Usage

Run Makefile by typing ``` make ```
As a result a folder /freq_estim/projects should be created and it will contain all Vivado files. After Vivado finishes all steps select ```View Reports``` option to see timing summary.
After finishing analisys run: ``` make clean ```

## Using yosys

Second option for synthesys is open source yosys toolchain. To get it on Ubuntu run:
``` sudo apt-get install yosys ```
To run yosys synthesys for Xilinx 7 series FPGA type:
``` make yosys_xil ```. In terminal a summary of used resources will be shown. This data can be used to cmpare results of Vivado and yosys.
To run yosys synthesys for CMOS type:
``` make yosys_cmos ```. Data in terminal will show how much of standard skywater sky130 PDK cells of  are utilized.

## Silicon compiler flow

To analyze behaviour of circuit in ASIC technology we need to transfer our verilog design to GDSII. It can be done by using siliconcompiler python library. On their [site](https://docs.siliconcompiler.com/en/stable/user_guide/installation.html) following instalation guide is given.
```
python3  --version  # check for Python 3
sudo  apt  update  # update package information
sudo  apt  install  python3-dev  python3-pip  python3-venv  # install dependencies
python3  -m  venv  ./venv  # create a virtual env
source  ./venv/bin/activate  # active virtual env (bash/zsh)
```
After finishing setup user can verify it by running sample design with:
``` sc -target asic_demo -remote ```
For running custom designs one needs to run following command:
``` sc execute_phase.v constr.sdc -target "skywater130_demo" -remote ```
After a while results will be shown and they will contain maximum working frequency of a given design. Resulting maximum frequency is 38.629MHz which I would prescribe to long routing wires to I/O ports in ASIC ( design is not implemented as out of context). Having in mind that in FPGA technology ratio of latency introduced by routing and funcitonality was about 3.6 this result achieved in ASIC can be declared good. To find detailed information of ASIC implementation whole system needs to be implemented.

## Results

After finished implementation results can be seen. Tool managed to pack design in a way that longest combinational path is less than 8ns. Worst Negative Slack (WNS) on setup is 0.079ns (my be different from run to run since tool uses heuristic algorithms). Longest time of 7ns corresponds to frequency of roughly 125MHz. Having in mind that a lot of logic will be build around our ALU we propose frequency of 100MHz for full design.