# List of tests to run
set test_list {cpu_test sort_test zbb_test mul_test}

# Iterate over each test and run it
foreach test $test_list {
    # Set the coverage database name for each test
    set db_name "covdb_$test"
   
    # Configure the XSIM command with the UVM test name and other options
    set xsim_command "set_property -name \{xsim.simulate.xsim.more_options\} -value \{-testplusarg UVM_TESTNAME=$test -testplusarg UVM_VERBOSITY=UVM_LOW -runall -cov_db_name $db_name\} -objects \[get_filesets sim_1\]"

    # Execute the XSIM command to set up the test environment
    eval $xsim_command
   
    # Launch the simulation
    puts "Running simulation for $test..."
    launch_simulation
   
    # Run all steps in the simulation
    run all
   
    # Close the simulation if there are more tests to run
    if {$test ne [lindex $test_list end]} {
        close_sim
    }
}

exec xcrg -report_format html -dir tmp/verif_skripta_test/verif_skripta_test.sim/sim_1/behav/xsim/ -report_dir tmp/