#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "separar.h"

#define TOL 1E-05

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

int main(int argc, char **argv) {	// se le pasa el archivo y la cantidad de conjuntos en la particion - PROBABLEMENTE MAS COSAS

int cant_vert;								// cantidad de vertices
int cant_aris;								// cantidad de aristas
int cant_part = atoi(argv[2]);				// tamaño de la particion
vector< vector<int> > particion(cant_part);	// como conformo la particion
vector< pair<int,int> > aristas;			// aristas

// proceso el archivo
procesar_archivo(cant_vert,cant_aris,aristas, argv[1]);

// mantengo una copia de las aristas porque desp me deshago de las que quedan adentro del mismo conjunto
vector< pair<int,int> > aristas_orig(aristas);

// particiono
armar_particion(cant_vert,cant_aris,cant_part,particion,aristas);

//~ for(int i = 0; i < particion.size(); i++)
//~ {
	//~ cout << "conjunto " << i << ": ";
	//~ for (int j = 0; j < particion[i].size(); j++)
	//~ {
		//~ cout << particion[i][j] << " ";
	//~ }
	//~ cout << endl;
//~ }

	int n = cant_vert*cant_part + cant_part;

	// Genero el problema de cplex.
	int status;
	CPXENVptr env; // Puntero al entorno.
	CPXLPptr lp; // Puntero al LP

	// Creo el entorno.
	env = CPXopenCPLEX(&status);

    
	if (env == NULL) {
		cerr << "Error creando el entorno" << endl;
		exit(1);
	}
    
	// Creo el LP.
	lp = CPXcreateprob(env, &status, "instancia coloreo particionado");


	if (lp == NULL) {
		cerr << "Error creando el LP" << endl;
		exit(1);
	}

	//LOS LIMITES cant_vert*cant_part + cant_part porque x_ij en nuestro PLEM i se acota por la cant de vert 
	//dado que representa eso, y j se acota por cant de vertices por que representa al color j y el coloreo
	//esta acotado por la cant de vertices... Luego, sumo un cant de vert para representar lo w_j.

	// Definimos las variables. No es obligatorio pasar los nombres de las variables, pero facilita el debug. La info es la siguiente:
	double *ub, *lb, *objfun; // Cota superior, cota inferior, coeficiente de la funcion objetivo.
	char *xctype, **colnames; // tipo de la variable (por ahora son siempre continuas), string con el nombre de la variable.
	ub = new double[n]; 
	lb = new double[n];
	objfun = new double[n];
	xctype = new char[n];
	colnames = new char*[n];

	for (int i = 0; i < n; i++) {
		ub[i] = 1;
		lb[i] = 0;
		colnames[i] = new char[10];
		
		//las primeras son los x_ij que no estan en la fo, por eso el 0, las siguientes son los w_j que si estan
		if (i<cant_vert*cant_part){
			objfun[i] = 0;  
			sprintf(colnames[i],"x_%d_%d",i%cant_vert,i/cant_vert); // x00,x10,...,xn0,x01..xn1... etc
		}else{
			objfun[i] = 1;
			sprintf(colnames[i],"w_%d",i-cant_vert*cant_part);	// w0,w1... etc
		}

		// van a ser todas binarias
		xctype[i] = 'B'; // 'C' es continua, 'B' binaria, 'I' Entera. Para LP (no enteros), este parametro tiene que pasarse como NULL. No lo vamos a usar por ahora.
		
	}
	
	// ninguna particion puede estar coloreada con un color de etiqueta mayor a su indice
	for (int k = 0; k < particion.size(); k++) {
		for (int j = k+1; j < cant_part; j++) {
			for (int i = 0; i < particion[k].size(); i++) {
				ub[particion[k][i] + j*cant_vert] = 0;
			}
		}
	}

	// Agrego las columnas.
	status = CPXnewcols(env, lp, n, objfun, lb, ub, xctype, colnames);
	  
	if (status) {
		cerr << "Problema agregando las variables CPXnewcols" << endl;
		exit(1);
	}
  
	// Libero las estructuras.
	for (int i = 0; i < cant_vert*cant_part + cant_part; i++) {
		delete[] colnames[i];
	}
  
	delete[] ub;
	delete[] lb;
	delete[] objfun;
	delete[] xctype;
	delete[] colnames;

	// CPLEX por defecto minimiza. Le cambiamos el sentido a la funcion objetivo si se quiere maximizar.
	// CPXchgobjsen(env, lp, CPX_MAX);

	// Generamos de a una las restricciones.
	// Estos valores indican:
	// ccnt = numero nuevo de columnas en las restricciones.
	// rcnt = cuantas restricciones se estan agregando.
	// nzcnt = # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.

	int ccnt = 0, nzcnt = 0; 
	int r1 = cant_vert*cant_part;
	int r2 = cant_aris*cant_part;
	int r3 = cant_part;
	int r4 = cant_part-1;
	int rcnt = r1+r2+r3+r4;

	char *sense = new char[rcnt];
	for (int i = 0; i < rcnt; i++) {
		if (i < r1+r2) {
			sense[i] = 'L';
		}else if (i < r1+r2+r3) {
			sense[i] = 'E';
		}else{
			sense[i] = 'G';
		}
	}

	double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
	int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
	int *matind = new int[(r1+r2+r3+r4)*n]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
	double *matval = new double[(r1+r2+r3+r4)*n]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.

	// wj es 1 sii algun vertice usa el color j
	// xij - wj <= 0
	for (int i = 0; i < r1; i++) {
		matbeg[i] = nzcnt;
		rhs[i] = 0;
		// xij
		matind[nzcnt] = i;
		matval[nzcnt] = 1;
		// wj
		matind[nzcnt+1] = cant_vert*cant_part + i/cant_vert;
		matval[nzcnt+1] = -1;
		nzcnt += 2;
	}
	
	// nodos vecinos no pueden usar el mismo color (contemplamos solo aristas entre conj de la particion)
	// xij + xkj <= 1 para todo (i,k) arista, j color
	int a,b;
	int i = r1;
	for (int e = 0; e < cant_aris; e++) {
		a = aristas[e].first;
		b = aristas[e].second;
		//~ for (int i = r1; i < r1+r2; i++) {
		for (int j = 0; j < cant_part; j++) {
			matbeg[i] = nzcnt;
			rhs[i] = 1;
			// xij
			matind[nzcnt] = a + j*cant_vert;
			matval[nzcnt] = 1;
			// xkj
			matind[nzcnt+1] = b + j*cant_vert;
			matval[nzcnt+1] = 1;
			nzcnt += 2;
			i++;
		}
	}
	
	// cada conjunto de la particion tiene un solo color asignado
	// sum entre los xi en p (sum entre los j colores (xij)) para todo i nodo y p conj de la particion
	for (int p = 0; p < cant_part; p++) {
		matbeg[i] = nzcnt;
		rhs[i] = 1;
		for (int k = 0; k < particion[p].size(); k++) {
			for (int j = 0; j < cant_part; j++) {
				matind[nzcnt] = particion[p][k] + j*cant_vert;
				matval[nzcnt] = 1;
				nzcnt++;
			}
		}
		i++;
	}
	
	// no se permite usar un color hasta no haberse usado todos los anteriores
	// wj - w(j+1) >= 0 para todo j color
	for(int j = 0; j < cant_part-1; j++) {
		matbeg[i] = nzcnt;
		rhs[i] = 0;
		// wj
		matind[nzcnt] = cant_vert*cant_part + j;
		matval[nzcnt] = 1;
		// w(j+1)
		matind[nzcnt+1] = cant_vert*cant_part + j + 1;
		matval[nzcnt+1] = -1;
		nzcnt += 2;
		i++;
	}

	// Esta rutina agrega la restriccion al lp.
	status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, NULL);
	if (status) {
		cerr << "Problema agregando restricciones." << endl;
		exit(1);
	}
	
    delete[] sense;
	delete[] rhs;
	delete[] matbeg;
	delete[] matind;
	delete[] matval;

	// Seteo de algunos parametros.
	// Para desactivar la salida poern CPX_OFF.
	status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);

	if (status) {
		cerr << "Problema seteando SCRIND" << endl;
		exit(1);
	}
	
	// Para que haga Branch & Bound:
	status = CPXsetintparam(env, CPX_PARAM_MIPSEARCH, CPX_MIPSEARCH_TRADITIONAL);
	if (status) {
		cerr << "Problema seteando para que haga branch and bound" << endl;
		exit(1);
		
	}

	// Para facilitar la comparación evitamos paralelismo:
	status = CPXsetintparam(env, CPX_PARAM_THREADS, 1); 
	
	if (status) {
		cerr << "Problema evitando paralelismo" << endl;
		exit(1);
	}
    
	// Para que no se adicionen planos de corte:
	status = CPXsetintparam(env,CPX_PARAM_EACHCUTLIM, 0);
	if (status) {
		cerr << "Problema configurando que no se adicionen planos de corte (a)" << endl;
		exit(1);
	}
	status = CPXsetintparam(env, CPX_PARAM_FRACCUTS, -1);
	if (status) {
		cerr << "Problema configurando que no se adicionen planos de corte (b)" << endl;
		exit(1);
	}
	
	// Para desactivar todos los preprocesamientos
	status = CPXsetintparam(env, CPX_PARAM_PRESLVND, -1);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_PRESLVND" << endl;
		exit(1);
	}
	status = CPXsetintparam(env, CPX_PARAM_REPEATPRESOLVE, 0);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_REPEATPRESOLVE" << endl;
		exit(1);
	}
	status = CPXsetintparam(env, CPX_PARAM_RELAXPREIND, 0);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_RELAXPREIND" << endl;
		exit(1);
	}
	status = CPXsetintparam(env, CPX_PARAM_REDUCE, 0);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_REDUCE" << endl;
		exit(1);
	}
	status = CPXsetintparam(env, CPX_PARAM_LANDPCUTS, -1);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_LANDPCUTS" << endl;
		exit(1);
	}

	// Por ahora no va a ser necesario, pero mas adelante si. Setea el tiempo
	// limite de ejecucion.
	status = CPXsetdblparam(env, CPX_PARAM_TILIM, 3600);

	if (status) {
		cerr << "Problema seteando el tiempo limite" << endl;
		exit(1);
	}


	// Escribimos el problema a un archivo .lp.
	status = CPXwriteprob(env, lp, "pcp.lp", NULL);	// usemos esto para ver que hacemos las cosas bien

	if (status) {
		cerr << "Problema escribiendo modelo" << endl;
		exit(1);
	}

	
	// matriz de adyacencia - se usa al separar cliques
	//~ char ady[cant_vert][cant_vert];
	char** ady = new char*[cant_vert];
	for (int h = 0; h < cant_vert; h++)
	{
		ady[h] = new char[cant_vert];
		for(int j = 0; j < cant_vert; j++)
		{
			ady[h][j] = 0;
		}
	}
	for(int h = 0; h < aristas_orig.size(); h++)
	{
		ady[aristas_orig[h].first][aristas[h].second] = 1;
		ady[aristas_orig[h].second][aristas[h].first] = 1;
	}

	// otra matriz de adyacencia - la voy a procesar y a usar al separar agujeros
	//~ char ady_bis[cant_vert][cant_vert];
	char** ady_bis = new char*[cant_vert];

	for (int h = 0; h < cant_vert; h++)
	{
		ady_bis[h] = new char[cant_vert];
		for(int j = 0; j < cant_vert; j++)
		{
			ady_bis[h][j] = ady[h][j];
		}
	}

	// saco 'ramas' del grafo para buscar agujeros sólo donde tiene sentido
	int ver_holes = podar_grafo(cant_vert, aristas_orig, ady_bis); // si da 1, no tiene sentido ver odd holes



	//********************* ACA VAN LOS PLANOS DE CORTE ******************************/
	
	


	for (int h = 0; h < cant_vert; h++)
	{
		delete[] ady[h];
		delete[] ady_bis[h];
	}
	delete[] ady;
	delete[] ady_bis;



    
	// Tomamos el tiempo de resolucion utilizando CPXgettime.
	double inittime, endtime;
	status = CPXgettime(env, &inittime);

	// Optimizamos el problema.
	//~ status = CPXlpopt(env, lp);	
	status = CPXmipopt(env, lp);	
	status = CPXgettime(env, &endtime);

	if (status) {
		cerr << "Problema optimizando CPLEX" << endl;
		exit(1);
	}

	// Chequeamos el estado de la solucion.
	int solstat;
	char statstring[510];
	CPXCHARptr p;
	solstat = CPXgetstat(env, lp);
	p = CPXgetstatstring(env, solstat, statstring);
	string statstr(statstring);
	cout << endl << "Resultado de la optimizacion: " << statstring << endl;
	if(solstat!=CPXMIP_OPTIMAL){
		exit(1);
	}  
    
	double objval;
	status = CPXgetobjval(env, lp, &objval);

	if (status) {
		cerr << "Problema obteniendo valor de mejor solucion." << endl;
		exit(1);
	}
    
	cout << "Datos de la resolucion: " << "\t" << objval << "\t" << (endtime - inittime) << endl; 

	// Tomamos los valores de la solucion y los escribimos a un archivo.
	std::string outputfile = "pcp.sol";
	ofstream solfile(outputfile.c_str());


	// Tomamos los valores de todas las variables. Estan numeradas de 0 a n-1.
	double *sol = new double[n];
	status = CPXgetx(env, lp, sol, 0, n - 1);

	if (status) {
		cerr << "Problema obteniendo la solucion del LP." << endl;
		exit(1);
	}

	// Solo escribimos las variables distintas de cero (tolerancia, 1E-05).
	solfile << "Status de la solucion: " << statstr << endl;
	for (int i = 0; i < n; i++) {
		if (sol[i] > TOL) {
			solfile << "x_" << i << " = " << sol[i] << endl;
		}
	}

  
	delete [] sol;
	solfile.close();

	return 0;
}
