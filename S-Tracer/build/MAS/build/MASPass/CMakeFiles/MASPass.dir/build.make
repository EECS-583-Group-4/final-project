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
CMAKE_SOURCE_DIR = /home/ngawad/final-project/S-Tracer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ngawad/final-project/S-Tracer/build

# Include any dependencies generated for this target.
include MAS/build/MASPass/CMakeFiles/MASPass.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include MAS/build/MASPass/CMakeFiles/MASPass.dir/compiler_depend.make

# Include the progress variables for this target.
include MAS/build/MASPass/CMakeFiles/MASPass.dir/progress.make

# Include the compile flags for this target's objects.
include MAS/build/MASPass/CMakeFiles/MASPass.dir/flags.make

MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.o: MAS/build/MASPass/CMakeFiles/MASPass.dir/flags.make
MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.o: /home/ngawad/final-project/MAS/MASPass/MASPass.cpp
MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.o: MAS/build/MASPass/CMakeFiles/MASPass.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ngawad/final-project/S-Tracer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.o"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.o -MF CMakeFiles/MASPass.dir/MASPass.cpp.o.d -o CMakeFiles/MASPass.dir/MASPass.cpp.o -c /home/ngawad/final-project/MAS/MASPass/MASPass.cpp

MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MASPass.dir/MASPass.cpp.i"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ngawad/final-project/MAS/MASPass/MASPass.cpp > CMakeFiles/MASPass.dir/MASPass.cpp.i

MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MASPass.dir/MASPass.cpp.s"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ngawad/final-project/MAS/MASPass/MASPass.cpp -o CMakeFiles/MASPass.dir/MASPass.cpp.s

MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.o: MAS/build/MASPass/CMakeFiles/MASPass.dir/flags.make
MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.o: /home/ngawad/final-project/MAS/MASPass/MAS.cpp
MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.o: MAS/build/MASPass/CMakeFiles/MASPass.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ngawad/final-project/S-Tracer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.o"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.o -MF CMakeFiles/MASPass.dir/MAS.cpp.o.d -o CMakeFiles/MASPass.dir/MAS.cpp.o -c /home/ngawad/final-project/MAS/MASPass/MAS.cpp

MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MASPass.dir/MAS.cpp.i"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ngawad/final-project/MAS/MASPass/MAS.cpp > CMakeFiles/MASPass.dir/MAS.cpp.i

MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MASPass.dir/MAS.cpp.s"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ngawad/final-project/MAS/MASPass/MAS.cpp -o CMakeFiles/MASPass.dir/MAS.cpp.s

# Object files for target MASPass
MASPass_OBJECTS = \
"CMakeFiles/MASPass.dir/MASPass.cpp.o" \
"CMakeFiles/MASPass.dir/MAS.cpp.o"

# External object files for target MASPass
MASPass_EXTERNAL_OBJECTS =

MAS/build/MASPass/MASPass.so: MAS/build/MASPass/CMakeFiles/MASPass.dir/MASPass.cpp.o
MAS/build/MASPass/MASPass.so: MAS/build/MASPass/CMakeFiles/MASPass.dir/MAS.cpp.o
MAS/build/MASPass/MASPass.so: MAS/build/MASPass/CMakeFiles/MASPass.dir/build.make
MAS/build/MASPass/MASPass.so: MAS/build/MASPass/CMakeFiles/MASPass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ngawad/final-project/S-Tracer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX shared module MASPass.so"
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MASPass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
MAS/build/MASPass/CMakeFiles/MASPass.dir/build: MAS/build/MASPass/MASPass.so
.PHONY : MAS/build/MASPass/CMakeFiles/MASPass.dir/build

MAS/build/MASPass/CMakeFiles/MASPass.dir/clean:
	cd /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass && $(CMAKE_COMMAND) -P CMakeFiles/MASPass.dir/cmake_clean.cmake
.PHONY : MAS/build/MASPass/CMakeFiles/MASPass.dir/clean

MAS/build/MASPass/CMakeFiles/MASPass.dir/depend:
	cd /home/ngawad/final-project/S-Tracer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ngawad/final-project/S-Tracer /home/ngawad/final-project/MAS/MASPass /home/ngawad/final-project/S-Tracer/build /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass /home/ngawad/final-project/S-Tracer/build/MAS/build/MASPass/CMakeFiles/MASPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : MAS/build/MASPass/CMakeFiles/MASPass.dir/depend

