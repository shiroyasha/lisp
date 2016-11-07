.PHONY: build test

build:
	cc -std=c99 -Wall lispy.c src/*.c -I include -ledit -lm -o lispy

test:
	bash test/run.sh
