#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

// ------------------------------- TIPOS DE DATOS ------------------------------------
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



ofstream bitacora;


int precision = 1000;


// -------------------------------- FUNCIONES DE IMPRESION ----------------------------------------------------

void imprimirCC(vector<Regla> CC)
{
    bitacora << "Conjunto conflicto: ";
    for(int i = 0; i < int(CC.size()) - 1; i++)
    {
        bitacora << CC[i].id << ", ";
    }
    bitacora << CC[CC.size()-1].id << endl;

}

void imprimirAntecedentes(Regla r)
{
    bitacora << "Antecedente/s de " << r.id << ": ";
    for(int i = 0; i < int(r.antecedentes.size()) - 1; i++)
    {
         bitacora << r.antecedentes[i] << ", ";
    }
    bitacora << r.antecedentes[r.antecedentes.size()-1] << endl;

}




// -------------------------------- FUNCIONES AUXILIARES SBR-FC --------------------------------------------
double maximo(double a, double b)
{
    if(a >= b)
        return a;
    else return b;
}

double minimo(double a, double b)
{
    if(a <= b)
        return a;
    else return b;
}

void leerBC(string BC, vector<Regla> &reglas)
{
    ifstream archivoBC(BC);

    int numeroReglas;
    archivoBC >> numeroReglas;

    string linea;
    getline(archivoBC,linea);
    while(getline(archivoBC,linea))
    {
        if(linea != "")
        {
            istringstream lineaRegla(linea);
            Regla regla;
            regla.enlace = Enlace::NO;

            
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
            lineaRegla >> regla.FC;
            reglas.push_back(regla);
        }

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

vector<Regla> equiparar(vector<Regla> baseConocimientos, string Meta)
{
    vector<Regla> CC;
    for(Regla r : baseConocimientos)
    {
        if(r.consecuente == Meta)
        {
            CC.push_back(r);
        }
    }
    imprimirCC(CC);
    return CC;
}

bool contenido(string Meta, vector<Hecho> baseHechos, Hecho &HMeta)
{
    for(Hecho hecho : baseHechos)
    {
        if(hecho.id == Meta)
        {
            HMeta = hecho;
            return true;
        }

    }
    return false;
}

bool estaVacio(vector<Regla> CC)
{
    return CC.size() == 0;
}

bool estaVacio(vector<string> NuevasMetas)
{
    return NuevasMetas.size() == 0;
}

// resuelve
Regla resolver(vector<Regla> CC)
{
    Regla r = CC[CC.size() - 1];
    return r;
}

void eliminar(vector<Regla> &CC)
{
    bitacora << "Regla eliminada " << CC[int(CC.size()) - 1].id << endl;
    CC.pop_back();
}

void eliminar(vector<string> &NuevasMetas)
{
    NuevasMetas.pop_back();
}

vector<string> extraerAntecedentes(Regla R)
{
    imprimirAntecedentes(R);
    return R.antecedentes;
}

string seleccionarMeta(vector<string> NuevasMetas)
{
    string Nmet = NuevasMetas[NuevasMetas.size() - 1];
    bitacora << "Nueva meta seleccionada: " << Nmet << endl;
    return Nmet;
}

double verificar(string Meta, vector<Hecho> &baseHechos, vector<Regla> baseConocimientos)
{
    double FCFinal = 0;
    Hecho HMeta;
    if(contenido(Meta,baseHechos,HMeta))
    {
        return HMeta.FC;
    }else{
        vector<Regla> CC = equiparar(baseConocimientos,Meta);
        int nCC = (int) CC.size();
        while(!estaVacio(CC))
        {

            Regla R = resolver(CC);
            eliminar(CC);
            vector<string> NuevasMetas = extraerAntecedentes(R);
            double FCLocal = 0;
            bool primeraVez = true;
            while(!estaVacio(NuevasMetas))
            {
                string Nmet = seleccionarMeta(NuevasMetas);
                eliminar(NuevasMetas);
                double FCVerificado = verificar(Nmet,baseHechos,baseConocimientos); // devuevle el FC del hecho o el calculado
                if(primeraVez)
                {
                    FCLocal = FCVerificado;
                    primeraVez = false;
                }else{
                    if(R.enlace == Enlace::Y)
                    {
                        bitacora << "Caso 1: minimo entre " << FCLocal << " y " << FCVerificado  << endl;
                        FCLocal = minimo(FCLocal,FCVerificado);
                    }else if (R.enlace == Enlace::O)
                    {
                        bitacora << "Caso 1: maximo entre " << FCLocal << " o " << FCVerificado  << endl;
                        FCLocal = maximo(FCLocal,FCVerificado);
                    }
                }
            }
            // caso 3
            bitacora << "Caso 3 con la regla " << R.id << ": " << R.FC << " * " << maximo(0,FCLocal) << " = " << round(R.FC * maximo(0,FCLocal) * precision) / precision<< endl;
            FCLocal = round(R.FC * maximo(0,FCLocal) * precision) / precision;

            // caso 2
            if(nCC > 1)
            {
                if(FCFinal >= 0 && FCLocal >= 0)
                {
                    bitacora << "Caso 2: " << FCFinal << " + " << FCLocal << " * (1-" << FCFinal << ") = " << round((FCFinal + FCLocal*(1-FCFinal))*precision) / precision << endl;
                    FCFinal = round((FCFinal + FCLocal*(1-FCFinal))*precision) / precision;
                }
                else if(FCFinal <= 0 && FCLocal <= 0)
                {
                    bitacora << "Caso 2: " << FCFinal << " + " << FCLocal << " * (1+" << FCFinal << ") = " << round((FCFinal + FCLocal*(1-FCFinal))*precision) / precision << endl;
                    FCFinal = round((FCFinal + FCLocal*(1+FCFinal))*precision) / precision;
                }
                else
                {
                    bitacora << "Caso 2: " << FCFinal << " + " << FCLocal << "/(1- " << minimo(abs(FCFinal),abs(FCLocal)) << ")" << endl;
                    FCFinal = round(((FCFinal + FCLocal)/(1 - minimo(abs(FCFinal),abs(FCLocal))))*precision) / precision;
                }
            }else{
                return FCLocal;
            }
        }

        HMeta.FC = FCFinal;
        baseHechos.push_back(HMeta);


    }

    return FCFinal;




}

void encadenamiento_hacia_atras(vector<Regla> &baseConocimientos, vector<Hecho> &baseHechos, string Meta)
{
     cout << verificar(Meta,baseHechos,baseConocimientos);
}

int main(int argc, char *argv[])
{
    bitacora.open("log.txt");
    vector<Regla> baseConocimientos;
    vector<Hecho> baseHechos;

    string Meta;
    leerBC(argv[1],baseConocimientos);
    leerBH(argv[2],baseHechos,Meta);
    encadenamiento_hacia_atras(baseConocimientos,baseHechos,Meta);

}

