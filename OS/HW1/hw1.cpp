#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define NUM 14
#define SIZE (NUM * sizeof(int))

sigset_t newset, zeroset;
static int sigFlag = 0;
 
void sig_handler(int signo)
{
	if (signo == SIGUSR1 || signo == SIGUSR2)
	{
		sigFlag = 1;
	}
}
 
void tell_wait()
{
	sigemptyset(&newset);
	sigemptyset(&zeroset);
	sigaddset(&newset, SIGUSR1);
	sigaddset(&newset, SIGUSR2);
	
	struct sigaction action;
	action.sa_handler = sig_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	
	if (sigaction(SIGUSR1, &action, NULL) < 0)
	{
		printf("sigaction error\n");
		exit(-1);
	}
	
	if (sigaction(SIGUSR2, &action, NULL) < 0)
	{
		printf("sigaction error\n");
		exit(-1);
	}
	
	if (sigprocmask(SIG_BLOCK, &newset, NULL) < 0)
	{
		printf("sigprocmask error\n");
		exit(-1);
	}
}
 
void tell_parent(pid_t pid)
{
	kill(pid, SIGUSR2);
}
 
void wait_parent()
{
	while(sigFlag == 0)
	{
		sigsuspend(&zeroset);
	}
	
	sigFlag = 0;
	
	if (sigprocmask(SIG_BLOCK, &newset, NULL) < 0)
	{
		printf("set sigprocmask error\n");
		exit(-1);
	}
}
 
void tell_child(pid_t pid)
{
	kill(pid, SIGUSR1);
}
 
void wait_child()
{
	while(sigFlag == 0)
	{
		sigsuspend(&zeroset);
	}
	
	sigFlag = 0;
	
	if (sigprocmask(SIG_BLOCK, &newset, NULL) < 0)
	{
		printf("set sigprocmask error\n");
		exit(-1);
	}
}

void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int find(int tmpx,int tmpy, int* arr, int tm)
{
	for(int i=1;i<=tm;++i)
	{
		if(tmpx==arr[i*2]&&tmpy==arr[i*2+1]) return 1;
	}
	
	return 0;
}

int main(void)
{

	pid_t pid;

	int r;

	const char *memname = "sample";
	const size_t region_size = sysconf(_SC_PAGE_SIZE);

	int fd = shm_open(memname, O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd == -1)
	  error_and_die("shm_open");

	r = ftruncate(fd, region_size);
	if (r != 0)
	error_and_die("ftruncate");

	int *ptr = (int *)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
	  error_and_die("mmap");
	
	//ptr[4]=x, ptr[5]=y, ptr[6]=0;
	
	int SEED;
	printf("prog1 ");
	scanf("%d", &SEED);
	srand(SEED);
	
	tell_wait();

	pid = fork();
	int count=0;

	if(pid<0)
	{
		fprintf(stderr,"Fork Failed\n");
		exit(-1);
	}
	else if(pid==0)
	{
		wait_parent();
		int x, y;
		x=rand()%10, y=rand()%10;
		x=rand()%10, y=rand()%10;

		tell_parent(getppid());
		
		wait_parent();
		printf("[%dChild]:OK\n",getpid());
		tell_parent(getppid());
		
		for(int i=0;i<5;++i)
		{				
			wait_parent();
			int tmpx=ptr[0]-x, tmpy=ptr[1]-y;
			if(tmpx==0&&tmpy==0) { ptr[6]=1;  printf("[%dChild]:You win\n",getpid()); i=6; ptr[4]=x, ptr[5]=y;}
			else if(i==4) { printf("[%dChild]:Miss, you lose\n",getpid()); ptr[4]=x, ptr[5]=y;}
			else if(tmpx==0) 
			{
				if(tmpy>0)
				{
					printf("[%dChild]:Miss up\n",getpid());
					ptr[13]=ptr[1];
				}
				else
				{
					printf("[%dChild]:Miss down\n",getpid());
					ptr[12]=ptr[1];
				} 
				ptr[9]=1;
				ptr[10]=ptr[0],ptr[11]=ptr[0]; 
			}
			else if(tmpy==0) 
			{
				if(tmpx>0)
				{
					printf("[%dChild]:Miss left\n",getpid());
					ptr[11]=ptr[0];
				}
				else
				{ 
					printf("[%dChild]:Miss right\n",getpid());
					ptr[10]=ptr[0];
				} 
				ptr[8]=1; 
				ptr[12]=ptr[1],ptr[13]=ptr[1];
			}
			else if(tmpx>0)
			{
				if(tmpy>0) {printf("[%dChild]:Miss up left\n",getpid()); ptr[11]=ptr[0], ptr[13]=ptr[1];}
				else {printf("[%dChild]:Miss down left\n",getpid()); ptr[11]=ptr[0], ptr[12]=ptr[1];}
			}
			else if(tmpx<0)
			{
				if(tmpy>0) {printf("[%dChild]:Miss up right\n",getpid()); ptr[10]=ptr[0], ptr[13]=ptr[1];}
				else {printf("[%dChild]:Miss down right\n",getpid()); ptr[10]=ptr[0], ptr[12]=ptr[1];}
			}
			//printf("%d %d %d %d %d %d %d %d\n",ptr[4],ptr[5],ptr[10],ptr[11],ptr[12],ptr[13],tmpx, tmpy);
			
			ptr[2]=tmpx, ptr[3]=tmpy;

			//if(++count==0) exit(0);
			tell_parent(getppid());
		}

	}
	else
	{

		ptr[6]=0, ptr[7]=SEED, ptr[8]=0, ptr[9]=0;
		tell_child(pid);
		wait_child();
		
		printf("[%dParent]:Create a child %d\n",getpid(),pid);
		tell_child(pid);
		wait_child();
		
		int /*_x=0, _y=0, x_=10, y_=10, */nowx=0, nowy=0;
		ptr[10]=0, ptr[11]=10, ptr[12]=0, ptr[13]=10;
		int ar[20];
		
		for(int i=0;i<5;++i)
		{
			if(ptr[6]) break;		
			
			//if(ptr[8]) _y=ptr[1], y_=ptr[1];
//			else
			//else if(ptr[2]<0) _x=_x+ptr[2]*(-1);
			//else x_=x_-ptr[2];
			
//			if(ptr[9]) _x=ptr[0], x_=ptr[0];
//			else 
//			else  if(ptr[3]<0)_y=_y+ptr[3]*(-1);
//			else y_=y_-ptr[3];
//			printf("%d %d %d %d\n",_x,x_,_y,y_);
			
//			if(x_-_x) nowx=rand()%(x_-_x)+_x;
//			if(y_-_y) nowy=rand()%(y_-_y)+_y;
			if(ptr[11]-ptr[10]) nowx=rand()%(ptr[11]-ptr[10])+ptr[10];
			if(ptr[13]-ptr[12]) nowy=rand()%(ptr[13]-ptr[12])+ptr[12];
			
			if(i>0)
			{
				while(find(nowx, nowy, ar, i))
				{
					if(ptr[11]-ptr[10]) nowx=rand()%(ptr[11]-ptr[10])+ptr[10];
					if(ptr[13]-ptr[12]) nowy=rand()%(ptr[13]-ptr[12])+ptr[12];
				}
			}
			
			ptr[0]=nowx, ptr[1]=nowy;
			ar[(i+1)*2]=nowx, ar[(i+1)*2+1]=nowy;
			
			printf("[%dParent]:Guess[%d,%d]\n",getpid(),nowx,nowy);
			
			tell_child(pid);
			wait_child();
		}
		
		waitpid(pid,NULL,0);
		
		printf("[%dParent]:Target[%d,%d]\n",getpid(), ptr[4], ptr[5]);
		
		r = munmap(ptr, region_size);
		if (r != 0)
		  error_and_die("munmap");

		r = shm_unlink(memname);
		if (r != 0)
		  error_and_die("shm_unlink");
		close(fd);
	}

}
