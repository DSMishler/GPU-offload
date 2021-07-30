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

int main(void)
{
   printf("Jacobi iteration offload test\n");



   double err, tol;
   int iter, iter_max,n,m;
   iter = 0;
   iter_max = 50000;
   n = m = 200;
   err = 1; // just start it greater than tol
   tol = 1e-4;
   double *A = malloc(n*m*sizeof(double));
   assert(A);
   double *Anew = malloc(n*m*sizeof(double));
   assert(Anew);

   srand(0);
   for(int i = 0; i < n; i++)
   {
      for(int j = 0; j < m; j++)
      {
         A[i*m + j] = (double)rand()/(double)RAND_MAX;
      }
   }
   // showArray(A,n,m);
///////////////////////////////////////////////////////////////////////////////
   double time_start = omp_get_wtime();
#pragma omp target data map(alloc:Anew) map(A) map(iter)
{
   while(err > tol && iter < iter_max)
   {
      err = 0.0;
      #pragma omp target teams distribute parallel for \
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
      #pragma omp target teams distribute parallel for
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
