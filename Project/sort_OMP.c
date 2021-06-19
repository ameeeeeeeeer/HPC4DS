#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>


//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void create_pointer_float(float** matrix, int y, int value);
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y); //to read feature matrix
void sort(float* eigenvalues, float** eigenvectors, int b_num);//for sorting

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
	int limit=x_num*y_num;

    //1. USED MATRICES CONSTRUCTION
    /*construction of feature and result matrix*/
    int *y; 
    int **x; 
    y=(int*)malloc((x_num*y_num) * sizeof(int));
    x=(int **)malloc((x_num*y_num) * sizeof(int*));
    int** x_transpose=(int **)malloc((b_num+2) * sizeof(int*)); //for the transpose
    int** x_product=(int **)malloc((b_num+2) * sizeof(int*)); //for the product of x and transpose
    /*initialization of matrix used*/
	//for x
	gettimeofday(&start, NULL);
	create_pointer(x, (x_num*y_num), (b_num+2));
	initialize_matrix(x, (b_num+2), (x_num*y_num));	 //all its value are setted to 0
	gettimeofday(&end, NULL);
    printf("time for creating and initializing x matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));




	//for x_transpose
	gettimeofday(&start, NULL);
	create_pointer(x_transpose, (b_num+2), (x_num*y_num));
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
	

	
	//4. EIGENVALUE CALCULATION
	float* eigenvalues=(float*)malloc((b_num+2) * sizeof(float));
	/*temporary*/
	for(i=0; i<(b_num+2); i++)
	{
		eigenvalues[i]=i;
	}
	//eigenvectors matrix
	float** eigenvectors=(float**)malloc((b_num+2) * sizeof(float*));
	create_pointer_float(eigenvectors, (b_num+2), (b_num+2));
	/*temporary*/
	for(i=0; i<(b_num+2); i++)
	{
		for(j=0; j<(b_num+2); j++)
		{
			eigenvectors[i][j]=i*j;
		}
		
	}
	
	//5. EIGENVALUES ORDERING AND EIGENVECTOR MATRIX CONSTRUCTION
	gettimeofday(&start, NULL);
	sort(eigenvalues, eigenvectors, b_num);
	gettimeofday(&end, NULL);
    printf("time for sorting main matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
    
  
       
   	

	
	
	return 0;
	
	
}

void create_pointer(int** matrix, int y, int value)
{
	 int i;

	#pragma omp parallel for \
        default(none) private(i) shared(matrix, value, y)
	 for(i=0; i<y; i++){
    	
    	matrix[i] = (int *)malloc((value) * sizeof(int));
 
	}
}

void create_pointer_float(float** matrix, int y, int value)
{
	 int i;
	 #pragma omp parallel for \
		default(none) private(i) shared(matrix, value, y)
	 for(i=0; i<y; i++){
    	
    	matrix[i] = (float *)malloc((value) * sizeof(float));
 
	}
}


void initialize_matrix(int** matrix, int x, int y)
{
	int i;
	int j;
	
	
	#pragma omp parallel for \
        default(none) private(i, j) shared(matrix, x, y)
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
    	
	
    	for(j=0; j<(b_num+2); j++)
    	{
    		int matrix_value;
    		fscanf( matrix_file, "%i", &matrix_value);
    		getc(matrix_file);		
    		
			x[i][j]=matrix_value;
			

    	
		}
	}

	

	
}









void sort(float* eigenvalues, float** eigenvectors, int b_num)
{
	int phase;
	int i;
	float tmp;
	float* tmp1;
	
	#pragma omp parallel for\
		default(none) private(i, tmp, tmp1, phase) shared(eigenvalues, eigenvectors, b_num) schedule(guided,4)
	for (phase=0; phase<(b_num+3); phase++)
	{

		if (phase%2==0)
			for(i=1; i<b_num+2; i+=2)
			{
				if(eigenvalues[i-1]<eigenvalues[i])
				{
					 tmp=eigenvalues[i-1];
					eigenvalues[i-1]=eigenvalues[i];
					eigenvalues[i]=tmp;
					
					tmp1=eigenvectors[i-1];
					eigenvectors[i-1]=eigenvectors[i];
					eigenvectors[i]=tmp1;
					
					
				}
			}
		
		else		
			for(i=1; i<b_num+1; i+=2)
			{
				if(eigenvalues[i]<eigenvalues[i+1])
				{
					tmp=eigenvalues[i+1];
					eigenvalues[i+1]=eigenvalues[i];
					eigenvalues[i]=tmp;
					
					
					tmp1=eigenvectors[i+1];
					eigenvectors[i+1]=eigenvectors[i];
					eigenvectors[i]=tmp1;
					
				}
			}
			
		
		
	
	
	}
	
	
	
	
}

