#CREATING PROJECT

create_project projekat_synth_test_1 /home/lazar/Desktop/tests/projekat_synth_test_1 -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo:part0:2.0 [current_project]
set_property simulator_language Verilog [current_project]
add_files -norecurse -scan_for_includes {/home/lazar/Desktop/y24-g05/rtl/src/alu.v /home/lazar/Desktop/y24-g05/rtl/src/cpop_module.v /home/lazar/Desktop/y24-g05/rtl/src/control_decoder.v /home/lazar/Desktop/y24-g05/rtl/src/control_path.v /home/lazar/Desktop/y24-g05/rtl/src/unsigned_mul.v /home/lazar/Desktop/y24-g05/rtl/src/branch_module.v /home/lazar/Desktop/y24-g05/rtl/src/hazard_unit.v /home/lazar/Desktop/y24-g05/rtl/src/forwarding_unit.v /home/lazar/Desktop/y24-g05/rtl/src/immediate.v /home/lazar/Desktop/y24-g05/rtl/src/signed_mul.v /home/lazar/Desktop/y24-g05/rtl/src/bram.v /home/lazar/Desktop/y24-g05/rtl/src/register_file.v /home/lazar/Desktop/y24-g05/rtl/src/clz_module.v /home/lazar/Desktop/y24-g05/rtl/src/risc_v_cpu_v1_0.v /home/lazar/Desktop/y24-g05/rtl/src/alu_decoder.v /home/lazar/Desktop/y24-g05/rtl/src/data_path.v /home/lazar/Desktop/y24-g05/rtl/src/risc_v_cpu_v1_0_S00_AXI.v /home/lazar/Desktop/y24-g05/rtl/src/top.v /home/lazar/Desktop/y24-g05/rtl/src/cpu.v}
import_files -force -norecurse
import_files -fileset constrs_1 -force -norecurse /home/lazar/Desktop/y24-g05/rtl/constraints.xdc
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1

# CREATING BUS INTERFACES FOR IP

update_compile_order -fileset sources_1
ipx::package_project -root_dir /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs -vendor user.org -library user -taxonomy /UserIP
set_property vendor FTN [ipx::current_core]
set_property name risc_v_cpu [ipx::current_core]
set_property display_name risc_v_cpu [ipx::current_core]
ipx::add_bus_interface instr_bram [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:bram_rtl:1.0 [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:bram:1.0 [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
ipx::add_port_map RST [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_reset [ipx::get_port_maps RST -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map CLK [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_clk [ipx::get_port_maps CLK -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DIN [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_data_in [ipx::get_port_maps DIN -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map EN [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_enable [ipx::get_port_maps EN -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DOUT [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_data_out [ipx::get_port_maps DOUT -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map WE [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_we [ipx::get_port_maps WE -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map ADDR [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_addr [ipx::get_port_maps ADDR -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_bus_interface data_bram [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:bram_rtl:1.0 [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:bram:1.0 [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
ipx::add_port_map RST [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_reset [ipx::get_port_maps RST -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map CLK [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_clk [ipx::get_port_maps CLK -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DIN [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_data_in [ipx::get_port_maps DIN -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map EN [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_enable [ipx::get_port_maps EN -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DOUT [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_data_out [ipx::get_port_maps DOUT -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map WE [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_we [ipx::get_port_maps WE -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map ADDR [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_addr [ipx::get_port_maps ADDR -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]

ipx::add_bus_parameter MASTER_TYPE [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property value BRAM_CTRL [ipx::get_bus_parameters MASTER_TYPE -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_bus_parameter MASTER_TYPE [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property value BRAM_CTRL [ipx::get_bus_parameters MASTER_TYPE -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_bus_parameter READ_WRITE_MODE [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property value READ_WRITE [ipx::get_bus_parameters READ_WRITE_MODE -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_bus_parameter READ_WRITE_MODE [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property value READ_WRITE [ipx::get_bus_parameters READ_WRITE_MODE -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]

# ASSOCIATING CLOCKS AND PACKING

ipx::associate_bus_interfaces -busif data_bram -clock data_mem_clk [ipx::current_core]
ipx::associate_bus_interfaces -busif instr_bram -clock instr_mem_clk [ipx::current_core]
ipx::save_core [ipx::current_core]
set_property  ip_repo_paths  /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs [current_project]
update_ip_catalog


# CONNECTING MODULES IN INTEGRATOR

create_bd_design "project_design"
update_compile_order -fileset sources_1
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
endgroup
startgroup
create_bd_cell -type ip -vlnv FTN:user:risc_v_cpu:1.0 risc_v_cpu_0
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
endgroup
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]
startgroup
apply_bd_automation -rule xilinx.com:bd_rule:bram_cntlr -config {BRAM "Auto" }  [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA]
apply_bd_automation -rule xilinx.com:bd_rule:bram_cntlr -config {BRAM "Auto" }  [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTB]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_0/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/risc_v_cpu_0/s00_axi} ddr_seg {Auto} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins risc_v_cpu_0/s00_axi]
endgroup
delete_bd_objs [get_bd_intf_nets axi_bram_ctrl_0_BRAM_PORTB] [get_bd_intf_nets axi_bram_ctrl_0_BRAM_PORTA] [get_bd_cells axi_bram_ctrl_0_bram]
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] [get_bd_intf_pins risc_v_cpu_0/instr_bram]
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTB] [get_bd_intf_pins risc_v_cpu_0/data_bram]

startgroup
set_property CONFIG.PROTOCOL {AXI4LITE} [get_bd_cells axi_bram_ctrl_0]
INFO: [xilinx.com:ip:axi_bram_ctrl:4.1-2] project_design_axi_bram_ctrl_0_0: In IP Integrator, please note that memory depth value gets calculated based on the Data Width of the IP and Address range selected in the Address Editor.Incase a validation error occured on the range of this parameter, please check if the selected Data width and the Address Range are valid. For valid Data width and memory depth values, please refer to the AXI BRAM Controller Product Guide.
INFO: [xilinx.co

# LAYOUT AND VALIDATION

regenerate_bd_layout
validate_bd_design

# GENERATE BLOCK DESIGN

ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]
update_ip_catalog -rebuild -repo_path /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs
upgrade_ip -vlnv FTN:user:risc_v_cpu:1.0 [get_ips project_design_risc_v_cpu_0_1] -log ip_upgrade.log
export_ip_user_files -of_objects [get_ips project_design_risc_v_cpu_0_1] -no_script -sync -force -quiet
generate_target all [get_files /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs/sources_1/bd/project_design/project_design.bd]
catch { config_ip_cache -export [get_ips -all project_design_axi_bram_ctrl_0_0] }
catch { config_ip_cache -export [get_ips -all project_design_axi_smc_0] }
catch { config_ip_cache -export [get_ips -all project_design_rst_ps7_0_100M_0] }
catch { config_ip_cache -export [get_ips -all project_design_risc_v_cpu_0_1] }
export_ip_user_files -of_objects [get_files /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs/sources_1/bd/project_design/project_design.bd] -no_script -sync -force -quiet
create_ip_run [get_files -of_objects [get_fileset sources_1] /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs/sources_1/bd/project_design/project_design.bd]
launch_runs project_design_processing_system7_0_0_synth_1 project_design_axi_bram_ctrl_0_0_synth_1 project_design_axi_smc_0_synth_1 project_design_rst_ps7_0_100M_0_synth_1 project_design_risc_v_cpu_0_1_synth_1 -jobs 6
export_simulation -of_objects [get_files /home/lazar/Desktop/tests/synth_test_2/synth_test_2.srcs/sources_1/bd/project_design/project_design.bd] -directory /home/lazar/Desktop/tests/synth_test_2/synth_test_2.ip_user_files/sim_scripts -ip_user_files_dir /home/lazar/Desktop/tests/synth_test_2/synth_test_2.ip_user_files -ipstatic_source_dir /home/lazar/Desktop/tests/synth_test_2/synth_test_2.ip_user_files/ipstatic -lib_map_path [list {modelsim=/home/lazar/Desktop/tests/synth_test_2/synth_test_2.cache/compile_simlib/modelsim} {questa=/home/lazar/Desktop/tests/synth_test_2/synth_test_2.cache/compile_simlib/questa} {xcelium=/home/lazar/Desktop/tests/synth_test_2/synth_test_2.cache/compile_simlib/xcelium} {vcs=/home/lazar/Desktop/tests/synth_test_2/synth_test_2.cache/compile_simlib/vcs} {riviera=/home/lazar/Desktop/tests/synth_test_2/synth_test_2.cache/compile_simlib/riviera}] -use_ip_compiled_libs -force -quiet

