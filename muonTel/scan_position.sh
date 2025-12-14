#!/bin/bash

# Macro to change automatically the yoffset of the detector, then start multiple run, one for each value of yoffest


values=(-50)
#values=(-55 -45 20 30)	# Set the values (cm)

for y in "${values[@]}"; 
do
	echo "Running simulation with yoffset = $y"

	# create a temporary .mac to start a run with the desired parameters
	cat > tmp.mac <<EOF
/geo/yoffset ${y} cm
/run/initialize
/run/beamOn 10000
EOF

  # Execute 
  ../muonTel-build/Telescope tmp.mac

  # Rename the output file
  mv output.root output_y${y}.root
done
