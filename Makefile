default:
	icc -Wall -pedantic -fopenmp -O3 -o itestGEMM naivegemm.c
	# icc -Wall -pedantic -fopenmp -O3 -o itestjacobi jacobi.c
	icx -Wall -pedantic -fiopenmp -fopenmp -O3 -o itestjacobi jacobi.c

debug:
	icc -Wall -pedantic -fopenmp -g -o testGEMM naivegemm.c
	icc -Wall -pedantic -fopenmp -g -o testjacobi jacobi.c

gcc: 
	gcc -Wall -pedantic -fopenmp -O3 -o testGEMM naivegemm.c
	gcc -Wall -pedantic -fopenmp -O3 -o testjacobi jacobi.c

# Experiment with differences between gcc and icc
