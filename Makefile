shell:
	g++ ./src/myshell.c -o ./bin/myshell
	g++ ./src/shell.c -o ./bin/shell

shell_run:
	./bin/shell