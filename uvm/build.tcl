# Specify the directory path
set dirPath "tmp"

# Check if the directory exists
if {[file exists $dirPath]} {
    # If it exists, delete the directory and its contents
    file delete -force $dirPath
    puts "Directory deleted at: $dirPath"
}

# Create a new empty directory
file mkdir $dirPath
puts "New directory created at: $dirPath"

# Building the project
puts "Creating new Vivado project at: $dirPath"
create_project verif_skripta_test $dirPath/verif_skripta_test -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo:part0:2.0 [current_project]
set_property simulator_language Verilog [current_project]
add_files -norecurse -scan_for_includes {../rtl/src/cpop_module.v ../rtl/src/control_path.v ../rtl/src/branch_module.v ../rtl/src/hazard_unit.v ../rtl/src/forwarding_unit.v ../rtl/src/immediate.v ../rtl/src/signed_mul.v ../rtl/src/register_file.v ../rtl/src/clz_module.v ../rtl/src/alu_decoder.v ../rtl/src/data_path.v ../rtl/src/risc_v_cpu_v1_0_S00_AXI.v ../rtl/src/top.v ../rtl/src/alu.v ../rtl/src/control_decoder.v ../rtl/src/unsigned_mul.v ../rtl/src/bram.v ../rtl/src/risc_v_cpu_v1_0.v ../rtl/src/cpu.v}
add_files -fileset sim_1 -norecurse -scan_for_includes {configurations/config_pkg.sv sequences/bram_seq_pkg.sv cpu_verif_top.sv agent/axi_agent_pkg.sv sequences/axi_seq_pkg.sv agent/bram_agent_pkg.sv interfaces/bram_if.sv test_pkg.sv interfaces/axi_if.sv}
import_files -force -norecurse
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1
set_property -name {xsim.elaborate.xelab.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.compile.xvlog.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.simulate.xsim.more_options} -value {-testplusarg UVM_TETSNAME=cpu_test -testplusarg UVM_VERBOSITY=UVM_LOW} -objects [get_filesets sim_1]
set_property top cpu_verif_top [get_filesets sim_1]
set_property top_lib xil_defaultlib [get_filesets sim_1]
update_compile_order -fileset sim_1