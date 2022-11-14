#include "log.h"
#include <cstdarg>
#include <cstdio>

#define ONE_KILO 1024

static LOG_LEVEL CUR_LEVEL = LOG_LEVEL::LOG_LEVEL_DEBUG;

int __log_output__(LOG_LEVEL level, const char *file_name, int line, const char *f,
           ...) {
  va_list args;
  char msg[ONE_KILO];
  va_start(args, f);
  vsnprintf(msg, sizeof(msg), f, args);
  va_end(args);

  if (level >= LOG_LEVEL::LOG_LEVEL_ERR && level <= CUR_LEVEL) {
    printf("%s : in file: %s:%d:\n", msg, file_name, line);
  }
  return 0;
}
