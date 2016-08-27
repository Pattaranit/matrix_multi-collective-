#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
FILE *fpA,*fpB,*fpO;

int **alloc_2d_int(int rows,int cols){
    int i ;
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array = (int **)malloc(rows*sizeof(int*));
    for (i=0; i<rows; i++) {
        array[i] = &(data[cols*i]);
    }
    return array;
}

int main(int argc, char *argv[]) {

	int rank, nproc;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	int **mat1=NULL;
	int **mat2=NULL;
	int **matR=NULL;
	int nrows1=0,ncolumns1=0;
	int nrows2=0,ncolumns2=0;
	int temp=0;
	
	int n=0,mod=0;

	if(rank==0){
	/* read file */

		fpA = fopen("D:\\3rdYrs\\first term\\CPE374 Parallel\\Lab\\MatrixMultipleDataSet_Small\\matrix1.txt","r");
		fscanf(fpA,"%d %d",&nrows1,&ncolumns1);
		fpB = fopen("D:\\3rdYrs\\first term\\CPE374 Parallel\\Lab\\MatrixMultipleDataSet_Small\\matrix2.txt","r");
		fscanf(fpB,"%d %d",&nrows2,&ncolumns2);

		mat1 = alloc_2d_int(nrows1,ncolumns1);
		mat2 = alloc_2d_int(nrows2,ncolumns2);
	
		int i,j;

		for(i=0;i<nrows1;i++){
			for(j=0;j<ncolumns1;j++){
				fscanf(fpA,"%d ",&mat1[i][j]);
			}
		}
		
		fclose(fpA);
		for(i=0;i<nrows2;i++){
			for(j=0;j<ncolumns2;j++){
				fscanf(fpB,"%d ",&mat2[i][j]);
			}
		}
		fclose(fpB);

		n=nrows1/nproc;
		mod=nrows1%nproc;
	}

	MPI_Bcast(&nrows1, 1,MPI_INT, 0,MPI_COMM_WORLD);
	MPI_Bcast(&nrows2, 1,MPI_INT, 0,MPI_COMM_WORLD);
	MPI_Bcast(&ncolumns1, 1,MPI_INT, 0,MPI_COMM_WORLD);
	MPI_Bcast(&ncolumns2, 1,MPI_INT, 0,MPI_COMM_WORLD);
	MPI_Bcast(&n, 1,MPI_INT, 0,MPI_COMM_WORLD);
	MPI_Bcast(&mod, 1,MPI_INT, 0,MPI_COMM_WORLD);

	if(rank!=0){
		mat2 = alloc_2d_int(nrows2,ncolumns2);
	}
	
	MPI_Bcast(&mat2[0][0], nrows2*ncolumns2,MPI_INT, 0,MPI_COMM_WORLD);

	
	if(rank==0){
		int i,j,k;

		for (i=1; i<nproc; i++) {

			MPI_Send(&mat1[(n*i)+mod][0], n*ncolumns1, MPI_INT, i, 10, MPI_COMM_WORLD);

        }

		matR = alloc_2d_int(nrows1,ncolumns2);

		for (i=0; i<n+mod; i++) {
            for (j=0; j<ncolumns2; j++) {
                matR[i][j]=0;
                for (k=0; k<nrows2; k++) {
                    matR[i][j] += mat1[i][k]*mat2[k][j];
                }
            }
        }

		for (i=1; i<nproc; i++) {
            MPI_Recv(&matR[(n*i)+mod][0], n*ncolumns2, MPI_INT,i, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

	}
	else{
		mat1 = alloc_2d_int(n,ncolumns1);

		MPI_Recv(&mat1[0][0],n*ncolumns1, MPI_INT, 0, 10, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		matR = alloc_2d_int(n,ncolumns2);

		int i,j,k;

		for (i=0; i<n; i++) {
            for (j=0; j<ncolumns2; j++) {
                matR[i][j]=0;
                for (k=0; k<nrows2; k++) {
                    matR[i][j] += mat1[i][k]*mat2[k][j];
                }
            }
        }

		MPI_Send(&matR[0][0], n*ncolumns2, MPI_INT, 0, 12, MPI_COMM_WORLD);
		
	}

	MPI_Finalize();
	return 0;
}