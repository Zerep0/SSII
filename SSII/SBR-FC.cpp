#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

typedef struct{
    string id;
    string regla;
    double FC;
}Regla;

typedef struct{
    string id;
    double FC;
}Hecho;


void leerBC(string BC, vector<Regla> &reglas)
{
    ifstream archivoBC(BC);

    int numeroReglas;
    archivoBC >> numeroReglas;
    
    string linea;
    getline(archivoBC,linea);
    while(getline(archivoBC,linea))
    {
        istringstream lineaRegla(linea);
        Regla regla;

        string certeza;
        getline(lineaRegla, regla.id, ':');
        getline(lineaRegla >> ws, regla.regla, ',');
        lineaRegla.ignore(4);
        getline(lineaRegla, certeza, '\n');
        regla.FC = stod(certeza);
        reglas.push_back(regla);
    }
}

void leerBH(string BH, vector<Hecho> &hechos, string &objetivo)
{
    ifstream archivoBH(BH);

    int numeroHechos;
    archivoBH >> numeroHechos;

    string linea;
    getline(archivoBH,linea);

    while(numeroHechos)
    {
        getline(archivoBH,linea);
        istringstream lineaHecho(linea);
        Hecho hecho;
        string certeza;
        getline(lineaHecho,hecho.id,',');
        lineaHecho.ignore(4);
        getline(lineaHecho,certeza,'\n');
        hecho.FC = stod(certeza);
        hechos.push_back(hecho);
        numeroHechos--;
    }
    getline(archivoBH,linea);
    archivoBH >> objetivo;

}



int main(int argc, char *argv[])
{
    vector<Regla> baseConocimientos;
    vector<Hecho> baseHechos;
    string objetivo;

    leerBC(argv[1],baseConocimientos);
    for(Regla r : baseConocimientos)
    {
        cout << r.id << ": " << r.regla << ", FC=" << r.FC << endl;
    }

    cout << endl << endl;

    leerBH(argv[2],baseHechos,objetivo);
    for(Hecho h : baseHechos)
    {
        cout << h.id << ", FC=" << h.FC << endl;
    }
    cout << "Objetivo\n" << objetivo << endl;

}