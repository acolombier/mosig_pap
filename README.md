## Instruction

For the sake of simplicity, we used a static 4 x 4 matrix declared in the file, which implies that you need to run MPI with `-n 16`

The Fox algorithm can be tested by running the following command:

```
make exercice1and2 && mpirun -n 16 exercice1and2 --algorithm fox
```

The Cannon algorithm can be tested by running the following command:

```
make exercice1and2 && mpirun -n 16 exercice1and2 --algorithm cannon
```

The data mangement version can be tested by running the following command:

```
make exercice3 && mpirun -n 16 exercice3 --algorithm <ALGO>
```
