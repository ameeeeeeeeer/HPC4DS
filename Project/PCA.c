#include <stdio.h>
#include <stdlib.h>
#include <omp>

//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void create_pointer_float(float** matrix, int y, int value); //to create rows in a dynamic matrix of float
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y); //to read feature matrix
void average_calculation(float* average, int** x, int b_num, int dividend, int max_divisor, int limit); //to calculate average for each feature
void mean_subtraction(float* average, int** x, int b_num, int dividend, int max_divisor); //subtraction of the mean to each value of the matrix
void transpose_calculation(int** x, int** x_transpose, int limit, int b_num); //transpose calulation
void product_calculation_1(int** x, int** x_transpose, int** x_product, int limit, int b_num); //product calcolation in integer
void sort(float* eigenvalues, float** eigenvector, int b_num); //sort function for eigenvalues and eigenvectors
void product_calculation_2(int** x, float** eigenvectors, float** final_matrix, int limit, int b_num); //matrix product calulcation in float

main(int argc, char **argv)
{
	//GENRAL VARIABLE
	/*used for cycle*/
	int i;
	int j;
	
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


    //1. USED MATRICES CONSTRUCTION
    /*construction of feature and result matrix*/
    int *y; 
    int **x; 
    y=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
    x=(int **)malloc((x_num*y_num*b_num) * sizeof(int*));
    int** x_transpose=(int **)malloc((b_num+2) * sizeof(int*)); //for the transpose
    int** x_product=(int **)malloc((b_num+2) * sizeof(int*)); //for the product of x and transpose
    /*initialization of matrix used*/
	create_pointer(x, (x_num*y_num*b_num), (b_num+2));
	initialize_matrix(x, (b_num+2), (x_num*y_num*b_num));	 //all its value are setted to 0
	create_pointer(x_transpose, (b_num+2), (x_num*y_num*b_num));
	create_pointer(x_product, (b_num+2), (b_num+2));
	/*read and create X matrix*/
    read_matrix(matrix_file,  x_num, y_num, b_num, x,  y);
	/*close the file*/
	fclose(matrix_file);
	
	
	//2. CENTERING
	int limit = x_num*y_num*b_num;
	/*calculation of max divisor number of pixels*/
	int max_divisor=0;
	int dividend=0;
	int rest=0;
	for(i = 1; i < limit; i++) {
		rest=(limit % i);
		if( rest == 0){
			max_divisor=i;
			dividend=limit/max_divisor;
		}
	}
	/*average for every feature calculation*/
	float* average=(float*)malloc((b_num+2) * sizeof(float));
	average_calculation( average, x, b_num,  dividend,  max_divisor,  limit);
	/*average subtraction*/
	mean_subtraction( average,  x, b_num, dividend, max_divisor);

	
	//3. TRANSPOSE CALCULATION AND PRODUCT
	/*transpose calculation*/
	transpose_calculation(x, x_transpose, limit, b_num);
	/*first matrix product*/
	product_calculation_1( x,  x_transpose,  x_product, limit,  b_num);
	
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
	sort(eigenvalues, eigenvectors, b_num);


	//6. FINAL PRODUCT
	float** final_matrix=(float**)malloc((limit) * sizeof(float*));//final matrix declaration and initialization
	create_pointer_float(final_matrix, limit, (b_num+2));
	product_calculation_2( x, eigenvectors, final_matrix, limit, b_num);
	
	
}

void create_pointer(int** matrix, int y, int value)
{
	 int i;
	  #pragma omp parallel for \
		default(none) private(i) shared(matrix)
	 for(i=0; i<y; i++){
    	
    	matrix[i] = (int *)malloc((value) * sizeof(int));
 
	}
}

void create_pointer_float(float** matrix, int y, int value)
{
	 int i;
	 #pragma omp parallel for \
		default(none) private(i) shared(matrix)
	 for(i=0; i<y; i++){
    	
    	matrix[i] = (float *)malloc((value) * sizeof(float));
 
	}
}

void initialize_matrix(int** matrix, int x, int y)
{
	int i;
	int j;
	#pragma omp parallel for \
		default(none) private( j, i) shared(matrix)
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
	#pragma omp parallel for \
		default(none) private(a, j, i, matrix_value_x, matrix_value_y, matrix_value, index) shared(x, y, x_num, y_num, b_num, matrix_file)
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
    	
    	#pragma omp for
    	for(a; a<b; a++)
    	{
    		x[a][0]= matrix_value_x;
    		x[a][1]= matrix_value_y;
    		
		}
		
		
		a = i*7; 
    	
		int matrix_value=0;
		
	
       	#pragma omp for 	    	
    	for(j=2; j<(b_num+2); j++)
    	{
    		fscanf( matrix_file, "%i", &matrix_value);
    		getc(matrix_file);		
    		x[a][j]=1;
    		
			int index=(j-2)+a;
			    	    
    		y[index]=matrix_value;
    	
		}
	}
	

	
}

	void average_calculation(float* average, int** x, int b_num, int dividend, int max_divisor, int limit)
	{
			
	int i;
	int j;
	#pragma omp parallel for \
		default(none) private(d, indice, j, i) shared(x, average, dividend, b_num, max_divisor, partial_total)
	for(i=0; i<(b_num+2); i++)
	{
		int total=0;
		int d=0;
		#pragma omp for
		for(d; d<(dividend); d++)
		{ 
		   int indice=0;
		   int partial_total=0;
		   for(j=0; j<max_divisor; j++)
		   {
		   	  indice=j + max_divisor*d;
		   	  partial_total+=x[indice][i];
		   	  
		   	
		   }

			total+=partial_total;		
		
		}
		
		
		
		average[i]=total/limit;
	}
	}
void mean_subtraction(float* average, int** x, int b_num, int dividend, int max_divisor)
{
	int i;
	int j;
	#pragma omp parallel for \
		default(none) private(d, indice, j, i) shared(x, average, dividend, b_num, max_divisor)
	for(i=0; i<(b_num+2); i++)
	{
		
		int d=0;
		#pragma omp for
		for(d; d<(dividend); d++)
		{ 
		   int indice=0;
		   for(j=0; j<max_divisor; j++)
		   {
		   	  indice=j + max_divisor*d;
		   	  x[indice][i]=x[indice][i]-average[i];
		   	
		   }
			
		
		}
		
		
	}
	
}
void transpose_calculation(int** x, int** x_transpose, int limit, int b_num)
{
	int i;
	int j;
	
	
	#pragma omp parallel for \
		default(none) private(i, j) shared(limit, b_num, result, x, x_transpose)
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
	#pragma omp parallel for \
		default(none) private(i, j, k) shared(limit, b_num, result, x_product, x_transpose, x)
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
	#pragma omp parallel for \
		default(none) private(i, j, k) shared(limit, b_num, result, final_matrix, x, eigenvectors)
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

void sort(float* eigenvalues, float** eigenvectors, int b_num)
{
	int phase;
	int i;
	
	#pragma omp parallel \
		default(none) shared(eigenvalues, eigenvectors, b_num) private(i, tmp, tmp1, phase)
	for (phase=0; phase<(b_num+3); phase++)
	{
		
	
		
		if (phase%2==0)
			#pragma omp for
			for(i=1; i<b_num+2; i+=2)
			{
				if(eigenvalues[i-1]<eigenvalues[i])
				{
					float tmp=eigenvalues[i-1];
					eigenvalues[i-1]=eigenvalues[i];
					eigenvalues[i]=tmp;
					
					float* tmp1=eigenvectors[i-1];
					eigenvectors[i-1]=eigenvectors[i];
					eigenvectors[i]=tmp1;
					
					
				}
			}
		
		else		
			#pragma omp for
			for(i=1; i<b_num+1; i+=2)
			{
				if(eigenvalues[i]<eigenvalues[i+1])
				{
					float tmp=eigenvalues[i+1];
					eigenvalues[i+1]=eigenvalues[i];
					eigenvalues[i]=tmp;
					
					
					float* tmp1=eigenvectors[i+1];
					eigenvectors[i+1]=eigenvectors[i];
					eigenvectors[i]=tmp1;
					
				}
			}
			
		
		
	
	
	}
	
	
	
	
}

