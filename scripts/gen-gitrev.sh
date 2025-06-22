#!/bin/sh
# SPDX-License-Identifier: (GPL-3.0-or-later)

# Usage: gen-gitrev.sh <c-filename>
#
# Generate C file containing version information

set -euf

SCRIPT="$(basename $0)"
SCRIPT_DIR="$(dirname $0)"

gitrev_file="${1:-version.c}"

exec > "$gitrev_file"

sha=$(git rev-parse HEAD)
desc=$(git describe --always --dirty)

cat <<EOF
#include <denton/version.h>
#include <stdint.h>

$(${SCRIPT_DIR}/autogen-notice.sh "$0" "$gitrev_file")

const char denton_version_str[] = "${desc}";
const char denton_version_sha_str[] = "${sha}";
const uint8_t denton_version_sha[] = { $(echo "${sha}" | sed 's/.\{2\}/0x&,/g') };
const char denton_version_date[] = __DATE__;
const char denton_version_time[] = __TIME__;

EOF
