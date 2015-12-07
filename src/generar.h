#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;

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
