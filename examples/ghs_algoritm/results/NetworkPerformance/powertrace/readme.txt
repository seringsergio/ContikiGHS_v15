0. Generar el archivo loglistener.txt en Cooja

1. Compilar el parser del loglistener.txt.

g++ -o parserLogListener_energy parserLogListener_energy.cpp

2. Ejecutar el parser para crear el archivo

./parserLogListener_energy > powertrace.dat

3. Coje un archivo y lo divide en varios, segun el ID del nodo. Compilar este divisor de archivos.

g++ -o divideFileByID_libp_AfterTreeConstruction divideFileByID_libp_AfterTreeConstruction.cpp
g++ -o divideFileByID_libp_TreeConstruction divideFileByID_libp_TreeConstruction.cpp

g++ -o divideFileByID_ghs_AfterTreeConstruction divideFileByID_ghs_AfterTreeConstruction.cpp
g++ -o divideFileByID_ghs_TreeConstruction divideFileByID_ghs_TreeConstruction.cpp

4. Ejecutar el divisor de archivos indicando el numero de nodos, por ejemplo 9. 


./divideFileByID_libp_TreeConstruction 40
./divideFileByID_libp_AfterTreeConstruction 40

./divideFileByID_ghs_TreeConstruction 40
./divideFileByID_ghs_AfterTreeConstruction 40
