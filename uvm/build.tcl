create_project verif_skripta_test /home/lazar/Desktop/tests/verif_skripta_test -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo:part0:2.0 [current_project]
set_property simulator_language Verilog [current_project]
add_files -norecurse -scan_for_includes {/home/lazar/Desktop/y24-g05/rtl/src/cpop_module.v /home/lazar/Desktop/y24-g05/rtl/src/control_path.v /home/lazar/Desktop/y24-g05/rtl/src/branch_module.v /home/lazar/Desktop/y24-g05/rtl/src/hazard_unit.v /home/lazar/Desktop/y24-g05/rtl/src/forwarding_unit.v /home/lazar/Desktop/y24-g05/rtl/src/immediate.v /home/lazar/Desktop/y24-g05/rtl/src/signed_mul.v /home/lazar/Desktop/y24-g05/rtl/src/register_file.v /home/lazar/Desktop/y24-g05/rtl/src/clz_module.v /home/lazar/Desktop/y24-g05/rtl/src/alu_decoder.v /home/lazar/Desktop/y24-g05/rtl/src/data_path.v /home/lazar/Desktop/y24-g05/rtl/src/risc_v_cpu_v1_0_S00_AXI.v /home/lazar/Desktop/y24-g05/rtl/src/top.v /home/lazar/Desktop/y24-g05/rtl/src/alu.v /home/lazar/Desktop/y24-g05/rtl/src/control_decoder.v /home/lazar/Desktop/y24-g05/rtl/src/unsigned_mul.v /home/lazar/Desktop/y24-g05/rtl/src/bram.v /home/lazar/Desktop/y24-g05/rtl/src/risc_v_cpu_v1_0.v /home/lazar/Desktop/y24-g05/rtl/src/cpu.v}
add_files -fileset sim_1 -norecurse -scan_for_includes {/home/lazar/Desktop/y24-g05/uvm/configurations/config_pkg.sv /home/lazar/Desktop/y24-g05/uvm/sequences/bram_seq_pkg.sv /home/lazar/Desktop/y24-g05/uvm/cpu_verif_top.sv /home/lazar/Desktop/y24-g05/uvm/agent/axi_agent_pkg.sv /home/lazar/Desktop/y24-g05/uvm/sequences/axi_seq_pkg.sv /home/lazar/Desktop/y24-g05/uvm/agent/bram_agent_pkg.sv /home/lazar/Desktop/y24-g05/uvm/interfaces/bram_if.sv /home/lazar/Desktop/y24-g05/uvm/test_pkg.sv /home/lazar/Desktop/y24-g05/uvm/interfaces/axi_if.sv}
import_files -force -norecurse
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
update_compile_order -fileset sim_1
set_property -name {xsim.elaborate.xelab.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.compile.xvlog.more_options} -value {-L uvm} -objects [get_filesets sim_1]
set_property -name {xsim.simulate.xsim.more_options} -value {-testplusarg UVM_TETSNAME=cpu_test -testplusarg UVM_VERBOSITY=UVM_LOW} -objects [get_filesets sim_1]
set_property dataflow_viewer_settings "min_width=16"   [current_fileset]