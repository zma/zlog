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
    zlogf_time("zlog is initialized\n");

    // without timestamp
    zlogf("Command line: \n");
    for (i = 0; i < argc; i++) {
        // use the directives as the format of printf
        zlogf("argv[%d]: %s \n", i, argv[i]);
    }
    zlogf("\n\n");

    zlogf_time("I flush the buffer explicitly.\n");
    zlog_flush_buffer();

    // with source code file and line info, and timestamp
    zlog_time(ZLOG_LOC, "Log with file and line. TIMESTAMPED.\n");

    // with source code file and line info, and without timestamp
    zlog(ZLOG_LOC, "Log with file and line. NO TIMESTAMP.\n");

    // Example only: let the flushing thread work
    printf("FOR TEST: I will sleep for 10 seconds... Please be patient.\n");
    sleep(10);

    zlogf_time("finish using zlog\n");

    // Before quit, finish zlog which will flush all the
    // logs in memory
    zlog_finish();

    return 0;
}

