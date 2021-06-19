#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y); //to read feature matrix
void average_calculation(float* average, int** x, int b_num,  int limit); //to calculate average for each feature
void mean_subtraction(float* average, int** x, int b_num, int limit); //subtraction of the mean to each value of the matrix
void transpose_calculation(int** x, int** x_transpose, int limit, int b_num); //transpose calulation
void product_calculation_1(int** x, int** x_transpose, int** x_product, int limit, int b_num); //product calcolation in integer
void product_calculation_2(int** x, float** eigenvectors, float** final_matrix, int limit, int b_num); //matrix product calulcation in float


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
	
		//2. CENTERING	
	/*average for every feature calculation*/
	float* average=(float*)malloc((b_num+2) * sizeof(float));
	 gettimeofday(&start, NULL);
	average_calculation( average, x, b_num,  limit);
	gettimeofday(&end, NULL);
	printf("time for calculate average for all features %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	/*average subtraction*/
    gettimeofday(&start, NULL);
	mean_subtraction( average,  x, b_num, limit);
	gettimeofday(&end, NULL);
	printf("time for subtract average  %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));


	//3. TRANSPOSE CALCULATION AND PRODUCT
	/*transpose calculation*/
	gettimeofday(&start, NULL);
	transpose_calculation(x, x_transpose, limit, b_num);
	gettimeofday(&end, NULL);
	printf("time for transpose  %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));

	/*first matrix product*/
	gettimeofday(&start, NULL);
	product_calculation_1( x,  x_transpose,  x_product, limit,  b_num);
	gettimeofday(&end, NULL);
	printf("time for product  %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	
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
    	
	
    	for(j=0; j<(b_num+2); j++)
    	{
    		int matrix_value;
    		fscanf( matrix_file, "%i", &matrix_value);
    		getc(matrix_file);		
    		
			x[i][j]=matrix_value;
			

    	
		}
	}

	

	
}

void average_calculation(float* average, int** x, int b_num,  int limit)
{
			
	int i;
	int j;

	for(i=0; i<(b_num+2); i++)
	{
		int total=0;
		int d=0;
		if(i>1)
		{
		

		   for(j=0; j<limit; j++)
		   {
		   	  
		   	  total+=x[j][i];
		   	  
		   	
		   }		
		   average[i]=total/limit;
		}else
		{
			for(j=0; j<limit; j++)
		    {
		   	  
		   	  average[i]+=average[i]+(x[j][i]/limit);
		   	  
		   	
		    }		
		}
		
	}
}
	
void mean_subtraction(float* average, int** x, int b_num, int limit)
{
	int i;
	int j;

	for(i=0; i<(b_num+2); i++)
	{
		
	
		   for(j=0; j<limit; j++)
		   {
		   	  
		   	  x[j][i]=x[j][i]-average[i];
		   	
		   }
	}
	
}

void transpose_calculation(int** x, int** x_transpose, int limit, int b_num)
{
	int i;
	int j;
	
	
	
	for(i=0; i<limit; i++)
	{
		for(j=0; j<b_num+2; j++)
		{
			x_transpose[j][i]=x[i][j];
		}
	}
}


void product_calculation_1(int** x, int** x_transpose, int** x_product, int limit, int b_num)
{
	
	int i;
	int j;
	
	for(i=0; i<(b_num+2);i++)
	{
		for(j=0; j<(b_num+2); j++)
		{
			int result=0;
			int k;
			for(k=0; k<(limit); k++)
			{
			    result+=x_transpose[i][k]*x[k][j];	
			}
			
			x_product[i][j]=result;
		}
		
	}
	
}



void product_calculation_2(int** x, float** eigenvectors, float** final_matrix, int limit, int b_num)
{
	
	int i;
	int j;
	
	for(i=0; i<(limit);i++)
	{
		for(j=0; j<(b_num+2); j++)
		{
			int result=0;
			int k;
			for(k=0; k<(b_num+2); k++)
			{
			    result+=x[i][k]*eigenvectors[k][j];	
			}
			
			final_matrix[i][j]=result;
			
		}
		
	
	}
	
	
}
