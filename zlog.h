#ifndef ZLOG_H_
# define ZLOG_H_

#define ZLOG_LOC __FILE__, __LINE__

// deprecated
#define LOC __FILE__, __LINE__

extern FILE* zlog_fout;
void zlog_init(char const* log_file);
void zlog_init_stdout(void);
void zlog_init_flush_thread();
void zlog_finish();
void zlog_flush_buffer();
void zlogf(char const * fmt, ...);
void zlogf_time(char const * fmt, ...);
void zlog(char* filename, int line, char const * fmt, ...);
void zlog_time(char* filename, int line, char const * fmt, ...);

#endif
