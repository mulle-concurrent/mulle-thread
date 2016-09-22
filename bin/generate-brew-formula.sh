#! /bin/sh
#
# Generate a formula for mulle-thread
#
PROJECT=mulle_thread
TARGET=mulle-thread
HOMEPAGE="http://www.mulle-kybernetik.com/software/git/${TARGET}"
DESC="Cross-platform threads and atomic operations"

get_version()
{
   local filename

   filename="$1"
   grep MULLE_THREAD_VERSION "${filename}" | \
   sed 's|(\([0-9]*\) \<\< [0-9]*)|\1|g' | \
   sed 's|^.*(\(.*\))|\1|' | \
   sed 's/ | /./g'
}


VERSION="${1:-`get_version "src/mulle_thread.h"`}"
[ $# -eq 0 ] || shift
ARCHIVEURL="${1:-http://www.mulle-kybernetik.com/software/git/${TARGET}/tarball/$VERSION}"
[ $# -eq 0 ] || shift

set -e

[ "$VERSION" = "" ] && exit 1
[ "$ARCHIVEURL" = "" ] && exit 1


TMPARCHIVE="/tmp/${TARGET}-${VERSION}-archive"

if [ ! -f  "${TMPARCHIVE}" ]
then
   curl -L -o "${TMPARCHIVE}" "${ARCHIVEURL}"
   if [ $? -ne 0 -o ! -f "${TMPARCHIVE}" ]
   then
      echo "Download failed" >&2
      exit 1
   fi
else
   echo "using cached file ${TMPARCHIVE} instead of downloading again" >&2
fi

#
# anything less than 17 KB is wrong
#
size="`du -k "${TMPARCHIVE}" | awk '{ print $ 1}'`"
if [ $size -lt 17 ]
then
   echo "Archive truncated or missing" >&2
   cat "${TMPARCHIVE}" >&2
   rm "${TMPARCHIVE}"
   exit 1
fi

HASH="`shasum -p -a 256 "${TMPARCHIVE}" | awk '{ print $1 }'`"

cat <<EOF
class ${PROJECT} < Formula
  homepage "${HOMEPAGE}"
  desc "${DESC}"
  url "${ARCHIVEURL}"
  version "${VERSION}"
  sha256 "${HASH}"

  def install
     system "./install.sh", "#{prefix}"
  end

  test do
  end
end
# FORMULA ${TARGET}.rb
EOF
