#
# Makefile
# jorgen, 2017-09-03 13:44
#

all: clean
	gcc src/*.c -O2 -o bin/webserver

clean:
	rm -f bin/webserver || true

run: all
	./bin/webserver 8188 public


# vim:ft=make
#
