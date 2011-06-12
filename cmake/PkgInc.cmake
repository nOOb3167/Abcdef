#PkgConfig related

IF(NOT DEFINED PKG_CONFIG_EXECUTABLE)
 SET(PKG_CONFIG_EXECUTABLE "C:/testM/Glade/OBS/mingw/bin/pkg-config.exe" CACHE PATH "Magic")
ENDIF()

IF(NOT DEFINED PKG_CONFIG_PATH)
 MESSAGE("DEFAULTING THE PKG_CONFIG_PATH")
 SET(PKG_CONFIG_PATH "C:/testM/Glade/OBS/OBS24/mingw/lib/pkgconfig" CACHE PATH "Derp")
 #GTK2 (NonFedora) = "C:/testM/Glade/OBS/OBS24/mingw/lib/pkgconfig"
 #GTK3 = "C:/testM/Glade/OBS/mingw/lib/pkgconfig"
ENDIF()

SET(ENV{PKG_CONFIG_PATH} "${PKG_CONFIG_PATH}" CACHE PATH "Magic")

FIND_PACKAGE(PkgConfig)
