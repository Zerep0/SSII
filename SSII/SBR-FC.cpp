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

double FCFinal = 0;

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
    Regla r = CC[((int)CC.size()) - 1];
    return r;
}

void eliminar(vector<Regla> &CC)
{
    CC.pop_back();
}

void eliminar(vector<string> &NuevasMetas)
{
    NuevasMetas.pop_back();
}

vector<string> extraerAntecedentes(Regla R)
{
    return R.antecedentes;
}

string seleccionarMeta(vector<string> NuevasMetas)
{
    string Nmet = NuevasMetas[((int)NuevasMetas.size()) - 1];
    return Nmet;
}

int verificar(string Meta, vector<Hecho> &baseHechos, vector<Regla> baseConocimientos)
{
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
                int FCVerificado = verificar(Nmet,baseHechos,baseConocimientos); // devuevle el FC del hecho o el calculado
                if(primeraVez)
                {
                    FCLocal = FCVerificado;
                    primeraVez = false;
                }else{
                    if(R.enlace == Enlace::Y)
                    {
                        FCLocal = minimo(FCLocal,FCVerificado);
                    }else if (R.enlace == Enlace::O)
                    {
                        FCLocal = maximo(FCLocal,FCVerificado);
                    }   
                }
            }
            // caso 3
            FCLocal = FCLocal * maximo(0,R.FC);

            // caso 2
            if(nCC > 1)
            {
                if(FCFinal >= 0 && FCLocal >= 0)
                {
                    FCFinal = FCFinal + FCLocal*(1-FCFinal);
                }
                else if(FCFinal <= 0 && FCLocal <= 0)
                {
                    FCFinal = FCFinal + FCLocal*(1+FCFinal);
                }
                else
                {
                    FCFinal = (FCFinal + FCLocal)/(1 - minimo(abs(FCFinal),abs(FCLocal)));
                }
            }else{
                FCFinal = FCLocal;
            }
        }

        baseHechos.push_back(HMeta);
        
        
    }






}

void encadenamiento_hacia_atras(vector<Regla> &baseConocimientos, vector<Hecho> &baseHechos, string Meta)
{
    verificar(Meta,baseHechos,baseConocimientos);
}



int main(int argc, char *argv[])
{
    vector<Regla> baseConocimientos;
    vector<Hecho> baseHechos;
    string Meta;
    leerBC(argv[1],baseConocimientos);
    leerBH(argv[2],baseHechos,Meta);
    encadenamiento_hacia_atras(baseConocimientos,baseHechos,Meta);

    /*for(Regla r : baseConocimientos)
    {
        cout << "Regla: " << r.id << " FC: " << r.FC << " Consecuente: " << r.consecuente << endl;
        for(string s : r.antecedentes)
        {
            cout << s << ", ";
        }
        cout << endl << "Enlace: " << r.enlace << endl;
    } 

    for(Regla r : baseConocimientos)
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
