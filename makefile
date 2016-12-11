output: control.c interact.c
	gcc control.c -o control
	gcc interact.c -o interact

clean:
	rm control
	rm interact

