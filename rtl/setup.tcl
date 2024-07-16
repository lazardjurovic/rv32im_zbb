start_gui
create_project risc-v_cpu ./projects -part xc7z010clg400-1
#set_property board_part digilentinc.com:zybo:part0:2.0

add_files -norecurse src/alu_decoder.v src/alu.v src/bram.v src/branch_module.v src/clz_module.v src/control_decoder.v src/control_path.v src/cpop_module.v src/cpu.v src/data_path.v src/forwarding_unit.v src/hazard_unit.v src/immediate.v src/register_file.v src/signed_mul.v src/top.v src/unsigned_mul.v
add_files -norecurse constraints.xdc

update_compile_order -fileset sources_1
set_property dataflow_viewer_settings "min_width=16"   [current_fileset]
set_property strategy Flow_PerfOptimized_high [get_runs synth_1]
launch_runs synth_1 -jobs 6

set_property strategy Performance_ExplorePostRoutePhysOpt [get_runs impl_1]
launch_runs impl_1 -jobs 6

report_timing_summary -delay_type min_max -report_unconstrained -check_timing_verbose -max_paths 10 -input_pins -routable_nets -name timing_1 -file timing_report.txt


create_project pakovanje ./projects/packing -part xc7z010clg400-1
set_property board_part digilentinc.com:zybo-z7-10:part0:1.2 [current_project]
set_property simulator_language Verilog [current_project]
add_files -norecurse src/alu_decoder.v src/alu.v src/bram.v src/branch_module.v src/clz_module.v src/control_decoder.v src/control_path.v src/cpop_module.v src/cpu.v src/data_path.v src/forwarding_unit.v src/hazard_unit.v src/immediate.v src/register_file.v src/signed_mul.v src/top.v src/unsigned_mul.v
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
add_files -norecurse constraints.xdc
open_project ./projects/packing/packing.xpr
update_compile_order -fileset sources_1
current_project pakovanje
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
update_compile_order -fileset sources_1
create_peripheral user.org user risc_v_cpu 1.0 -dir /home/lazar/Desktop/projekat_pakovanje/ip_repo
add_peripheral_interface S00_AXI -interface_mode slave -axi_type lite [ipx::find_open_core user.org:user:risc_v_cpu:1.0]
generate_peripheral -driver -bfm_example_design -debug_hw_example_design -force [ipx::find_open_core user.org:user:risc_v_cpu:1.0]
write_peripheral [ipx::find_open_core user.org:user:risc_v_cpu:1.0]
set_property  ip_repo_paths  /home/lazar/Desktop/projekat_pakovanje/ip_repo/risc_v_cpu_1_0 [current_project]
update_ip_catalog -rebuild

ipx::merge_project_changes files [ipx::current_core]
ipx::merge_project_changes hdl_parameters [ipx::current_core]
ipx::add_bus_interface instr_mem_init_bram [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:bram_rtl:1.0 [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:bram:1.0 [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property display_name instr_mem_init_bram [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property description {Instriction memory initialization BRAM interface} [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
ipx::add_port_map RST [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name s00_axi_aresetn [ipx::get_port_maps RST -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map CLK [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name s00_axi_aclk [ipx::get_port_maps CLK -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DIN [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_data_in [ipx::get_port_maps DIN -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map EN [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_enable [ipx::get_port_maps EN -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DOUT [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_data_out [ipx::get_port_maps DOUT -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map WE [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_we [ipx::get_port_maps WE -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map ADDR [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name instr_mem_init_addr [ipx::get_port_maps ADDR -of_objects [ipx::get_bus_interfaces instr_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_bus_interface data_mem_init_bram [ipx::current_core]
set_property abstraction_type_vlnv xilinx.com:interface:bram_rtl:1.0 [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property bus_type_vlnv xilinx.com:interface:bram:1.0 [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property display_name data_mem_init_bram [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property description {Data memory initialization BRAM port} [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
ipx::add_port_map RST [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name s00_axi_aresetn [ipx::get_port_maps RST -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map CLK [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name s00_axi_aclk [ipx::get_port_maps CLK -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DIN [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_data_in [ipx::get_port_maps DIN -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map EN [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_enable [ipx::get_port_maps EN -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map DOUT [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_data_out [ipx::get_port_maps DOUT -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map WE [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_we [ipx::get_port_maps WE -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::add_port_map ADDR [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]
set_property physical_name data_mem_init_addr [ipx::get_port_maps ADDR -of_objects [ipx::get_bus_interfaces data_mem_init_bram -of_objects [ipx::current_core]]]
ipx::update_checksums [ipx::current_core]
ipx::save_core [ipx::current_core]
set_property core_revision 2 [ipx::current_core]
ipx::update_source_project_archive -component [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]