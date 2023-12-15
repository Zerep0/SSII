#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

// ------------------------------- TIPOS DE DATOS ------------------------------------

// un enumerado indica el tipo de enlace entre los antecedentes
typedef enum{
    Y, O, NO
}Enlace;

/*Estructura de la regla, guarda el nombre, la lista de antecedentes, consecuente,
tipo de enlace entre antecedentes y factor de certeza*/ 
typedef struct{
    string id;
    vector<string> antecedentes;
    string consecuente;
    Enlace enlace;
    double FC;
}Regla;

// Guarda el nombre del hecho y su factor de certeza
typedef struct{
    string id;
    double FC;
}Hecho;


// fichero que guarda el registro de reglas aplicadas y pasos a seguir durante el encaminamiento hacia atrás
ofstream bitacora;

// variable que te permite ajustar la precisión del resultado variando el número de decimales
int precision = 1000;


// -------------------------------- FUNCIONES DE IMPRESION ----------------------------------------------------

// imprime los elementos de un conjunto conflicto
void imprimirCC(vector<Regla> CC)
{
    bitacora << "Conjunto conflicto: ";
    for(int i = 0; i < int(CC.size()) - 1; i++)
    {
        bitacora << CC[i].id << ", ";
    }
    bitacora << CC[CC.size()-1].id << endl;

}

// imprime los nombres de los antecedentes de la regla
void imprimirAntecedentes(Regla r)
{
    bitacora << "Antecedente/s de " << r.id << ": ";
    for(int i = 0; i < int(r.antecedentes.size()) - 1; i++)
    {
         bitacora << r.antecedentes[i] << ", ";
    }
    bitacora << r.antecedentes[r.antecedentes.size()-1] << endl;

}

// tratamiento de la meta con respecto al factor de certeza obtenido
void analisisCasos(double resultadoFC, string Meta)
{
    if(resultadoFC <= -0.75 && resultadoFC >= -1)
    {
        cout << "Podemos afirmar casi en su total seguridad que " << Meta << " no va a darse" << endl;
        bitacora << "Podemos afirmar casi en su total seguridad que " << Meta << " no va a darse" << endl;
    }
    else if(resultadoFC <= -0.65 && resultadoFC > -0.75)
    {
        cout << "Las evidencias son bastante claras y apuntan a que " << Meta << " no va a darse" << endl;
        bitacora << "Las evidencias son bastante claras y apuntan a que " << Meta << " no va a darse" << endl;
    }
    else if(resultadoFC <= -0.5 && resultadoFC >= -0.64)
    {
        cout << "Aunque no hay una evidencia clara, hay indicios de que " << Meta << " puede no llegar a darse" << endl;
        bitacora << "Aunque no hay una evidencia clara, hay indicios de que " << Meta << " puede no llegar a darse" << endl;
    }
    else if(resultadoFC < 0 && resultadoFC >= -0.49)
    {
        cout << "No hay una evidencia clara de que " << Meta << " va a ocurrir" << endl;
        bitacora << "No hay una evidencia clara de que " << Meta << " va a ocurrir" << endl;
    }else if(resultadoFC == 0)
    {
        cout << "No tenemos ninguna evidencia sobre si " << Meta << " puede llegar a ocurrir" << endl;
    }
    else if(resultadoFC > 0 && resultadoFC <= 0.49)
    {
        cout << "Hay una ligera evidencia de que " << Meta << " pueda llegar a suceder" << endl;
        bitacora << "Hay una ligera evidencia de que " << Meta << " pueda llegar a suceder" << endl;
    }else if(resultadoFC >= 0.5 && resultadoFC < 0.64)
    {
        cout << "Aunque no hay una evidencia clara, hay indicios de que " << Meta << " pueda llegar a darse" << endl;
        bitacora << "Aunque no hay una evidencia clara, hay indicios de que " << Meta << " pueda llegar a darse" << endl;
    }else if(resultadoFC >= 0.65 && resultadoFC < 0.75)
    {
        cout << "Las evidencias son bastante claras y apuntan a que " << Meta << " va a darse" << endl;
        bitacora << "Las evidencias son bastante claras y apuntan a que " << Meta << " va a darse" << endl;
    }else{
        cout << "Podemos afirmar casi en su total seguridad que " << Meta << " va a darse" << endl;
        bitacora << "Podemos afirmar casi en su total seguridad que " << Meta << " va a darse" << endl;
    }
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

//--------------------------------- FUNCIONES PRINCIPALES SBR-FC --------------------------------------------

// dado el nombre del fichero, procesa la base de conocimientos y la guarda en un vector de reglas
void leerBC(string BC, vector<Regla> &reglas)
{
    ifstream archivoBC(BC);

    int numeroReglas;
    archivoBC >> numeroReglas;

    string linea;
    getline(archivoBC,linea); // omision de una linea vacia
    // mientras queden lineas y no sean vacias, generamos un flujo de caractes
    while(getline(archivoBC,linea))
    {
        if(linea != "")
        {
            istringstream lineaRegla(linea);
            Regla regla;
            // en un principio partimos de la base de que hay un solo antecedente hasta que nos cruzemos una disyunción/conjunción
            regla.enlace = Enlace::NO;

            
            getline(lineaRegla, regla.id, ':');

            string h;
            // por cada espacio que se encuentre hasta entonces, ve si es un antecedente, un enlace o debe omitirlo
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

// dado el nombre del fichero, procesa la base de hechos para guardar el objetivo y la lista de hechos
void leerBH(string BH, vector<Hecho> &hechos, string &objetivo)
{
    ifstream archivoBH(BH);

    int numeroHechos;
    archivoBH >> numeroHechos;

    string linea;
    getline(archivoBH,linea);
    // mientras queden hechos va leyendo la linea, la convierte en un flujo de caracteres, y selecciono aquellas parte que me interesan usando delimitadores
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

/* Dada una base de conocimientos y una meta, devuelve una lista con 
aquellas reglas cuyos cosecuentes coincidan con la meta establecida */
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

/* Verifica si una meta esta en la base de hechos, si es cierto devuelve true
y guarda el contenido de la meta*/
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

// verifica si esta vacio el conjunto conflicto
bool estaVacio(vector<Regla> CC)
{
    return CC.size() == 0;
}

// verifica si esta vacio la lista de nuevas metas
bool estaVacio(vector<string> NuevasMetas)
{
    return NuevasMetas.size() == 0;
}

// devuelve la última regla del conjunto conflicto
Regla resolver(vector<Regla> CC)
{
    return CC.back();
}

// elimina la última regla del conjunto conflicto
void eliminar(vector<Regla> &CC)
{
    bitacora << "Regla eliminada " << CC.back().id << endl;
    CC.pop_back();
}

// elimina la meta de la ultima posicion
void eliminar(vector<string> &NuevasMetas)
{
    NuevasMetas.pop_back();
}

// dada un regla, devuelve los antedecentes de esta
vector<string> extraerAntecedentes(Regla R)
{
    imprimirAntecedentes(R);
    return R.antecedentes;
}

// dado una lista de metas, selecciona la de la ultima posicion
string seleccionarMeta(vector<string> NuevasMetas)
{
    string Nmet =  NuevasMetas.back();
    bitacora << "Nueva meta seleccionada: " << Nmet << endl;
    return Nmet;
}

// procedimiento recursivo que realiza el encaminamiento hacia atras
double verificar(string Meta, vector<Hecho> &baseHechos, vector<Regla> baseConocimientos)
{
    double FCFinal = 0;
    Hecho HMeta;
    // si la meta esta contenida en la base de hechos guardala y devuelve su factor de certeza
    if(contenido(Meta,baseHechos,HMeta))
    {
        return HMeta.FC;
    }else{
        // estraemos el conjunto conflicto de la meta
        vector<Regla> CC = equiparar(baseConocimientos,Meta);
        int nCC = (int) CC.size();
        // mientras no este vacio resolvemos todas las reglas y las vamos eliminando
        while(!estaVacio(CC))
        {

            Regla R = resolver(CC);
            eliminar(CC);
            // extraemos los antecedentes de la regla extraida
            vector<string> NuevasMetas = extraerAntecedentes(R);
            double FCLocal = 0;
            bool primeraVez = true;
            // mientras queden metas se seleccionan y se van eliminando
            while(!estaVacio(NuevasMetas))
            {
                string Nmet = seleccionarMeta(NuevasMetas);
                eliminar(NuevasMetas);
                // devuelve el valor de la meta encontrado en la base de echos o devuelto de su cálculo previo
                double FCVerificado = verificar(Nmet,baseHechos,baseConocimientos); 
                // si es la primera vez no hay enlaces
                if(primeraVez)
                {
                    FCLocal = FCVerificado;
                    primeraVez = false;
                }else{
                    // si entra aqui quiere decir que hay mas de una antecente y puede estar concatenado con conjunciones o con disyunciones
                    if(R.enlace == Enlace::Y)
                    {
                        bitacora << "Caso 1 entre " << Nmet << " y el resultado acumulado: minimo entre " << FCLocal << " y " << FCVerificado  << endl;
                        FCLocal = minimo(FCLocal,FCVerificado);
                    }else if (R.enlace == Enlace::O)
                    {
                        bitacora << "Caso 1 entre " << Nmet << " y el resultado acumulado: maximo entre " << FCLocal << " o " << FCVerificado  << endl;
                        FCLocal = maximo(FCLocal,FCVerificado);
                    }
                }
            }
            // cuando has calculado aplicando el caso 1 los antecedentes, con su regla se aplica el 
            // caso 3
            bitacora << "Caso 3 con la regla " << R.id << ": " << R.FC << " * " << maximo(0,FCLocal) << " = " << round(R.FC * maximo(0,FCLocal) * precision) / precision<< endl;
            FCLocal = round(R.FC * maximo(0,FCLocal) * precision) / precision;

            // si hay varias reglas que desenvocan en el mismo consecuente entonces se iran acumulando las aportaciones de cada una de ellas y se podra aplicar
            // caso 2
            if(nCC > 1)
            {
                if(FCFinal >= 0 && FCLocal >= 0)
                {
                    bitacora << "Caso 2 añadiendo al hecho " << Meta << " el resultado de la regla " << R.id << ": " << FCFinal << " + " << FCLocal << " * (1-" << FCFinal << ") = " << round((FCFinal + FCLocal*(1-FCFinal))*precision) / precision << endl;
                    FCFinal = round((FCFinal + FCLocal*(1-FCFinal))*precision) / precision;
                }
                else if(FCFinal <= 0 && FCLocal <= 0)
                {
                    bitacora << "Caso 2 añadiendo al hecho " << Meta << " el resultado de la regla " << R.id << ": " << FCFinal << " + " << FCLocal << " * (1+" << FCFinal << ") = " << round((FCFinal + FCLocal*(1-FCFinal))*precision) / precision << endl;
                    FCFinal = round((FCFinal + FCLocal*(1+FCFinal))*precision) / precision;
                }
                else
                {
                    bitacora << "Caso 2 añadiendo al hecho " << Meta << " el resultado de la regla " << R.id << ": " << FCFinal << " + " << FCLocal << "/(1- " << minimo(abs(FCFinal),abs(FCLocal)) << ")" << endl;
                    FCFinal = round(((FCFinal + FCLocal)/(1 - minimo(abs(FCFinal),abs(FCLocal))))*precision) / precision;
                }
            }else{
                return FCLocal;
            }
        }
        // una vez calculada la meta, se añade a la base de hechos para no tener que volver a calcularla si se la volviera a encontrar
        HMeta.FC = FCFinal;
        baseHechos.push_back(HMeta);


    }

    return FCFinal;




}

// llama al metodo recursivo verificar que devolvera el resultado y lo analiza para obtener una conclusión
void encadenamiento_hacia_atras(vector<Regla> &baseConocimientos, vector<Hecho> &baseHechos, string Meta)
{
    double resultadoFC = verificar(Meta,baseHechos,baseConocimientos);
    analisisCasos(resultadoFC, Meta);
}

int main(int argc, char *argv[])
{
    string nombreFichero = (string)argv[1] + (string)argv[2];
    // inicialización del log
    bitacora.open(nombreFichero);
    bitacora << nombreFichero << endl;
    vector<Regla> baseConocimientos;
    vector<Hecho> baseHechos;

    string Meta;
    leerBC(argv[1],baseConocimientos);
    leerBH(argv[2],baseHechos,Meta);
    encadenamiento_hacia_atras(baseConocimientos,baseHechos,Meta);

}

