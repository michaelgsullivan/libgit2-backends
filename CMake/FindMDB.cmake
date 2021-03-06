# - find MDB
# MDB_INCLUDE_DIR - Where to find Sqlite 3 header files (directory)
# MDB_LIBRARIES - Sqlite 3 libraries
# MDB_LIBRARY_RELEASE - Where the release library is
# MDB_LIBRARY_DEBUG - Where the debug library is
# MDB_FOUND - Set to TRUE if we found everything (library, includes and executable)

# Copyright (c) 2010 Pau Garcia i Quiles, <pgquiles@elpauer.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# Generated by CModuler, a CMake Module Generator - http://gitorious.org/cmoduler

IF( MDB_INCLUDE_DIR AND MDB_LIBRARY_RELEASE AND MDB_LIBRARY_DEBUG )
    SET(MDB_FIND_QUIETLY TRUE)
ENDIF( MDB_INCLUDE_DIR AND MDB_LIBRARY_RELEASE AND MDB_LIBRARY_DEBUG )

FIND_PATH( MDB_INCLUDE_DIR lmdb.h  )

FIND_LIBRARY(MDB_LIBRARY_RELEASE NAMES lmdb )

FIND_LIBRARY(MDB_LIBRARY_DEBUG NAMES lmdb lmdbd  HINTS /usr/lib/debug/usr/lib/ )

IF( MDB_LIBRARY_RELEASE OR MDB_LIBRARY_DEBUG AND MDB_INCLUDE_DIR )
	SET( MDB_FOUND TRUE )
ENDIF( MDB_LIBRARY_RELEASE OR MDB_LIBRARY_DEBUG AND MDB_INCLUDE_DIR )

IF( MDB_LIBRARY_DEBUG AND MDB_LIBRARY_RELEASE )
	# if the generator supports configuration types then set
	# optimized and debug libraries, or if the CMAKE_BUILD_TYPE has a value
	IF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
		SET( MDB_LIBRARIES optimized ${MDB_LIBRARY_RELEASE} debug ${MDB_LIBRARY_DEBUG} )
	ELSE( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
    # if there are no configuration types and CMAKE_BUILD_TYPE has no value
    # then just use the release libraries
		SET( MDB_LIBRARIES ${MDB_LIBRARY_RELEASE} )
	ENDIF( CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE )
ELSEIF( MDB_LIBRARY_RELEASE )
	SET( MDB_LIBRARIES ${MDB_LIBRARY_RELEASE} )
ELSE( MDB_LIBRARY_DEBUG AND MDB_LIBRARY_RELEASE )
	SET( MDB_LIBRARIES ${MDB_LIBRARY_DEBUG} )
ENDIF( MDB_LIBRARY_DEBUG AND MDB_LIBRARY_RELEASE )

IF( MDB_FOUND )
	IF( NOT MDB_FIND_QUIETLY )
		MESSAGE( STATUS "Found mdb header file in ${MDB_INCLUDE_DIR}")
		MESSAGE( STATUS "Found mdb libraries: ${MDB_LIBRARIES}")
	ENDIF( NOT MDB_FIND_QUIETLY )
ELSE(MDB_FOUND)
	IF( MDB_FIND_REQUIRED)
		MESSAGE( FATAL_ERROR "Could not find mdb" )
	ELSE( MDB_FIND_REQUIRED)
		MESSAGE( STATUS "Optional package mdb was not found" )
	ENDIF( MDB_FIND_REQUIRED)
ENDIF(MDB_FOUND)
