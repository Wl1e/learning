# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/wlle/files/codes/learning

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wlle/files/codes/learning/build

# Include any dependencies generated for this target.
include learning/net/http/CMakeFiles/learning_http.dir/depend.make

# Include the progress variables for this target.
include learning/net/http/CMakeFiles/learning_http.dir/progress.make

# Include the compile flags for this target's objects.
include learning/net/http/CMakeFiles/learning_http.dir/flags.make

learning/net/http/CMakeFiles/learning_http.dir/httpRequest.cpp.o: learning/net/http/CMakeFiles/learning_http.dir/flags.make
learning/net/http/CMakeFiles/learning_http.dir/httpRequest.cpp.o: ../learning/net/http/httpRequest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wlle/files/codes/learning/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object learning/net/http/CMakeFiles/learning_http.dir/httpRequest.cpp.o"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/learning_http.dir/httpRequest.cpp.o -c /home/wlle/files/codes/learning/learning/net/http/httpRequest.cpp

learning/net/http/CMakeFiles/learning_http.dir/httpRequest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/learning_http.dir/httpRequest.cpp.i"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wlle/files/codes/learning/learning/net/http/httpRequest.cpp > CMakeFiles/learning_http.dir/httpRequest.cpp.i

learning/net/http/CMakeFiles/learning_http.dir/httpRequest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/learning_http.dir/httpRequest.cpp.s"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wlle/files/codes/learning/learning/net/http/httpRequest.cpp -o CMakeFiles/learning_http.dir/httpRequest.cpp.s

learning/net/http/CMakeFiles/learning_http.dir/httpResponse.cpp.o: learning/net/http/CMakeFiles/learning_http.dir/flags.make
learning/net/http/CMakeFiles/learning_http.dir/httpResponse.cpp.o: ../learning/net/http/httpResponse.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wlle/files/codes/learning/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object learning/net/http/CMakeFiles/learning_http.dir/httpResponse.cpp.o"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/learning_http.dir/httpResponse.cpp.o -c /home/wlle/files/codes/learning/learning/net/http/httpResponse.cpp

learning/net/http/CMakeFiles/learning_http.dir/httpResponse.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/learning_http.dir/httpResponse.cpp.i"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wlle/files/codes/learning/learning/net/http/httpResponse.cpp > CMakeFiles/learning_http.dir/httpResponse.cpp.i

learning/net/http/CMakeFiles/learning_http.dir/httpResponse.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/learning_http.dir/httpResponse.cpp.s"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wlle/files/codes/learning/learning/net/http/httpResponse.cpp -o CMakeFiles/learning_http.dir/httpResponse.cpp.s

learning/net/http/CMakeFiles/learning_http.dir/httpServer.cpp.o: learning/net/http/CMakeFiles/learning_http.dir/flags.make
learning/net/http/CMakeFiles/learning_http.dir/httpServer.cpp.o: ../learning/net/http/httpServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/wlle/files/codes/learning/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object learning/net/http/CMakeFiles/learning_http.dir/httpServer.cpp.o"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/learning_http.dir/httpServer.cpp.o -c /home/wlle/files/codes/learning/learning/net/http/httpServer.cpp

learning/net/http/CMakeFiles/learning_http.dir/httpServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/learning_http.dir/httpServer.cpp.i"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wlle/files/codes/learning/learning/net/http/httpServer.cpp > CMakeFiles/learning_http.dir/httpServer.cpp.i

learning/net/http/CMakeFiles/learning_http.dir/httpServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/learning_http.dir/httpServer.cpp.s"
	cd /home/wlle/files/codes/learning/build/learning/net/http && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wlle/files/codes/learning/learning/net/http/httpServer.cpp -o CMakeFiles/learning_http.dir/httpServer.cpp.s

# Object files for target learning_http
learning_http_OBJECTS = \
"CMakeFiles/learning_http.dir/httpRequest.cpp.o" \
"CMakeFiles/learning_http.dir/httpResponse.cpp.o" \
"CMakeFiles/learning_http.dir/httpServer.cpp.o"

# External object files for target learning_http
learning_http_EXTERNAL_OBJECTS =

lib/liblearning_http.so: learning/net/http/CMakeFiles/learning_http.dir/httpRequest.cpp.o
lib/liblearning_http.so: learning/net/http/CMakeFiles/learning_http.dir/httpResponse.cpp.o
lib/liblearning_http.so: learning/net/http/CMakeFiles/learning_http.dir/httpServer.cpp.o
lib/liblearning_http.so: learning/net/http/CMakeFiles/learning_http.dir/build.make
lib/liblearning_http.so: lib/liblearning_net.so
lib/liblearning_http.so: lib/liblearning_base.so
lib/liblearning_http.so: learning/net/http/CMakeFiles/learning_http.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/wlle/files/codes/learning/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX shared library ../../../lib/liblearning_http.so"
	cd /home/wlle/files/codes/learning/build/learning/net/http && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/learning_http.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
learning/net/http/CMakeFiles/learning_http.dir/build: lib/liblearning_http.so

.PHONY : learning/net/http/CMakeFiles/learning_http.dir/build

learning/net/http/CMakeFiles/learning_http.dir/clean:
	cd /home/wlle/files/codes/learning/build/learning/net/http && $(CMAKE_COMMAND) -P CMakeFiles/learning_http.dir/cmake_clean.cmake
.PHONY : learning/net/http/CMakeFiles/learning_http.dir/clean

learning/net/http/CMakeFiles/learning_http.dir/depend:
	cd /home/wlle/files/codes/learning/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wlle/files/codes/learning /home/wlle/files/codes/learning/learning/net/http /home/wlle/files/codes/learning/build /home/wlle/files/codes/learning/build/learning/net/http /home/wlle/files/codes/learning/build/learning/net/http/CMakeFiles/learning_http.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : learning/net/http/CMakeFiles/learning_http.dir/depend

