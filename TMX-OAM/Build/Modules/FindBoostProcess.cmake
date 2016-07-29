# 
# FindBoostProcess - Looks for the JSON CPP library
#
# Note that this library may be stored in a pre-configured area
#
 
find_path( BoostProcess_INCLUDE_DIR boost/process.hpp HINTS ${TMX_LIBRARIES_DIR} )

set(BoostProcess_INCLUDE_DIRS ${BoostProcess_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BoostProcess DEFAULT_MSG BoostProcess_INCLUDE_DIR)

mark_as_advanced( BoostProcess_INCLUDE_DIR )
