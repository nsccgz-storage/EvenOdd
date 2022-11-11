#pragma once
#include <stdio.h>

enum LOG_LEVEL {
  LOG_LEVEL_ERR = 1,
  LOG_LEVEL_INFO,
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_LAST
};

int output(LOG_LEVEL level, const char *file_name, int line, const char *f,
           ...);

#define LOG_ERROR(fmt, ...)                                                    \
  LOG_OUTPUT(LOG_LEVEL::LOG_LEVEL_ERR, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                     \
  LOG_OUTPUT(LOG_LEVEL::LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)                                                    \
  LOG_OUTPUT(LOG_LEVEL::LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define LOG_OUTPUT(level, fmt, ...)                                            \
  do {                                                                         \
    output(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__);                     \
  } while (0)
