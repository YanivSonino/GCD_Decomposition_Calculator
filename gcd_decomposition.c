/*
 * file:ex7b.c
 * program that creates 3 threads:
 * 1.gcd thread
 * 2.decomposition thread
 * 3.client thread
 * the user enters a char for action(g for gcs,d for decomposition),and
 * than gets an answer from the target thread
 */
//-----------------------------Include Section-------------------------
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
//---------------------------define Section----------------------------
#define NUM_OF_THREADS 3
#define GCD_ARR_SIZE 3
#define FACTOR_ARR_SIZE 11
#define GCD_SERVER 0
#define DECOMP_SERVER 1
#define USER_THREAD 2
//---------------------------Prototype Section----------------------------
void open_threads(int index);
void* gcd_server(void* args);
void* decomposition_server(void* args);
void* user_thread(void* args);
int search_gcd(const int a,const int b);
void catch_int(int sig_num);
void primeFactors();
void catch_usr1(int sig_num);
void print_factorials();
void clean_array();
//---------------------------Global Section-------------------------------
pthread_t thread_data[NUM_OF_THREADS];
int gcd_data[GCD_ARR_SIZE];
int decomp_data[FACTOR_ARR_SIZE];

//-----------------------------------MAIN----------------------------------
int main()
{
	//define signal handlers
	signal(SIGUSR1,catch_usr1);
	signal(SIGINT,catch_int);

	//to run on threads index
	int index;

	//open all threads
	for(index =0;index<NUM_OF_THREADS;index++){
		open_threads(index);
	}

	pthread_exit(NULL);
	exit(EXIT_SUCCESS);
}
//--------------------------Function Section-------------------------------
//-------------------------------------------------------------------------
/*
 * function to open 3 threads, 1 for gcd calculator,1 for decomposition
 * calculator and 1 for user high end
 */
void open_threads(int index)
{
	//for thread opening
	int status;

	if(index == 0)
	{
		status = pthread_create(&(thread_data[index]),NULL,
				gcd_server,NULL);
	}
	else if(index == 1)
	{
		status = pthread_create(&(thread_data[index]),NULL,
				decomposition_server,NULL);
	}
	else
	{
		status = pthread_create(&(thread_data[index]),NULL,
				user_thread,NULL);
	}
	if(status !=0)
	{
		puts("pthread_create failed");
		exit(EXIT_FAILURE);
	}
}
//-------------------------------------------------------------------------
/*
 * function to act like a gcd server.the function waits for signal from
 * user high end and calculates the gcd for 2 numbers user sent. than it
 * sends a signal back to user high end.
 */
void* gcd_server(void* args)
{
	while(1)
	{
		//wait for signal from user
		pause();
		//calculate
		gcd_data[2] = search_gcd(gcd_data[0],gcd_data[1]);
		//send signal to user high end
		pthread_kill(thread_data[USER_THREAD],SIGUSR1);
	}

	pthread_exit(NULL);
}
//-------------------------------------------------------------------------
/*
 * function to act like a decomposition server.the function waits for
 * signal from user high end and calculates the decomposition for 2 numbers
 * user sent. than it sends a signal back to user high end.
 */
void* decomposition_server(void* args)
{
	while(1)
	{
		//wait for signal from user
		pause();
		//clean array and calculate
		clean_array();
		if(decomp_data[0] != 0)
			primeFactors();
		//send signal to user high end
		pthread_kill(thread_data[USER_THREAD],SIGUSR1);
	}
	pthread_exit(NULL);
}
//-------------------------------------------------------------------------
/*
 * function to act as client server. the function waits for action from the
 * user and numbers and sends the data to the wanted server.when the server
 * signals that it ended calculate, the client will print the answer
 */
void* user_thread(void* args)
{
	char action;
	while(1)
	{
		scanf("%c",&action);
		if (action == 'g')
		{
			scanf("%d %d",&gcd_data[0],&gcd_data[1]);
			getchar();
			//send signal to server
			pthread_kill(thread_data[0],SIGUSR1);
			//wait for server to signal
			pause();
			printf("%d\n",gcd_data[2]);
		}
		else if(action == 'd'){
			scanf("%d",&decomp_data[0]);
			getchar();
			//send signal to server
			pthread_kill(thread_data[DECOMP_SERVER],SIGUSR1);
			//wait for server to signal
			pause();
			print_factorials();

		}
	}

	pthread_exit(NULL);
}
//-------------------------------------------------------------------------
//That function gets 2 numbers and checks for there GCD.
//The function returns there GCD.
int search_gcd(const int a,const int b){
	// Everything divides 0
	if (a == 0)
		return b;
	if (b == 0)
		return a;

	// base case
	if (a == b)
		return a;

	// a is greater
	if (a > b)
		return search_gcd(a-b, b);
	return search_gcd(a, b-a);
}
//-------------------------------------------------------------------------
//That function gets an array and number,the function enters
//the 10 factorial of the number into the
//the array in cell 0-10 and finish
void primeFactors()
{

	int index,counter=1;
	// Print the number of 2s that divide n
	while (decomp_data[0]%2 == 0&&counter < FACTOR_ARR_SIZE)

	{

		decomp_data[counter] = 2;
		decomp_data[0] = decomp_data[0]/2;
		counter++;
	}

	// n must be odd at this point.  So we can skip
	// one element (Note i = i +2)
	for (index = 3; index <= decomp_data[0]/2 && counter < FACTOR_ARR_SIZE; index = index+2)
	{
		// While i divides n, print i and divide n
		while (decomp_data[0] % index == 0 && counter < FACTOR_ARR_SIZE)
		{
			decomp_data[counter] = index;
			decomp_data[0]= decomp_data[0]/index;
			counter++;
		}
	}

	// This condition is to handle the case when n
	// is a prime number greater than 2
	if (decomp_data[0] > 2 && counter < FACTOR_ARR_SIZE)
		decomp_data[counter] = decomp_data[0];
}



//-------------------------------------------------------------------------
void catch_int(int sig_num){exit(EXIT_SUCCESS);}
//-------------------------------------------------------------------------
void catch_usr1(int sig_num){}
//-------------------------------------------------------------------------
//That program gets the array of factorial numbers and prints
//them
void print_factorials()
{
	int index;

	for(index=1;index<FACTOR_ARR_SIZE && decomp_data[index]!=0;index++)
	{
		printf("%d ",decomp_data[index]);
	}

	if(index != 1)
		printf("\n");
}
//-------------------------------------------------------------------------
//That function gets an array and 2 indexs start and end.
//The function set all values in the array to be zeros
void clean_array()
{
	int index;
	for(index=1;index<FACTOR_ARR_SIZE;index++)
	{
		decomp_data[index]=0;
	}
}

