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
CMAKE_SOURCE_DIR = /home/stanislav/llvm-course/llvm-pass/pass

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stanislav/llvm-course/llvm-pass/pass

# Include any dependencies generated for this target.
include static/CMakeFiles/VisualDumpPass.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include static/CMakeFiles/VisualDumpPass.dir/compiler_depend.make

# Include the progress variables for this target.
include static/CMakeFiles/VisualDumpPass.dir/progress.make

# Include the compile flags for this target's objects.
include static/CMakeFiles/VisualDumpPass.dir/flags.make

static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o: static/CMakeFiles/VisualDumpPass.dir/flags.make
static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o: static/visual_dump.cpp
static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o: static/CMakeFiles/VisualDumpPass.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/stanislav/llvm-course/llvm-pass/pass/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o"
	cd /home/stanislav/llvm-course/llvm-pass/pass/static && /usr/bin/clang++-14 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o -MF CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o.d -o CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o -c /home/stanislav/llvm-course/llvm-pass/pass/static/visual_dump.cpp

static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.i"
	cd /home/stanislav/llvm-course/llvm-pass/pass/static && /usr/bin/clang++-14 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stanislav/llvm-course/llvm-pass/pass/static/visual_dump.cpp > CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.i

static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.s"
	cd /home/stanislav/llvm-course/llvm-pass/pass/static && /usr/bin/clang++-14 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stanislav/llvm-course/llvm-pass/pass/static/visual_dump.cpp -o CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.s

# Object files for target VisualDumpPass
VisualDumpPass_OBJECTS = \
"CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o"

# External object files for target VisualDumpPass
VisualDumpPass_EXTERNAL_OBJECTS =

static/libVisualDumpPass.so: static/CMakeFiles/VisualDumpPass.dir/visual_dump.cpp.o
static/libVisualDumpPass.so: static/CMakeFiles/VisualDumpPass.dir/build.make
static/libVisualDumpPass.so: static/CMakeFiles/VisualDumpPass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/stanislav/llvm-course/llvm-pass/pass/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module libVisualDumpPass.so"
	cd /home/stanislav/llvm-course/llvm-pass/pass/static && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VisualDumpPass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
static/CMakeFiles/VisualDumpPass.dir/build: static/libVisualDumpPass.so
.PHONY : static/CMakeFiles/VisualDumpPass.dir/build

static/CMakeFiles/VisualDumpPass.dir/clean:
	cd /home/stanislav/llvm-course/llvm-pass/pass/static && $(CMAKE_COMMAND) -P CMakeFiles/VisualDumpPass.dir/cmake_clean.cmake
.PHONY : static/CMakeFiles/VisualDumpPass.dir/clean

static/CMakeFiles/VisualDumpPass.dir/depend:
	cd /home/stanislav/llvm-course/llvm-pass/pass && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stanislav/llvm-course/llvm-pass/pass /home/stanislav/llvm-course/llvm-pass/pass/static /home/stanislav/llvm-course/llvm-pass/pass /home/stanislav/llvm-course/llvm-pass/pass/static /home/stanislav/llvm-course/llvm-pass/pass/static/CMakeFiles/VisualDumpPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : static/CMakeFiles/VisualDumpPass.dir/depend

