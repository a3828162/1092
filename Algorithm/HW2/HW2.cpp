#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct edge
{
	int u;
	int v;
	int cost;

	bool operator<(const edge& r) { return cost < r.cost; }
};

vector<edge> edges;
vector<int> parent, Rank;
int n, m, cost;

int find(int f)
{
	return parent[f] == f ? f : parent[f] = find(parent[f]);
}

void Kruskal()
{
	int j = 0;

	for (int i = 0; i < edges.size(); ++i)
	{
		int a = find(edges[i].u), b = find(edges[i].v);

		if (a != b)
		{
			if (Rank[a] < Rank[b]) parent[a] = b;
			else if (Rank[a] > Rank[b]) parent[b] = a;
			else if (Rank[a] == Rank[b]) parent[a] = b, ++Rank[b];

			cost *= edges[i].cost;
			cost %= 65537;

			if (++j == n - 1) return;
		}
	}

}

int main()
{
	while (cin >> n >> m && (n || m))
	{
		parent.assign(n, 0);
		Rank.assign(n, 0);
		edges.assign(m, edge());
		cost = 1;

		for (int i = 0; i < m; ++i)
		{
			cin >> edges[i].u >> edges[i].v >> edges[i].cost;
		}

		for (int i = 0; i < n; ++i) parent[i] = i;

		sort(begin(edges), end(edges));

		Kruskal();

		cout << cost << "\n";
		//cout << cost%65537 << "\n";
	}

	return 0;
}