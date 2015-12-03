#ifndef SEPARAR_H
#define SEPARAR_H
#define TOL 1E-05

vector< vector<int> > separo_clique(const double* sol, int cant_part, int cant_vert, const char** ady, int cant);
vector< vector<int> > separo_agujero(const double* sol, int cant_part, int cant_vert,const char** ady_bis, int cant);

//separo_agujero

#endif
