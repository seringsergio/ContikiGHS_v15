#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

int main()
{


    std::ifstream file("loglistener.txt");
    std::string line;
    std::string readStr = "stats ";

    while (std::getline(file, line)) // Saco linea por linea del archivo
    {
        if (line.find(readStr) != std::string::npos) //Si la linea contiene la string "READ"
        {
            std::istringstream ss(line);

            //Variable para guardar los datos que vienen en el archivo
            std::string Time;
            std::string Mote;
            std::string str;

            std::string rimeaddr_node_addr;
            std::string beaconsent;
            std::string acksent;
            std::string datasent;

            //Sacar los todos los valores de la linea que estan separados por espacio
            ss >> Time >> Mote >> str >> rimeaddr_node_addr >> beaconsent >> acksent >> datasent;

            //Imprimir los valores guardados
            std::cout << rimeaddr_node_addr << " " << beaconsent << " " << acksent <<
            " "  << datasent << "\n";

        }
    }
}

