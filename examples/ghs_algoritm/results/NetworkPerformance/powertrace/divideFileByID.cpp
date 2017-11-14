#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>     /* atoi */
#include <vector>
#include <iomanip>

using namespace std;

int main(int argc, char *argv[])
{
    std::ifstream file("powertrace.dat");
    std::string line;

    if ( argc != 2 ) // argc should be 2 for correct execution
    {
      // We print argv[0] assuming it is the program name
      std::cout<<"FALTA EL NUMERO DE NODOS - usage: "<< argv[0] <<" <Numero de Nodos>\n";
    }
    else
    {
        int num_nodos = atoi(argv[1]);
        //std::cout<<"Numero de nodos = "<< num_nodos <<"\n";


        // Create N files, where N is the num_nodos
        //Create N files, where N is num_nodos
        string filename;
        ofstream files;
        for (int i = 1; i <= num_nodos; i++)
        {
            stringstream a;
            a << i;
            filename = "nodo_" + a.str();
            filename += ".txt";
            //cout << filename << endl;
            files.open(filename.c_str(), ios::out);
            files.close();
        }


        while (std::getline(file, line)) // Saco linea por linea del archivo
        {
                std::istringstream ss(line);

                //Variable para guardar los datos que vienen en el archivo
                float linkaddr_node_addr;
                std::string all_cpu;
                std::string all_lpm;
                std::string all_transmit;
                std::string all_listen;

                //Sacar los todos los valores de la linea que estan separados por espacio
                ss >> linkaddr_node_addr >> all_cpu >> all_lpm >> all_transmit >> all_listen;
                //Imprimir los valores guardados
                //std::cout << linkaddr_node_addr << " " << all_cpu << " " <<
                //all_lpm  << all_transmit << all_listen <<"\n";

                //En que archivo guardo la linea?
                for (float i = 1; i <= num_nodos; i++)
                {
                    if(linkaddr_node_addr == i)
                    {
                        //escribo en archivo i
                        stringstream a;
                        a << i;
                        filename = "nodo_" + a.str();
                        filename += ".txt";
                        files.open(filename.c_str(), std::ios_base::app);
                        //cout << filename << endl;
                        files <<   std::fixed << setprecision(1) << linkaddr_node_addr << " "
                        << all_cpu << " " << all_lpm << " " << all_transmit << " "
                        << all_listen <<"\n";
                        files.close();
                    }
                }

                //Imprimir los valores en archivos
                //SI (ID ==8)
                    //escribo en archivo 8
                //....
                // and so on!!
        } //end of while



    }//END else ( argc != 2 )

} //END main
