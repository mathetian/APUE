cc = g++
flags = -lpthread

pthread1: pthread1.c
	$(cc)  $^ -o $@ $(flags)
cleanup: cleanup.c
	$(cc)  $^ -o $@ $(flags)
