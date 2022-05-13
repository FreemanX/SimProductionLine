
#include "utils.h"

static pthread_mutex_t print_mx;
static int print_mx_inited = 0;
static void initDebugMX() {
  pthread_mutex_init(&print_mx, NULL);
  print_mx_inited = 1;
}

void flushed_printf(const char *format, ...) {
  if (!print_mx_inited) initDebugMX();
  pthread_mutex_lock(&print_mx);
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  fflush(stdout);
  pthread_mutex_unlock(&print_mx);
}

void err_printf(const char *msgHead, int line, const char *what, ...) {
  if (!print_mx_inited) initDebugMX();
  pthread_mutex_lock(&print_mx);
  PRINT_BLUE
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  fprintf(stderr, "[%02d:%02d:%02d]", tm.tm_hour, tm.tm_min, tm.tm_sec);
  PRINT_RED_B
  fprintf(stderr, "|ERROR!| %s(%d): ", msgHead, line);
  PRINT_DEFAULT
  va_list args;
  va_start(args, what);
  vfprintf(stderr, what, args);
  va_end(args);
  fflush(stderr);
  pthread_mutex_unlock(&print_mx);
}

void debug_printf(const char *funcName, const char *format, ...) {
  if (!print_mx_inited) initDebugMX();
  pthread_mutex_lock(&print_mx);
  PRINT_BLUE
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  printf("[%02d:%02d:%02d]", tm.tm_hour, tm.tm_min, tm.tm_sec);
  PRINT_YELLOW_B
  printf("%s ", funcName);
  PRINT_CYAN
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  fflush(stdout);
  PRINT_DEFAULT
  pthread_mutex_unlock(&print_mx);
}

double getCurrentTime() {
  return omp_get_wtime();
}
