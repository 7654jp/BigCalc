bigcalc: main.c ibigcalc.c fbigcalc.c
	gcc main.c ibigcalc.c fbigcalc.c -o bigcalc
clean:
	rm -f bigcalc
