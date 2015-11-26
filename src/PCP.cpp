#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

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
	// cout << "inicializo srand" << endl;
	srand(time(NULL));
	// cout << "armo arreglo de nodos" << endl;
	vector<int> nodos;
	for(int i = 0; i < cant_vert; i++) {
		nodos.push_back(i);
	}

	int pertenencia[cant_vert];
	// cout << "primera ronda" << endl;
	int n;
	vector<int>::iterator it;
	// primera tanda para que no queden conj vacios
	// cout << "cant_part = " << cant_part << endl;
	// cout << "cant_vert = " << cant_vert << endl;
	for (int i = 0; i < cant_part; i++)
	{
		n = rand() % nodos.size();
		// cout << "n = " << n << endl;
		it = nodos.begin() + n;
		// cout << "en el medio" << endl;
		// cout << "a ver el tamanio: " << particion.size() << endl;
		particion[i].push_back(*it);
		// cout << "lo que agregue = " << *it << endl;
		pertenencia[*it] = i;
		it = nodos.erase(it);
		// cout << "liquide una vuelta" << endl;
	}
	// cout << "segunda ronda" << endl;
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
	// cout << "arreglo las aristas" << endl;
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

int main(int argc, char **argv) {	// se le pasa el archivo y la cantidad de conjuntos en la particion

// Datos de la instancia de dieta
//~ int n = 3;		// cant variables
//~ double costo[] = {1.8, 2.3,1.5};	// coefs fo
//~ // filas de la matriz
//~ double calorias[] = {170,50,300};
//~ double calcio[] = {3,400,40};
//~ // bi
//~ double minCalorias = 2000;
//~ double maxCalorias = 2300;
//~ double minCalcio = 1200;
//~ double maxPan = 3;
//~ double minLeche = 2;

//--------------Supongamos que a esta altura tengo las cosas en las siguientes variables--------
int cant_vert;
int cant_aris;
// cout << "agarro el 2do param" << endl;
int cant_part = atoi(argv[2]);
// cout << cant_part << endl;
vector< vector<int> > particion(cant_part);
vector< pair<int,int> > aristas;
// cout << "agarro el 1er param" << endl;
// cout << argv[1] << endl;
procesar_archivo(cant_vert,cant_aris,aristas, argv[1]);
// cout << "procese el archivo" << endl;
armar_particion(cant_vert,cant_aris,cant_part,particion,aristas);
cout << "arme particion" << endl;
for(int i = 0; i < particion.size(); i++)
{
	cout << "conjunto " << i << ": ";
	for (int j = 0; j < particion[i].size(); j++)
	{
		cout << particion[i][j] << " ";
	}
	cout << endl;
}
int n = cant_vert*cant_part + cant_part;

//cant_vert   : cantidad de vertices del grafo V
//cant_aris   : cantidad de aristas del grafo V -- solo las que unen distintas particiones
//cant_part   : cantidad de particiones del grafo V
//particion: arreglo de part que tienen adentro una lista de nodos que la componen.
//aristas     : lista de pares que representan las aristas

//algo que me diga que nodos estan en cada partición



//----------------------------------------------------------------------------------------------

// Genero el problema de cplex.
	int status;
	CPXENVptr env; // Puntero al entorno.
	CPXLPptr lp; // Puntero al LP	// VER QUE ESTO SEA LO MISMO

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


    cout << "ya se creo el lp" << endl;
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
	colnames = new char*[n];  //no estoy seguro de que le podamos poner nombre a esto

	for (int i = 0; i < n; i++) {
		ub[i] = 1;  //supongo que esta es la cota, es binaria
		lb[i] = 0;  //supongo que esta es la cota, es binaria
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
	delete[] xctype;
	delete[] colnames;

	cout << "boundee el lp" << endl;

	// CPLEX por defecto minimiza. Le cambiamos el sentido a la funcion objetivo si se quiere maximizar.
	// CPXchgobjsen(env, lp, CPX_MAX);

	// ------------------------------Hasta aca ya no es dieta, gordos(?)---------------------------------------


	// Generamos de a una las restricciones.
	// Estos valores indican:
	// ccnt = numero nuevo de columnas en las restricciones.
	// rcnt = cuantas restricciones se estan agregando.
	// nzcnt = # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.

	int ccnt = 0/*, rcnt = 3*/, nzcnt = 0; 
	// rcnt = 2*cant_vert*cant_part+(cant_part-1)+cant_aris*cant_part;
	int r1 = cant_vert*cant_part;
	int r2 = cant_aris*cant_part;
	int r3 = cant_part;
	int r4 = cant_part-1;
	int rcnt = r1+r2+r3+r4;

	//~ char sense[] = {'G','L','G'}; // Sentido de la desigualdad. 'G' es mayor o igual y 'E' para igualdad.
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

	// Podria ser que algun coeficiente sea cero. Pero a los sumo vamos a tener 3*n coeficientes. CPLEX va a leer hasta la cantidad
	// nzcnt que le pasemos.

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
			matind[nzcnt] = a + j*cant_part;
			matval[nzcnt] = 1;
			// xkj
			matind[nzcnt+1] = b + j*cant_part;
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
	
	cout << "hice restricciones" << endl;
	//~ //Restriccion de minimas calorias
	//~ matbeg[0] = nzcnt;
	//~ rhs[0] = minCalorias;
	//~ for (int i = 0; i < n; i++) {
		//~ matind[nzcnt] = i;
		//~ matval[nzcnt] = calorias[i];
		//~ nzcnt++;
	//~ }
//~ 
	//~ //Restriccion de maximas calorias
	//~ matbeg[1] = nzcnt;
	//~ rhs[1] = maxCalorias;
	//~ for (int i = 0; i < n; i++) {
		//~ matind[nzcnt] = i;
		//~ matval[nzcnt] = calorias[i];
		//~ nzcnt++;
	//~ }
//~ 
	//~ //Restriccion de minimo calcio
	//~ matbeg[2] = nzcnt;
	//~ rhs[2] = minCalcio;
	//~ for (int i = 0; i < n; i++) {
		//~ matind[nzcnt] = i;
		//~ matval[nzcnt] = calcio[i];
		//~ nzcnt++;
	//~ }

	// Esta rutina agrega la restriccion al lp.
	status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, NULL);
	cout << "agregue restricciones" << endl;      
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
    
	// Tomamos el tiempo de resolucion utilizando CPXgettime.
	double inittime, endtime;
	status = CPXgettime(env, &inittime);

	// Optimizamos el problema.
	//~ status = CPXlpopt(env, lp);	
	status = CPXmipopt(env, lp);	
	cout << "hice mipopt" << endl;
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
