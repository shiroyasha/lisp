.PHONY: build

build:
	cc -std=c99 -Wall lispy.c src/*.c -I include -ledit -lm -o lispy
