set project_name "Tcl test"
set verilog_files {"/home/lazar/Desktop/tcl_test/alu.v"}
set xdc_files {"/home/lazar/Desktop/tcl_test/constraints.xdc"}

start_gui
create_project tcl_test /home/lazar/Desktop/tcl_test/projects -part xc7z010clg400-1
#set_property board_part digilentinc.com:zybo:part0:2.0

add_files -norecurse /home/lazar/Desktop/tcl_test/alu.v
add_files -norecurse /home/lazar/Desktop/tcl_test/constraints.xdc

update_compile_order -fileset sources_1
set_property dataflow_viewer_settings "min_width=16"   [current_fileset]
set_property strategy Flow_PerfOptimized_high [get_runs synth_1]
set_property -name {STEPS.SYNTH_DESIGN.ARGS.MORE OPTIONS} -value {-mode out_of_context} -objects [get_runs synth_1]
launch_runs synth_1 -jobs 6

set_property strategy Performance_Explore [get_runs impl_1]
launch_runs impl_1 -jobs 6

open_run impl_1

