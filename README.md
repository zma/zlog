# zlog #

A C In-memory Logging Library

Hompage: http://www.ericzma.com/projects/zlog/

## Introduction ##

zlog is a C in-memory logging library. It allocates a buffer in memory for logging, and flush the buffer to the log file (or stdout) when the buffer is full, or it is instructed explicitly by the program or flushing thread.

### Ejecution example ###

Example.c represents an use example of zlog library containing both INFO
and DEBUG messages. If you want to include DEBUG messages you must compile
with *-D DEBUG* flag. 

Makefile contains the corresponding gcc compilation commands to both INFO
and DEBUG mode. Please keep in mind that DEBUG mode will print INFO and
DEBUG messages whilst INFO mode will discard all DEBUG messages. The 
following Make commands can be used to compile either in DEBUG or INFO modes:

* ``make`` or ``make info`` for **INFO** mode compilation.
* ``make debug`` for **DEBUG** mode compilation. 

## Source code ##

Source code of zlog can be downloaded from http://github.com/zma/zlog/ . The source code is released into the public domain.

## Author ##

Zhiqiang Ma (http://www.ericzma.com).


