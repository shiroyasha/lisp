.PHONY: build

build:
	cc -std=c99 -Wall mpc.c lispy.c -ledit -lm -o lispy
