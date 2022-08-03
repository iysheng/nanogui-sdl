# Locate FFmpeg library
# This module defines
# FFmpeg_LIBRARY, the name of the library to link against
# FFmpeg_FOUND, if false, do not try to link to FFmpeg
# FFmpeg_INCLUDE_DIR, where to find SDL.h
#
# This module responds to the the flag:
# FFmpeg_BUILDING_LIBRARY
# If this is defined, then no FFmpegmain will be linked in because
# only applications need main().
# Otherwise, it is assumed you are building an application and this
# module will attempt to locate and set the the proper link flags
# as part of the returned FFmpeg_LIBRARY variable.
#
# Don't forget to include SDLmain.h and SDLmain.m your project for the
# OS X framework based version. (Other versions link to -lFFmpegmain which
# this module will try to find on your behalf.) Also for OS X, this
# module will automatically add the -framework Cocoa on your behalf.
#
#
# Additional Note: If you see an empty FFmpeg_LIBRARY_TEMP in your configuration
# and no FFmpeg_LIBRARY, it means CMake did not find your FFmpeg library
# (FFmpeg.dll, libsdl2.so, FFmpeg.framework, etc).
# Set FFmpeg_LIBRARY_TEMP to point to your FFmpeg library, and configure again.
# Similarly, if you see an empty FFmpegMAIN_LIBRARY, you should set this value
# as appropriate. These values are used to generate the final FFmpeg_LIBRARY
# variable, but when these values are unset, FFmpeg_LIBRARY does not get created.
#
#
# $FFmpegDIR is an environment variable that would
# correspond to the ./configure --prefix=$FFmpegDIR
# used in building FFmpeg.
# l.e.galup  9-20-02
#
# Modified by Eric Wing.
# Added code to assist with automated building by using environmental variables
# and providing a more controlled/consistent search behavior.
# Added new modifications to recognize OS X frameworks and
# additional Unix paths (FreeBSD, etc).
# Also corrected the header search path to follow "proper" SDL guidelines.
# Added a search for FFmpegmain which is needed by some platforms.
# Added a search for threads which is needed by some platforms.
# Added needed compile switches for MinGW.
#
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of
# FFmpeg_LIBRARY to override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.
#
# Note that the header path has changed from FFmpeg/SDL.h to just SDL.h
# This needed to change because "proper" SDL convention
# is #include "SDL.h", not <FFmpeg/SDL.h>. This is done for portability
# reasons because not all systems place things in FFmpeg/ (see FreeBSD).

#=============================================================================
# Copyright 2003-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

SET(FFmpeg_SEARCH_PATHS
	/opt/red_aarch64/ffmpeg
)

FIND_PATH(FFmpeg_INCLUDE_DIR libavcodec/avcodec.h
	HINTS
	$ENV{FFmpegDIR}
	PATH_SUFFIXES include
	PATHS ${FFmpeg_SEARCH_PATHS}
)

FIND_LIBRARY(FFmpeg_LIBRARY_TEMP
	NAMES avcodec avformat avutil swscale
	HINTS
	$ENV{FFmpegDIR}
	PATH_SUFFIXES lib/ 
	PATHS ${FFmpeg_SEARCH_PATHS}
	NO_CACHE
	NO_DEFAULT_PATH
)

IF(NOT FFmpeg_BUILDING_LIBRARY)
	IF(NOT ${FFmpeg_INCLUDE_DIR} MATCHES ".framework")
		# Non-OS X framework versions expect you to also dynamically link to
		# FFmpegmain. This is mainly for Windows and OS X. Other (Unix) platforms
		# seem to provide FFmpegmain for compatibility even though they don't
		# necessarily need it.
		FIND_LIBRARY(FFmpegMAIN_LIBRARY
			NAMES FFmpegmain
			HINTS
			$ENV{FFmpegDIR}
			PATH_SUFFIXES lib/x64 lib/x86
			PATHS ${FFmpeg_SEARCH_PATHS}
		)
	ENDIF(NOT ${FFmpeg_INCLUDE_DIR} MATCHES ".framework")
ENDIF(NOT FFmpeg_BUILDING_LIBRARY)

# FFmpeg may require threads on your system.
# The Apple build may not need an explicit flag because one of the
# frameworks may already provide it.
# But for non-OSX systems, I will use the CMake Threads package.
IF(NOT APPLE)
	FIND_PACKAGE(Threads)
ENDIF(NOT APPLE)

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lFFmpegmain -lFFmpeg -lmwindows
# (Actually on second look, I think it only needs one of the m* libraries.)
IF(MINGW)
	SET(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
ENDIF(MINGW)

IF(FFmpeg_LIBRARY_TEMP)
	# For FFmpegmain
	IF(NOT FFmpeg_BUILDING_LIBRARY)
		IF(FFmpegMAIN_LIBRARY)
			SET(FFmpeg_LIBRARY_TEMP ${FFmpegMAIN_LIBRARY} ${FFmpeg_LIBRARY_TEMP})
		ENDIF(FFmpegMAIN_LIBRARY)
	ENDIF(NOT FFmpeg_BUILDING_LIBRARY)

	# For OS X, FFmpeg uses Cocoa as a backend so it must link to Cocoa.
	# CMake doesn't display the -framework Cocoa string in the UI even
	# though it actually is there if I modify a pre-used variable.
	# I think it has something to do with the CACHE STRING.
	# So I use a temporary variable until the end so I can set the
	# "real" variable in one-shot.
	IF(APPLE)
		SET(FFmpeg_LIBRARY_TEMP ${FFmpeg_LIBRARY_TEMP} "-framework Cocoa")
	ENDIF(APPLE)

	# For threads, as mentioned Apple doesn't need this.
	# In fact, there seems to be a problem if I used the Threads package
	# and try using this line, so I'm just skipping it entirely for OS X.
	IF(NOT APPLE)
		SET(FFmpeg_LIBRARY_TEMP ${FFmpeg_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
	ENDIF(NOT APPLE)

	# For MinGW library
	IF(MINGW)
		SET(FFmpeg_LIBRARY_TEMP ${MINGW32_LIBRARY} ${FFmpeg_LIBRARY_TEMP})
	ENDIF(MINGW)

	# Set the final string here so the GUI reflects the final state.
	SET(FFmpeg_LIBRARY ${FFmpeg_LIBRARY_TEMP} CACHE STRING "Where the FFmpeg Library can be found")
	# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
	SET(FFmpeg_LIBRARY_TEMP "${FFmpeg_LIBRARY_TEMP}" CACHE INTERNAL "")
ENDIF(FFmpeg_LIBRARY_TEMP)

INCLUDE(FindPackageHandleStandardArgs)
message("FFmpeg_LIBRARY ${FFmpeg_LIBRARY}")
message("FFmpeg_INCLUDE_DIR ${FFmpeg_INCLUDE_DIR}")

FIND_PACKAGE_HANDLE_STANDARD_ARGS(FFmpeg REQUIRED_VARS FFmpeg_LIBRARY FFmpeg_INCLUDE_DIR)
