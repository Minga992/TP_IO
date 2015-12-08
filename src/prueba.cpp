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
	//~ cout << n << ":";
	//~ for (int i = 0; i < aristas.size(); i++)
	//~ {
		//~ cout << aristas[i].first+1 << "-" << aristas[i].second+1 << ",";
	//~ }
	//~ cout << endl;
	//~ 
	double* sol = new double[n]; 
	for (int i = 0; i < n; i++){ sol[i] = 0; }
	sol[1] = 1;
	sol[5] = 0.5;
	sol[6] = 0.5;
	sol[7] = 0.5;
	sol[8] = 0.5;
	sol[9] = 0.333333;
	sol[10] = 0.333333;
	sol[11] = 0.333333;
	sol[12] = 0.666667;
	sol[13] = 0.333333;
	sol[15] = 0.333333;
	sol[16] = 0.333333;
	sol[17] = 0.333333;
	sol[19] = 0.333333;
	sol[21] = 0.166667;
	sol[22] = 0.333333;
	sol[25] = 0.333333;
	sol[26] = 0.166667;
	sol[28] = 0.166667;
	sol[29] = 0.166667;
	sol[30] = 0.166667;
	sol[32] = 0.166667;
	sol[35] = 0.166667;
	sol[36] = 0.166667;
	sol[37] = 0.166667;
	sol[39] = 0.166667;
	sol[41] = 0.166667;
	sol[42] = 0.166667;
	sol[43] = 0.166667;
	sol[44] = 0.166667;
	sol[46] = 0.166667;
	sol[47] = 0.166667;
	sol[48] = 0.166667;
	sol[49] = 0.166667;
	sol[50] = 0.166667;
	sol[52] = 0.166667;
	sol[54] = 0.166667;
	sol[55] = 0.166667;
	sol[56] = 0.166667;
	sol[57] = 0.166667;
	sol[59] = 0.166667;
	sol[60] = 0.166667;
	sol[62] = 0.166667;
	sol[63] = 0.166667;
	sol[65] = 0.166667;
	sol[67] = 0.166667;
	sol[68] = 0.166667;
	sol[69] = 0.166667;
	sol[70] = 0.166667;
	sol[72] = 0.166667;
	sol[75] = 0.166667;
	sol[76] = 0.166667;
	sol[169] = 1;
	sol[170] = 0.333333;
	sol[171] = 0.166667;
	sol[172] = 0.166667;
	sol[173] = 0.166667;
	sol[174] = 0.166667;
	cout << "ya tengo sol" << endl;
	//~ for (int i = 0; i < n; i++)
	//~ {
		//~ sol[i] = 0.9;
	//~ }
	//~ sol[n] = 1;
	
	vector< vector<int> > cliques = separo_clique(sol,cant_vert,cant_vert,ady,10);
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
