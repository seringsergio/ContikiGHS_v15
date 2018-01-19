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
        if (line.find(readStr) != std::string::npos) //Si la linea contiene la string readStr
        {
            std::istringstream ss(line);

            //Variable para guardar los datos que vienen en el archivo
            std::string Time;
            std::string Mote;
            std::string str; //Es el identificador. En este caso "stats"

            std::string linkaddr_node_addr;
            std::string connect_sent;
            std::string initiate_sent;
            std::string test_sent;
            std::string accept_sent;
            std::string reject_sent;
            std::string report_sent;
            std::string changeroot_sent;

            //Sacar los todos los valores de la linea que estan separados por espacio
            ss >> Time >> Mote >> str 
               >> linkaddr_node_addr >> connect_sent >> initiate_sent >> test_sent >> accept_sent >> reject_sent >> report_sent >> changeroot_sent;

            //Imprimir los valores guardados
            std::cout << linkaddr_node_addr << " " << connect_sent << " " << initiate_sent << " "  << test_sent << " " << accept_sent << " " << reject_sent << " " << report_sent << " "
                      << changeroot_sent << "\n";

        }
    }
}

