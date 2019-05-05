#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[])
{
	constant int N=2;
	constant int start=1;
	constant int distance[]={1, 3; 4, 2;};
  #pragma omp parallel for shared(N, start, distance) private(minDist, bestPath)
	{
		for (int i=0; i<N; i++)
		{
			for(int j=0; j<N; j++)
			{
				
			}
		}
	}
}
