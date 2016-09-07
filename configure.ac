# Copyright (c) 2016 Intel Corporation. All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sub license, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice (including the
# next paragraph) shall be included in all copies or substantial portions
# of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
# IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
# ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# CMRT-API version
# - increment major for any ABI change (which shall not occur!)
# - increment minor for any interface change (e.g. new/modified function)
# - increment micro for any other change (new flag, new codec definition, etc.)
# - reset micro version to zero when minor version is incremented
# - reset minor version to zero when major version is incremented
m4_define([cmrt_api_major_version], [0])
m4_define([cmrt_api_minor_version], [10])
m4_define([cmrt_api_micro_version], [1])

m4_define([cmrt_api_version],
          [cmrt_api_major_version.cmrt_api_minor_version.cmrt_api_micro_version])

# CMRT package version number, (as distinct from shared library version)
# XXX: we want the package version to remain at 1.0.x for CMRT-API 0.10.y
#
# - major version is automatically generated from CMRT-API major version
# - minor version is automatically generated from CMRT-API minor version
# - increment micro for any library release
# - reset micro version to zero when CMRT-API major or minor version is changed
m4_define([cmrt_major_version], [m4_eval(cmrt_api_major_version + 1)])
m4_define([cmrt_minor_version], [m4_eval(cmrt_api_minor_version - 10)])
m4_define([cmrt_micro_version], [6])
m4_define([cmrt_pre_version],   [0])

m4_define([cmrt_version],
          [cmrt_major_version.cmrt_minor_version.cmrt_micro_version])
m4_if(cmrt_pre_version, [0], [], [
m4_append([cmrt_version], cmrt_pre_version, [.pre])
])

# CMRT library version number (generated, do not change)
# XXX: we want the SONAME to remain at libcmrt.so.1 for CMRT-API major == 0
#
# The library name is generated libcmrt.<x>.<y>.0 where
# <x> = CMRT-API major version + 1
# <y> = 100 * CMRT-API minor version + CMRT-API micro version
#
# For example:
# CMRT-API 0.10.0 generates libcmrt.so.1.1000.0
# CMRT-API 0.14.1 generates libcmrt.so.1.1401.0
# CMRT-API 1.2.13 generates libcmrt.so.2.213.0
m4_define([cmrt_interface_bias], [m4_eval(cmrt_api_major_version + 1)])
m4_define([cmrt_interface_age],  [0])
m4_define([cmrt_binary_age],
          [m4_eval(100 * cmrt_api_minor_version + cmrt_api_micro_version - cmrt_interface_age)])

m4_define([cmrt_lt_current],
          [m4_eval(100 * cmrt_api_minor_version + cmrt_api_micro_version + cmrt_interface_bias)])
m4_define([cmrt_lt_revision],
          [m4_eval(cmrt_interface_age)])
m4_define([cmrt_lt_age],
          [m4_eval(cmrt_binary_age - cmrt_interface_age)])

# libva minimum version requirement
m4_define([va_api_version], [0.34])
m4_define([libva_package_version], [1.2.0])

# libdrm minimum version requirement
m4_define([libdrm_version], [2.4.23])

AC_PREREQ([2.57])
AC_INIT([cmrt], [cmrt_version], [libva@lists.freedesktop.org],
        [cmrt])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([dist-bzip2])

AC_CONFIG_HEADERS([src/config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

TODAY="`LC_ALL=C date +'%a, %d %b %Y %X %z'`"
AC_SUBST(TODAY)

CMRT_API_MAJOR_VERSION=cmrt_api_major_version
CMRT_API_MINOR_VERSION=cmrt_api_minor_version
CMRT_API_MICRO_VERSION=cmrt_api_micro_version
CMRT_API_VERSION=cmrt_api_version
AC_SUBST(CMRT_API_MAJOR_VERSION)
AC_SUBST(CMRT_API_MINOR_VERSION)
AC_SUBST(CMRT_API_MICRO_VERSION)
AC_SUBST(CMRT_API_VERSION)

CMRT_MAJOR_VERSION=cmrt_major_version
CMRT_MINOR_VERSION=cmrt_minor_version
CMRT_MICRO_VERSION=cmrt_micro_version
CMRT_VERSION=cmrt_version
AC_SUBST(CMRT_MAJOR_VERSION)
AC_SUBST(CMRT_MINOR_VERSION)
AC_SUBST(CMRT_MICRO_VERSION)
AC_SUBST(CMRT_VERSION)
AC_DEFINE([CMRT_PRE_VERSION], [cmrt_pre_version], [Pre-version of the runtime])

CMRT_LT_CURRENT=cmrt_lt_current
CMRT_LT_REV=cmrt_lt_revision
CMRT_LT_AGE=cmrt_lt_age
CMRT_LT_VERSION="$CMRT_LT_CURRENT:$CMRT_LT_REV:$CMRT_LT_AGE"
CMRT_LT_LDFLAGS="-version-info $CMRT_LT_VERSION"
AC_SUBST(CMRT_LT_VERSION)
AC_SUBST(CMRT_LT_LDFLAGS)


AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
PKG_PROG_PKG_CONFIG

AC_C_BIGENDIAN
AC_HEADER_STDC
AC_SYS_LARGEFILE
AC_CHECK_LIB([m], [sin])
AC_CHECK_FUNCS([log2f])

CMRT_PACKAGE_VERSION=cmrt_version
AC_SUBST(CMRT_PACKAGE_VERSION)

dnl Check for recent enough DRM
LIBDRM_VERSION=libdrm_version
PKG_CHECK_MODULES([DRM], [libdrm >= $LIBDRM_VERSION])
AC_SUBST(LIBDRM_VERSION)

dnl Check for CMRT-API
PKG_CHECK_MODULES(LIBVA_DEPS,     [libva >= va_api_version])

pkgconfigdir=${libdir}/pkgconfig
AC_SUBST(pkgconfigdir)

AC_OUTPUT([
    Makefile
    src/Makefile
    pkgconfig/Makefile
    pkgconfig/libcmrt.pc
    ])

# Print a small summary
echo
echo "cmrt - ${CMRT_VERSION} (CMRT-API ${CMRT_API_VERSION})"
echo
echo Installation prefix  .............. : $prefix
echo
