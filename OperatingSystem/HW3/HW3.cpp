#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <queue>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>  

using namespace std;

queue<int> sits;
int n, SEED, linenum, shotroomnum;
int currentP;
bool stuinroom;
int* totalshot;
pthread_mutex_t mutex1;

struct data
{
	int ID;
};

int numstudentshot = 0;

void gettime()
{

	struct tm* local;
	time_t t;
	t = time(NULL);
	local = localtime(&t);
	cout << local->tm_hour << ":" << local->tm_min << ":";
	if (local->tm_sec < 10) cout << "0" << local->tm_sec << " ";
	else cout << local->tm_sec << " ";

}

bool finish()
{
	for (int i = 1; i < n + 1; ++i) if (totalshot[i] != 3) return false;

	return true;
}

void* student_work(void* arg)
{
	data* D = (data*)arg;

	while (totalshot[D->ID] < 3)
	{
		if (totalshot[D->ID] < 1)
		{
			int tmp;
			tmp = rand() % 11;
			sleep(tmp);
		}

		if (pthread_mutex_trylock(&mutex1))
		{
			if (sits.size() < 3) 
			{

				sits.push(D->ID);
				gettime(); cout << "Student" << D->ID << ": Sitting #" << sits.size() << endl;
				while (D->ID != currentP);

				pthread_mutex_lock(&mutex1); // critical section start

				if (!sits.empty()) sits.pop();
				--shotroomnum;
				gettime(); cout << "Student" << D->ID << ": Entering to get a shot" << endl;
				++totalshot[D->ID];
				sleep(2);

				if (totalshot[D->ID] != 3) { gettime(); cout << "Student" << D->ID << ": Leaving the room" << endl; }
				if (!sits.empty()) currentP = sits.front();

				pthread_mutex_unlock(&mutex1); // critical section end

				if (totalshot[D->ID] == 3) break;

				int tmp;
				tmp = (rand() % 21) + 10;
				sleep(tmp);
			}
			else
			{
				int tmp;
				tmp = (rand() % 6) + 5;
				sleep(tmp);
			}
		}
		else // critical section start
		{ 
			--shotroomnum;  
			gettime(); cout << "Student" << D->ID << ": Entering to get a shot" << endl;
			++totalshot[D->ID];
			sleep(2);
			if (totalshot[D->ID] != 3) { gettime(); cout << "Student" << D->ID << ": Leaving the room" << endl; }

			pthread_mutex_unlock(&mutex1); // critical section end

			if (totalshot[D->ID] == 3) break;

			int tmp;
			tmp = (rand() % 21) + 10;
			sleep(tmp);
		}

	}

	gettime(); cout << "Student" << D->ID << ": Leaving to write YZU CS305 programs" << endl;

	pthread_exit(0);
}

void* robot_work(void* arg)
{
	shotroomnum = 1;

	while (!finish()) 
	{
		if (sits.size() == 0 && shotroomnum == 1) { gettime(); cout << "Robot: Sleep" << endl; } // wait for student
		while (shotroomnum == 1) ;

		pthread_mutex_lock(&mutex1); // critical section start
		++shotroomnum;
		if (!sits.empty()) currentP = sits.front();

		pthread_mutex_unlock(&mutex1); // critical section end
	}

	gettime(); cout << "Robot: All " << n << " students receive vaccines." << endl;

	pthread_exit(0);
}

int main(int argc, char* argv[])
{
	n = atoi(argv[1]);
	SEED = atoi(argv[2]);
	
	if(n<10 || n>20) exit(1);
	else if(SEED<0 || SEED>100) exit(1);
	
	linenum = 0;
	currentP = 0;
	shotroomnum = 0;
	srand(SEED);
	stuinroom = false;

	/*if (pthread_mutex_init(&mutex1, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}*/
	pthread_mutex_init(&mutex1, NULL);

	pthread_t* callThd = new pthread_t[n + 1];

	data* dat = new data[n + 1];

	totalshot = new int[n + 1]();


	pthread_create(&callThd[0], NULL, robot_work, NULL);
	for (int i = 1; i < n + 1; ++i)
	{
		dat[i].ID = i;

		pthread_create(&callThd[i], NULL, student_work, (void*)&dat[i]);
	}

	for (int i = 0; i < n + 1; ++i)
	{
		pthread_join(callThd[i], NULL);
	}

	gettime(); cout << "Today shot finished" << endl;

	return 0;
}
