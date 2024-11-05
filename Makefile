s: compile run clean

compile: serverv4.c 
	gcc serverv4.c -o echo_s

run: 
	./echo_s -p 8080

clean:
	rm echo_s

