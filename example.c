/*
 * Bug reports to: Zhiqiang Ma (http://fclose.com/zma/)
 * An example of zlog usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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
    
    zlogf_time("zlog is initialized\n");

    zlogf_time("Command line: \n");
    for (i = 0; i < argc; i++) {
        zlogf_time("%s ", argv[i]);
    }

    // Example only
    printf("FOR TEST: I will sleep for 600 seconds... \n"
            "Please be patient.\n");
    sleep(600);

    zlogf_time("finish using zlog\n");

    // Before quit, finish zlog which will flush all the 
    // logs in memory
    zlog_finish();
    
    return 0;
}

