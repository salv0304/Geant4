#!/bin/bash

# Macro to change automatically the theta angle of the detector, then start multiple run, one for each value of the angle
values=(0 10 20 30 40 50 60 70 80)	# Set the values (deg)

for y in "${values[@]}"; 
do
	echo "Running simulation with rotation of angle = $y deg"
	
		# create a temporary .mac to start a run with the desired parameters
	cat > tmp.mac <<EOF
/geo/angle ${y} deg
/run/initialize
/run/beamOn 100000
EOF

  # Execute 
  ../muonTel-build/Telescope tmp.mac

  # Rename the output file
  mv output.root output_theta${y}.root
done
