#include <mpi.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#define SORT 1
#define END 2
// jeżeli zerowy nie będzie uczestniczył w sortowaniu
// Procesów musi być o jeden więcej niż TABSIZE
#define TABSIZE 100
#define MSG_TAG 100


int send_number(int * tablica, int * local_tab,int tablica_size, int local_tab_size, int rank, int size)
{
    int number_to_send,number_local;
    for (int i=0;i<tablica_size;i++) {
        number_to_send=tablica[i];
        for(int j=0;j<local_tab_size;j++){
            number_local=local_tab[j];
            if(number_to_send>number_local){
                local_tab[j]=number_to_send;
                number_to_send=number_local;
                if(number_to_send==-1)
                break;
            }
        }
        if(rank+1<size){
        MPI_Send(&number_to_send,1,MPI_INT,rank+1,MSG_TAG,MPI_COMM_WORLD);
        }
    }
}
// Kompilujemy mpicc, uruchamiamy mpirun -np N ./a.out liczby_do_sortowania
// gdzie N to liczba procesów
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    
    int size,rank;
    int tablica[TABSIZE]={-1};
    int sorted[TABSIZE]={-1};
    int local_tab_size;
    int tmp=-1;
    int i;
    int end=0;
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    local_tab_size = (int)(TABSIZE/size);
    int local_tab[local_tab_size];
    printf("%d rank\n",rank);
    memset(local_tab,-1,local_tab_size*sizeof(int));
    if (argc<2) {
	    MPI_Finalize();
	    printf("Za malo argumentow\n");
	    exit(0);
    }


    if (rank == 0) {
        printf("Otwieram plik\n");
	FILE *f;int i;
	f = fopen(argv[1],"r");
	for (i=0;i<TABSIZE;i++) fscanf(f, "%d", &(tablica[i]));
    send_number(tablica,local_tab,sizeof(tablica)/sizeof(int),sizeof(local_tab)/sizeof(int),rank,size);
    memcpy(sorted,local_tab,local_tab_size*sizeof(int));
    int dummy=-1;
    if(size>1)
    MPI_Send( &dummy, 1, MPI_INT, 1, END, MPI_COMM_WORLD); //koniec liczb
    for (i=1;i<size;i++) {
        printf("%d\n\n\n\n",i);
        MPI_Recv(&(sorted[i*local_tab_size]), local_tab_size , MPI_INT, i, MSG_TAG, MPI_COMM_WORLD, &status);
    }

    for (i=0;i<TABSIZE;i++) {
        printf("%d ",sorted[i]);
    }
    printf("\n");

    } else { 
        int received_number;
        while (!end) {
	    MPI_Recv(&received_number, local_tab_size, MPI_INT, rank-1, MPI_ANY_TAG,  \
                    MPI_COMM_WORLD, &status);
	    if (status.MPI_TAG==END) {
		end=1;
        printf("KONIEC\n");
		MPI_Send(local_tab,local_tab_size,MPI_INT,0,MSG_TAG,MPI_COMM_WORLD);
        int dummy=-1;
        if(rank<size)
        MPI_Send( &dummy, 1, MPI_INT, rank+1, END, MPI_COMM_WORLD);
	    } else {
		send_number(&received_number,local_tab,sizeof(received_number)/sizeof(int),sizeof(local_tab)/sizeof(int),rank,size);
	    } 
        }   
        }
    MPI_Finalize();
}