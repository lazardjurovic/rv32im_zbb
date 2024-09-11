#!/bin/bash

# Clean previous runs
rm -rf xsim.dir xelab.* webtalk* xsim.log *.wdb *.wcfg

# Run each test sequentially
echo "Running cpu_test..."
xsim cpu_test -R -log cpu_test.log
if [ $? -ne 0 ]; then
    echo "cpu_test - FAILED"
    exit 1
fi

echo "Running sort_test..."
xsim sort_test -R -log sort_test.log
if [ $? -ne 0 ]; then
    echo "sort_test - FAILED"
    exit 1
fi

echo "Running zbb_test..."
xsim zbb_test -R -log zbb_test.log
if [ $? -ne 0 ]; then
    echo "zbb_test - FAILED"
    exit 1
fi

echo "Running mul_test..."
xsim mul_test -R -log mul_test.log
if [ $? -ne 0 ]; then
    echo "mul_test - FAILED"
    exit 1
fi

echo "All tests PASSED!"
