

exercice1and2: exercice1and2.c
	mpicc exercice1and2.c -o exercice1and2 -lm
    
exercice3: exercice3.c
	mpicc exercice3.c -o exercice3 -lm

clean:
	rm -rf exercice3 exercice1and2
