#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([emoskit], [0.1], [little.zhou@foxmail.com])
AM_INIT_AUTOMAKE([foreign])
#AC_CONFIG_SRCDIR([status.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
${CXXFLAGS="-O2"}
AC_LANG(C++)
AX_CHECK_COMPILE_FLAG([-ffunction-sections], [CXXFLAGS+=" -ffunction-sections"], [])
AX_CHECK_COMPILE_FLAG([-Wl,-gc-sections], [LDFLAGS+=" -Wl,-gc-sections"], [])

AC_PROG_CXX
AC_PROG_CC

AX_CXX_COMPILE_STDCXX_11

# Checks for libraries.
AC_PROG_RANLIB

# Check whether the user's system supports pthread
AC_SEARCH_LIBS(pthread_create, pthread)
if test "x$ac_cv_search_pthread_create" = "xno"; then
	AC_MSG_ERROR([Can not enable threads without the POSIX thread library.])
fi

# Check whether the user's system supports protobuf
tryprotobufdir=""
AC_ARG_WITH(protobuf,
	[AS_HELP_STRING([--with-protobuf=PATH],[Specify path to protobuf installation])],
	[
		if test "x$withval" != "xno" ; then
                        tryprotobufdir=$withval
                fi
	]
)

PB_URL=https://github.com/google/protobuf

AC_CACHE_CHECK([for protobuf], ac_cv_protobuf_dir, [
	pb_found=no

        LIBS="-lprotobuf $LIBS"

	# Check user specified path
	if test "x$tryprotobufdir" != "x" -a -d "$tryprotobufdir" ; then
      		if test -d "$tryprotobufdir/lib" ; then
        		LDFLAGS="-L$tryprotobufdir/lib $LDFLAGS"
      		else
        		LDFLAGS="-L$tryprotobufdir $LDFLAGS"
      		fi
      		if test -d "$tryprotobufdir/include" ; then
        		CPPFLAGS="-I$tryprotobufdir/include $CPPFLAGS"
      		else
        		CPPFLAGS="-I$tryprotobufdir $CPPFLAGS"
     	 	fi

		protoc_found=no
		if test -d "$tryprotobufdir/bin" ; then
                        protoc_dir="$tryprotobufdir/bin"
                else    
                        protoc_dir="$tryprotobufdir"
                fi

                $protoc_dir/protoc network_proto.proto --cpp_out=. && protoc_found=yes

                if test $protoc_found = yes ; then
                        AC_TRY_LINK([#include <google/protobuf/compiler/importer.h>],
                                [google::protobuf::compiler::DiskSourceTree tree;],
                                [pb_linked=yes],
                                [pb_linked=no])
                fi
                if test $protoc_found = yes -a $pb_linked = yes ; then
                        ac_cv_protobuf_dir="$tryprotobufdir"
                        pb_found=yes
                fi
	fi

	# Check system path
	if test $pb_found = no ; then
		protoc_found=no
		protoc network_proto.proto --cpp_out=. && protoc_found=yes
		if test $protoc_found = yes ; then
			AC_TRY_LINK([#include <google/protobuf/compiler/importer.h>],
				[google::protobuf::compiler::DiskSourceTree tree;],
				[pb_linked=yes],
				[pb_linked=no])
		fi
		if test $protoc_found = yes -a $pb_linked = yes ; then
			ac_cv_protobuf_dir="system"
			pb_found=yes
		fi
	fi

	if test $pb_found = no ; then
		ac_cv_protobuf_dir=no
    		AC_MSG_ERROR([protobuf is required.  You can get it from $PB_URL, If it is already installed, specify its path using --with-protobuf=dir])
	fi

])

AC_DEFINE_UNQUOTED([PROTOBUF_HOME], ["$ac_cv_protobuf_dir"], ["Define the home path of protobuf"])

if test "x$prefix" = "xNONE" ; then
	AC_DEFINE_UNQUOTED([EMOSKIT_HOME], ["$ac_default_prefix"], ["Define the home path of emoskit"])
else
	AC_DEFINE_UNQUOTED([EMOSKIT_HOME], ["$prefix"], ["Define the home path of emoskit"])
fi

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero clock_gettime localtime_r memmove memset select socket strerror strrchr])

#AC_CONFIG_FILES([Makefile
#                 codegen/Makefile])
AC_OUTPUT(Makefile)
