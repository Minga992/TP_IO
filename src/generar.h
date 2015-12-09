#include <vector>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <fstream>

using namespace std;

void exportar(int cant_nodos, double densidad, vector< pair<int,int> > aristas)
{
	// sale con formato dimac
	
	char nombre[9];
	
	int dens = densidad*10;	// para los valores que usamos de densidad, esto sirve
	
	sprintf(nombre,"inst_%d_%d",cant_nodos,dens);
	
	ofstream archivo;
	archivo.open(nombre);
	
	archivo << "p edge " << cant_nodos << " " << aristas.size() << endl;
	
	for (int i = 0; i < aristas.size(); i++)
	{	cout << i << endl;
		archivo << "e " << aristas[i].first+1 << " " << aristas[i].second+1 << endl;
	}
	
	archivo.close();
}

void generar_grafo(int cant_nodos, double densidad)
{
	vector< pair<int,int> > aristas_A;
	vector< pair<int,int> > aristas_B;
	
	// todas las aristas posibles
	for (int i = 0; i < cant_nodos; i++)
	{
		for (int j = i+1; j < cant_nodos; j++)
		{
			aristas_A.push_back(make_pair(i,j));
		}
	}
	
	// voy a filtrar por lo mas chico
	int cant;
	if (densidad > 0.5){
		cant = aristas_A.size()*(1-densidad);
	}else{
		cant = aristas_A.size()*densidad;
	}
	
	int r;	
	vector< pair<int,int> >::iterator it;
	for (int i = 0; i < cant; i++)
	{
		it = aristas_A.begin();
		r = rand() % aristas_A.size();
		it += r;
		aristas_B.push_back(*it);
		it = aristas_A.erase(it);
	}
	
	if (densidad > 0.5){
		// las aristas de mi grafo están en aristas_A
		exportar(cant_nodos, densidad, aristas_A);
	}else{
		// las aristas de mi grafo están en aristas_B
		exportar(cant_nodos, densidad, aristas_B);
	}
}

char** genero_cliques(int tam, int& n)
{
	int cant_puentes = 0;
	vector<char> puentes(tam,0);
	for (int  i = 0; i < tam; i++)
	{
		puentes[i] = rand() % 2;
		if(puentes[i] == 1){ cant_puentes++; } 
	}
	
	n = tam + cant_puentes*(tam-1);
	char** ady = new char* [n];
	for(int i = 0; i < n; i++)
	{
		ady[i] = new char[n];
		for(int j = 0; j < n; j++)
		{
			ady[i][j] = 0;
		}
	}
	
	for (int i = 0; i < tam; i++)
	{
		for (int j = 0; j < tam; j++)
		{
			if (i != j){ ady[i][j] = 1; }
		}
	}
	
	int j = tam;
	for (int i = 0; i < tam; i++)
	{
		if (puentes[i] == 1)
		{
			for (int c = 0; c < tam-1; c++)
			{
				ady[i][j] = 1;
				ady[j][i] = 1;
				j++;
			}
		}
	}
	
	for (int c = 0; c < cant_puentes; c++)
	{
		for (int i = 0; i < tam-1; i++)
		{
			int ii = tam+(tam-1)*c + i;
			for (j = 0; j < tam-1; j++)
			{
				int jj = tam+(tam-1)*c + j;
				if(ii != jj){ 
					ady[ii][jj] = 1; 
					ady[jj][ii] = 1; 
				}
			}
		}
	}
	
	return ady;
}


char** genero_agujeros(int tam, int& n)
{
	//~ int cant_puentes = 0;
	n = tam;
	vector<int> apendices(tam,0);
	for (int  i = 0; i < tam; i++)
	{
		apendices[i] = rand() % (tam+2);
		//~ if(puentes[i] == 1){ cant_puentes++; } 
		if(apendices[i] < tam)
		{
			n += apendices[i];
		}else if (apendices[i] == tam){
			n += tam-1;
		}else{
			n += 2;
		}
	}
	
	//~ n = tam + cant_puentes*(tam-1);
	char** ady = new char* [n];
	for(int i = 0; i < n; i++)
	{
		ady[i] = new char[n];
		for(int j = 0; j < n; j++)
		{
			ady[i][j] = 0;
		}
	}
	
	for (int i = 0; i < tam; i++)
	{
		ady[i][(i+1)%tam] = 1;
		ady[(i+1)%tam][i] = 1;
	}
	
	int j = tam;
	for (int i = 0; i < tam; i++)
	{
		if (apendices[i] < tam)
		{
			for (int h = 0; h < apendices[i]; h++)
			{
				ady[i][j] = 1;
				ady[j][i] = 1;
				j++;
			}
		}else if (apendices[i] == tam+1){
			ady[i][j] = 1;
			ady[j][i] = 1;
			ady[j][j+1] = 1;
			ady[j+1][j] = 1;
			j += 2;
		}else{
			ady[i][j] = 1;
			ady[j][i] = 1;
			for (int h = 0; h < tam-2; h++)
			{
				ady[j][j+1] = 1;
				ady[j+1][j] = 1;
				j++;
			}
			ady[i][j] = 1;
			ady[j][i] = 1;
			j++;
		}
		
		
		//~ if (puentes[i] == 1)
		//~ {
			//~ for (int c = 0; c < tam-1; c++)
			//~ {
				//~ ady[i][j] = 1;
				//~ j++;
			//~ }
		//~ }
	}
	
	//~ for (int c = 0; c < cant_puentes; c++)
	//~ {
		//~ for (int i = 0; i < tam-1; i++)
		//~ {
			//~ int ii = tam+(tam-1)*c + i;
			//~ for (j = 0; j < tam-1; j++)
			//~ {
				//~ int jj = tam+(tam-1)*c + j;
				//~ if(ii != jj){ ady[ii][jj] = 1; }
			//~ }
		//~ }
	//~ }
	
	return ady;
}
