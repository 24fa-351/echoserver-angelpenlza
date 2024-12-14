s: compile run clean

compile: echo.c 
	gcc echo.c -o echo_s

run: 
	./echo_s -p 8080

clean:
	rm echo_s

