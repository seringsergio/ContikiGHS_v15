#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>     /* atoi */
#include <vector>


struct information_nodo
{
    int id;
    std::string parent;
    std::string weight;
    std::string Branch;
    std::string Name;
};

int main(int argc, char *argv[])
{

    std::ifstream file("loglistener.txt");
    std::string line;
    std::string readStr = "END";

    if ( argc != 2 ) // argc should be 2 for correct execution
    {
      // We print argv[0] assuming it is the program name
      std::cout<<"FATLA EL NUMERO DE NODOS - usage: "<< argv[0] <<" <Numero de Nodos>\n";
    }
    else
    {
        int num_nodos = atoi(argv[1]);
        //std::cout<<"Numero de nodos = "<< num_nodos <<"\n";


        //struct information_nodo info_nodo[num_nodos];
        std::vector<information_nodo> info_nodo(num_nodos+1);

        while (std::getline(file, line)) // Saco linea por linea del archivo
        {
            if (line.find(readStr) != std::string::npos) //Si la linea contiene la string "END"
            {
                std::istringstream ss(line);

                //Variable para guardar los datos que vienen en el archivo
                std::string Time;
                std::string Mote;
                std::string END;
                int id;
                std::string parent;
                std::string weight;
                std::string Branch;
                std::string Name;

                //Sacar los todos los valores de la linea que estan separados por espacio
                ss >> Time >> Mote >> END >> id >> parent >> weight >> Branch >> Name;
                //Imprimir los valores guardados
                /*std::cout << id << " " << parent << " " <<
                weight  << " 0" << " 0 " <<  Branch <<
                " " <<  Name <<"\n";*/

                //tener una nueva entrada del vector

                //update values
                info_nodo[id].id = id;
                info_nodo[id].parent = parent;
                info_nodo[id].weight = weight;
                info_nodo[id].Branch = Branch;
                info_nodo[id].Name = Name;

            }
        } //end of while

        //std::cout<<"Termine de leer el archivo = "<< num_nodos <<"\n";

        //Termine de leer el archivo
        int aux;
        for(aux = 1; aux <= num_nodos; aux = aux + 1)
        {
            //Imprimir los valores guardados
            std::cout << info_nodo[aux].id << " " << info_nodo[aux].parent << " " <<
            info_nodo[aux].weight  << " 0" << " 0 " <<  info_nodo[aux].Branch <<
            " " <<  info_nodo[aux].Name <<"\n";
        }

    } //end si entre el numero de nodos correcto

}
