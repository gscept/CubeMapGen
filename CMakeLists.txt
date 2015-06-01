#-------------------------------------------------------------------------------
# CubeMapGen
#-------------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(CubeMapGen)

SET(SRC CBBoxInt32.cpp
		 CCubeMapProcessor.cpp
		 CImageSurface.cpp
		 ErrorMsg.cpp)
		 
SET(HEAD CBBoxInt32.h
		  CCubeMapProcessor.h
		  CImageSurface.h
		  ErrorMsg.h
		  Types.h
		  VectorMacros.h)
		  
		  
IF(WIN32)
    set(CMAKE_CXX_FLAGS /EHsc)
ELSE()
    SET(CMAKE_CXX_FLAGS -std=c++11)
ENDIF()
SOURCE_GROUP("CubeMapGen" FILES ${SRC} ${HEAD})
LIST(APPEND CODE ${SRC} ${HEAD})

ADD_LIBRARY(CubeMapGen STATIC ${CODE})
SET_TARGET_PROPERTIES(CubeMapGen PROPERTIES FOLDER "extlibs")