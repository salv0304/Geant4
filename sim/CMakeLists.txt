# Setup the project
cmake_minimum_required(VERSION 3.16...3.27 FATAL_ERROR) # Required version
project(Simulation)		# Title of the project

# Find packages
find_package(Geant4 REQUIRED ui_all vis_all)

# Geant4 include directories
include(${Geant4_USE_FILE})

# Compile using all sources files and including all headers
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Create the executable (name main code sources headers)
add_executable(sim sim.cc ${sources} ${headers})
target_include_directories(sim PRIVATE ${PROJECT_SOURCE_DIR}/include)	#add your directories
target_link_libraries(sim ${Geant4_LIBRARIES})	# You need to link the geant4 libraries

add_custom_target(Simulation DEPENDS sim)

