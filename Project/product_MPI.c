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


	
			
		
	
		
			MPI_Barrier(MPI_COMM_WORLD );
				
				
			if(my_rank==0)
			{
					gettimeofday(&start, NULL);
				
			}
			
			
			int comm_counter=1;
			
			int number_calls=0;
			
			if(my_rank != 0)
			{
				total=(b_num+2)*(b_num+2);
				int fake_counter=1;
				
				for(i=0; i<total; i++)
				{
					if(fake_counter==my_rank)
					{
						number_calls=number_calls+1;
					}
					
						if(fake_counter<(comm_sz-1))
						{
							fake_counter=comm_counter+1;
						}else
						{
							fake_counter=1;
						}
				}
			}
			
			MPI_Barrier(MPI_COMM_WORLD );

			
			if(my_rank==0)
			{
				
				
				
				for(i=0; i<b_num+2; i++)
				{
					for(j=0; i<b_num+2; i++)
					{
						//manda vettori da moltiplicare 
						
						int* row=x_traspose[i];
						int* column=x_traspose[j];
						tag= (b_num+2)*i + j;
						
						MPI_Send(row, limit,MPI_INT,comm_counter, tag, MPI_COMM_WORLD);
						MPI_Send(column, limit,MPI_INT,comm_counter, tag, MPI_COMM_WORLD);

						
						if(comm_counter<(comm_sz-1))
						{
							comm_counter=comm_counter+1;
						}else
						{
							comm_counter=1;
						}
						
					}
				
				}
				
				MPI_Status status;
				
				//prendo i valori di ritorno
				for(i=0; i<b_num+2; i++)
				{
					for(j=0; i<b_num+2; i++)
					{
						int value=0;
						int tag=0;
						
						MPI_Recv (value , 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
						tag=status.MPI_TAG;
						
						int index_x=tag/(b_num+2);
						int index_y= tag%(b_num+2);
						
						x_product[index_x][index_y]=value;
						
						

					}
				}
				
				//creo la matrice
			
			}else
			{
				int results= (int*)malloc((number_calls) * sizeof(int));
				int tags= (int*)malloc((number_calls) * sizeof(int));
				MPI_Status status;
				
				for(i=0; i<number_calls; i++)
				{
						int* row=(int*)malloc((limit) * sizeof(int));
						int* column=(int*)malloc((limit) * sizeof(int));
					
					//attendo di ottenere il vettore
					MPI_Recv (row , limit,MPI_INT,0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
					MPI_Recv (column , limit, MPI_INT,0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);

					
					//faccio il lavoro
					int value=0;
					for(k=0; k<limit; k++)
					{
						value=value+row[k]*column[k];
					}
					
					//salvo in un vettore
					results[i]=value;
					tags[i]=status.MPI_TAG;
				}
				
				//rimando in dietro
				for(i=0; i<number_calls; i++)
				{
					MPI_Send(results[i], 1, MPI_INT, 0, tags[i], MPI_COMM_WORLD);
				}
				
				
			}
			
			
			if(my_rank==0)
			{
				gettimeofday(&end, NULL);
				printf("time for calculate product+transpose for all features %ld\n", ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec)));
	
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

