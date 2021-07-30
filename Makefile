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

timingTest:
	icx -Wall -pedantic -fiopenmp -fopenmp -O3 -o itestjacobiCPU jacobiCPU.c
	icx -Wall -pedantic -fiopenmp -fopenmp -O3 -o itestjacobiGPU0 jacobiGPUBase.c
	icx -Wall -pedantic -fiopenmp -fopenmp -O3 -o itestjacobiGPU1 jacobiGPUMap.c
	icx -Wall -pedantic -fiopenmp -fopenmp -O3 -o itestjacobiGPU2 jacobiGPUSC.c
	cat times.txt >> oldtimings.txt
	./itestjacobiCPU  1000 1000 >  times.txt
	./itestjacobiGPU0 1000 1000 >> times.txt
	./itestjacobiGPU1 1000 1000 >> times.txt
	./itestjacobiGPU2 1000 1000 >> times.txt

clean:
	rm itestjacobiCPU itestjacobiGPU0 itestjacobiGPU1 itestjacobiGPU2 \
	itestjacobi itestGEMM testGEMM testjacobi


# Experiment with differences between gcc and icc
