#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

double max(double a, double b)
{
   return ((a > b) ? (a) : (b));
}

void showArray(double *A,int n, int m)
{
   for(int j = 0; j < m; j++) //note this function goes through j in the outer loop
   {
      printf("\n");
      for(int i = 0; i < n; i++)
      {
	 printf("%-4.3f  ", A[i*m + j]);
      }
   }
   printf("\n");
}

int main(int nargs, char **argv)
{
   printf("\nJacobi iteration offload test: CPU\n");
   int iter, iter_max,n,m;

   if(nargs == 1)
   {
      n = m = 200;
   }
   else if(nargs == 2)
   {
      m = 200;
      n = atoi(argv[1]);
   }
   else // if(nargs > 2)
   {
      n = atoi(argv[1]);
      m = atoi(argv[2]);
   }
   double err, tol;
   iter_max = 50000;
   err = 1; // just start it greater than tol
   tol = 1e-2;
   double *A = malloc(n*m*sizeof(double));
   assert(A);
   double *Anew = malloc(n*m*sizeof(double));
   assert(Anew);
   printf("m=%d\tn=%d\ttol=%g\n",m,n,tol);

   srand(0);
   for(int i = 0; i < n; i++)
   {
      for(int j = 0; j < m; j++)
      {
         A[i*m + j] = (double)rand()/(double)RAND_MAX;
      }
   }
   // showArray(A,n,m);
   iter = 0;
///////////////////////////////////////////////////////////////////////////////
   double time_start = omp_get_wtime();
   while(err > tol && iter < iter_max)
   {
      err = 0.0;
      #pragma omp parallel for \
        reduction(max:err)
      for(int i = 1; i < n-1; i++)
      {
         for(int j = 1; j < m-1; j++)
         {
            Anew[i*m + j] = 0.25 * (A[i*m + j+1]+A[i*m + j-1]+
		                    A[(i-1)*m + j]+A[(i+1)*m + j]); // 4 flops
	    err = max(err, fabs(Anew[i*m + j] - A[i*m + j])); // 2 flops,
	                                                     //3 if abs() counts
         }
      }
      #pragma omp parallel for
      for(int i = 1; i < n-1; i++)
      {
         for(int j = 1; j < m-1; j++)
         {
            A[i*m + j] = Anew[i*m + j];
         }
      }
      // printf("iter: %02d\terr: %.5f \ttol: %.5f\n",iter,err,tol);
      iter++;
   }
   double time_stop = omp_get_wtime();
///////////////////////////////////////////////////////////////////////////////
   double time_total = time_stop-time_start;
   double GFlopss;
   
   printf("iters: %d\n", iter);

   GFlopss = (4.0+2.0)*m*n*iter/time_total/1e9;
   printf("process completed with:\n");
   printf("walltime of %8.4fs\n",time_total);
   printf("GFlops/s of %8.4f\n",GFlopss);

   free(A);
   free(Anew);
   return 0;
}
