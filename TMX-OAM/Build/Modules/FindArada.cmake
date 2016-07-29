# 
# FindArada - Looks for the Arada libraries Wave, WaveEncDec and Bluetooth
#
# Note that this library may be stored in a pre-compiled area
#
 
set(INC_HINTS ${MIPS_TOOLCHAIN_DIR}/incs)
set(LIB_HINTS ${MIPS_TOOLCHAIN_DIR}/lib)

message(STATUS "Searching under ${INC_HINTS}")
find_path( ARADA_INCLUDE_DIR wave.h HINTS ${INC_HINTS} )
find_library( ARADA_WAVE_LIBRARY NAMES wave HINTS ${LIB_HINTS} NO_DEFAULT_PATH )
find_library( ARADA_WAVEENCDEC_LIBRARY NAMES wave-encdec HINTS ${LIB_HINTS} NO_DEFAULT_PATH )
find_library( ARADA_BLUETOOTH_LIBRARY NAMES bluetooth HINTS ${LIB_HINTS} NO_DEFAULT_PATH )

set(ARADA_INCLUDE_DIRS ${ARADA_INCLUDE_DIR} )
set(ARADA_LIBRARIES ${ARADA_WAVE_LIBRARY} ${ARADA_WAVEENCDEC_LIBRARY} ${ARADA_BLUETOOTH_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ARADA DEFAULT_MSG ARADA_INCLUDE_DIR ARADA_LIBRARIES)

include_directories( ${ARADA_INCLUDE_DIR} )
mark_as_advanced( ARADA_INCLUDE_DIR ARADA_WAVE_LIBRARY ARADA_WAVEENCDEC_LIBRARY ARADA_BLUETOOTH_LIBRARY )
