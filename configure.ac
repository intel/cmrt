# intel-hybrid-driver package version number
m4_define([cmrt_major_version], [0])
m4_define([cmrt_minor_version], [0])
m4_define([cmrt_micro_version], [1])
m4_define([cmrt_pre_version],   [0])
m4_define([cmrt_version],
          [cmrt_major_version.cmrt_minor_version.cmrt_micro_version])
m4_if(cmrt_pre_version, [0], [], [
m4_append([cmrt_version], cmrt_pre_version, [.pre])
])

# libva minimum version requirement
m4_define([va_api_version], [0.34])
m4_define([libva_package_version], [1.2.0])

# libdrm minimum version requirement
m4_define([libdrm_version], [2.4.23])

AC_PREREQ([2.57])
AC_INIT([cmrt], [cmrt_version], [libva@lists.freedesktop.org],
        [libcmrt])
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([1.9 tar-ustar])

AC_CONFIG_HEADERS([src/config.h])

INTEL_DRIVER_MAJOR_VERSION=cmrt_major_version
INTEL_DRIVER_MINOR_VERSION=cmrt_minor_version
INTEL_DRIVER_MICRO_VERSION=cmrt_micro_version
AC_DEFINE([INTEL_DRIVER_MAJOR_VERSION], [cmrt_major_version], [Major version of the driver])
AC_DEFINE([INTEL_DRIVER_MINOR_VERSION], [cmrt_minor_version], [Minor version of the driver])
AC_DEFINE([INTEL_DRIVER_MICRO_VERSION], [cmrt_micro_version], [Micro version of the driver])
AC_DEFINE([INTEL_DRIVER_PRE_VERSION], [cmrt_pre_version], [Preversion of the driver])

INTEL_DRIVER_LT_LDFLAGS="-avoid-version"
AC_SUBST(INTEL_DRIVER_LT_LDFLAGS)

dnl Use pretty build output with automake >= 1.11
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])], [
    AM_DEFAULT_VERBOSITY=1
    AC_SUBST(AM_DEFAULT_VERBOSITY)
])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX

AC_C_BIGENDIAN
AC_HEADER_STDC
AC_SYS_LARGEFILE
AC_CHECK_LIB([m], [sin])
AC_CHECK_FUNCS([log2f])

LIBVA_PACKAGE_VERSION=libva_package_version
AC_SUBST(LIBVA_PACKAGE_VERSION)

dnl Check for recent enough DRM
LIBDRM_VERSION=libdrm_version
PKG_CHECK_MODULES([DRM], [libdrm >= $LIBDRM_VERSION])
AC_SUBST(LIBDRM_VERSION)

dnl Check for VA-API
PKG_CHECK_MODULES(LIBVA_DEPS,     [libva >= va_api_version])

AC_OUTPUT([
    Makefile
    src/Makefile
    ])
