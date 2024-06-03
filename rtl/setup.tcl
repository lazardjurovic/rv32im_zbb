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