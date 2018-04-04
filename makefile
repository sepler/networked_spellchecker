# Stephen Epler, cis3207 - Operating Systems Lab 3: Networked spellchecker
myshell: main.c
	gcc spellchecker.h spellchecker.c main.c queue.h queue.c -lpthread -o spellcheck