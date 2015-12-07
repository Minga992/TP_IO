#include "generar.h"
#include "procesar.h"
#include "separar.h"

using namespace std;

int main()
{	
	srand(time(NULL));
	
	int n;
	char** ady = genero_cliques(5,n);
	//~ char** ady = genero_agujeros(5,n);
	
	vector< pair<int,int> > aristas;
	for (int i = 0; i < n; i++)
	{
		for (int j = i; j < n; j++)
		{
			if (ady[i][j]){ aristas.push_back(make_pair(i,j)); }
		}
	}
	
	cout << n << ":";
	for (int i = 0; i < aristas.size(); i++)
	{
		cout << aristas[i].first+1 << "-" << aristas[i].second+1 << ",";
	}
	cout << endl;
	
	double* sol = new double[n+1]; 
	for (int i = 0; i < n; i++)
	{
		sol[i] = 0.9;
	}
	sol[n] = 1;
	
	vector< vector<int> > cliques = separo_clique(sol,1,n,ady,10);
	
	for(int i = 0; i < cliques.size(); i++)
	{
		cout << endl << "clique " << i << endl;
		for(int j = 0; j < cliques[i].size()-1; j++)
		{
			cout << cliques[i][j]+1 << " ";
		}
	}
	cout << endl;
	
	//~ int agujero = podar_grafo(7,aristas,ady);
	
	
	//~ for (int i = 0; i < 7; i++)
	//~ {
		//~ for (int j = 0; j < 7; j++)
		//~ {
			//~ cout << (int)ady[i][j] << " ";
		//~ }
		//~ cout << endl;
	//~ }
	//~ vector< vector<int> > holes = separo_agujero(sol,5,8,ady,5);
	//~ 
	//~ for(int i = 0; i < holes.size(); i++)
	//~ {
		//~ cout << endl << "agujero " << i << endl;
		//~ for(int j = 0; j < holes[i].size(); j++)
		//~ {
			//~ cout << holes[i][j] << " ";
		//~ }
	//~ }
	//~ cout << endl;
	//~ 
	delete[] ady;
	delete[] sol;

	return 0;
}
