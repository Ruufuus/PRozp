#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define PROCES_COUNT 4
#define ITERATION_COUNT 1000000000
#define ROOT 0
#define MSG_TAG 100
int monte_carlo_pi(){
    int i,circle_hit_count=0;
    double x,y;
    for(i=0;i<ITERATION_COUNT;i++){
        x=(double)rand()/(RAND_MAX);
        y=(double)rand()/(RAND_MAX);        
        if(pow(x,2)+pow(y,2)<=1){
            circle_hit_count+=1;
        }
    }
    return circle_hit_count;
}
int main(int argc,char **argv)
{
    int size,tid;
    int R;

    MPI_Init(&argc, &argv); 

    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Comm_rank( MPI_COMM_WORLD, &tid );

    srand( tid );

    long double pi,circle_hit_count_global=0;
    int res;

    if ( tid == 0 ) {
	MPI_Status status;
        int i;
        pi=0;
    for(i=0;i<PROCES_COUNT-1;i++){
	    MPI_Recv( &res, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        circle_hit_count_global+=res;
        pi=(long double)circle_hit_count_global/(ITERATION_COUNT*(i+1))*4;
        printf("Nowe przyblizone PI wynosi %Lf po otrzymaniu wyniku od %d!\n",pi,status.MPI_SOURCE);
    }
        printf("Przybli¿enie pi po zebraniu danych od %d procesów wynosi %Lf\n", PROCES_COUNT-1,pi);
    } else {
        res=monte_carlo_pi();
	    MPI_Send( &res, 1, MPI_INT, ROOT, MSG_TAG, MPI_COMM_WORLD );
    }

    MPI_Finalize();
}