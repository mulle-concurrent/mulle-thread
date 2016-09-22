#! /bin/sh


get_version()
{
   local filename

   filename="$1"
   grep MULLE_THREAD_VERSION "${filename}" | \
   sed 's|(\([0-9]*\) \<\< [0-9]*)|\1|g' | \
   sed 's|^.*(\(.*\))|\1|' | \
   sed 's/ | /./g'
}


TAG="${1:-`get_version "src/mulle_thread.h"`}"


executable="`which mulle-bootstrap`"
directory="`dirname -- "${executable}"`/../libexec"


. "${directory}/mulle-bootstrap-logging.sh"


git_must_be_clean()
{
   local name
   local clean

   name="${1:-${PWD}}"

   if [ ! -d .git ]
   then
      fail "\"${name}\" is not a git repository"
   fi

   clean=`git status -s --untracked-files=no`
   if [ "${clean}" != "" ]
   then
      fail "repository \"${name}\" is tainted"
   fi
}


set -e

git_must_be_clean
git push public master

# seperate step, as it's tedious to remove tag when
# previous push fails

git tag "${TAG}"
git push public master --tags

./bin/generate-brew-formula.sh  > ../homebrew-software/mulle-thread.rb
(
	cd ../homebrew-software ;
   git add mulle-thread.rb ;
 	git commit -m "${TAG} release of mulle-thread" mulle-thread.rb ;
 	git push origin master
)

