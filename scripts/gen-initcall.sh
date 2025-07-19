#!/bin/sh
# SPDX-License-Identifier: (GPL-3.0-or-later)

# Usage: gen-initcall.sh <kernel-object> <filename>
#
# Generate initcall table by parsing the .initcall section
#
# ARGUMENTS
#  kernel-object  compiled kernel object code with initcall symbols
#  filename       *.c file to generate

set -euf

SCRIPT="$(basename $0)"
SCRIPT_DIR="$(dirname $0)"

DENTON_OBJ=${1}
INITCALL_FILE=${2:-initcall.c}

# Dump ".initcall" section's binary contents, which is NUL terminated strings
# containing dependency pairs in a partial ordering:
# 
# Contents of section .discard.initcall.deps:
#  0000 6561726c 7920636f 72650063 6f726520  early core.core 
#  0010 73756273 79737465 6d007375 62737973  subsystem.subsys
#  0020 74656d20 64657669 636500             tem device.     
# 
initcalls=$(
	objdump --section=.discard.initcall.deps --full-contents "$DENTON_OBJ" |
		grep '^ *[0-9a-f]\{4\}' | # get "binary contents" lines,
		cut --characters 7-42   | # cut the binary data characters,
		tr --delete '[:blank:]' | # cut all whitespace to make one line,
		xxd -r -p               | # parse binary data into ASCII,
		tr '\0' '\n'            | # replace NUL with something for tsort
		tsort
         )

# truncates $INITCALL_FILE and redirect stdout to it for the rest of the script
exec > "$INITCALL_FILE"

cat <<EOF
/* SPDX-License-Identifier: (GPL-3.0-or-later) */

$(${SCRIPT_DIR}/autogen-notice.sh "$0" "$INITCALL_FILE")
#include <denton/stddef.h>
#include <denton/initcall.h>
#include <stdlib.h>

$([ ! -z "${initcalls}" ] && printf "extern void __init_%s(void);\n" ${initcalls})

void (*__initcalls[]) (void) =
{
$([ ! -z "${initcalls}" ] && printf "\t__init_%s,\n" ${initcalls})
	NULL,
};
EOF
