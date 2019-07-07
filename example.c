/*
 * Bug reports to: Zhiqiang Ma (http://www.ericzma.com)
 * An example of zlog usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "zlog.h"

int main(int argc, char* argv[])
{
    int i = 0;

    // log to stdout
    zlog_init_stdout();

    // or log to file
    // zlog_init("/tmp/zlog.log");

    // Optional: create flushing thread that flush the log
    //     for each period of time
    // Otherwise, the in-memory log is flushed when the buffer
    //     if full
    zlog_init_flush_thread();

    // start to use the in-memory log

    // with timestamp
    zlogf_time(ZLOG_DEBUG_LOG_MSG, "zlog is initialized (debug)\n");
    zlogf_time(ZLOG_INFO_LOG_MSG, "zlog is initialized (info)\n");

    // without timestamp
    zlogf(ZLOG_INFO_LOG_MSG, "Command line: \n");
    for (i = 0; i < argc; i++) {
        // use the directives as the format of printf
        zlogf(ZLOG_INFO_LOG_MSG, "argv[%d]: %s \n", i, argv[i]);
        zlogf(ZLOG_DEBUG_LOG_MSG, "(DEBUG) argv[%d]: %s \n", i, argv[i]);
    }
    zlogf(ZLOG_INFO_LOG_MSG, "\n\n");

    zlogf_time(ZLOG_INFO_LOG_MSG, "I flush the buffer explicitly.\n");
    
    zlogf(ZLOG_INFO_LOG_MSG, "This is just a ## [+] debug message [+] ##\n");
    zlog_flush_buffer();

    // with source code file and line info, and timestamp
    zlog_time(ZLOG_INFO_LOG_MSG, ZLOG_LOC, "Log with file and line. TIMESTAMPED.\n");
    zlog_time(ZLOG_DEBUG_LOG_MSG, ZLOG_LOC, "(DEBUG) Log with file and line. TIMESTAMPED.\n");

    // with source code file and line info, and without timestamp
    zlog(ZLOG_INFO_LOG_MSG, ZLOG_LOC, "Log with file and line. NO TIMESTAMP.\n");

    // Example only: let the flushing thread work
    printf("FOR TEST: I will sleep for 200 seconds... Please be patient.\n");
    sleep(200);

    zlogf_time(ZLOG_INFO_LOG_MSG, "finish using zlog\n");

    // Before quit, finish zlog which will flush all the
    // logs in memory
    zlog_finish();

    return 0;
}

