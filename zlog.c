/*
 * Zlog utility
 * By: Eric Ma https://www.ericzma.com
 * Released under Unlicense
 */

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "zlog-config.h"
#include "zlog.h"

// --------------------------------------------------------------
// zlog utilities
FILE* zlog_fout = NULL;
const char* zlog_file_log_name = NULL;

char _zlog_buffer[ZLOG_BUFFER_SIZE][ZLOG_BUFFER_STR_MAX_LEN];
int _zlog_buffer_size = 0;

pthread_mutex_t _zlog_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
// --------------------------------------------------------------

static inline void _zlog_buffer_lock(void)
{
    pthread_mutex_lock(&_zlog_buffer_mutex);
}

static inline void _zlog_buffer_unlock(void)
{
    pthread_mutex_unlock(&_zlog_buffer_mutex);
}

static void _zlog_flush_buffer(void)
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
// zlog_get_buffer may flush the buffer, which require I/O ops
static inline char* zlog_get_buffer(void)
{
    _zlog_buffer_lock();
    if (_zlog_buffer_size == ZLOG_BUFFER_SIZE) {
        _zlog_flush_buffer();
    }

    // allocate buffer
    _zlog_buffer_size++;
    return _zlog_buffer[_zlog_buffer_size-1];
}

static inline void zlog_finish_buffer(void)
{
#ifdef ZLOG_FORCE_FLUSH_BUFFER
    _zlog_flush_buffer();
#endif
    _zlog_buffer_unlock();
}

// Error reporting
static inline void print_error(const char *function_name, char *error_msg){
    fprintf(stderr, "Error in function %s: %s\n", function_name, error_msg);
}

// --------------------------------------------------------------

void zlog_init(char const* log_file)
{
    zlog_file_log_name = strdup(log_file);

    zlog_fout = fopen(log_file, "a+");

    if(!zlog_fout){
        print_error(__func__, strerror(errno));
    }
}

void zlog_init_stdout(void)
{
    zlog_fout = stdout;
}

void zlog_init_stderr(void)
{
    zlog_fout = stderr;
}

static void* zlog_buffer_flush_thread(void* interval_sec)
{
    struct timeval tv;
    time_t lasttime;
    time_t curtime;
    int flush_interval = (int)(intptr_t)interval_sec;
    int sleep_time_sec = ZLOG_SLEEP_TIME_SEC > flush_interval ? flush_interval : ZLOG_SLEEP_TIME_SEC;

    gettimeofday(&tv, NULL);

    lasttime = tv.tv_sec;

    do {
        sleep(sleep_time_sec);
        gettimeofday(&tv, NULL);
        curtime = tv.tv_sec;
        if ( (curtime - lasttime) >= flush_interval ) {
            // ZLOG_LOG_LEVEL is used to make the buffer flushing
            // seamlessly for the users. It does not matter what level
            // the messages are at this point because, if they do
            // not meet message level requirement, that wouldn't
            // have been buffered in the first place.
            zlogf_time(ZLOG_LOG_LEVEL, "Flush buffer.\n");
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

void zlog_init_flush_thread(void)
{
    zlog_init_flush_thread_with_interval(ZLOG_FLUSH_INTERVAL_SEC);
}

void zlog_init_flush_thread_with_interval(int interval_sec)
{
    pthread_t thr;
    pthread_create(&thr, NULL, zlog_buffer_flush_thread, (void*)(intptr_t)interval_sec);
    zlogf_time(ZLOG_LOG_LEVEL, "Flush thread is created.\n");
}

void zlog_flush_buffer(void)
{
    _zlog_buffer_lock();
    _zlog_flush_buffer();
    _zlog_buffer_unlock();
}

void zlog_finish(void)
{
    zlog_flush_buffer();
    if (zlog_fout != stdout || zlog_fout != stderr) {
        fclose(zlog_fout);
        zlog_fout = stdout;
    }

}

inline void zlogf(int msg_level, char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif
    if(msg_level <= ZLOG_LOG_LEVEL){
        va_list va;
        char* buffer = NULL;

        va_start(va, fmt);
        buffer = zlog_get_buffer();

        vsnprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, fmt, va);
        zlog_finish_buffer();
        va_end(va);
    }
}

void zlogf_time(int msg_level, char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif

    if(msg_level <= ZLOG_LOG_LEVEL){
        char timebuf[ZLOG_BUFFER_TIME_STR_MAX_LEN];
        struct timeval tv;
        //time_t curtime;
        char* buffer = NULL;

        // time_t ltime;
        // ltime = time(NULL);

        va_list va;

        gettimeofday(&tv, NULL);
        //curtime=tv.tv_sec;
        struct tm *tm_struct = localtime(&tv.tv_sec);
        snprintf(timebuf, ZLOG_BUFFER_TIME_STR_MAX_LEN, "%d:%02d:%d", tm_struct->tm_hour, tm_struct->tm_min, tm_struct->tm_sec);
        buffer = zlog_get_buffer();
        snprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, "[%s.%06lds] ", timebuf, tv.tv_usec);
        buffer += strlen(timebuf) + 11; // space for time

        va_start(va, fmt);
        vsnprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, fmt, va);
        zlog_finish_buffer();
        va_end(va);
    }
}

void zlog_time(int msg_level, char* filename, int line, char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif
    if(msg_level <= ZLOG_LOG_LEVEL){
        static char timebuf[ZLOG_BUFFER_TIME_STR_MAX_LEN];
        struct timeval tv;
        time_t curtime;
        char* buffer = NULL;

        va_list va;

        gettimeofday(&tv, NULL);
        curtime=tv.tv_sec;
        struct tm *tm_struct = localtime(&tv.tv_sec);
        snprintf(timebuf, ZLOG_BUFFER_TIME_STR_MAX_LEN, "%ld", curtime);
        snprintf(timebuf, ZLOG_BUFFER_TIME_STR_MAX_LEN, "%d:%02d:%d", tm_struct->tm_hour, tm_struct->tm_min, tm_struct->tm_sec);

        buffer = zlog_get_buffer();
        snprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, "[%s.%06lds] [@%s:%d] ", timebuf, tv.tv_usec, filename, line);
        buffer += strlen(buffer); // print at most 5 digit of line

        va_start(va, fmt);
        vsnprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, fmt, va);
        zlog_finish_buffer();
        va_end(va);
    }
}

void zlog(int msg_level, char* filename, int line, char const * fmt, ...)
{
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif
    if(msg_level <= ZLOG_LOG_LEVEL){
        char* buffer = NULL;
        va_list va;

        buffer = zlog_get_buffer();
        snprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, "[@%s:%d] ", filename, line);
        buffer += strlen(buffer);
        va_start(va, fmt);

        vsnprintf(buffer, ZLOG_BUFFER_STR_MAX_LEN, fmt, va);
        zlog_finish_buffer();
        va_end(va);
    }
}

const char* zlog_get_log_file_name(void){
#ifdef ZLOG_DISABLE_LOG
    return ;
#endif
    return zlog_file_log_name;
}

// End zlog utilities

