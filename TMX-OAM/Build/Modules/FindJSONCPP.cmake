# 
# FindJSONCPP - Looks for the JSON CPP library
#
# Note that on Cygwin, this library may be stored in a pre-compiled area
#
 
if (CYGWIN)
  set(INC_HINTS ${TMX_LIBRARIES_DIR}/cygwin/include)
  set(LIB_HINTS HINTS ${TMX_LIBRARIES_DIR}/cygwin/lib)
elseif (ARM_BUILD)
  set(LIB_HINTS HINTS ${ARM_TOOLCHAIN_DIR}/lib)
endif (CYGWIN) 
  
find_path( JSONCPP_INCLUDE_DIR json.h json/json.h HINTS /usr/include/jsoncpp /usr/local/include/jsoncpp ${INC_HINTS} )
find_library( JSONCPP_LIBRARY NAMES jsoncpp ${LIB_HINTS} )

set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY} )
set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JSONCPP DEFAULT_MSG JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)

mark_as_advanced( JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR )
