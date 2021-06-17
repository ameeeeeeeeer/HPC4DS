#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

//DECLARED FUNCTION
void create_pointer(int** matrix, int y, int value); //to creater rows in a dynamic matrix of int
void initialize_matrix(int** matrix, int x, int y); //to set all value of a int matrix to 0
void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y, int rest, int** x_padded); //to read feature matrix
void average_calculation(float* average, int** x, int b_num,  int limit, int limit_subd, int comm_sz, int* calc_buffer); //to calculate average for each feature
void average_calculation_slave(int** x, int limit_subd, int b_num, int* calc_buffer); //to calculate average for each feature
void transpose_calculation(int** x, int** x_transpose, int limit, int b_num); //transpose calulation

main(int argc, char **argv)
{
	int comm_sz;
	int my_rank;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	int root=0;
	
	int rest;
	
	//GENRAL VARIABLE
		/*used for cycle*/
		int i;
		int j;
			
		/*used for performance measure*/
		struct timeval start, end;
		
		/*access to first line infos*/
   		int	x_num=0; //number of x
		int y_num=0; //numbero of y
		int b_num=0; //number of channels
		
		int limit;
		int limit_correct;
		int limit_subd;
		
		int *y; 
   		int **x; 
   		int **x_padded;
   		int** x_transpose;
   		int** x_transpose_padded;
   		int** x_product;
   		
	
	if(my_rank == 0)//only process 0 execute reading and initializing of file
	{
	
		//ACCESS TO FILE
		/*access to file*/
		FILE* matrix_file=fopen(argv[1], "r");
		
		fscanf( matrix_file, "%i", &x_num);
		getc(matrix_file);
		fscanf( matrix_file, "%i", &y_num);
		getc(matrix_file);
		fscanf( matrix_file, "%i", &b_num);
		 limit = x_num*y_num*b_num;
		rest=limit%comm_sz;
		
		
		 limit_correct=limit+(comm_sz-rest);

 	   //1. USED MATRICES CONSTRUCTION
  	  /*construction of feature and result matrix*/

 	   	y=(int*)malloc((x_num*y_num*b_num) * sizeof(int));
    	x=(int **)malloc((x_num*y_num*b_num) * sizeof(int*));
    	if(rest!=0)
    	{
    		x_padded=(int **)malloc((limit_correct) * sizeof(int*));
    		create_pointer(x_padded, (limit_correct), (b_num+2));
			initialize_matrix(x_padded, (b_num+2), (limit_correct));
			x_transpose_padded=(int **)malloc((b_num+2) * sizeof(int*)); //for the transpose
			create_pointer(x_transpose_padded, (b_num+2), (limit_correct));
		}
    	 x_transpose=(int **)malloc((b_num+2) * sizeof(int*)); //for the transpose
    	 x_product=(int **)malloc((b_num+2) * sizeof(int*)); //for the product of x and transpose
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
   		 read_matrix(matrix_file,  x_num, y_num, b_num, x,  y, rest, x_padded);
		gettimeofday(&end, NULL);
   		 printf("time for reading main matrix %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
		/*close the file*/
		fclose(matrix_file);
		
		if(rest==0)
		{
			limit_subd=limit/comm_sz;

		}else
		{
			limit_subd=limit_correct/comm_sz;
		}
		
		printf("limit subd %i \n", limit_subd);
	}
	
	//broadcast subdivision of vertically matrix
	MPI_Bcast(&limit_subd, 1, MPI_INT, root, MPI_COMM_WORLD);
	//broadcast subdivision of vertically matrix
	MPI_Bcast(&b_num, 1, MPI_INT, root, MPI_COMM_WORLD);


	


	if (my_rank==0)
	{
		//3. TRANSPOSE CALCULATION AND PRODUCT
		/*transpose calculation*/
		if(rest==0)
		{
			transpose_calculation(x, x_transpose, limit, b_num);

		}else
		{
			transpose_calculation(x_padded, x_transpose_padded, limit_correct, b_num);
			
			x_transpose=x_transpose_padded;
		}
	}


		//2. CENTERING
		/*average for every feature calculation*/
		float* average;
		if(my_rank==0)
		{
					average=(float*)malloc((b_num+2) * sizeof(float));

		}

			MPI_Barrier(MPI_COMM_WORLD );
		
			if(my_rank==0)
			{	
					gettimeofday(&start, NULL);

			}
		
			
			for(i=0; i<(b_num+2); i++)
			{
				int* calc_buffer;
				int* recv_buffer=(int*)malloc((limit_subd) * sizeof(int));
				
					MPI_Scatter(x_transpose[i], limit_subd, MPI_INT, recv_buffer, limit_subd, MPI_INT, 0, MPI_COMM_WORLD);	
			
				
				int sub_avg=0;
				int counter=0;
				for(counter=0; counter<limit_subd; counter++)
				{
					sub_avg+=recv_buffer[counter];
				}
				if(my_rank==0)
				{
					calc_buffer=(int*)malloc((comm_sz) * sizeof(int));
				}
				
				MPI_Gather(&sub_avg, 1, MPI_INT, calc_buffer, 1, MPI_INT, 0, MPI_COMM_WORLD);
				if(my_rank==0)
				{
					int final_avg=0;
					for(counter=0; counter<comm_sz; counter++)
					{
						final_avg+=calc_buffer[counter];
					}
					average[i]=final_avg/limit;
				}
			
			}
		
			if(my_rank==0)
			{
				gettimeofday(&end, NULL);
				printf("time for calculate average for all features %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	
			}
			
		
	
		
				MPI_Barrier(MPI_COMM_WORLD );




	
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

void read_matrix(FILE* matrix_file, int x_num, int y_num, int b_num, int**x, int* y, int rest, int** x_padded)
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
    		
    			if(rest != 0)
				{
			
    		
    				x_padded[a][0]= matrix_value_x;
    				x_padded[a][1]= matrix_value_y;
    		
			
				}
    		
		}
		
	
		
		
		a = i*7; 
		b= a;
    	
		int matrix_value=0;
		
	
    	for(j=2; j<(b_num+2); j++)
    	{
    		fscanf( matrix_file, "%i", &matrix_value);
    		getc(matrix_file);		
    		
			x[b][j]=1;
			
			if(rest != 0)
			{
				x_padded[b][j]=1;
			}
    		
			int index=(j-2)+a;
			    	    
    		y[index]=matrix_value;
    		
    	    b=b+1;
    	
		}
		
		
	}	
}

void average_calculation(float* average, int** x, int b_num,  int limit, int limit_subd, int comm_sz, int* calc_buffer)
{
			
	int i;
	


}
	


void average_calculation_slave(int** x,  int limit_subd, int b_num, int*calc_buffer)
{
	int i=0;
	for(i=0; i<(b_num+2); i++)
	{
		int* recv_buffer=(int*)malloc((limit_subd) * sizeof(int));
		MPI_Scatter(x[i], limit_subd, MPI_INT, recv_buffer, limit_subd, MPI_INT, 0, MPI_COMM_WORLD);	
		int sub_avg=0;
		int counter=0;
		for(counter=0; counter<limit_subd; counter++)
		{
			sub_avg+=recv_buffer[counter];
		}
				printf("iter %i di %i \n", i, sub_avg);

		MPI_Gather(&sub_avg, 1, MPI_INT, calc_buffer, 1, MPI_INT, 0, MPI_COMM_WORLD);	
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

