#include <iostream>

#include "interprete.h"

using namespace std;

int main()
{
    while(true) // Para que la terminal se mantenga siempre en ejecucion
    {
        int teclado; // para almacenar cada caracter escrito en la consola
        bool descartar = false; // para descartar los comentarios
        string texto = ""; // para concatenar solo caracteres validos ingresados en consola

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
