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

create_project skripta_pakovanje $dirPath/skripta_pakovanje -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.0 [current_project]
current_project skripta_pakovanje
set_property simulator_language Verilog [current_project]
add_files -norecurse -scan_for_includes {/src/control_path.v /src/register_file.v /src/risc_v_cpu_v1_0.v /src/alu.v /src/cpu.v /src/data_path.v /src/risc_v_cpu_v1_0_S00_AXI.v /src/bram.v /src/alu_decoder.v /src/branch_module.v /src/clz_module.v /src/signed_mul.v /src/control_decoder.v /src/cpop_module.v /src/forwarding_unit.v /src/top.v /src/unsigned_mul.v /src/hazard_unit.v /src/immediate.v}
import_files -force -norecurse
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1

ipx::package_project -root_dir $dirPath/skripta_pakovanje.srcs/sources_1/imports -vendor user.org -library user -taxonomy /UserIP

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
ipx::add_bus_parameter MASTER_TYPE [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
ipx::add_bus_parameter READ_WRITE_MODE [ipx::get_bus_interfaces instr_bram -of_objects [ipx::current_core]]
ipx::add_bus_parameter MASTER_TYPE [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
ipx::add_bus_parameter READ_WRITE_MODE [ipx::get_bus_interfaces data_bram -of_objects [ipx::current_core]]
set_property core_revision 2 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]

ipx::save_core [ipx::current_core]
set_property  ip_repo_paths  $dirPath/skripta_pakovanje/skripta_pakovanje.srcs/sources_1/imports [current_project]
current_project pakovanje_dan_drugi
current_project skripta_pakovanje
update_ip_catalog

create_bd_design "design_1"

update_compile_order -fileset sources_1
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0
endgroup
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
endgroup
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl_0]
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_1

endgroup
set_property -dict [list CONFIG.SINGLE_PORT_BRAM {1}] [get_bd_cells axi_bram_ctrl_1]
startgroup
create_bd_cell -type ip -vlnv FTN:user:risc_v_cpu:1.0 risc_v_cpu_0
endgroup
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] [get_bd_intf_pins risc_v_cpu_0/instr_bram]
connect_bd_intf_net [get_bd_intf_pins axi_bram_ctrl_1/BRAM_PORTA] [get_bd_intf_pins risc_v_cpu_0/data_bram]
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" apply_board_preset "1" Master "Disable" Slave "Disable" }  [get_bd_cells processing_system7_0]
startgroup
apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {Auto} Clk_xbar {Auto} Master {/processing_system7_0/M_AXI_GP0} Slave {/axi_bram_ctrl_0/S_AXI} ddr_seg {Auto} intc_ip {New AXI SmartConnect} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]
endgroup
regenerate_bd_layout
set_property range 16K [get_bd_addr_segs {processing_system7_0/Data/SEG_axi_bram_ctrl_0_Mem0}]
set_property range 16K [get_bd_addr_segs {processing_system7_0/Data/SEG_axi_bram_ctrl_1_Mem0}]
set_property range 1K [get_bd_addr_segs {processing_system7_0/Data/SEG_risc_v_cpu_0_reg0}]
validate_bd_design
startgroup
set_property -dict [list CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {45}] [get_bd_cells processing_system7_0]
endgroup
validate_bd_design
make_wrapper -files [get_files $dirPath/skripta_pakovanje/skripta_pakovanje.srcs/sources_1/bd/design_1/design_1.bd] -top
add_files -norecurse $dirPath/skripta_pakovanje/skripta_pakovanje.gen/sources_1/bd/design_1/hdl/design_1_wrapper.v
update_compile_order -fileset sources_1
set_property top design_1_wrapper [current_fileset]
make_wrapper -files [get_files $dirPath/skripta_pakovanje/skripta_pakovanje.srcs/sources_1/bd/design_1/design_1.bd] -top
update_compile_order -fileset sources_1
launch_runs impl_1 -to_step write_bitstream -jobs 6
write_hw_platform -fixed -include_bit -force -file $dirPath/skripta_pakovanje/design_1_wrapper.xsa