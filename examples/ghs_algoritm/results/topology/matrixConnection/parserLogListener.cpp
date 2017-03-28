#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

int main()
{


    std::ifstream file("loglistener.txt");
    std::string line;
    std::string readStr = "END";

    while (std::getline(file, line)) // Saco linea por linea del archivo
    {
        if (line.find(readStr) != std::string::npos) //Si la linea contiene la string "END"
        {
            std::istringstream ss(line);

            //Variable para guardar los datos que vienen en el archivo
            std::string Time;
            std::string Mote;
            std::string END;
            std::string id;
            std::string parent;
            double weight;
            std::string Branch;
            double Name;

            //Sacar los todos los valores de la linea que estan separados por espacio
            ss >> Time >> Mote >> END >> id >> parent >> weight >> Branch >> Name;
            //Imprimir los valores guardados
            std::cout << id << " " << parent << " " << weight  << " 0" << " 0 " <<  Branch << " " <<  Name <<"\n";

        }
    }
}
