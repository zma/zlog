# In order to compile in DEBUG mode, simply add -D DEBUG flag to gcc.
# If you dont want debug mode, simply delete it. 

default: info

info:
	 gcc -Wall -Wextra example.c zlog.c -o example -lpthread
	 ./example

debug:
	gcc -Wall -Wextra example.c zlog.c -o example -lpthread -D DEBUG
	 ./example