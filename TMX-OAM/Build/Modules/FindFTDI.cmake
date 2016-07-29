# 
# FindFTDI - Looks for the OpenOCD FTDI library and dependent USB library
#
# Note that on Cygwin, this library may be stored in a pre-compiled area
#
 
if (CYGWIN)
  set(INC_HINTS HINTS ${TMX_LIBRARIES_DIR}/cygwin/include)
  set(LIB_HINTS HINTS ${TMX_LIBRARIES_DIR}/cygwin/lib)
elseif (ARM_BUILD)
  set(LIB_HINTS HINTS ${ARM_TOOLCHAIN_DIR}/lib)
endif (CYGWIN) 
  
find_path( USB_INCLUDE_DIR usb.h ${INC_HINTS} )
find_library( USB_LIBRARY NAMES usb ${LIB_HINTS} )

set(USB_LIBRARIES ${USB_LIBRARY} )
set(USB_INCLUDE_DIRS ${USB_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(USB DEFAULT_MSG USB_INCLUDE_DIR USB_LIBRARY)

find_path( FTDI_INCLUDE_DIR ftdi.h ${CYGWIN_INC_HINTS} )
find_library( FTDI_LIBRARY NAMES ftdi ${CYGWIN_LIB_HINTS} )

set(FTDI_LIBRARIES ${FTDI_LIBRARY} )
set(FTDI_INCLUDE_DIRS ${FTDI_INCLUDE_DIR} )

find_package_handle_standard_args(FTDI DEFAULT_MSG FTDI_INCLUDE_DIR FTDI_LIBRARY)

mark_as_advanced( USB_LIBRARY USB_INCLUDE_DIR FTDI_LIBRARY FTDI_INCLUDE_DIR )
