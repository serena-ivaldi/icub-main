
IF(NOT AFFACTIONPRIMITIVES_FOUND)

    MESSAGE(STATUS "Using AFFACTIONPRIMITIVESConfig.cmake")
    
    SET(LIB_DIR ${AFFACTIONPRIMITIVES_DIR})
    SET(AFFACTIONPRIMITIVES_INCLUDE_DIRS ${LIB_DIR}/include)
    
    
    IF(NESTED_BUILD)
    
        IF(WIN32 AND NOT CYGWIN)
           SET(AFFACTIONPRIMITIVES_LIBRARIES optimized affActionPrimitives debug affActionPrimitivesd)
        ELSE(WIN32 AND NOT CYGWIN)
           SET(AFFACTIONPRIMITIVES_LIBRARIES affActionPrimitives)
        ENDIF(WIN32 AND NOT CYGWIN)

    ELSE(NESTED_BUILD)
    
      FIND_LIBRARY(AFFACTIONPRIMITIVES_LIBRARIES affActionPrimitives ${ICUB_DIR}/lib ${LIB_DIR})  
    
      IF(NOT AFFACTIONPRIMITIVES_LIBRARIES)
    
        FIND_LIBRARY(AFFACTIONPRIMITIVES_LIBRARIES_RELEASE affActionPrimitives 
    		         ${ICUB_DIR}/lib/Release ${LIB_DIR}/Release NO_DEFAULT_PATH)
        FIND_LIBRARY(AFFACTIONPRIMITIVES_LIBRARIES_DEBUG affActionPrimitivesd 
    		         ${ICUB_DIR}/lib/Debug ${LIB_DIR}/Debug NO_DEFAULT_PATH)
    
        IF(AFFACTIONPRIMITIVES_LIBRARIES_RELEASE AND NOT AFFACTIONPRIMITIVES_LIBRARIES_DEBUG)
           SET(AFFACTIONPRIMITIVES_LIBRARIES ${AFFACTIONPRIMITIVES_LIBRARIES_RELEASE} CACHE PATH "release version of library" FORCE)
        ENDIF(AFFACTIONPRIMITIVES_LIBRARIES_RELEASE AND NOT AFFACTIONPRIMITIVES_LIBRARIES_DEBUG)
    
        IF(AFFACTIONPRIMITIVES_LIBRARIES_DEBUG AND NOT AFFACTIONPRIMITIVES_LIBRARIES_RELEASE)
           SET(AFFACTIONPRIMITIVES_LIBRARIES ${AFFACTIONPRIMITIVES_LIBRARIES_DEBUG} CACHE PATH "debug version of library" FORCE)
        ENDIF(AFFACTIONPRIMITIVES_LIBRARIES_DEBUG AND NOT AFFACTIONPRIMITIVES_LIBRARIES_RELEASE)
    
        IF(AFFACTIONPRIMITIVES_LIBRARIES_DEBUG AND AFFACTIONPRIMITIVES_LIBRARIES_RELEASE)
           SET(AFFACTIONPRIMITIVES_LIBRARIES optimized ${AFFACTIONPRIMITIVES_LIBRARIES_RELEASE}
    			                 debug ${AFFACTIONPRIMITIVES_LIBRARIES_DEBUG} CACHE PATH "debug and release version of library" FORCE)
        ENDIF(AFFACTIONPRIMITIVES_LIBRARIES_DEBUG AND AFFACTIONPRIMITIVES_LIBRARIES_RELEASE)
    
        MARK_AS_ADVANCED(AFFACTIONPRIMITIVES_LIBRARIES_RELEASE)
        MARK_AS_ADVANCED(AFFACTIONPRIMITIVES_LIBRARIES_DEBUG)
    
      ENDIF(NOT AFFACTIONPRIMITIVES_LIBRARIES)
    
    ENDIF(NESTED_BUILD)
    
    
    # Add YARP dependencies
    FIND_PACKAGE(YARP)
    SET(AFFACTIONPRIMITIVES_LIBRARIES ${AFFACTIONPRIMITIVES_LIBRARIES} ${YARP_LIBRARIES})

    # Add CTRLLIB dependencies
    FIND_PACKAGE(CTRLLIB REQUIRED)
    SET(AFFACTIONPRIMITIVES_INCLUDE_DIRS ${AFFACTIONPRIMITIVES_INCLUDE_DIRS} ${CTRLLIB_INCLUDE_DIRS})
    SET(AFFACTIONPRIMITIVES_LIBRARIES ${AFFACTIONPRIMITIVES_LIBRARIES} ${CTRLLIB_LIBRARIES})
    
    
    IF(AFFACTIONPRIMITIVES_INCLUDE_DIRS AND AFFACTIONPRIMITIVES_LIBRARIES)
       SET(AFFACTIONPRIMITIVES_FOUND TRUE)
    ELSE(AFFACTIONPRIMITIVES_INCLUDE_DIRS AND AFFACTIONPRIMITIVES_LIBRARIES)
       SET(AFFACTIONPRIMITIVES_FOUND FALSE)
    ENDIF(AFFACTIONPRIMITIVES_INCLUDE_DIRS AND AFFACTIONPRIMITIVES_LIBRARIES)
    
ENDIF(NOT AFFACTIONPRIMITIVES_FOUND)



