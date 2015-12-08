#include <vector>
#include <cstdlib>
#include <iostream>

#define TOL 1E-05

using namespace std;

vector< vector<int> > separo_clique(const double* sol, int cant_part, int cant_vert,char** ady, int cant)
{
	vector< vector<int> > res;
	for(int j = 0; j < cant_part; j++)
	{
		// este color solo me interesa si fue usado
		if(sol[cant_vert*cant_part + j] > TOL)
		{
			vector<int> nodos_nom(cant_vert);
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
				double max = -1;
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
				nodos_val_aux[id] = -2;
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
				cout << nodos_nom[s] << " " << j << endl;
				if(s < cant_vert)
				{
					// arranco la clique con el nodo que encontre
					//~ clique.push_back(nodos_nom[s]);
					clique.push_back(s);
					control[s] = 1;
					int k;
					double suma = nodos_val[nodos_nom[s]];	// voy sumando para controlar que la clique sirve
					// miro los demas nodos
					for(int i = 0; i < cant_vert; i++)
					{
						// si no es el nodo que agarré al principio...
						if(i != s)
						{
							// veo si es compañero de todos los que estan en la clique hasta ahora
							for(k = 0; k < clique.size(); k++)
							{
								//~ if(ady[clique[k]][nodos_nom[i]] == 0) { break; }
								if(ady[nodos_nom[clique[k]]][nodos_nom[i]] == 0) { break; }
							}
							// si pude mirar toda la clique que fui armando...
							if(k == clique.size())
							{	//cout << i;
								// si este nodo suma, sumo
								if (nodos_val[nodos_nom[i]] > TOL)
								{
									suma += nodos_val[nodos_nom[i]];
								}else{
									// si ya dejo de sumar, veo que valga la pena seguir
									cout << "suma" << suma << " " << nodos_nom[i] << " " << j << endl;
									if(suma <= sol[cant_vert*cant_part + j]){ break; }
								}
								// si llegue hasta aca, agrego este nodo a la clique
								//~ clique.push_back(nodos_nom[i]);
								clique.push_back(i);
							}
							//~ cout << endl;
						}
					}
					// si esta clique corta a x*, la agrego a res
					if(suma > sol[cant_vert*cant_part + j])
					{	
						for(int h = 0; h < clique.size(); h++)
						{	//cout << h;
							control[clique[h]] = 1;
							clique[h] = nodos_nom[clique[h]];
						}
						//~ cout << endl;
						clique.push_back(j);	// aviso para qué color sirve esta clique
						res.push_back(clique);
						//~ cout << endl;
					}
					clique.clear();
				}else{ break; }	// si todos los nodos fueron usados en alguna clique, no busco más
			}
		}	
	}
	
	return res;
}


vector< vector<int> > separo_agujero(const double* sol, int cant_part, int cant_vert,char** ady_bis, int cant)
{
	vector< vector<int> > res;
	for(int j = 0; j < cant_part; j++)
	{
		// este color solo me interesa si fue usado
		if(sol[cant_vert*cant_part + j] > TOL)
		{
			vector<int> nodos_nom(cant_vert);
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
				double max = -1;
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
				nodos_val_aux[id] = -2;
			}
			
			// empiezo a buscar agujeros
			vector<int> hole;	// aca voy armando el agujero
			vector<char> control(cant_vert,0);	// aca voy controlando para asegurar que no repito
			while (res.size() < cant)	// mientras no haya armado todo lo que necesito...
			{
				// busco al primer nodo que aun no haya usado en un agujero
				int s = 0;
				while(s < cant_vert && control[s] != 0) { s++; }
				// si encontre alguno...
				//~ cout << s << endl;
				if(s < cant_vert)
				{
					// arranco el agujero con el nodo que encontre
					//~ hole.push_back(nodos_nom[s]);
					hole.push_back(s);
					control[s] = 1;
					int i,k;
					int ult = s;
					double suma = nodos_val[nodos_nom[s]];	// voy sumando para controlar que el agujero sirve
					//~ cout << "suma: " << suma << endl;
					// miro los demas nodos
					for(i = 0; i < cant_vert; i++)
					{
						// si no es el nodo que agarré al principio y es amigo del ultimo que agregué...
						if(i != s && ady_bis[nodos_nom[i]][nodos_nom[ult]])
						{	
							// veo si cierra un circuito con los nodos que agarré hasta ahora
							for(k = hole.size()-2; k >= 0; k--)
							{
								if(ady_bis[nodos_nom[hole[k]]][nodos_nom[i]] == 1) { break; }
							}
							// si no cierra circuito, lo tomo
							if(k == -1)
							{	
								// si este nodo suma, sumo
								if (nodos_val[nodos_nom[i]] > TOL)
								{
									suma += nodos_val[nodos_nom[i]];
									//~ cout << "suma: " << suma << endl;
								}else{
									// si ya dejo de sumar, veo que valga la pena seguir
									if(suma <= sol[cant_vert*cant_part + j]){ break; }
								}
								// si llegue hasta aca, agrego este nodo al agujero
								//~ hole.push_back(nodos_nom[i]);
								hole.push_back(i);
								ult = i; 
								i = 0;
							}else if(k == 0){	// ya complete el agujero
								// veo que el agujero conseguido sirve.  si no, sigo mirando
								if(hole.size() > 3 && hole.size() % 2 == 0)
								{
									if (nodos_val[nodos_nom[i]] > TOL)
									{
										suma += nodos_val[nodos_nom[i]];
									}
									//~ hole.push_back(nodos_nom[i]);
									hole.push_back(i);
									break;
								}
							}
						}
					}
					// si este agujero corta a x*, la agrego a res
					if(i < cant_vert && suma > sol[cant_vert*cant_part + j])
					{	//cout << "voy a cambiar control" << endl;
						for(int h = 0; h < hole.size(); h++)
						{
							control[hole[h]] = 1;
							hole[h] = nodos_nom[hole[h]];
						}
						//~ cout << "voy a pushbackear" << endl;
						hole.push_back(j);	// aviso para qué color sirve este agujero
						res.push_back(hole);
						//~ cout << "arme agujero" << endl;
					}
					hole.clear();
				}else{ break; }	// si todos los nodos fueron usados en algun agujero, no busco más
			}
		}	
	}
	
	return res;
}
