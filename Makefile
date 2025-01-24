shell:
	g++ ./src/myshell.c -o ./bin/myshell
	g++ ./src/shell.c -o ./bin/shell

run:
	./bin/myshell