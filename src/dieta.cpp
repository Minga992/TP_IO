#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN
#include <string>
#include <vector>

#define TOL 1E-05

int main(int argc, char **argv) {

  // Datos de la instancia de dieta
  int n = 3;		// cant variables
  double costo[] = {1.8, 2.3,1.5};	// coefs fo
  // filas de la matriz
  double calorias[] = {170,50,300};
  double calcio[] = {3,400,40};
  // bi
  double minCalorias = 2000;
  double maxCalorias = 2300;
  double minCalcio = 1200;
  double maxPan = 3;
  double minLeche = 2;
    
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
  lp = CPXcreateprob(env, &status, "instancia dieta");

    
  if (lp == NULL) {
    cerr << "Error creando el LP" << endl;
    exit(1);
  }

    
  // Definimos las variables. No es obligatorio pasar los nombres de las variables, pero facilita el debug. La info es la siguiente:
  double *ub, *lb, *objfun; // Cota superior, cota inferior, coeficiente de la funcion objetivo.
  char *xctype, **colnames; // tipo de la variable (por ahora son siempre continuas), string con el nombre de la variable.
  ub = new double[n]; 
  lb = new double[n];
  objfun = new double[n];
  xctype = new char[n];
  colnames = new char*[n];
  
  for (int i = 0; i < n; i++) {
    ub[i] = CPX_INFBOUND;
    lb[i] = 0.0;
    objfun[i] = costo[i];
    // van a ser todas binarias
    xctype[i] = 'C'; // 'C' es continua, 'B' binaria, 'I' Entera. Para LP (no enteros), este parametro tiene que pasarse como NULL. No lo vamos a usar por ahora.
    colnames[i] = new char[10];
  }

  // Nombre de la variable x_m
  sprintf(colnames[0],"x_m");

  // Nombre de la variable x_l y cota inferior
  lb[1] = 2;
  sprintf(colnames[1],"x_l");

  // Nombre de la variable x_p y cota superior
  ub[2] = 3;
  sprintf(colnames[2],"x_p");
  
  // Agrego las columnas.
  status = CPXnewcols(env, lp, n, objfun, lb, ub, NULL, colnames);
  
  if (status) {
    cerr << "Problema agregando las variables CPXnewcols" << endl;
    exit(1);
  }
  
  // Libero las estructuras.
  for (int i = 0; i < n; i++) {
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

  int ccnt = 0, rcnt = 3, nzcnt = 0; 

  char sense[] = {'G','L','G'}; // Sentido de la desigualdad. 'G' es mayor o igual y 'E' para igualdad.

  double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
  int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
  int *matind = new int[3*n]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
  double *matval = new double[3*n]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.

  // Podria ser que algun coeficiente sea cero. Pero a los sumo vamos a tener 3*n coeficientes. CPLEX va a leer hasta la cantidad
  // nzcnt que le pasemos.


  //Restriccion de minimas calorias
  matbeg[0] = nzcnt;
  rhs[0] = minCalorias;
  for (int i = 0; i < n; i++) {
     matind[nzcnt] = i;
     matval[nzcnt] = calorias[i];
     nzcnt++;
  }

  //Restriccion de maximas calorias
  matbeg[1] = nzcnt;
  rhs[1] = maxCalorias;
  for (int i = 0; i < n; i++) {
     matind[nzcnt] = i;
     matval[nzcnt] = calorias[i];
     nzcnt++;
  }

  //Restriccion de minimo calcio
  matbeg[2] = nzcnt;
  rhs[2] = minCalcio;
  for (int i = 0; i < n; i++) {
     matind[nzcnt] = i;
     matval[nzcnt] = calcio[i];
     nzcnt++;
  }

  // Esta rutina agrega la restriccion al lp.
  status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, NULL);
      
  if (status) {
    cerr << "Problema agregando restricciones." << endl;
    exit(1);
  }
      
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
  status = CPXwriteprob(env, lp, "dieta2.lp", NULL);
    
  if (status) {
    cerr << "Problema escribiendo modelo" << endl;
    exit(1);
  }
    
  // Tomamos el tiempo de resolucion utilizando CPXgettime.
  double inittime, endtime;
  status = CPXgettime(env, &inittime);

  // Optimizamos el problema.
  status = CPXlpopt(env, lp);	// CPXmipopt??

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
  std::string outputfile = "dieta.sol";
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
