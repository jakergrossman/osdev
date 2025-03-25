#!/bin/sh

DENTON_OBJ=$1
INITCALL_FILE=$2

ERR_FILE=$(mktemp)

# This dumps out the ".initcall" section's binary contents.  The section itself
# is a bunch of NUL terminated strings, one right after another, each
# containing a dependency pair. The NULs are converted into newlines, resulting
# in a long stream of dependency pairs
#
# The pairs are then sorted into an order that satisfies the required dependences
# via `tsort`, which does a topological sort of the dependency tree.
#
initcalls=$(
	objdump --section=.discard.initcall.deps --full-contents "$DENTON_OBJ" \
	| grep '^ *[0-9a-f]\{4\}' \
	| cut -c 7-42             \
	| tr -d ' \n'             \
	| xxd -r -p               \
	| tr '\0' '\n'            \
	| tsort
)

ERR=$(cat $ERR_FILE)
rm $ERR_FILE

# Errors from tsort generally indicate a cycle in the input
if ! [ -z "$ERR" ]
then
    echo "tsort reported an error!" 1>&2
    echo "$ERR" 1>&2

    exit 1
fi

{
	echo "/* THIS FILE IS AUTO GENERATED BY: $(basename $0) */"
	echo
	echo "#include <denton/stddef.h>"
	echo "#include <denton/initcall.h>"
	echo "#include <stdlib.h>"
	echo

	for i in $initcalls
	do
		echo "extern void __init_$i(void);"
	done

	echo
	echo "void (*__initcalls[]) (void) = {"

	for i in $initcalls
	do
		echo "    __init_$i,"
	done

	echo "    NULL"
	echo "};"
} > "$INITCALL_FILE"
