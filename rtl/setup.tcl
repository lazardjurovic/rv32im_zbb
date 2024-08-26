set_property board_part digilentinc.com:zybo-z7-10:part0:1.2 [current_project]
set_property simulator_language Verilog [current_project]
add_files -norecurse -scan_for_includes {/home/lazar/Desktop/y24-g05/rtl/src/alu.v /home/lazar/Desktop/y24-g05/rtl/src/cpop_module.v /home/lazar/Desktop/y24-g05/rtl/src/control_decoder.v /home/lazar/Desktop/y24-g05/rtl/src/control_path.v /home/lazar/Desktop/y24-g05/rtl/src/unsigned_mul.v /home/lazar/Desktop/y24-g05/rtl/src/branch_module.v /home/lazar/Desktop/y24-g05/rtl/src/hazard_unit.v /home/lazar/Desktop/y24-g05/rtl/src/forwarding_unit.v /home/lazar/Desktop/y24-g05/rtl/src/immediate.v /home/lazar/Desktop/y24-g05/rtl/src/signed_mul.v /home/lazar/Desktop/y24-g05/rtl/src/bram.v /home/lazar/Desktop/y24-g05/rtl/src/register_file.v /home/lazar/Desktop/y24-g05/rtl/src/clz_module.v /home/lazar/Desktop/y24-g05/rtl/src/risc_v_cpu_v1_0.v /home/lazar/Desktop/y24-g05/rtl/src/alu_decoder.v /home/lazar/Desktop/y24-g05/rtl/src/data_path.v /home/lazar/Desktop/y24-g05/rtl/src/risc_v_cpu_v1_0_S00_AXI.v /home/lazar/Desktop/y24-g05/rtl/src/top.v /home/lazar/Desktop/y24-g05/rtl/src/cpu.v}
import_files -force -norecurse
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
ipx::package_project -root_dir /home/lazar/Desktop/tests/bitstream_gen_2/bitstream_gen_2.srcs/sources_1/imports -vendor user.org -library user -taxonomy /UserIP
set_property vendor FTN [ipx::current_core]
set_property name risc_v_cpu [ipx::current_core]
set_property display_name risc_v_cpu [ipx::current_core]
set_property description risc_v_cpu_ [ipx::current_core]
ipx::add_bus_interface instr_bram [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:bram_rtl:1.0 [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:bram:1.0 [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
ipx::add_port_map RST [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_reset [ipx::get_port_maps RST -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
ipx::add_port_map CLK [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_clk [ipx::get_port_maps CLK -of_objects [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]]
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
set_property physical_name data_mem_init_reset [ipx::get_port_maps RST -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
ipx::add_port_map CLK [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_clk [ipx::get_port_maps CLK -of_objects [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]]
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
set_property core_revision 2 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]

ipx::save_core [ipx::current_core]
set_property  ip_repo_paths  /home/lazar/Desktop/tests/bitstream_gen_2/bitstream_gen_2.srcs/sources_1/imports [current_project]
update_ip_catalog
create_bd_design "projekat_packed"
update_compile_order -fileset sources_1
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
endgroup
startgroup
create_bd_cell -type ip -vlnv FTN:user:risc_v_cpu:1.0 risc_v_cpu_0
endgroup
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] [get_bd_intf_pins risc_v_cpu_0/instr_bram]
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTB] [get_bd_intf_pins risc_v_cpu_0/data_bram]
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]
startgroup
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_0/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/risc_v_cpu_0/s00_axi} ddr_seg {Auto} intc_ip {New AXI Interconnect} master_apm {0}}  [get_bd_intf_pins risc_v_cpu_0/s00_axi]
Slave segment '/risc_v_cpu_0/s00_axi/reg0' is being assigned into address space '/processing_system7_0/Data' at <0x4000_2000 [ 4K ]>.
endgroup
regenerate_bd_layout
validate_bd_design
startgroup
set_property CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {45} [get_bd_cells processing_system7_0]
endgroup

make_wrapper -files [get_files /home/lazar/Desktop/tests/bitstream_gen_2/bitstream_gen_2.srcs/sources_1/bd/projekat_packed/projekat_packed.bd] -top
add_files -norecurse /home/lazar/Desktop/tests/bitstream_gen_2/bitstream_gen_2.gen/sources_1/bd/projekat_packed/hdl/projekat_packed_wrapper.v
update_compile_order -fileset sources_1
set_property top projekat_packed_wrapper [current_fileset]
update_compile_order -fileset sources_1
reset_run synth_1
launch_runs impl_1 -to_step write_bitstream -jobs 6