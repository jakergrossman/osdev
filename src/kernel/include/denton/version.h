#ifndef DENTON_VERSION_H
#define DENTON_VERSION_H

#include <stdint.h>

extern const char    denton_version_str[];
extern const char    denton_version_sha_str[];
extern const uint8_t denton_version_sha[];
extern const char    denton_version_date[];
extern const char    denton_version_time[];

#define DENTON_VERSION_USE_LARGE_SPLASH 1

#if DENTON_VERSION_USE_LARGE_SPLASH == 1
#define DENTON_VERSION_SPLASH R"(
      ___           ___           ___           ___           ___           ___     
     /\  \         /\  \         /\__\         /\  \         /\  \         /\__\
    /::\  \       /::\  \       /::|  |        \:\  \       /::\  \       /::|  |
   /:/\:\  \     /:/\:\  \     /:|:|  |         \:\  \     /:/\:\  \     /:|:|  |
  /:/  \:\__\   /::\~\:\  \   /:/|:|  |__       /::\  \   /:/  \:\  \   /:/|:|  |__
 /:/__/ \:|__| /:/\:\ \:\__\ /:/ |:| /\__\     /:/\:\__\ /:/__/ \:\__\ /:/ |:| /\__\
 \:\  \ /:/  / \:\~\:\ \/__/ \/__|:|/:/  /    /:/  \/__/ \:\  \ /:/  / \/__|:|/:/  /
  \:\  /:/  /   \:\ \:\__\       |:/:/  /    /:/  /       \:\  /:/  /      |:/:/  /
   \:\/:/  /     \:\ \/__/       |::/  /    /:/  /         \:\/:/  /       |::/  /
    \::/  /       \:\__\         /:/  /     \/__/           \::/  /        /:/  /
     \/__/         \/__/         \/__/                       \/__/         \/__/

                 OS VERSION: %s
                 GIT SHA:    %s
                 BUILD TIME: %s, %s

)"


#else /* DENTON_VERSION_USE_LARGE_SPLASH == 1 */

#define DENTON_VERSION_SPLASH "DENTON OPERATING SYSTEM\n"

#endif /* DENTON_VERSION_USE_LARGE_SPLASH != 1 */

#define DENTON_VERSION_SPLASH_FMT \
    "OS Version %s, Compiled %s %s\n"

#define DENTON_VERSION_SPLASH_FMT_ARGS \
 denton_version_str, denton_version_sha_str,__DATE__, __TIME__

#endif /* DENTON_VERSION_H */
