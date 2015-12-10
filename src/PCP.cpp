#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "procesar.h"
#include "separar.h"

#define TOL 1E-05

using namespace std;

int main(int argc, char **argv) {	// se le pasa el archivo y la cantidad de conjuntos en la particion - PROBABLEMENTE MAS COSAS

	/******************************************************************/
	/**************** ACOMODO VARIABLES Y PROCESO COSAS ***************/
	/******************************************************************/
	//~ cout << "acomodo variables y proceso cosas" << endl;
	int cant_vert;								// cantidad de vertices
	int cant_aris;								// cantidad de aristas
	int cant_part = atoi(argv[2]);				// tamaño de la particion
	vector< vector<int> > particion(cant_part);	// como conformo la particion
	vector< pair<int,int> > aristas;			// aristas
	
	int cant_iter_cortes = atoi(argv[3]); // ??????
	int cant = atoi(argv[4]); // ????????

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


	/******************************************************************/
	/*********************** ARRANCO CON CPLEX ************************/
	/******************************************************************/
	//~ cout << "arranco con cplex" << endl;
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


	/******************************************************************/
	/*******************          COLUMNAS        *********************/
	/******************************************************************/
	//~ cout << "columnas" << endl;
	//LOS LIMITES cant_vert*cant_part + cant_part porque x_ij en nuestro PLEM i se acota por la cant de vert 
	//dado que representa eso, y j se acota por cant de vertices por que representa al color j y el coloreo
	//esta acotado por la cant de vertices... Luego, sumo un cant de vert para representar lo w_j.

	// Definimos las variables. No es obligatorio pasar los nombres de las variables, pero facilita el debug. La info es la siguiente:
	double *ub, *lb, *objfun; // Cota superior, cota inferior, coeficiente de la funcion objetivo.
	char **colnames; // string con el nombre de la variable.
	ub = new double[n]; 
	lb = new double[n];
	objfun = new double[n];
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
	status = CPXnewcols(env, lp, n, objfun, lb, ub, NULL, colnames);
	  
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
	//~ delete[] xctype;
	delete[] colnames;

	// CPLEX por defecto minimiza. Le cambiamos el sentido a la funcion objetivo si se quiere maximizar.
	// CPXchgobjsen(env, lp, CPX_MAX);


	/******************************************************************/
	/**********************          FILAS        *********************/
	/******************************************************************/
	//~ cout << "filas" << endl;
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
	int *matind = new int[rcnt*n]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
	double *matval = new double[rcnt*n]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.

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


	/******************************************************************/
	/*******************    ALGUNOS PARAMETROS    *********************/
	/******************************************************************/
	//~ cout << "algunos paramteros" << endl;
	// Para desactivar la salida poern CPX_OFF.
	status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);

	if (status) {
		cerr << "Problema seteando SCRIND" << endl;
		exit(1);
	}
	
	// Setea el tiempo limite de ejecucion.
	status = CPXsetdblparam(env, CPX_PARAM_TILIM, 900);

	if (status) {
		cerr << "Problema seteando el tiempo limite" << endl;
		exit(1);
	}


	//~ // Escribimos el problema a un archivo .lp.
	//~ status = CPXwriteprob(env, lp, "pcp.lp", NULL);	// usemos esto para ver que hacemos las cosas bien
//~ 
	//~ if (status) {
		//~ cerr << "Problema escribiendo modelo" << endl;
		//~ exit(1);
	//~ }

	
	/******************************************************************/
	/*******************          MATRICES        *********************/
	/******************************************************************/
	//~ cout << "matrices" << endl;
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
		ady[aristas_orig[h].first][aristas_orig[h].second] = 1;
		ady[aristas_orig[h].second][aristas_orig[h].first] = 1;
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
	//~ cout << "voy a ver agujeros? " << ver_holes << endl;

	/******************************************************************/
	/*******************      PLANOS DE CORTE     *********************/
	/******************************************************************/
	//~ cout << "planos de corte" << endl;
	double inittime, endtime;
	//~ std::string outputfile2(argv[5]);
	//~ ofstream solfile2(outputfile2.c_str(),std::ofstream::out | std::ofstream::app);
	//~ solfile2 << argv[1] << endl;
	clock_t start,end;
	start = clock();
	for (int h = 0; h < cant_iter_cortes; h++)	// cant_iter_cortes se levanta de argv - ver donde va
	{
		//~ solfile2 << "Iteracion " << h << ": " << endl;
		// resuelvo el lp (relajacion lineal)
		cout << "resuelvo relajacion " << h << endl;
		// Tomamos el tiempo de resolucion utilizando CPXgettime.
		status = CPXgettime(env, &inittime);

		// Optimizamos el problema como lp
		status = CPXlpopt(env, lp);	
		//~ status = CPXmipopt(env, lp);	
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
		if(solstat!=CPX_STAT_OPTIMAL){
			exit(1);
		}  
		
		double objval;
		status = CPXgetobjval(env, lp, &objval);

		if (status) {
			cerr << "Problema obteniendo valor de mejor solucion." << endl;
			exit(1);
		}
		
		cout << "Datos de la resolucion: " << "\t" << objval << "\t" << (endtime - inittime) << endl; 
		//~ if(h == 0) {
		//~ // Tomamos los valores de la solucion y los escribimos a un archivo.
		//~ std::string outputfile = "pcp.sol";
		//~ ofstream solfile(outputfile.c_str());
		//~ }

		// Tomamos los valores de todas las variables. Estan numeradas de 0 a n-1.
		double *sol = new double[n];
		status = CPXgetx(env, lp, sol, 0, n - 1);

		if (status) {
			cerr << "Problema obteniendo la solucion del LP." << endl;
			exit(1);
		}
		//~ if(h == 0){
		// Solo escribimos las variables distintas de cero (tolerancia, 1E-05).
		// Tomamos los valores de la solucion y los escribimos a un archivo.
		//~ std::string outputfile = "pcp.sol";
		//~ ofstream solfile(outputfile.c_str());
		//~ solfile2 << "Status de la solucion: " << statstr << endl;
		//~ solfile2 << "valor de la fo: " << objval << endl;
		//~ for (int j = 0; j < n; j++) {
			//~ if (sol[j] > TOL) {
				//~ solfile2 << "x_" << j << " = " << sol[j] << endl;
			//~ }
		//~ }

		//~ solfile.close();
		//~ }
		
		// si sol ya es entero, chau
		bool sol_int = true;
		for (int z = 0; z < n; z++)
		{
			if (sol[z] > TOL && sol[z] < 1-TOL)
			{
				sol_int = false;
				break;
			}
		}
		if(sol_int) { 
			//~ solfile2 << "¡SOL ENTERA!" << endl;
			break; }
		
		// si no, empiezo a cortar		
		vector< vector<int> > cortes_clique;
		vector< vector<int> > cortes_odd_hole;
		cortes_clique = separo_clique(sol, cant_part, cant_vert, ady, cant);	// cant se levanta de argv
		// miro odd holes solo si vale la pena
		if(ver_holes == 0) { cout << "veo agujeros" << endl;
			cortes_odd_hole = separo_agujero(sol, cant_part, cant_vert, ady_bis, cant);
		}
		
		//~ for(int z = 0; z < cortes_clique.size(); z++)
		//~ {
			//~ cout << "clique " << z << ": ";
			//~ for (int w = 0; w < cortes_clique[z].size(); w++)
			//~ {
				//~ cout << cortes_clique[z][w] << " ";
			//~ }
			//~ cout << endl;
		//~ }
		
		// si no pude encontrar cortes, me voy y hago el b&b para el mip (cambiar tipo y toda la bola)
		if(cortes_clique.empty() && cortes_odd_hole.empty()){ break; }
		
		// itero los cortes que se encontraron y armo las nuevas restricciones
		rcnt = cortes_clique.size()+cortes_odd_hole.size();
		double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
		int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
		int *matind = new int[rcnt*n]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
		double *matval = new double[rcnt*n]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.
		nzcnt = 0;
		char *sense = new char[rcnt];
		for (int z = 0; z < rcnt; z++) {
			sense[z] = 'L';
		}

		// cliques:
		// (sum xpj0 para todo p en la clique) - wj0 <= 0
		for (i = 0; i < cortes_clique.size(); i++) {
			matbeg[i] = nzcnt;
			rhs[i] = 0;
			int j = cortes_clique[i].back();
			// xpj0
			for (int k = 0; k < cortes_clique[i].size()-1; k++)
			{
				matind[nzcnt] = cortes_clique[i][k] + j*cant_vert;
				matval[nzcnt] = 1;
				nzcnt++;
			}
			// wj0
			matind[nzcnt] = cant_vert*cant_part + j;
			matval[nzcnt] = -1;
			nzcnt++;
		}
		
		// agujeros impares
		// (sum xpj0 para todo p en el agujero impar) - k*wj0 <= 0 (|agujero| = 2k+1)
		for (int c = 0; c < cortes_odd_hole.size(); c++) {
			matbeg[i] = nzcnt;
			rhs[i] = 0;
			int j = cortes_odd_hole[c].back();
			// xpj0
			for (int k = 0; k < cortes_odd_hole[c].size()-1; k++) 
			{
				matind[nzcnt] = cortes_odd_hole[c][k] + j*cant_vert;
				matval[nzcnt] = 1;
				nzcnt++;
			}
			// wj0
			matind[nzcnt] = cant_vert*cant_part + j;
			matval[nzcnt] = (int)(cortes_odd_hole[c].size()-1)/(-2);
			nzcnt++;
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
		delete[] sol;
	}
	end = clock();
	//~ solfile2 << "ESTO ES LO QUE TARDE: " << difftime(end,start) << endl << endl;
	//~ solfile2.close();

	for (int h = 0; h < cant_vert; h++)
	{
		delete[] ady[h];
		delete[] ady_bis[h];
	}
	delete[] ady;
	delete[] ady_bis;


	/******************************************************************/
	/*******************      CAMBIO LP A MIP      ********************/
	/******************************************************************/

	// cambio a mip
	//~ status = CPXchgprobtype(env,lp,CPXPROB_MILP);
	//~ if (status) {
		//~ cerr << "Problema cambiando LP a MIP" << endl;
		//~ exit(1);
	//~ }
	
	// cambio tipo de variables a binarias
	char *xctype = new char[n];
	//~ int *indices = new int[n];
	for (int h = 0; h < n; h++){ 
		xctype[h] = 'B';
		//~ indices
	}
	//~ status = CPXcopyctype(env,lp,xctype);
	status = CPXcopyctype(env,lp,xctype);
	if (status) {
		cerr << "Problema cambiando variables a binarias" << endl;
		exit(1);
	}
	//~ status = CPXcheckcopyctype(env,lp,xctype);
	//~ if (status) {
		//~ cerr << "Problemaaaaaaaaaaa cambiando variables a binarias" << endl;
		//~ exit(1);
	//~ }
	
	delete[] xctype;
	

	/******************************************************************/
	/****************     PARAMETROS PARA B&B     *********************/
	/******************************************************************/
	
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
	
	// Recorrido del arbol
	status = CPXsetintparam(env, CPX_PARAM_NODESEL, 0);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_NODESEL" << endl;
		exit(1);
	}
	
	// Selección de variable de branching
	status = CPXsetintparam(env, CPX_PARAM_VARSEL, 1);
	if (status) {
		cerr << "Problema configurando CPX_PARAM_VARSEL" << endl;
		exit(1);
	}
	
	/******************************************************************/
	/*******************          HAGO B&B         ********************/
	/******************************************************************/
	// Escribimos el problema a un archivo .lp.
	status = CPXwriteprob(env, lp, "pcp.lp", NULL);	// usemos esto para ver que hacemos las cosas bien

	if (status) {
		cerr << "Problema escribiendo modelo" << endl;
		exit(1);
	}    
	
	// Tomamos el tiempo de resolucion utilizando CPXgettime.
	//~ double inittime, endtime;
	status = CPXgettime(env, &inittime);

	// Optimizamos el problema.
	//~ status = CPXlpopt(env, lp);	
	status = CPXmipopt(env, lp);	
	status = CPXgettime(env, &endtime);

	if (status) {
		cerr << "Problema optimizando CPLEX" << endl;
		exit(1);
	}
	
	std::string outputfile(argv[5]);
	ofstream solfile(outputfile.c_str(),std::ofstream::out | std::ofstream::app);
	solfile << argv[1] << " " << cant_part << endl;
	if(cant_iter_cortes == 0)
	{
		solfile << "Branch and Bound" << endl;
	}else{
		solfile << "Cut and Branch" << endl;
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
		double objval;
		status = CPXgetobjval(env, lp, &objval);
		int nodecnt = CPXgetnodecnt(env,lp);
		solfile << "Valor objetivo: " << objval << endl;
		solfile << "Cantidad de nodos: " << nodecnt << endl;
		solfile << "Status de la solucion: " << statstr << endl;
		exit(1);
	}  
    
	double objval;
	status = CPXgetobjval(env, lp, &objval);

	if (status) {
		cerr << "Problema obteniendo valor de mejor solucion." << endl;
		exit(1);
	}
	
    int nodecnt = CPXgetnodecnt(env,lp);
    
	cout << "Datos de la resolucion: " << "\t" << objval << "\t" << nodecnt << "\t" << (endtime - inittime) << endl; 

	// Tomamos los valores de la solucion y los escribimos a un archivo.
	//~ std::string outputfile = "pcpmip.sol";
	//~ std::string outputfile(argv[5]);
	//~ ofstream solfile(outputfile.c_str(),std::ofstream::out | std::ofstream::app);
	//~ solfile << argv[1] << endl;
	//~ if(cant_iter_cortes == 0)
	//~ {
		//~ solfile << "Branch and Bound" << endl;
	//~ }else{
		//~ solfile << "Cut and Branch" << endl;
	//~ }
	solfile << "Valor objetivo: " << objval << endl;
	solfile << "Cantidad de nodos: " << nodecnt << endl;
	solfile << "Tiempo: " << (endtime-inittime) << endl;

	// Tomamos los valores de todas las variables. Estan numeradas de 0 a n-1.
	double *sol = new double[n];
	status = CPXgetx(env, lp, sol, 0, n - 1);

	if (status) {
		cerr << "Problema obteniendo la solucion del LP." << endl;
		exit(1);
	}

	// Solo escribimos las variables distintas de cero (tolerancia, 1E-05).
	solfile << "Status de la solucion: " << statstr << endl;
	//~ for (int i = 0; i < n; i++) {
		//~ if (sol[i] > TOL) {
			//~ solfile << "x_" << i << " = " << sol[i] << endl;
		//~ }
	//~ }

  
	delete [] sol;
	solfile.close();

	return 0;
}
