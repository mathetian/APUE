cc = g++
flags = -lpthread

pthread1: pthread1.c
	$(cc)  $^ -o $@ $(flags)
cleanup: cleanup.c
	$(cc)  $^ -o $@ $(flags)
recursive: recursive.c
	$(cc) $^ -o $@ $(flags)
getenv_r: getenv_r.c
	$(cc) $^ -o $@ $(flags)
synchronous_signal: synchronous_signal.c
	$(cc) $^ -o $@ $(flags)
reReadingDeamon: reReadingDeamon.c
	$(cc) $^ -o $@ $(flags)
reReadingDeamon2: reReadingDeamon2.c
	$(cc) $^ -o $@ $(flags)
DB: process.c
	$(cc) $^ -o db $(flags) lib_db.a
static: db.c helper.c
	gcc -Wall -c $^
	