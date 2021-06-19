#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void initialize_matrix_variant(int** matrix, int x, int y);
void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y); //to read feature matrix
void calculate_norm(int** x, double* norm, int limit);
void calculate_Q(int** x, int** Q, double* norm, int limit);
void calculate_R(int** x, int** Q, int** R, int limit);
int calculate_product(int** x, int** Q, int i, int j, int limit);


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




	int** fake_product_matrix=(int **)malloc((b_num+2) * sizeof(int*));
	create_pointer(fake_product_matrix, b_num+2, b_num+2);
	initialize_matrix_variant(fake_product_matrix, b_num+2, b_num+2);
	int** fake_Q=(int **)malloc((b_num+2) * sizeof(int*));
	create_pointer(fake_Q, b_num+2, b_num+2);
	initialize_matrix(fake_Q, b_num+2, b_num+2);
	int** fake_R=(int **)malloc((b_num+2) * sizeof(int*));
	create_pointer(fake_R, b_num+2, b_num+2);
	initialize_matrix(fake_R, b_num+2, b_num+2);
	
	double* norm=(double *)malloc((b_num+2) * sizeof(double));
   	calculate_norm( fake_product_matrix, norm, b_num+2);
   	calculate_Q(fake_product_matrix, fake_Q, norm, b_num+2);
   	calculate_R(fake_product_matrix, fake_Q, fake_R, b_num+2);
   	

	
	
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

void initialize_matrix_variant(int** matrix, int x, int y)
{
	int i;
	int j;
	
	for(i=0; i<y; i++)
	{
		for(j=0; j<x; j++)
		{
			matrix[i][j]=i*j;
		}
	}
	

}

void calculate_norm(int** x, double* norm, int limit)
{
	int i;
	int j;
	
	for(j=0; j<limit; j++)
	{
		for(i=0; i<limit; i++)
		{
			norm[j]+=(x[i][j]*x[i][j]);
			
		}
		
		norm[j]=sqrt(norm[j]);
		
	}
}

void calculate_Q(int** x, int** Q, double* norm, int limit)
{
	int i;
	int j;
	
	for(j=0; j<limit; j++)
	{
		for(i=0; i<limit; i++)
		{
			Q[i][j]=(x[i][j]/norm[j]);
			
		}
		
	}
}

void calculate_R(int** x, int** Q, int** R, int limit)
{
	int i;
	int j;
	
	for(i=0; i<limit; i++)
	{
		for(j=0; j<limit; j++)
		{
			if(j>=i)
			{
				R[i][j]=calculate_product( x, Q, i, j, limit);
			}
		}
	}
	
}

int calculate_product(int** x, int** Q, int i, int j, int limit)
{
	int h=0;

	int result=0;
	
	for(h=0; h<limit; h++)
	{
		result=x[h][j]*Q[h][i];
	}
	
}

