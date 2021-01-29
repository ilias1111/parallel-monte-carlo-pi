#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include <limits.h>


#define niter 125000000000

unsigned int seed = 676767676; 


//Random number generator with linear congruential generator
double randUint(int i){
    
	seed = (i + seed) * 1103515245 + 123456;
    
    return seed / (double)UINT_MAX ;
}


int main(void){

    
    long count = 0;
    long mycount = 0;
    
    double pi;


    //mpi init
    int myid,comm_sz,proc;
    int numprocs=8;
    MPI_Status status;
    int master =0;
    int tag = 123;
    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);


    srand(seed);

    for( long i = 0 ; i < niter ; i++ ) {
        
        //Getting the coordinates y,x ε [0,1]
        double x = randUint(myid);
        double y = randUint(myid);
		
        //Checking if in unit circle
        if (x*x + y*y <= 1.0)
		    mycount ++;
	
    }

    //If myid 0 then it is the master
    if(myid==0){

        //updating count 
        count = mycount;
        for (proc=1; proc<numprocs; proc++) {

            //reciend threads count number and updating master count
            MPI_Recv(&mycount,1,MPI_REAL,proc,tag,MPI_COMM_WORLD,&status);
            count +=mycount;

        }

        //Calcuting the ratio and as a result the pi
        pi=(double)count/((double)niter*numprocs)*4;

        printf("MPI_PA : # of trials = %14ld , estimate of pi is %1.16f AND an absolute error of %g\n",(long)niter*numprocs,pi,fabs(pi - M_PI));
    }
    else { // else is slave process and must send to master

      MPI_Send(&mycount,1,MPI_REAL,master,tag,MPI_COMM_WORLD);


    }

    MPI_Finalize();

    return 0;

}