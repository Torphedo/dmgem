cmake_minimum_required(VERSION 3.16) # Latest version on apt, and nothing newer is needed.
project(dmgem
	VERSION 0.0.1
	LANGUAGES C
)

set(CMAKE_C_STANDARD 99)

# Using this setup to run other CMakeLists.txt build scripts makes it
# easier to add unit tests or other separate scripts in the future.
add_subdirectory(external/arena_allocator)
add_subdirectory(src)

# Copy test ROMs to builder folder if present.
if (EXISTS "bin/")
    file(COPY "bin/" DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/src/bin)
endif()

