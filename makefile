all: alarmclock.c
	gcc -g -Wall -o alarmclock alarmclock.c

clean:
	$(RM) alarmclock