#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/report_utils.h"

static void __report(const char *tag, const char *fmt, va_list args) {
  if (tag) {
      fprintf(stderr, "%s", tag);
  }
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);
}

void report_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[ERROR]: ", fmt, args);
}

void report_warning(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[WARNING]: ", fmt, args);
}

void die(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  __report("[ERROR]: ", fmt, args);
  exit(EXIT_FAILURE);
}
