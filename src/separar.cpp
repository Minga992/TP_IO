#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;

vector< vector<int> > separo_clique(const double* sol, int cant_part, int cant_vert,const char** ady, int cant)
{
	vector< vector<int> > res;
	for(int j = 0; j < cant_part; j++)
	{
		// este color solo me interesa si fue usado
		if(sol[cant_vert*cant_part + j] > TOL)
		{
			vector<double> nodos_nom(cant_vert);
			vector<double> nodos_val(cant_vert);
			
			// me guardo los valores de los nodos para este color en la solucion hallada
			for(int i = 0; i < cant_vert; i++)
			{
				nodos_val[i] = sol[j*cant_vert + i];
			}
			
			vector<double> nodos_val_aux(nodos_val);
			
			// los 'ordeno' en nodos_nom de > a <
			for(int i = 0; i < cant_vert; i++)
			{
				int max = 0;
				int id = 0;
				for(int k = 0; k < cant_vert; k++)
				{
					if (nodos_val_aux[k] > max)
					{
						id = k;
						max = nodos_val_aux[k];
					}
				}
				nodos_nom[i] = id;
				nodos_val_aux[id] = -1;
			}
			
			// empiezo a buscar cliques
			vector<int> clique;	// aca voy armando la clique
			vector<char> control(cant_vert,0);	// aca voy controlando para asegurar cliques maximales
			while (res.size() < cant)	// mientras no haya armado todo lo que necesito...
			{
				// busco al primer nodo que aun no haya usado en una clique
				int s = 0;
				while(s < cant_vert && control[s] != 0) { s++; }
				// si encontre alguno...
				if(s < cant_vert)
				{
					// arranco la clique con el nodo que encontre
					clique.push_back(nodos_nom[s]);
					int k;
					int suma = nodos_val[nodos_nom[s]];	// voy sumando para controlar que la clique sirve
					// miro los demas nodos
					for(int i = 0; i < cant_nodos; i++)
					{
						// si no es el nodo que agarré al principio...
						if(i != s)
						{
							// veo si es compañero de todos los que estan en la clique hasta ahora
							for(k = 0; k < clique.size(); k++)
							{
								if(ady[clique[k]][nodos_nom[i]] == 0) { break; }
							}
							// si pude mirar toda la clique que fui armando...
							if(k == clique.size())
							{
								// si este nodo suma, sumo
								if (nodos_val[nodos_nom[i]] > TOL)
								{
									suma += nodos_val[nodos_nom[i]];
								}else{
									// si ya dejo de sumar, veo que valga la pena seguir
									if(suma <= sol[cant_vert*cant_part + j]){ break; }
								}
								// si llegue hasta aca, agrego este nodo a la clique
								clique.push_back(nodos_nom[i]);
							}
						}
					}
					// si esta clique corta a x*, la agrego a res
					if(suma > sol[cant_vert*cant_part + j])
					{
						for(int h = 0; h < clique.size(); h++)
						{
							control[clique[h]] = 1;
						}
						clique.push_back(j);	// aviso para qué color sirve esta clique
						res.push_back(clique);
					}
					clique.clear();
				}else{ break; }	// si todos los nodos fueron usados en alguna clique, no busco más
			}
		}	
	}
	
	return res;
}
