
#include <iostream>
#include <cstdio> // getchar()
#include "interprete.h"
//#include <thread>
//#include <chrono>

using namespace std;
/*
void func1()
{

 int inicio =10;
 for (int  i= 0;  i< inicio; i++) {
     cout<<"func1: "<<i<<endl;
 }
}

void func2(){
    int inicio =10;
    for (int  i= 0;  i< inicio; i++) {
        cout<<"func2: "<<i<<endl;
    }
}
char g_modo = 'T'; // inicia en modo lectura de teclado pero puede pasar a lectura de Base de Datos

void leer_teclado()
{

}

void leerDeNodejs() // se mantiene a la espera de que node js le indique que debe ejecutar instrucciones
{
    FILE *fichero;
    char estado;
    while(true)
    {
        fichero = fopen("/home/carlos/Escritorio/compartido.txt","r+");// abrir el modo lectura y escritura
        if(fichero != NULL)
        {

            fread(&estado,1,1,fichero);
            if(estado == '1')
                break;
            this_thread::sleep_for(chrono::seconds(2));

            fclose(fichero);
        }else{
            cout<<"Error al abrir el fichero en 'leerDeNodejs'\n";
        }
    }

    cout<<"alfin logre salir porque,estado="<<endl;
}*/


int main()
{

   // leer_teclado();
    //Ambas deben ejecutarse de manera concurrente
    /*std::thread t1(leer_teclado);
    std::thread t2(leerDeNodejs);

    t1.join();
    t2.join();*/


    while(true) // Para que la terminal se mantenga siempre en ejecucion
    {
        int teclado; // para almacenar cada caracter escrito en la consola
        bool descartar = false; // para descartar los comentarios
        string texto = ""; // para concatenar solo caracteres validos ingresados en consola
        cout<<"INGRESE INSTRUCCION:";
        while( (teclado = getchar()) != 10 )// Lee todos los caracteres entrantes cuando se le de enter 10 = LF
        {
            if(teclado==35) // teclado == # -> descarta todo lo siguiente
                descartar = true;

            if(!descartar) // solo almacena caracteres validos
                texto += teclado;
        }

        if(texto != "") // verificar si se ingreso texto en la consola
        {
               Interprete(texto); // enviamos el texto con la instruccion a la clase "Interprete"
        }
    }


    return 0;
}



