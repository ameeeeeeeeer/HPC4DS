#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y); //to read feature matrix
void transpose_calculation_variant(int m, int n, int nnz, int* csrRowPtr, int* csrColldx, int* csrVal, int* cscCollPtr, int* cscRowldx, int* cscVal); //transpose calulation

main(int argc, char **argv)
{
	//GENRAL VARIABLE
	/*used for cycle*/
	int i;
	int j;
	/*used for performance measure*/
	struct timeval start, end;
	
	//ACCESS TO FILE
	/*access to file*/
	FILE* matrix_file=fopen(argv[1], "r");
	/*access to first line infos*/
    int	x_num=0; //number of x
	int y_num=0; //numbero of y
	int b_num=0; //number of channels
	fscanf( matrix_file, "%i", &x_num);
	getc(matrix_file);
	fscanf( matrix_file, "%i", &y_num);
	getc(matrix_file);
	fscanf( matrix_file, "%i", &b_num);
	int limit = x_num*y_num*b_num;

    //1. USED MATRICES CONSTRUCTION
    /*construction of feature and result matrix*/
    int *y; 
    int **x; 
    y=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
    x=(int **)malloc((x_num*y_num*b_num) * sizeof(int*));
    int** x_transpose=(int **)malloc((b_num+2) * sizeof(int*)); //for the transpose
    int** x_product=(int **)malloc((b_num+2) * sizeof(int*)); //for the product of x and transpose
    /*initialization of matrix used*/
	//for x
	gettimeofday(&start, NULL);
	create_pointer(x, (x_num*y_num*b_num), (b_num+2));
	initialize_matrix(x, (b_num+2), (x_num*y_num*b_num));	 //all its value are setted to 0
	gettimeofday(&end, NULL);
    printf("time for creating and initializing x matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	//for x_transpose
	gettimeofday(&start, NULL);
	create_pointer(x_transpose, (b_num+2), (x_num*y_num*b_num));
	gettimeofday(&end, NULL);
    printf("time for creating and initializing x_transpose matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	//for x_product
    gettimeofday(&start, NULL);
	create_pointer(x_product, (b_num+2), (b_num+2));
	gettimeofday(&end, NULL);
    printf("time for creating and initializing x_product matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	/*read and create X matrix*/
    gettimeofday(&start, NULL);
    read_matrix(matrix_file,  x_num, y_num, b_num, x,  y);
	gettimeofday(&end, NULL);
    printf("time for reading main matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	/*close the file*/
	fclose(matrix_file);
	



	//3. TRANSPOSE CALCULATION AND PRODUCT
	/*transpose calculation*/
	int* csrRowPtr=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
	for ( i=0; i<limit; i++)
	{
	
			csrRowPtr[i]=i;
		
	}
	int* csrVal=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
	for ( i=0; i<limit; i++)
	{
		csrVal[i]=1;
	}
	int* csrColldx=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
	for ( i=0; i<limit; i++)
	{
		
		csrColldx[i]=i%b_num;
	}
	int* cscColPtr=(int*)malloc((b_num) * sizeof(int));
	for ( i=0; i<b_num; i++)
	{
		
		cscColPtr[i]=0;
	}
	int* cscVal=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
	for ( i=0; i<limit; i++)
	{
		
		cscVal[i]=0;
	}
	
	int* cscRowldx=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
	for ( i=0; i<limit; i++)
	{
		
		cscRowldx[i]=0;
	}
	
	gettimeofday(&start, NULL);
	transpose_calculation_variant(limit, b_num, limit, csrRowPtr, csrColldx, csrVal, cscColPtr, cscRowldx, cscVal); //transpose calulation
	gettimeofday(&end, NULL);
	printf("time for transpose  %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));

	
	
	for ( i=0; i<limit; i++)
	{
		
		printf("%i ", cscRowldx[i]);
	}
	
	printf("\n");
	
	for ( i=0; i<limit; i++)
	{
		
		printf("%i ", cscVal[i]);
	}
		printf("\n");

	
	for ( i=0; i<b_num; i++)
	{
		
		printf("%i ",  cscColPtr[i]);
	}
		printf("\n");

	
	return 0;
	
	
}


void create_pointer(int** matrix, int y, int value)
{
	 int i;


	 for(i=0; i<y; i++){
    	
    	matrix[i] = (int *)malloc((value) * sizeof(int));
 
	}
}


void initialize_matrix(int** matrix, int x, int y)
{
	int i;
	int j;
	
	for(i=0; i<y; i++)
	{
		for(j=0; j<x; j++)
		{
			matrix[i][j]=0;
		}
	}
	

}

void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y)
{
	int i;
	int j;
	
	for(i=0; i<(x_num*y_num); i++)
    {
    	int matrix_value_x=0;
    	int matrix_value_y=0;
    	fscanf( matrix_file, "%i", &matrix_value_x);
    	getc(matrix_file);	
    	fscanf( matrix_file, "%i", &matrix_value_y);
    	getc(matrix_file);	

    	
    	int a = i*7;
    	int b=a+7;
    	
    	for(a; a<b; a++)
    	{
    		x[a][0]= matrix_value_x;
    		x[a][1]= matrix_value_y;
    		
		}
		
		
		a = i*7; 
		b= a;
    	
		int matrix_value=0;
		
	
    	for(j=2; j<(b_num+2); j++)
    	{
    		fscanf( matrix_file, "%i", &matrix_value);
    		getc(matrix_file);		
    		
			x[b][j]=1;
			
			
    		
			int index=(j-2)+a;
			    	    
    		y[index]=matrix_value;
    		
    	    b=b+1;
    	
		}
	}

	
	
}


void transpose_calculation_variant(int m, int n, int nnz, int* csrRowPtr, int* csrColldx, int* csrVal, int* cscColPtr, int* cscRowldx, int* cscVal) //transpose calulation
{
	int i;
	int j;
	
	int* curr=(int*)malloc((n) * sizeof(int));
	
	
	for(i=0; i<m; i++)
	{
		
		for(j=csrRowPtr[i]; j<csrRowPtr[i+1]; j++)
		{
			cscColPtr[csrColldx[j]+1]++;
		}
	
	}
	
	int* cscColPtr1=(int*)malloc((b_num) * sizeof(int));
	#pragma omp parallel for \
		default(none) private(i,j) shared( cscColPtr1)
	for(i=1; i<n+1; i++)
	{
		
		for(j=0; j<i; j++)
		{
			value+=cscColPtr[i];
		}
		
		cscColPtr1[i]=value;
	
	}
	
	
	#pragma omp parallel for \
			default(none) private(i) shared(csrColPtr, cscColPtr1)
	for(i=1; i<n+1; i++)
	{
		cscColPtr[i]+=cscColPtr1[i];
	
    }	
	
	
	#pragma omp parallel for \
			default(none) private(i,j) shared(cscVal, csrVal, cscRowldx, csrColldx, cscColPtr)
	for(i=0; i<m; i++)
	{
		for(j=csrRowPtr[i]; j<csrRowPtr[i+1];j++)
		{
			int loc= cscColPtr[csrColldx[j]]+curr[csrColldx[j]]++;
			cscRowldx[loc]=i;
			cscVal[loc]=csrVal[j];
		}
	}
	

}
