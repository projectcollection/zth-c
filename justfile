# compiler_flags := "-Wall -Wunused-parameter -Werror -fsanitize=address"
compiler_flags := "-Wall -Wunused-parameter -Werror"

[private]
@default:
    just --list

run file *args: (build file)
    ./bin/{{file}} {{args}}

build file *args: clean
    gcc {{compiler_flags}} -std=c99 -o ./bin/{{file}} src/{{file}}.c {{args}}

debug file *args: (build file "-g")
    -valgrind --leak-check=full ./bin/{{file}} {{args}}  -s

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
    ./bin/{{file}} -f test.db -a "Timmy H.,123 Sheshire Ln., 120"
    ./bin/{{file}} -f test.db -a "Timmy H.,123 Sheshire Ln., 120"
    ./bin/{{file}} -f test.db -a "Timmy H.,123 Sheshire Ln., 120"
    ./bin/{{file}} -f test.db -a "Timmy H.,123 Sheshire Ln., 120"
    ./bin/{{file}} -f test.db -l 
 
