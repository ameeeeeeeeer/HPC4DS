
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
	

	
	
	for(i=1; i<n+1; i++)
	{
		cscColPtr[i]+=cscColPtr[i-1];
	
	}
	
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
