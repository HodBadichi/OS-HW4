# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/student/Desktop/OS-HW4/WET

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/student/Desktop/OS-HW4/WET/build

# Include any dependencies generated for this target.
include CMakeFiles/malloc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/malloc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/malloc.dir/flags.make

CMakeFiles/malloc.dir/test3.cpp.o: CMakeFiles/malloc.dir/flags.make
CMakeFiles/malloc.dir/test3.cpp.o: ../test3.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/student/Desktop/OS-HW4/WET/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/malloc.dir/test3.cpp.o"
	/usr/bin/x86_64-linux-gnu-g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/malloc.dir/test3.cpp.o -c /home/student/Desktop/OS-HW4/WET/test3.cpp

CMakeFiles/malloc.dir/test3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/malloc.dir/test3.cpp.i"
	/usr/bin/x86_64-linux-gnu-g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/student/Desktop/OS-HW4/WET/test3.cpp > CMakeFiles/malloc.dir/test3.cpp.i

CMakeFiles/malloc.dir/test3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/malloc.dir/test3.cpp.s"
	/usr/bin/x86_64-linux-gnu-g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/student/Desktop/OS-HW4/WET/test3.cpp -o CMakeFiles/malloc.dir/test3.cpp.s

CMakeFiles/malloc.dir/test3.cpp.o.requires:

.PHONY : CMakeFiles/malloc.dir/test3.cpp.o.requires

CMakeFiles/malloc.dir/test3.cpp.o.provides: CMakeFiles/malloc.dir/test3.cpp.o.requires
	$(MAKE) -f CMakeFiles/malloc.dir/build.make CMakeFiles/malloc.dir/test3.cpp.o.provides.build
.PHONY : CMakeFiles/malloc.dir/test3.cpp.o.provides

CMakeFiles/malloc.dir/test3.cpp.o.provides.build: CMakeFiles/malloc.dir/test3.cpp.o


# Object files for target malloc
malloc_OBJECTS = \
"CMakeFiles/malloc.dir/test3.cpp.o"

# External object files for target malloc
malloc_EXTERNAL_OBJECTS =

malloc: CMakeFiles/malloc.dir/test3.cpp.o
malloc: CMakeFiles/malloc.dir/build.make
malloc: CMakeFiles/malloc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/student/Desktop/OS-HW4/WET/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable malloc"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/malloc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/malloc.dir/build: malloc

.PHONY : CMakeFiles/malloc.dir/build

CMakeFiles/malloc.dir/requires: CMakeFiles/malloc.dir/test3.cpp.o.requires

.PHONY : CMakeFiles/malloc.dir/requires

CMakeFiles/malloc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/malloc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/malloc.dir/clean

CMakeFiles/malloc.dir/depend:
	cd /home/student/Desktop/OS-HW4/WET/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/student/Desktop/OS-HW4/WET /home/student/Desktop/OS-HW4/WET /home/student/Desktop/OS-HW4/WET/build /home/student/Desktop/OS-HW4/WET/build /home/student/Desktop/OS-HW4/WET/build/CMakeFiles/malloc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/malloc.dir/depend

