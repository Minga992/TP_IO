#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

void procesar_archivo(int& cant_vert, int& cant_aris, vector< pair<int,int> >& aristas, char* nom_archivo)
{
	ifstream archivo;
	archivo.open(nom_archivo);

	char c;
	string ignorar;
	int a, b;

	while(true){
		c = archivo.peek();
		switch(c)
		{
			case 'p':
				archivo >> c;	// p
				archivo >> ignorar;	// edge
				archivo >> cant_vert;	// cantidad de nodos
				archivo >> cant_aris;	// cantidad de aristas
				break;

			case 'e':
				archivo >> c;	// e
				archivo >> a;	// un extremo de la arista
				archivo >> b;	// el otro extremo
				aristas.push_back(make_pair(a-1,b-1));	// resto 1 porque vienen de 1 a n y los usamos de 0 a n-1
				break;

			default:
				getline(archivo,ignorar);	// es una linea que no me interesa
		}
		if(archivo.peek() == EOF) { break; }
	}

	archivo.close();
}

void armar_particion(int cant_vert,int& cant_aris,int cant_part,
	vector< vector<int> >& particion,vector< pair<int,int> >& aristas){
	srand(time(NULL));
	vector<int> nodos;
	for(int i = 0; i < cant_vert; i++) {
		nodos.push_back(i);
	}

	int pertenencia[cant_vert];
	int n;
	vector<int>::iterator it;
	// primera tanda para que no queden conj vacios
	for (int i = 0; i < cant_part; i++)
	{
		n = rand() % nodos.size();
		it = nodos.begin() + n;
		particion[i].push_back(*it);
		pertenencia[*it] = i;
		it = nodos.erase(it);
	}
	int p;
	// asigno los demas
	while(nodos.size() != 0)
	{
		n = rand() % nodos.size();
		it = nodos.begin() + n;
		p = rand() % cant_part;
		particion[p].push_back(*it);
		pertenencia[*it] = p;
		it = nodos.erase(it);
	}
	
	//~ int x;
	//~ int y;
	//~ 
	//~ for (int i = 0; i < cant_part; i++)
	//~ {
		//~ cout << endl << "Cantidad de nodos en conjunto " << i << ": ";
		//~ cin >> x;
		//~ cout << endl << "Ingresar los nombres de los nodos: " << endl;
		//~ for (int j = 0; j < x; j++)
		//~ {
			//~ cin >> y;
			//~ particion[i].push_back(y);
			//~ pertenencia[y] = i;
		//~ }
	//~ }
	
	int a,b;
	vector< pair<int,int> >::iterator ita = aristas.begin();

	while(ita != aristas.end())
	{
		a = (*ita).first;
		b = (*ita).second;
		// si la arista esta adentro de un conjunto, la fleto, si no no
		if(pertenencia[a] == pertenencia[b]) {
			ita = aristas.erase(ita);
		}else{
			ita++;
		}
	}

	cant_aris = aristas.size();
}

int podar_grafo(int cant_vert,const vector< pair<int,int> >& aristas_orig, char** ady_bis)
{
	vector<int> grados(cant_vert, 0);
	
	// calculo el grado de cada nodo
	for (int i = 0; i < aristas_orig.size(); i++)
	{
		grados[aristas_orig[i].first]++;
		grados[aristas_orig[i].second]++;
	}
	
	bool hay_cambios = true;
	
	while(hay_cambios)
	{
		hay_cambios = false;
		for (int i = 0; i < cant_vert; i++)
		{
			if(grados[i] == 1)	// es una hoja
			{
				// busco al padre y podo
				for(int j = 0; j < cant_vert; j++)
				{
					if (ady_bis[i][j])
					{ 
						ady_bis[i][j] = 0;
						ady_bis[j][i] = 0;
						grados[i]--;
						grados[j]--;
						break; 
					}
				}
				hay_cambios = true;
			}
		}	
	}
	
	int cuenta = 0;
	for(int i = 0; i < cant_vert; i++)
	{
		if(grados[i] > 0) { cuenta++; }
	}
	
	if (cuenta > 0){ return 0; }
	
	return 1;
}

