.PHONY: build

build:
	cc -std=c99 -Wall lispy.c src/*.c -I src -ledit -lm -o lispy
