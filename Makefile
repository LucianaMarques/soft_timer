main: soft_timer.o /usr/include/time.h hmcu_timer.h
	cc -o main.c sof_timer.o


soft_timer.o: soft_timer.h hmcu_timer.h /usr/include/inttypes.h /usr/include/time.h /usr/include/math.h
	cc -c soft_timer.c

clean:
	rm main soft_timer.o

