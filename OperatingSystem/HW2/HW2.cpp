#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
using namespace std;

struct data
{
    string ID;
    string Da;
    int start;
    int tmm;
};

void* searchf(void* arg)
{

    data* D = (data*)arg;

    pthread_t tid = pthread_self();

    cout << "[Tid=" << tid << "] search " << D->ID << " at " << ((D->start - 1) * (D->Da.size() / 2)) << " ";

    vector<int> tmp;

    // print 8 words and push the find position to the vector
    int aa = 0;
    for (int start = ((D->start - 1) * (D->Da.size() / D->tmm)); start < ((D->start) * (D->Da.size() / D->tmm)); start++)
    {

        if (aa < 8) cout << D->Da[start];
        aa++;
        for (int j = 0; j < D->ID.size() && (start + j) < D->Da.size() && D->Da[start + j] == D->ID[j]; j++)
        {
            if (j == D->ID.size() - 1)
            {
                tmp.push_back(start);
            }
        }

    }

    // allocate the memory to store the data

    cout << " ";
    int* result = (int*)malloc(sizeof(int) * (tmp.size() + 1));

    result[0] = tmp.size();

    // push the data to memory
    // memory[0] is its size

    for (int i = 0; i < tmp.size(); ++i)
    {
        result[i + 1] = tmp[i];
        cout << result[i + 1] << " ";
    }

    cout << endl;


    //return the memory address

    pthread_exit((void*)result);
}

int main(int argc, char* argv[]) {

    struct rusage ru;
    struct timeval utime;
    struct timeval stime;


    fstream file(argv[1], ios::in);

    if (!file)
    {
        cout << "Wrong open data\n";
        exit(1);
    }

    string s, s1, s2;;

    // read data

    int tm = 0;
    file >> s >> tm;

    vector<string> totalstring;

    string tmps;
    while (file >> tmps) totalstring.push_back(tmps);

    void* ret;

    // distinguish ? in input data and change it

    for (vector<string>::iterator it = totalstring.begin(); it != totalstring.end();)
    {
        bool x = false;
        for (int j = 0; j < (*it).size(); ++j)
        {
            if ((*it)[j] == '?')
            {
                string t1, t2, t3, t4;
                t1 = *it;
                t1[j] = 'A';
                t2 = *it;
                t2[j] = 'T';
                t3 = *it;
                t3[j] = 'C';
                t4 = *it;
                t4[j] = 'G';

                totalstring.erase(it);
                totalstring.insert(totalstring.begin(), t1);
                totalstring.insert(totalstring.begin(), t2);
                totalstring.insert(totalstring.begin(), t3);
                totalstring.insert(totalstring.begin(), t4);

                x = true;
                break;
            }
        }
        if (x) it = totalstring.begin();
        else ++it;
    }

    // distinguish {} in input data and change it

    for (vector<string>::iterator it = totalstring.begin(); it != totalstring.end();)
    {
        bool flag = false;
        for (int j = 0; j < (*it).size(); ++j)
        {

            if ((*it)[j] == '{')
            {
                int aaa = j;
                vector<char> q;
                while (1)
                {
                    ++aaa;

                    if ((*it)[aaa] == 'A' || (*it)[aaa] == 'T' || (*it)[aaa] == 'C' || (*it)[aaa] == 'G') q.push_back((*it)[aaa]);

                    if ((*it)[aaa] == '}')
                    {
                        string e = (*it);
                        totalstring.erase(it);
                        for (int i = 0; i < q.size(); ++i)
                        {

                            string tmmm;
                            bool y = true;
                            int count = 0;
                            for (int k = 0; k < e.size(); ++k)
                            {
                                if (e[k] == '{' && count == 0) y = false;
                                if (y) { tmmm.push_back(e[k]); }
                                if (e[k] == '}' && count == 0) { y = true; count = 1; }
                            }

                            if (q[i] == 'A') tmmm.insert(j, "A");
                            else if (q[i] == 'T') tmmm.insert(j, "T");
                            else if (q[i] == 'C') tmmm.insert(j, "C");
                            else if (q[i] == 'G') tmmm.insert(j, "G");

                            totalstring.insert(totalstring.begin(), tmmm);
                        }

                        flag = true;
                        break;
                    }
                }
                if (flag) break;
            }
        }
        if (flag) it = totalstring.begin();
        else ++it;
    }

    // dynamic array

    pthread_t* callThd = new pthread_t[totalstring.size() * tm];

    data* dat = new data[totalstring.size() * tm];

    // create pthread and give it data

    int tt = 0;
    for (int i = 0; i < totalstring.size(); ++i)
    {
        for (int j = 1; j <= tm; ++j)
        {
            dat[tt].ID = totalstring[i];
            dat[tt].Da = s;
            dat[tt].start = j;
            dat[tt].tmm = tm;
            pthread_create(&callThd[tt], NULL, searchf, (void*)&dat[tt]);
            tt++;
        }
    }

    // use map+set to save the data

    map<string, set<int>> anss;

    for (vector<string>::iterator it = totalstring.begin(); it != totalstring.end(); ++it) anss[*it];

    // use join and get return address then put the answer in the map

    for (int i = 0; i < totalstring.size() * tm; ++i)
    {
        pthread_join(callThd[i], &ret);
        int* result = (int*)ret;

        if (result[0] != 0)
        {
            int tcc = result[0] + 1;
            for (int j = 1; j < tcc; ++j) anss[dat[i].ID].insert(result[j]);

        }

        // free the memory

        free(result);
    }

    // print the answer

    map<string, set<int>>::iterator iter;
    for (iter = anss.begin(); iter != anss.end(); ++iter)
    {
        cout << "[" << iter->first << "]" << " ";
        for (set<int>::iterator it = iter->second.begin(); it != iter->second.end(); ++it) cout << *it << " ";
        cout << endl;
    }

    // get user time and system time
    getrusage(RUSAGE_SELF, &ru);

    utime = ru.ru_utime;
    stime = ru.ru_stime;
    double utime_used = utime.tv_sec + (double)utime.tv_usec / 1000000.0;
    double stime_used = stime.tv_sec + (double)stime.tv_usec / 1000000.0;

    double CPUtime = utime_used + stime_used;
    CPUtime *= 1000;
    cout << "CPU time: " << CPUtime << " ms" << endl;

    // delete the dynamic array

    delete[] dat;
    delete[] callThd;
    return 0;
}