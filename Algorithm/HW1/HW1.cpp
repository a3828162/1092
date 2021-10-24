#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

int LCS(string st1, string st2)
{
    vector<vector<int>> len(st1.size(), vector<int>(st2.size()));

    for (size_t i = 1; i < st1.size(); ++i)
        for (size_t j = 1; j < st2.size(); ++j)
            st1[i] == st2[j] ? len[i][j] = len[i - 1][j - 1] + 1 : len[i][j] = max(len[i - 1][j], len[i][j - 1]);
            //if (st1[i] == st2[j]) len[i][j] = len[i - 1][j - 1] + 1;
            //else len[i][j] = max(len[i - 1][j], len[i][j - 1]);

    return len[st1.size() - 1][st2.size() - 1];
}

int main()
{
    string s1, s2;

    while (cin >> s1 >> s2)
    {
        string s11, s22;
        s11 = "0" + s1;
        s22 = "0" + s2;
        size_t Size = LCS(s11, s22);
        cout << Size << "\n";
    }

	return 0;
}