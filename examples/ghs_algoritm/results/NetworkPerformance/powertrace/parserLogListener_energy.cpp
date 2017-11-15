#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

int main()
{


    std::ifstream file("loglistener.txt");
    std::string line;
    std::string readStr = " P ";

    while (std::getline(file, line)) // Saco linea por linea del archivo
    {
        if (line.find(readStr) != std::string::npos) //Si la linea contiene la string "READ"
        {
            std::istringstream ss(line);

            //Variable para guardar los datos que vienen en el archivo
            std::string Time;
            std::string Mote;
            //std::string str;
            std::string clock_time;
            std::string P;
            std::string linkaddr_node_addr;
            std::string seqno;
            std::string all_cpu;
            std::string all_lpm;
            std::string all_transmit;
            std::string all_listen;
            std::string all_idle_transmit;
            std::string all_idle_listen;
            std::string cpu;
            std::string lpm;
            std::string transmit;
            std::string listen;
            std::string idle_transmit;
            std::string idle_listen;


            //Sacar los todos los valores de la linea que estan separados por espacio
            ss >> Time >> Mote >> /*str >>*/ clock_time >> P >> linkaddr_node_addr >> seqno
            >> all_cpu >> all_lpm >> all_transmit >> all_listen >> all_idle_transmit >> all_idle_listen >>
            cpu >> lpm >> transmit >> listen >> idle_transmit >> idle_listen;

            //Imprimir los valores guardados
            std::cout << linkaddr_node_addr << " " << all_cpu << " " << all_lpm <<
            " "  << all_transmit << " " << all_listen << "\n";

        }
    }
}
