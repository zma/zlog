#ifndef ZLOG_H_
# define ZLOG_H_

#define ZLOG_LOC __FILE__, __LINE__

#define ZLOG_DEBUG_LOG_MSG 1
#define ZLOG_INFO_LOG_MSG 0

#ifdef DEBUG
	#define ZLOG_LOG_LEVEL 1
#else
	#define ZLOG_LOG_LEVEL 0
#endif

extern FILE* zlog_fout;

// Start API

// initialize zlog: flush to a log file
void zlog_init(char const* log_file);
// initialize zlog: flush to a STDOUT
void zlog_init_stdout(void);
// creating a flushing thread
void zlog_init_flush_thread();
// finish using the zlog; clean up
void zlog_finish();
// explicitely flush the buffer in memory
void zlog_flush_buffer();

// log an entry; using the printf format
void zlogf(int msg_level, char const * fmt, ...);

// log an entry with a timestamp
void zlogf_time(int msg_level, char const * fmt, ...);

// log an entry with the filename and location;
//   the first 2 arguments can be replaced by ZLOG_LOC which 
//   will be filled by the compiler
void zlog(int msg_level, char* filename, int line, char const * fmt, ...);

// log an entry with the filename and location with a timestamp
void zlog_time(int msg_level, char* filename, int line, char const * fmt, ...);

// End API

#endif
