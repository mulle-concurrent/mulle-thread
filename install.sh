#! /bin/sh

PATH="$HOME/.linuxbrew/bin:/usr/local/bin:${HOME}/bin:${PATH}"


mulle-bootstrap -a

CMAKE_INSTALL_PREFIX="${1:-/usr/local}"

mkdir build 2> /dev/null
cd build

case "`uname`" in
   MINGW*)
      CMAKE_GENERATOR="NMake Makefiles"
      MAKE=nmake
   ;;

   *)
      CMAKE_GENERATOR="Unix Makefiles"
      MAKE=make
   ;;
esac


cmake -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}" -G "${CMAKE_GENERATOR}" ..

"${MAKE}" install
