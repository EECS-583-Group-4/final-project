# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ngawad/final-project/S-Detector/MASBasedPass

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ngawad/final-project/S-Detector/MASBasedPass/build

# Utility rule file for install-MASPass.

# Include any custom commands dependencies for this target.
include /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/compiler_depend.make

# Include the progress variables for this target.
include /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/progress.make

/home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass:
	cd /home/ngawad/final-project/S-Detector/MAS/build/MASPass && /usr/bin/cmake -DCMAKE_INSTALL_COMPONENT="MASPass" -P /home/ngawad/final-project/S-Detector/MASBasedPass/build/cmake_install.cmake

install-MASPass: /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass
install-MASPass: /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/build.make
.PHONY : install-MASPass

# Rule to build all files generated by this target.
/home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/build: install-MASPass
.PHONY : /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/build

/home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/clean:
	cd /home/ngawad/final-project/S-Detector/MAS/build/MASPass && $(CMAKE_COMMAND) -P CMakeFiles/install-MASPass.dir/cmake_clean.cmake
.PHONY : /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/clean

/home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/depend:
	cd /home/ngawad/final-project/S-Detector/MASBasedPass/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ngawad/final-project/S-Detector/MASBasedPass /home/ngawad/final-project/MAS/MASPass /home/ngawad/final-project/S-Detector/MASBasedPass/build /home/ngawad/final-project/S-Detector/MAS/build/MASPass /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : /home/ngawad/final-project/S-Detector/MAS/build/MASPass/CMakeFiles/install-MASPass.dir/depend

