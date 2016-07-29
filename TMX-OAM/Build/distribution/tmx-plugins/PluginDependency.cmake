IF (REQUIRE_XERCES_C)
    # Find include directory
    FIND_PATH(XercesC_INCLUDE_DIR
              NAMES "xercesc/util/PlatformUtils.hpp"
              DOC "Xerces-C++ include directory")
    MARK_AS_ADVANCED(XercesC_INCLUDE_DIR)
    
    # Find all XercesC libraries
    FIND_LIBRARY(XercesC_LIBRARY "xerces-c"
      DOC "Xerces-C++ libraries")
    MARK_AS_ADVANCED(XercesC_LIBRARY)
ENDIF ()

IF (REQUIRE_SNMP)
    FIND_PATH(NETSNMP_INCLUDE_DIR snmp.h
      /usr/include/net-snmp/library
    )
    
    SET(NETSNMP_NAMES netsnmp)
    FIND_LIBRARY(NETSNMP_LIBRARY
      NAMES ${NETSNMP_NAMES}
      PATHS ${LIB_HINTS} /usr/lib /usr/local/lib
    )
    
    SET(NETSNMPAGENT_NAMES netsnmpagent)
    FIND_LIBRARY(NETSNMPAGENT_LIBRARY
      NAMES ${NETSNMPAGENT_NAMES}
      PATHS ${LIB_HINTS} /usr/lib /usr/local/lib
    )
    
    SET(NETSNMPHELPERS_NAMES netsnmphelpers)
    FIND_LIBRARY(NETSNMPHELPERS_LIBRARY
      NAMES ${NETSNMPHELPERS_NAMES}
      PATHS ${LIB_HINTS} /usr/lib /usr/local/lib
    )
    
    SET(NETSNMPMIBS_NAMES netsnmpmibs)
    FIND_LIBRARY(NETSNMPMIBS_LIBRARY
      NAMES ${NETSNMPMIBS_NAMES}
      PATHS ${LIB_HINTS} /usr/lib /usr/local/lib
    )
    
    SET(NETSNMPTRAPD_NAMES netsnmptrapd)
    FIND_LIBRARY(NETSNMPTRAPD_LIBRARY
      NAMES ${NETSNMPTRAPD_NAMES}
      PATHS ${LIB_HINTS} /usr/lib /usr/local/lib
    )
    
    SET(NETSNMP_LIBRARIES
      ${NETSNMP_LIBRARY}
      ${NETSNMPAGENT_LIBRARY}
      ${NETSNMPHELPERS_LIBRARY}
      ${NETSNMPMIBS_LIBRARY}
    #  ${NETSNMPTRAPD_LIBRARY}
    )
ENDIF ()

IF (REQUIRE_CURL)
    FIND_PACKAGE(CURL REQUIRED)
ENDIF ()
