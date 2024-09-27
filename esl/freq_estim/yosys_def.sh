yosys -p "synth_ecp5 -json hardware.json" execute_phase.v
nextpnr-ecp5 --12k --lpf-allow-unconstrained  --package CABGA381 --json hardware.json --textcfg hardware.config --lpf ulx3s.lpf