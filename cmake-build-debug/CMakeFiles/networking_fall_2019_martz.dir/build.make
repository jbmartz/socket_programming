# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jmartz/networking_fall_2019_martz

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jmartz/networking_fall_2019_martz/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/networking_fall_2019_martz.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/networking_fall_2019_martz.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/networking_fall_2019_martz.dir/flags.make

CMakeFiles/networking_fall_2019_martz.dir/main.cpp.o: CMakeFiles/networking_fall_2019_martz.dir/flags.make
CMakeFiles/networking_fall_2019_martz.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jmartz/networking_fall_2019_martz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/networking_fall_2019_martz.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/networking_fall_2019_martz.dir/main.cpp.o -c /Users/jmartz/networking_fall_2019_martz/main.cpp

CMakeFiles/networking_fall_2019_martz.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/networking_fall_2019_martz.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jmartz/networking_fall_2019_martz/main.cpp > CMakeFiles/networking_fall_2019_martz.dir/main.cpp.i

CMakeFiles/networking_fall_2019_martz.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/networking_fall_2019_martz.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jmartz/networking_fall_2019_martz/main.cpp -o CMakeFiles/networking_fall_2019_martz.dir/main.cpp.s

# Object files for target networking_fall_2019_martz
networking_fall_2019_martz_OBJECTS = \
"CMakeFiles/networking_fall_2019_martz.dir/main.cpp.o"

# External object files for target networking_fall_2019_martz
networking_fall_2019_martz_EXTERNAL_OBJECTS =

networking_fall_2019_martz: CMakeFiles/networking_fall_2019_martz.dir/main.cpp.o
networking_fall_2019_martz: CMakeFiles/networking_fall_2019_martz.dir/build.make
networking_fall_2019_martz: CMakeFiles/networking_fall_2019_martz.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jmartz/networking_fall_2019_martz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable networking_fall_2019_martz"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/networking_fall_2019_martz.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/networking_fall_2019_martz.dir/build: networking_fall_2019_martz

.PHONY : CMakeFiles/networking_fall_2019_martz.dir/build

CMakeFiles/networking_fall_2019_martz.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/networking_fall_2019_martz.dir/cmake_clean.cmake
.PHONY : CMakeFiles/networking_fall_2019_martz.dir/clean

CMakeFiles/networking_fall_2019_martz.dir/depend:
	cd /Users/jmartz/networking_fall_2019_martz/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jmartz/networking_fall_2019_martz /Users/jmartz/networking_fall_2019_martz /Users/jmartz/networking_fall_2019_martz/cmake-build-debug /Users/jmartz/networking_fall_2019_martz/cmake-build-debug /Users/jmartz/networking_fall_2019_martz/cmake-build-debug/CMakeFiles/networking_fall_2019_martz.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/networking_fall_2019_martz.dir/depend

