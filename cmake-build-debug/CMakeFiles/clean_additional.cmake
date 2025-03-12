# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/lidar_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/lidar_autogen.dir/ParseCache.txt"
  "lidar_autogen"
  )
endif()
