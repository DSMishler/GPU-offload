#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <assert.h>

// intel devcloud: use qsub -I to start up on the cores you want.

void generateMatrix(double**M, int rows, int columns, double scale);
void printMatrix(double *A, int rows, int columns);
void multiplyMatrix(double *A, int Arows, int Acolumns,
                    double *B, int Brows, int Bcolumns,
                    double *C);
void multiplyMatrixSingleThreaded(double *A, int Arows, int Acolumns,
                                 double *B, int Brows, int Bcolumns,
                                 double *C);
double numFlops(int m, int n, int k);

int main(void)
{
   printf("General Matrix Multiply (gemm) OpenMP Offload project\n");

   double *A, *B, *C;
   int aRows,aColumns,bRows,bColumns,cRows,cColumns,m,n,k;
   m = cRows = aRows = 1000; // m
   n = cColumns = bColumns = 1000; // n
   k = bRows = aColumns = 1000; // k

   int printing=0;
   srand(1);

   generateMatrix(&A,aRows,aColumns,1);
   generateMatrix(&B,bRows,bColumns,1);
   generateMatrix(&C,cRows,cColumns,0);
   if (printing)
   {
      printMatrix(A,aRows,aColumns);
      printMatrix(B,bRows,bColumns);
   }

   double time_start = omp_get_wtime();
   multiplyMatrixSingleThreaded(A,aRows,aColumns,B,bRows,bColumns,C);
   double time_stop = omp_get_wtime();
   double time_total = time_stop - time_start;
   double GFlopss = numFlops(m,n,k)/time_total/1e9;

   if (printing)
   {
      printMatrix(C,cRows,cColumns);
   }

   printf("process completed with:\n");
   printf("walltime of %8.4fs\n",time_total);
   printf("GFlops/s of %8.4f\n",GFlopss);
   return 0;
}

void generateMatrix(double**Mp, int rows, int columns, double scale)
{
   double*M;
   M = malloc(rows*columns*sizeof(double));
   assert(M);
   int i;
   for(i = 0; i < rows*columns; i++)
   {
      M[i] = (double)rand()/(double)RAND_MAX*scale;
   }
   *Mp = M;
}

void printMatrix(double *A, int rows, int columns)
{
   int i,j;
   int lda = rows;
   printf("\n");
   for(i = 0; i < rows; i++)
   {
      for(j = 0; j < columns; j++)
      {
         printf("%-4.3f  ",A[i + lda*j]);
      }
      printf("\n");
   }
}

void multiplyMatrix(double *A, int Arows, int Acolumns,
                    double *B, int Brows, int Bcolumns,
                    double *C)
{
   int i,j,k;
   int Crows = Arows;
   int Ccolumns = Bcolumns;
   if (Acolumns != Brows)
   {
      printf("Can't multiply these matrices!\n");
      return;
   }
   int depth = Acolumns;
   int lda = Arows;
   int ldb = Brows;
   int ldc = Crows;

   // thread-safe as long as no two threads work the same row at same time.
   #pragma omp parallel for
   for(i = 0; i < Crows; i++)
   {
      for(j = 0; j < Ccolumns; j++)
      {
         C[i + lda*j] = 0;
         for(k = 0; k < depth; k++)
         {
            C[i + ldc*j] += A[i + lda*k]*B[k + ldb*j];
         }
      }
   }
}

void multiplyMatrixSingleThreaded(double *A, int Arows, int Acolumns,
                    double *B, int Brows, int Bcolumns,
                    double *C)
{
   int i,j,k;
   int Crows = Arows;
   int Ccolumns = Bcolumns;
   if (Acolumns != Brows)
   {
      printf("Can't multiply these matrices!\n");
      return;
   }
   int depth = Acolumns;
   int lda = Arows;
   int ldb = Brows;
   int ldc = Crows;

   for(i = 0; i < Crows; i++)
   {
      for(j = 0; j < Ccolumns; j++)
      {
         C[i + lda*j] = 0;
         for(k = 0; k < depth; k++)
         {
            C[i + ldc*j] += A[i + lda*k]*B[k + ldb*j];
         }
      }
   }
}

double numFlops(int m, int n, int k)
{
   return 2.0*m*n*k;
}
