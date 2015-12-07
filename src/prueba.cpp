#include "procesar.h"
#include "separar.h"

using namespace std;

int main()
{
	double sol[45] = {0.4,0.4,0.3,0.2,0.1,0.1,0.3,0.8,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		1,0,0,0,0};
	//~ int ady[4][4];
	char** ady = new char* [8];
	for(int i = 0; i < 8; i++)
	{
		ady[i] = new char[8];
		for(int j = 0; j < 8; j++)
		{
			//~ if (i == j) { ady[i][j] = 0; }
			//~ else { ady[i][j] = 1; }
			//~ 
			ady[i][j] = 0;
		}
	}
	
	ady[2][3] = 1;
	ady[3][2] = 1;
	ady[1][0] = 1;
	ady[0][1] = 1;
	ady[1][2] = 1;
	ady[2][1] = 1;
	ady[3][4] = 1;
	ady[4][3] = 1;
	ady[0][4] = 1;
	ady[4][0] = 1;
	ady[6][0] = 1;
	ady[0][6] = 1;
	ady[5][4] = 1;
	ady[4][5] = 1;
	ady[5][7] = 1;
	ady[7][5] = 1;
	ady[7][6] = 1;
	ady[6][7] = 1;
	
	vector< pair<int,int> > aristas;
	for (int i = 0; i < 8; i++)
	{
		for (int j = i; j < 7; j++)
		{
			if (ady[i][j]){ aristas.push_back(make_pair(i,j)); }
		}
	}
	//~ 
	//~ for (int i = 0; i < aristas.size(); i++)
	//~ {
		//~ cout << aristas[i].first << " " << aristas[i].second << endl;
	//~ }
	
	//~ int agujero = podar_grafo(7,aristas,ady);
	
	
	//~ for (int i = 0; i < 7; i++)
	//~ {
		//~ for (int j = 0; j < 7; j++)
		//~ {
			//~ cout << (int)ady[i][j] << " ";
		//~ }
		//~ cout << endl;
	//~ }
	
	vector< vector<int> > holes = separo_agujero(sol,5,8,ady,5);
	
	for(int i = 0; i < holes.size(); i++)
	{
		cout << endl << "agujero " << i << endl;
		for(int j = 0; j < holes[i].size(); j++)
		{
			cout << holes[i][j] << " ";
		}
	}
	cout << endl;
	
	delete[] ady;

	return 0;
}
