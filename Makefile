shell:
	gcc ./src/shell.c -o ./bin/shell

shell_optimized:
	gcc ./src/shell.c -O3 -o ./bin/shelloptimized

run:
	./bin/shell

run_optimized:
	./bin/shelloptimized
	./bin/shelloptimized