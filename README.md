# VacuumRobot
Project for KSCR. Controller and simulation. 

# Requirements
- cmake
- libjson-c-dev - sudo apt install libjson-c-dev
- python3
- pygame - pip install pygame

# Build project
. ./build_script.sh

# Run visualization script
cd scripts/
python3 Threading_main.py

# Run vacuum robot app (press ENTER to exit)
cd ./build/
sudo ./vacuum_robot