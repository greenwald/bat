AC_INIT(src/BCModel.cxx)
: ${CXXFLAGS=""}
: ${CFLAGS=""}
: ${FFLAGS=""}
AM_INIT_AUTOMAKE(BAT, 0.9.3)
AM_CONFIG_HEADER(config.h)
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_HEADER_STDC
AC_FUNC_UTIME_NULL

dnl
dnl Check for Darwin
dnl
AM_CONDITIONAL([IS_DARWIN], [test x$(uname) = xDarwin])

dnl
dnl Check for debug mode
dnl
AC_ARG_ENABLE(
	[debug],
	[  --enable-debug          Compile in debug mode],
	[
		case "${enableval}" in
			yes) debug=true ; echo Compiling in debug mode ;;
			no)  debug=false ;;
			*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
		esac
	],
	[debug=false]
)
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

dnl
dnl Check for parallelization mode
dnl
AC_ARG_ENABLE(
	[parallelization],
	[AS_HELP_STRING([--enable-parallelization],[compile with OpenMP thread parallelization (default no)])],
	[
		case "${enableval}" in
			yes) parallelization=1 ;
				 CXXFLAGS+=" -fopenmp" ;
				 LDFLAGS+=" -fopenmp" ;;
			no)  parallelization=0 ;;
			*) AC_MSG_ERROR([bad value ${enableval} for --enable-parallelization]) ;;
		esac
	],
	[parallelization=0]
)

AC_DEFINE_UNQUOTED([THREAD_PARALLELIZATION], [$parallelization], [OpenMP thread parallelization])
AM_CONDITIONAL([THREAD_PARALLELIZATION], [test $parallelization = 1])

dnl
dnl Check for Cuba
dnl
AC_ARG_WITH(
	[cuba],
	[AS_HELP_STRING([--with-cuba[[=DIR]]],[compile with Cuba support (default no). Optional DIR is top of Cuba installation])],
	[
		use_cuba=yes
		echo "compiling with Cuba support: required Cuba version 3.1 or later"
		echo "Checking $with_cuba"
	],
	[use_cuba=no]
)

AC_ARG_WITH(cuba_include_dir, AS_HELP_STRING([--with-cuba-include-dir=DIR    path to cuba.h file]))
AC_ARG_WITH(cuba_lib_dir, AS_HELP_STRING([--with-cuba-lib-dir=DIR        path to libcuba.a file]))

if test "x$use_cuba" != xno; then
dnl check to see if lib and include can be determined automatically from $with_cuba
dnl if cuba just compiled, everything is in one directory
	if test -z "$with_cuba_include_dir" ; then
		if test -d "$with_cuba/include" ; then
			with_cuba_include_dir="$with_cuba/include"
		elif test -d $with_cuba ; then
			with_cuba_include_dir="$with_cuba"
		fi
	fi

	if test -z "$with_cuba_lib_dir" ; then
		if test -d "$with_cuba/lib" ; then
			with_cuba_lib_dir="$with_cuba/lib"
		elif test -d $with_cuba ; then
			with_cuba_lib_dir="$with_cuba"
		fi
	fi

	cubaincflags=""
	if test -n "$with_cuba_include_dir"; then
		echo "checking for cuba.h in $with_cuba_include_dir"
		cubaincflags=-I$with_cuba_include_dir
		CFLAGS+=" $cubaincflags"
	else
		echo "checking for cuba.h in system default directories"
	fi

	AC_CHECK_HEADERS(cuba.h, [with_cuba_inc=yes], [with_cuba_inc=no])

	if test x$with_cuba_inc = xno; then
		echo could not find cuba.h
		echo Try compiling without cuba
		echo
		exit 1
	else
		cubaldflags=""
		if test -n "$with_cuba_lib_dir"; then
			echo "checking for cuba library in path $with_cuba_lib_dir"
			cubaldflags="-L$with_cuba_lib_dir"
			LDFLAGS+=" $cubaldflags"
		else
			echo "checking for cuba library in system default directories"
		fi

		AC_CHECK_LIB(cuba,main,[with_cuba_lib=yes],[with_cuba_lib=no])

		if test x$with_cuba_lib = xno; then
			echo could not find libcuba.a
			echo Try compiling without cuba
			echo
			exit 1
		else
			AC_SUBST(CUBAINCLUDE,[$cubaincflags])
			AC_SUBST(CUBALIB,[$cubaldflags])
		fi
	fi
	AM_CONDITIONAL([CUBA], [test x$with_cuba_lib = xyes && test x$with_cuba_inc = xyes])
else
	echo compiling without cuba
	AM_CONDITIONAL([CUBA], [test x$use_cuba = xyes])
fi

dnl
dnl Check for ROOT
dnl
AC_DEFUN(
	[REQUIRE_ROOT_VERSION],
	[require_ROOT_version=$1]
)
REQUIRE_ROOT_VERSION([5.22])

dnl RooStats
AC_ARG_WITH(
	[roostats],
	[AS_HELP_STRING([--with-roostats],[compile with RooStats support (default no)])],
	[
		use_roostats=yes
		REQUIRE_ROOT_VERSION([5.27.04])
		echo "compiling with RooStats support: required ROOT version $require_ROOT_version or later"
	],
	[use_roostats=no]
)
dnl ROOT
m4_include(tools/build/root.m4)
ROOT_PATH(
	$require_ROOT_version,
	,
	[
		AC_MSG_ERROR([ROOT cannot be found. Please ensure that it is installed
                  and that either \$ROOTSYS is defined corectly or root-config is in the path.
                  One can pass --with-rootsys=/path/to/rootsys to configure as well.])
		exit
	]
)
dnl add some libraries to set of basic root libraries
ROOTLIBS="$ROOTLIBS -lMinuit"

dnl mathmore is a requirement of roostats
AC_MSG_CHECKING(whether ROOT is compiled with MathMore support)
hasmathmore=`$ROOTCONF --has-mathmore`
AC_MSG_RESULT($hasmathmore)
AM_CONDITIONAL([ROOTMATHMORE], [test x$hasmathmore = xyes])

dnl RooFit
if test $use_roostats = yes; then
	HAS_ROOSTATS(
		,
		[
			AC_MSG_ERROR([ROOT was compiled without RooFit support.
				Recompile ROOT or provide path to different version.])
			AC_MSG_ERROR([Compiling BAT without RooStats support.])
			use_roostats=no
		]
	)
fi
AM_CONDITIONAL([ROOSTATS], [test x$use_roostats = xyes])
if test $use_roostats = yes; then
   ROOTLIBS="$ROOTLIBS -lRooFitCore -lRooFit -lRooStats -lFoam"
fi

AC_OUTPUT(
	Makefile
	src/Makefile
	models/base/Makefile
	models/mtf/Makefile
	models/mvc/Makefile
	benchmarks/test/Makefile
	examples/Makefile
	examples/basic/binomial/Makefile
	examples/basic/combination1d/Makefile
	examples/basic/combination2d/Makefile
	examples/basic/errorpropagation/Makefile
	examples/basic/poisson/Makefile
	examples/basic/rootOutput/Makefile
	examples/advanced/polynomialfit/Makefile
	examples/advanced/roointerface/Makefile
	examples/advanced/mtf/systematics/Makefile
	examples/advanced/mvc/PPP/Makefile
	examples/advanced/mvc/oneObservable/Makefile
	examples/advanced/mvc/nuisance/Makefile
	examples/advanced/mvc/inverseProblem/Makefile
	examples/advanced/mvc/constraints/Makefile
	examples/advanced/referencecounting/Makefile
	examples/expert/TrialFunction/Makefile
	tools/MakefileTemplate
	tools/CreateProject.sh,[chmod a+x tools/CreateProject.sh]
)
