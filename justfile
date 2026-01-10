[private]
@default:
    just --list

run file *args: (build file)
    ./bin/{{file}} {{args}}

build file *args: clean
    gcc -std=c99 -Wall -Werror -fsanitize=address -o ./bin/{{file}} src/{{file}}.c {{args}}

debug file: (build file "-g")
    -valgrind --leak-check=full ./bin/{{file}} -s

[private]
clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

[private]
test file: (build file) 
    -./bin/{{file}} -f test.db
    ./bin/{{file}} -f test.db -n
    ./bin/{{file}} -f test.db
 
