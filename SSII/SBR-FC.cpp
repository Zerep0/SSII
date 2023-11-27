#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

typedef enum{
    Y, O, NO
}Enlace;

typedef struct{
    string id;
    vector<string> antecedentes;
    string consecuente;
    Enlace enlace;
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
        regla.enlace = Enlace::NO;

        string certeza;
        getline(lineaRegla, regla.id, ':');

        string h;
        while(getline(lineaRegla >> ws, h, ' ') && h != "Entonces")
        {
            if(h != "Si" && h != "o" && h != "y")
            {
                regla.antecedentes.push_back(h);
            }else
            {
                if(h == "o")
                {
                    regla.enlace = Enlace::O;
                }else if(h == "y")
                {
                    regla.enlace = Enlace::Y;
                }
            }
        }
        getline(lineaRegla, regla.consecuente, ',');
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

vector<pair<string,string>> consecuentes(vector<Regla> &baseConocimientos,string objetivo)
{

}


void equiparar()
{

}


bool contenido(string objetivo, vector<Hecho> &baseHechos)
{
    for(Hecho hecho : baseHechos)
    {
        if(hecho.id == objetivo)
            return true;
    }
    return false;
}

bool verificar(string objetivo, vector<Hecho> &baseHechos)
{
    if(contenido(objetivo,baseHechos))
    {
        return true;
    }else{

    }






}

bool encadenamiento_hacia_atras(vector<Regla> &baseConocimientos, vector<Hecho> &baseHechos, string objetivo)
{
    if(verificar(objetivo,baseHechos))
        return true;
    else return false;
}



int main(int argc, char *argv[])
{
    vector<Regla> baseConocimientos;
    vector<Hecho> baseHechos;
    string objetivo;
    leerBC(argv[1],baseConocimientos);
    leerBH(argv[2],baseHechos,objetivo);

    for(Regla r : baseConocimientos)
    {
        cout << "Regla: " << r.id << " FC: " << r.FC << " Consecuente: " << r.consecuente << endl;
        for(string s : r.antecedentes)
        {
            cout << s << ", ";
        }
        cout << endl << "Enlace: " << r.enlace << endl;
    } 

    /*for(Regla r : baseConocimientos)
    {
        cout << r.id << ": " << r.regla << ", FC=" << r.FC << endl;
    }

    cout << endl << endl;


    for(Hecho h : baseHechos)
    {
        cout << h.id << ", FC=" << h.FC << endl;
    }
    cout << "Objetivo\n" << objetivo << endl;*/

}
