shell:
	g++ ./src/shell.c -o ./bin/shell

shell_optimized:
	g++ ./src/shell.c -o3 -o ./bin/shelloptimized

run:
	./bin/myshell

run_optimized:
	./bin/shelloptimized