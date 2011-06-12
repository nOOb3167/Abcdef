#!/bin/bash

export PKG_CONFIG_LIBDIR="/usr/i686-pc-mingw32/sys-root/mingw/lib/pkgconfig:/usr/share/pkgconfig"
exec pkg-config $@

