#!/bin/sh

# Usage: autogen-notice.sh <scriptname> <filename> [comment-language] [inner-width]
#
# Print a notice for an auto generated file in the form of a comment.
#
# ARGUMENTS
#   scriptname          name of the script generating the file
#   filename            name of the file being generated
#   comment-language    language to base the comment style on
#   inner-width         inner width of notice text

set -ef

caller="$1"
output="$2"
language="${3:-c}"
width="${4:-44}"

case "${language}" in
    cxx|cpp|cc)
        front="//"
        middle="/"
        back="//"
        ;;
    c)
        front="/*"
        middle="*"
        back="*/"
        ;;
    lua)
        front="--"
        middle="-"
        back="--"
        ;;
esac

middle_width=$(expr "$width" - "${#front}" - "${#back}")

body="$(cat | awk "BEGIN {FS=\"\\n\"} { \$1 = sprintf(\"%-${middle_width}s\", \$1) } 1" <<EOF
 THIS FILE WAS AUTO-GENERATED.
 ANY CHANGES MAY BE LOST.

 SOURCE NAME:  $(basename $output)
 SCRIPT NAME:  $(basename $caller)
 GENERATED ON: $(date -u +"%Y-%m-%dT%H:%M:%SZ")
 GIT REVISION: $(git describe --always --dirty)
EOF
)"

banner=$(
    printf -- "%s" "$front" &&
    printf -- "$middle%.0s" $(seq "$middle_width") &&
    printf -- "%s\n" "$back"
)

echo "${banner}"
echo "${body}" | sed "s@.*@$front&$back@"
echo "${banner}"
