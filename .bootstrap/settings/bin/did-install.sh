#! /bin/sh 
#

. mulle-bootstrap-functions.sh


linkit()
{
   local dst
   local src
   local name

   src="$1"
   dst="$2"
   name="$3"

   local relative
   local csrc
   local cdst

   csrc="`canonicalize_path "${src}"`"
   cdst="`canonicalize_path "${dst}"`"

   relative="`relative_path_between "${csrc}" "${cdst}"`"

   relative="`dirname "${relative}"`"
   case "${relative}" in
      ""|".")
         src="`basename "${src}"`"
         ;;
      *)
         src="${relative}/`basename "${src}"`"
         ;;
   esac

   case "${dst}" in
      ""|".")
         dst="${name}"
         ;;
      *)
         dst="${dst}/${name}"
         ;;
   esac


   log_info "Setting up symlink \"${dst}\" as \"${src}\""
   exekutor ln -s -f "${src}" "${dst}"
}


if [ "$2" = "mulle-thread" ]
then
   linkit "${CLONES_SUBDIR}/mintomic" "${CLONES_SUBDIR}/$2" "mulle_mintomic"
fi

if [ "$2" = "mintomic" ]
then
   linkit "${CLONES_SUBDIR}/mintomic" "." "mulle_mintomic"
fi
