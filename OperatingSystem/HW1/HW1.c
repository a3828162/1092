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


#define NUM 9
#define SIZE (NUM * sizeof(int))

void error_and_die(const char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

int find(int tmpx, int tmpy, int* arr, int tm)
{
	for (int i = 1; i <= tm; ++i)
	{
		if (tmpx == arr[i * 2] && tmpy == arr[i * 2 + 1]) return 1;
	}

	return 0;
}

int main(void)
{

	pid_t pid;

	int r;

	const char* memname = "sample";
	const size_t region_size = sysconf(_SC_PAGE_SIZE);

	int fd = shm_open(memname, O_CREAT | O_TRUNC | O_RDWR, 0666);
	if (fd == -1)
		error_and_die("shm_open");

	r = ftruncate(fd, region_size);
	if (r != 0)
		error_and_die("ftruncate");

	int* ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (ptr == MAP_FAILED)
		error_and_die("mmap");

	int SEED, IM;
	printf("prog1 ");
	scanf("%d", &SEED);
	srand(SEED);

	while (SEED < 0 || SEED>100)
	{
		printf("prog1 ");
		scanf("%d", &SEED);
	}

	ptr[5] = 10;

	pid = fork();
	//int count = 0;

	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed\n");
		exit(-1);
	}
	else if (pid == 0)
	{
		while (ptr[5] == 10);
		int x, y;

		srand(SEED + 1);
		x = rand() % 10, y = rand() % 10;

		printf("[%dChild]:OK\n", getpid());

		ptr[5] = 10;
		ptr[6] = 0;
		for (int i = 0; i < 5; ++i)
		{
			while (ptr[5] == 10);

			int tmpx = ptr[0] - x, tmpy = ptr[1] - y;

			if (ptr[0] == 0 && ptr[1] == 0) { ptr[6] = 1; }

			ptr[7] = tmpx;
			ptr[8] = tmpy;

			if (tmpx > 0) ptr[7] = 3;
			else if (tmpx == 0) ptr[7] = 2;
			else ptr[7] = 1;
			if (tmpy > 0) ptr[8] = 3;
			else if (tmpy == 0) ptr[8] = 2;
			else ptr[8] = 1;

			if (tmpx == 0 && tmpy == 0) { ptr[4] = 1;  printf("[%dChild]:You win\n", getpid()); i = 6; ptr[2] = x, ptr[3] = y; }
			else if (i == 4) { printf("[%dChild]:Miss, you lose\n", getpid()); ptr[2] = x, ptr[3] = y; }
			else if (tmpx == 0)
			{
				if (tmpy > 0) printf("[%dChild]:Miss up\n", getpid());
				else printf("[%dChild]:Miss down\n", getpid());
			}
			else if (tmpy == 0)
			{
				if (tmpx > 0) printf("[%dChild]:Miss left\n", getpid());
				else printf("[%dChild]:Miss right\n", getpid());

			}
			else if (tmpx > 0)
			{
				if (tmpy > 0) { printf("[%dChild]:Miss up left\n", getpid()); }
				else { printf("[%dChild]:Miss down left\n", getpid()); }
			}
			else if (tmpx < 0)
			{
				if (tmpy > 0) { printf("[%dChild]:Miss up right\n", getpid()); }
				else { printf("[%dChild]:Miss down right\n", getpid()); }
			}

			ptr[5] = 10;
		}

	}
	else
	{

		ptr[4] = 0;
		printf("[%dParent]:Create a child %d\n", getpid(), pid);

		int nowx = 0, nowy = 0;
		int _x = 0, x_ = 10, _y = 0, y_ = 10;
		int flag = 0;
		int ar[20];

		ptr[5] = 20;
		ptr[7] = ptr[8] = 0;

		while (ptr[5] == 20);

		for (int i = 0; i < 5; ++i)
		{

			if (ptr[4]) break;

			if (i > 0)
			{

				if (ptr[7] == 2)
				{
					if (ptr[8] == 3) y_ = ptr[1];
					else _y = ptr[1];

					flag = 1;
					_x = ptr[0], x_ = ptr[0];
				}
				else if (ptr[8] == 2)
				{
					if (ptr[7] == 3) x_ = ptr[0];
					else _x = ptr[0];

					flag = 1;
					_y = ptr[1], y_ = ptr[1];
				}
				else if (ptr[7] == 3)
				{
					if (ptr[8] == 3) { x_ = ptr[0], y_ = ptr[1]; }
					else { x_ = ptr[0], _y = ptr[1]; }
				}
				else if (ptr[7] == 1)
				{
					if (ptr[8] == 3) { _x = ptr[0], y_ = ptr[1]; }
					else { _x = ptr[0], _y = ptr[1]; }
				}
			}


			if (flag)
			{
				while (find(nowx, nowy, ar, i))
				{
					if (x_ - _x) nowx = rand() % (x_ - _x) + _x;
					if (y_ - _y) nowy = rand() % (y_ - _y) + _y;
				}
			}
			else
			{
				if (_x == 0 && x_ == 10 && _y == 0 && y_ == 10)
				{
					if (x_ - _x) nowx = rand() % (x_ - _x) + _x;
					if (y_ - _y) nowy = rand() % (y_ - _y) + _y;
				}
				else
				{
					if (x_ - _x) nowx = rand() % (x_ - _x) + _x;
					if (y_ - _y) nowy = rand() % (y_ - _y) + _y;

					if ((_x == 0 && _y == 0) && !ptr[6])
					{
						while ((nowx == x_) || (nowy == y_))
						{
							if (!(_x == 0 && x_ == 1)) nowx = rand() % (x_ - _x) + _x;
							else
							{
								nowx = 0;
								break;
							}

							if (!(_y == 0 && y_ == 1)) nowy = rand() % (y_ - _y) + _y;
							else
							{
								nowy = 0;
								break;
							}

						}
					}
					else if ((_x != 0 && _y != 0) || ptr[6] == 1)
					{
						while ((nowx == _x || nowx == x_) || (nowy == _y || nowy == y_))
						{
							if (!(_x == 0 && x_ == 1)) nowx = rand() % (x_ - _x) + _x;
							else
							{
								nowx = 0;
								break;
							}

							if (!(_y == 0 && y_ == 1)) nowy = rand() % (y_ - _y) + _y;
							else
							{
								nowy = 0;
								break;
							}

						}
					}
					else if (_x != 0)
					{
						while ((nowx == _x || nowx == x_) || (nowy == y_))
						{
							if (!(_x == 0 && x_ == 1)) nowx = rand() % (x_ - _x) + _x;
							else
							{
								nowx = 0;
								break;
							}

							if (!(_y == 0 && y_ == 1)) nowy = rand() % (y_ - _y) + _y;
							else
							{
								nowy = 0;
								break;
							}

						}
					}
					else if (_y != 0)
					{
						while ((nowx == x_) || (nowy == _y || nowy == y_))
						{
							if (!(_x == 0 && x_ == 1)) nowx = rand() % (x_ - _x) + _x;
							else
							{
								nowx = 0;
								break;
							}

							if (!(_y == 0 && y_ == 1)) nowy = rand() % (y_ - _y) + _y;
							else
							{
								nowy = 0;
								break;
							}

						}
					}

				}
			}
			ptr[0] = nowx, ptr[1] = nowy;
			ar[(i + 1) * 2] = nowx, ar[(i + 1) * 2 + 1] = nowy;

			printf("[%dParent]:Guess[%d,%d]\n", getpid(), nowx, nowy);

			ptr[5] = 20;
			while (ptr[5] == 20);
		}

		waitpid(pid, NULL, 0);

		printf("[%dParent]:Target[%d,%d]\n", getpid(), ptr[2], ptr[3]);

		r = munmap(ptr, region_size);
		if (r != 0)
			error_and_die("munmap");

		r = shm_unlink(memname);
		if (r != 0)
			error_and_die("shm_unlink");
		close(fd);

	}

}