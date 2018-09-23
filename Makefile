soft_timer.o: soft_timer.h hmcu_timer.h /usr/include/inttypes.h /usr/include/time.h /usr/include/math.h
	gcc soft_timer.c -lm -o soft_timer

clean:
	rm soft_timer

