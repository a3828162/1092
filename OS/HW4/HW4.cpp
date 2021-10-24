#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
using namespace std;

int available[5] = {}; 
int work[5] = {};
int Max[100][5] = {};
int allocation[100][5] = {};
int need[100][5] = {};
vector<vector<int>> request; 
vector<int> waitingline;  // waiting line 
int Count = 0; //record the number of gid
queue<int> Sequence;
bool waitingcheck = false; //是否要把request放回waiting line
int initial = 0; // 1 = safe, 2 = unsafe

bool safealgorithm(int start, int gid); // do safe algorithm

bool Safe(int tmp1[], int tmp2[]); //判斷work 夠不夠need

bool AllFinish(bool finish[]); // 判斷finish是否全為true

void Execute(int gid); // 執行bank algorithm

void Printgid(int gid); // 輸出request的資料

void Examinateline(); // 當release resourse時檢查waiting line 裡的 request能不能執行

int main(int argc, char* argv[])
{
	ifstream infile(argv[1], ios::in);
	//ifstream infile("prog4data.txt", ios::in);

	if (infile) cout << "Success data open\n";
	else
	{
		cout << "Wrong data open\n";
		exit(1);
	}

	string t;

	getline(infile, t); // read data start
	while (1)
	{
		getline(infile, t);
		if (t[0] == '#') break;
		stringstream ss;
		ss << t;

		for (int i = 0; i < 5; ++i)
		{
			int tmp;
			ss >> tmp;
			available[i] = tmp;
		}
	}

	while (1)
	{
		getline(infile, t);
		if (t[0] == '#') break;
		stringstream ss;
		ss << t;

		int t0;

		ss >> t0;
		
		for (int i = 0; i < 5; ++i)
		{
			int tmp;
			ss >> tmp;
			Max[t0][i] = tmp;
		}

		++Count;
	}

	while (1)
	{
		getline(infile, t);
		if (t[0] == '#') break;
		stringstream ss;
		ss << t;

		int t0;

		ss >> t0;

		for (int i = 0; i < 5; ++i)
		{
			int tmp;
			ss >> tmp;
			allocation[t0][i] = tmp;
		}
	}

	while (getline(infile, t))
	{
		stringstream ss;
		ss << t;

		vector<int> a;
		int b;

		for (int i = 0; i < 6; ++i)
		{
			ss >> b;
			a.push_back(b);
		}
		char c;
		ss >> c;
		if (c == 'a') a.push_back(100); // 100 = allocate
		else if (c == 'r') a.push_back(200); // 200 = realease

		request.push_back(a);
	}

	// read data end

	for (int i = 0; i < Count; ++i) // caculate need
	{
		for (int j = 0; j < 5; ++j)
		{
			need[i][j] = Max[i][j] - allocation[i][j];
		}
	}

	// 判斷初始狀態
	cout << "Initial state:";
	if (safealgorithm(1, 100))
	{
		cout << " safe, safe sequence = [";

		while (1)
		{
			cout << Sequence.front();
			Sequence.pop();
			if (Sequence.empty()) break;
			else cout << ",";
		}

		cout << "]\n";
		initial = 1;
	}
	else
	{
		cout << " unsafe\n";
		while (!Sequence.empty()) Sequence.pop();
		initial = 2;
	}

	// 處理 request
	for (int i = 0; i < request.size(); ++i)
	{
		if (request[i][6] == 100)
		{
			if (initial == 1) Execute(i); 
			else
			{
				Printgid(i);
				cout << ": initial state is unsafe, wait for resource release\n";
				waitingline.push_back(i);
			}
		}
		else if (request[i][6] == 200)
		{
			bool ttt = true;

			for (int j = 0; j < 5; ++j) //判斷夠不夠多資源可以resourse
			{
				if (allocation[request[i][0]][j] < request[i][j])
				{
					ttt = false;
					break;
				}
			}

			if (ttt)
			{
				Printgid(i);
				for (int j = 0; j < 5; ++j)
				{
					available[j] += request[i][j + 1];
					allocation[request[i][0]][j] -= request[i][j + 1];
					need[request[i][0]][j] += request[i][j + 1];
				}

				cout << ": RELEASE granted, AVAILABLE = (";
				for (int j = 0; j < 5; ++j)
				{
					cout << available[j];
					if (j != 4) cout << ",";
				}
				cout << ")\n";

				Examinateline();
			}
			else
			{
				Printgid(i);
				cout << ": RELEASE no granted\n";
			}
		}
	}

	return 0;
}

void Execute(int gid)
{
	// 判斷是否合法
	for (int i = 0; i < 5; ++i)
	{
		if (request[gid][i + 1] > need[request[gid][0]][i])
		{
			Printgid(gid);
			cout << ": NEED error, request aborted\n";
			waitingcheck = true;
			return;
		}
	}

	Printgid(gid);
	cout << ": NEED OK\n";

	for (int i = 0; i < 5; ++i)
	{
		if (request[gid][i + 1] > available[i])
		{
			Printgid(gid);
			cout << ": AVAILABLE error, request aborted\n";
			waitingcheck = true;
			return;
		}
	}

	Printgid(gid);
	cout << ": AVAILABLE OK\n";

	Printgid(gid);
	cout << ": WORK = (";

	for (int i = 0; i < 5; ++i) available[i] -= request[0][i + 1], //按照request給資源
		allocation[request[0][0]][i] += request[0][i + 1],
		need[request[0][0]][i] -= request[0][i + 1];

	for (int i = 0; i < 5; ++i)
	{
		cout << available[i];
		if (i != 4) cout << ",";
	}
	cout << ")\n";

	// 判斷是否granted 是則給資源 否則進入waiting line
	if (safealgorithm(2, gid))
	{
		Printgid(gid);
		cout << ": request granted, safe sequence = [";
		while (1)
		{
			cout << Sequence.front();
			Sequence.pop();
			if (Sequence.empty()) break;
			else cout << ",";
		}
		cout << "]\n";
		waitingcheck = true;
	}
	else
	{
		Printgid(gid);
		cout << ": requese no granted, wait for source release\n"; 

		for (int i = 0; i < 5; ++i) available[i] += request[0][i + 1], // 還原狀態
			allocation[request[0][0]][i] -= request[0][i + 1], 
			need[request[0][0]][i] += request[0][i + 1];
		
		bool q = true;
		for (int i = 0; i < waitingline.size(); ++i)
		{
			if (waitingline[i] == gid) q = false;
		}
		if(q) waitingline.push_back(gid);
		waitingcheck = false;
	}
}

void Examinateline()
{
	vector<int> tmptmp;
	for (vector<int>::iterator it = waitingline.begin(); it != waitingline.end();++it)
	{
		Execute(*it);
		if(!waitingcheck) tmptmp.push_back(*it);
	}
	waitingline = tmptmp;
}

void Printgid(int gid)
{
	cout << "(";
	for (int i = 0; i < 6; ++i)
	{
		cout << request[gid][i];
		if (i != 5) cout << ",";
	}
	cout << ")";
}

bool safealgorithm(int start, int gid) // do safe algorithm
{
	bool Finish[100] = {};

	for (int i = 0; i < 5; ++i) work[i] = available[i];

	for (int i = 0; i < Count; ++i)
	{
		if (!Finish[i] && Safe(work, need[i]))
		{
			for (int j = 0; j < 5; ++j) work[j] += allocation[i][j];
			Finish[i] = true;
			Sequence.push(i);

			if (start == 2) // start 1 代表是初始那一次 2代表是request
			{
				Printgid(gid);
				cout << ": gid " << i << " finish, WORK = (";
				for (int i = 0; i < 5; ++i)
				{
					cout << work[i];
					if (i != 4) cout << ",";
				}
				cout << ")\n";
			}

			i = 0;
		}
	}

	if (AllFinish(Finish)) return true;
	else return false;
}

bool Safe(int tmp1[], int tmp2[])
{
	for (int i = 0; i < 5; ++i) if (tmp1[i] < tmp2[i]) return false;

	return true;
}

bool AllFinish(bool Finish[])
{
	for (int i = 0; i < Count; ++i) if (!Finish[i]) return false;

	return true;
}