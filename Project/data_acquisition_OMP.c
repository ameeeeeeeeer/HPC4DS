#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void read_matrix(char* file, int x_num, int y_num, int b_num, int**x, int* y); //to read feature matrix


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
	/*close the file*/
	fclose(matrix_file);

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
    read_matrix(argv[1],  x_num, y_num, b_num, x,  y);
	gettimeofday(&end, NULL);
    printf("time for reading main matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	
	
	int limit=x_num*y_num*b_num;
	for(i=0; i<limit; i++)
	{
		for(j=0; j<(b_num+2); j++)
		{
			printf("%i ", x[i][j]);
		}
		
	printf("\n");
	
	}
	
		for(i=0; i<limit; i++)
	{
		
			printf("%i \n", y[i]);
	
		
	
	
	}
	
	
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

void read_matrix(char* file, int x_num, int y_num, int b_num, int**x, int* y)
{
	int i;
	int j;
	

	#pragma omp parallel for \
                default(none) private(i, j) shared( x_num, y_num, b_num, x, y, file)
	for(i=0; i<(x_num*y_num); i++)
    {
    	int matrix_value=0;
    	FILE* matrix_file=fopen(file, "r");
    	int z;
    	for(z=0; z<i+3; z++)
    	{
    		fscanf( matrix_file, "%i", &matrix_value);
    		getc(matrix_file);	
		}


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
    	
		matrix_value=0;
		
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
