/*
 * Zlog utility
 * Zhiqiang Ma
 * http://zhiqiangma.com
 * Released under GPLv3
 * Last update: Sep. 16, 2011
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "zlog.h"

// #define ZLOG_DISABLE_LOG 1
#define ZLOG_BUFFER_STR_MAX_LEN 128
#define ZLOG_BUFFER_SIZE (0x1 << 22)
// #define ZLOG_REAL_WORLD_TIME 1

// only for debug, enabling this will slow down the log
// #define ZLOG_FORCE_FLUSH_BUFFER

// zlog utilities
FILE* zlog_fout = NULL;

// --------------------------------------------------------------
char _zlog_buffer[ZLOG_BUFFER_SIZE][ZLOG_BUFFER_STR_MAX_LEN];
int _zlog_buffer_size = 0;

pthread_mutex_t _zlog_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

static inline void _zlog_buffer_lock()
{
       pthread_mutex_lock(&_zlog_buffer_mutex);
}

static inline void _zlog_buffer_unlock()
{
       pthread_mutex_unlock(&_zlog_buffer_mutex);
}

static void _zlog_flush_buffer()
{
    int i = 0;
    for (i = 0; i < _zlog_buffer_size; i++) {
        fprintf(zlog_fout, "%s", _zlog_buffer[i]);
    }
    fflush(zlog_fout);
    _zlog_buffer_size = 0;
}

// first zlog_get_buffer, write to @return
// then zlog_finish_buffer
//
// zlog_get_buffer may flush the buffer, which requires lots I/O ops
static inline char* zlog_get_buffer()
{
    _zlog_buffer_lock();
    if (_zlog_buffer_size == ZLOG_BUFFER_SIZE) {
        _zlog_flush_buffer();
    }

    // allocate buffer
    _zlog_buffer_size++;
    return _zlog_buffer[_zlog_buffer_size-1];
}

static inline void zlog_finish_buffer()
{
#ifdef ZLOG_FORCE_FLUSH_BUFFER
    _zlog_flush_buffer();
#endif
    _zlog_buffer_unlock();
}

// --------------------------------------------------------------

void zlog_init(char const* log_file)
{
    zlog_fout = fopen(log_file, "a+");
}

void zlog_init_stdout()
{
    zlog_fout = stdout;
}

#define ZLOG_FLUSH_INTERVAL_SEC 180
// In practice: flush size < .8 * BUFFER_SIZE
#define ZLOG_BUFFER_FLUSH_SIZE (0.8 * ZLOG_BUFFER_SIZE)

void* zlog_buffer_flush_thread(void* arg)
{
    struct timeval tv;
    time_t lasttime;
    time_t curtime;

    gettimeofday(&tv, NULL);

    lasttime = tv.tv_sec;

    do {
        sleep(10);
        gettimeofday(&tv, NULL);
        curtime = tv.tv_sec;
        if ( (curtime - lasttime) >= ZLOG_FLUSH_INTERVAL_SEC ) {
            zlogf_time("Flush buffer.\n");
            zlog_flush_buffer();
            lasttime = curtime;
        } else {
            _zlog_buffer_lock();
            if (_zlog_buffer_size >= ZLOG_BUFFER_FLUSH_SIZE ) {
                _zlog_flush_buffer();
            }
            _zlog_buffer_unlock();
        }
    } while (1);
    return NULL;
}

void zlog_init_flush_thread()
{
    pthread_t thr;
    pthread_create(&thr, NULL, zlog_buffer_flush_thread, NULL);
    zlogf_time("Flush thread is created.\n");
}

void zlog_flush_buffer()
{
    _zlog_buffer_lock();
    _zlog_flush_buffer();
    _zlog_buffer_unlock();
}

void zlog_finish()
{
    zlog_flush_buffer();
    if (zlog_fout != stdout) {
        fclose(zlog_fout);
    }
    zlog_fout = stdout;
}

inline void zlogf(char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif

    va_list va;
    char* buffer = NULL;

    va_start(va, fmt);
    // vfprintf(zlog_fout, fmt, va);
    // fflush(zlog_fout);
    buffer = zlog_get_buffer();
    vsprintf(buffer, fmt, va);
    zlog_finish_buffer();
    va_end(va);
}

#define STR_BUFFER_SIZE 128
inline void zlogf_time(char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif

    char timebuf[STR_BUFFER_SIZE];
    struct timeval tv;
    time_t curtime;
    char* buffer = NULL;

    va_list va;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
#ifdef ZLOG_REAL_WORLD_TIME
    strftime(timebuf, STR_BUFFER_SIZE, "%m-%d-%Y %T", localtime(&curtime));
#else
    sprintf(timebuf, "%ld", curtime);
#endif
    // fprintf(zlog_fout, "[%s.%06lds] ", buffer, tv.tv_usec);
    buffer = zlog_get_buffer();
    sprintf(buffer, "[%s.%06lds] ", timebuf, tv.tv_usec);
    buffer += strlen(timebuf) + 11; // space for time

    va_start(va, fmt);
    // vfprintf(zlog_fout, fmt, va);
    vsprintf(buffer, fmt, va);
    // fflush(zlog_fout);
    zlog_finish_buffer();
    va_end(va);
}
#undef STR_BUFFER_SIZE

#define STR_BUFFER_SIZE 128
inline void zlog_time(char* filename, int line, char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif

    static char timebuf[STR_BUFFER_SIZE];
    struct timeval tv;
    time_t curtime;
    char* buffer = NULL;

    va_list va;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
#ifdef ZLOG_REAL_WORLD_TIME
    strftime(timebuf, STR_BUFFER_SIZE, "%m-%d-%Y %T", localtime(&curtime));
#else
    sprintf(timebuf, "%ld", curtime);
#endif

    buffer = zlog_get_buffer();
    // fprintf(zlog_fout, "[%s.%06lds] [@%s:%d]", timebuf, tv.tv_usec, filename, line);
    sprintf(buffer, "[%s.%06lds] [@%s:%d]", timebuf, tv.tv_usec, filename, line);
    // buffer += strlen(timebuf) + 13 + strlen(filename) + 7; // print at most 5 digit of line
    buffer += strlen(buffer); // print at most 5 digit of line

    va_start(va, fmt);
    // vfprintf(zlog_fout, fmt, va);
    vsprintf(buffer, fmt, va);
    // fflush(zlog_fout);
    zlog_finish_buffer();
    va_end(va);
}
#undef STR_BUFFER_SIZE

inline void zlog(char* filename, int line, char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif

    char* buffer = NULL;
    va_list va;

    buffer = zlog_get_buffer();
    // fprintf(zlog_fout, "[@%s:%d]", filename, line);
    sprintf(buffer, "[@%s:%d]", filename, line);
    va_start(va, fmt);
    // vfprintf(zlog_fout, fmt, va);
    vsprintf(buffer, fmt, va);
    // fflush(zlog_fout);
    zlog_finish_buffer();
    va_end(va);
}
// End zlog utilities

