#include "generar.h"
#include "procesar.h"
#include "separar.h"

using namespace std;

int main(int argc, char **argv)
{	
	int cant_vert;
	int cant_aris;
	vector< pair<int,int> > aristas;
	//~ char* archivo = "prueba.txt";
	procesar_archivo(cant_vert, cant_aris, aristas, argv[1]);
	cout << "procese" << endl;
	char** ady = new char*[cant_vert];
	for (int h = 0; h < cant_vert; h++)
	{
		ady[h] = new char[cant_vert];
		for(int j = 0; j < cant_vert; j++)
		{
			ady[h][j] = 0;
		}
	}
	for(int h = 0; h < aristas.size(); h++)
	{
		ady[aristas[h].first][aristas[h].second] = 1;
		ady[aristas[h].second][aristas[h].first] = 1;
	}
	cout << "ya tengo matriz" << endl;
	int n = cant_vert*cant_vert+cant_vert;
	
	//~ srand(time(NULL));
	
	//~ int n;
	//~ char** ady = genero_cliques(5,n);
	//~ char** ady = genero_agujeros(5,n);
	//~ 
	//~ vector< pair<int,int> > aristas;
	//~ for (int i = 0; i < n; i++)
	//~ {
		//~ for (int j = i; j < n; j++)
		//~ {
			//~ if (ady[i][j] && ady[j][i]){ aristas.push_back(make_pair(i,j)); }
		//~ }
	//~ }
	//~ 
	//~ ofstream archivo;
	//~ archivo.open("prueba.txt");
	//~ 
	//~ archivo << "p edge " << n << " " << aristas.size() << endl;
	//~ 
	//~ for (int i = 0; i < aristas.size(); i++)
	//~ {
		//~ archivo << "e " << aristas[i].first+1 << " " << aristas[i].second+1 << endl;
	//~ }
	//~ 
	//~ archivo.close();
	cout << cant_vert << ":";
	for (int i = 0; i < aristas.size(); i++)
	{
		cout << aristas[i].first+1 << "-" << aristas[i].second+1 << ",";
	}
	cout << endl;
	
	double* sol = new double[n]; 
	for (int i = 0; i < n; i++){ sol[i] = 0; }
	sol[0] = 0.5;
	sol[1] = 0.5;
	sol[2] = 0.5;
	sol[4] = 0.5;
	sol[5] = 0.5;
	sol[6] = 0.5;
	sol[7] = 0.5;
	sol[8] = 0.5;
	sol[10] = 1;
	sol[13] = 0.5;
	sol[14] = 0.5;
	sol[15] = 0.5;
	sol[17] = 0.5;
	sol[19] = 0.5;
	sol[20] = 0.5;
	sol[24] = 1;
	sol[29] = 0.5;
	sol[35] = 0.5;
	sol[38] = 0.5;
	sol[42] = 0.5;
	sol[44] = 0.5;
	sol[47] = 0.5;
	sol[48] = 0.5;
	sol[51] = 0.5;
	sol[169] = 1;
	sol[170] = 1;
	sol[171] = 0.5;
	sol[172] = 0.5;
cout << "ya tengo sol" << endl;
	//~ for (int i = 0; i < n; i++)
	//~ {
		//~ sol[i] = 0.9;
	//~ }
	//~ sol[n] = 1;
	
	vector< vector<int> > cliques = separo_clique(sol,cant_vert,cant_vert,ady,5);
	cout << "ya tengo cliques" << endl;
	for(int i = 0; i < cliques.size(); i++)
	{
		cout << endl << "clique " << i << endl;
		for(int j = 0; j < cliques[i].size(); j++)
		{
			cout << cliques[i][j]+1 << " ";
		}
	}
	cout << endl;
	cout << "ya me voy" << endl;
	//~ int agujero = podar_grafo(n,aristas,ady);
	//~ 
	//~ vector< pair<int,int> > aristas2;
	//~ for (int i = 0; i < n; i++)
	//~ {
		//~ for (int j = i; j < n; j++)
		//~ {
			//~ if (ady[i][j]){ aristas2.push_back(make_pair(i,j)); }
		//~ }
	//~ }
	//~ 
	//~ cout << n << ":";
	//~ for (int i = 0; i < aristas2.size(); i++)
	//~ {
		//~ cout << aristas2[i].first+1 << "-" << aristas2[i].second+1 << ",";
	//~ }
	//~ cout << endl;
	//~ 
	//~ vector< vector<int> > holes = separo_agujero(sol,1,n,ady,10);
	//~ 
	//~ for(int i = 0; i < holes.size(); i++)
	//~ {
		//~ cout << endl << "agujero " << i << endl;
		//~ for(int j = 0; j < holes[i].size()-1; j++)
		//~ {
			//~ cout << holes[i][j]+1 << " ";
		//~ }
	//~ }
	//~ cout << endl;
	
	delete[] ady;
	delete[] sol;

	return 0;
}
