#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
#include <time.h>
#include <string.h>
#include <omp.h>

void func(){
	int G = 16;
	#pragma omp parallel
	{
		#pragma omp single
		printf("Thread:%d\tG has initial value before the while loop:%d\n", omp_get_thread_num(), G);
		while (G > 0) {
			int L = 0;
			#pragma omp task shared(G) final (G<=0)
			{
				printf("Thread:%d\tL enterred while loop with value:%d\n", omp_get_thread_num(), L);
				
				#pragma omp critical
				{
					//similar to pop 
					G -= 9;
					L = G;
				}
				printf("Thread:%d\tL decremented:%d\n", omp_get_thread_num(),L);

				#pragma omp parallel for shared(G, L)
				for (int i=0; i<5; i++){
					#pragma omp critical
					{
						//similar to push
						L += 1;
						G = L;
					}
					printf("Thread:%d\tL incremented:%d\n", omp_get_thread_num(),L);
				}
				//similar to freePath
				printf("Thread:%d\tExecute this to see only a multiple of 4, L:%d\n", omp_get_thread_num(),L);
			}
		}
	}
	printf("G exited while loop with value:%d\n", G);
}

int main(){
	printf ( "Number of processors available = %d\n", omp_get_num_procs() );
  	printf ( "Number of threads =              %d\n", omp_get_max_threads() );
	func();
	return 0;
}