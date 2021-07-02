#ifndef ANALIZADOR_LEXICO_H
#define ANALIZADOR_LEXICO_H

#include <iostream>

using namespace std;

typedef struct token
{
    int num_token;
    string lexema;
}Token;

Token analizador_lexico(string *texto)
{
    Token t;
    char caracter; // para almacenar un caracter que este adentro de *texto
    bool termino = false; // Para determinar cuando se detiene el ciclo while
    string lexema=""; // para concatenar los caracteres e ir formando un lexema
    int estado = 0;

    while( (!termino) && (texto->length() > 0)) // mientras no termino y texto no esta vacio.
    {
        caracter = texto->at(0); // obtener el primer caracter del string

        switch(estado)
        {
            case 0:{
                if(( (caracter>=65)&&(caracter<=90) ) || ( (caracter>=97)&&(caracter<=122) )) // son letras
                {
                    lexema += caracter;
                    estado = 1; // ir a case 1
                    t.num_token = 1;
                }
                else if(caracter==32) // espacio en blanco
                {
                    estado = 2; // ir a case 2
                    lexema += caracter;
                    t.num_token = 2;
                }
                else if(caracter==45) // '  -  ' -> reconoce el guion
                {
                    lexema += caracter;
                    t.num_token = 3;
                    termino=true;
                    texto->erase(0,1); // salta a la siguiente direccion de memoria manualmente

                }
                else if(caracter==61) // '  =  ' -> reconoce el igual
                {
                    lexema += caracter;
                    t.num_token = 4;
                    termino=true;
                    texto->erase(0,1); // salta a la siguiente direccion de memoria manualmente
                }
                else if(caracter>=48 && caracter<=57) // reconocera el primer numero (0-9)
                {
                     estado=3; // ir a case 3
                     lexema += caracter;
                     t.num_token = 5;
                }
                else if(caracter==47) //   recibe diagonal para el inicio de un path
                {
                    estado=4; // ir a case 4
                    lexema += caracter;
                    t.num_token = 6;
                }
                else if(caracter==34) // recibe comillas
                {
                    estado=5;// ir a case 5
                    t.num_token = 7;

                }
                else // para concatenar caracteres que no pertenecen al lenguaje
                {
                    lexema += caracter;
                    t.num_token = -1;
                    estado = -1;
                    //termino=1;
                }

            }break;

            case 1:{// anterior -> case 0
                if(( (caracter>=65)&&(caracter<=90) ) || ( (caracter>=97)&&(caracter<=122)) || (caracter>=48 && caracter<=57) || (caracter==95)) // son letras | numeros | _
                {
                    estado = 1; // permanece en el mismo case 1 siempre que sean letras o numeros
                    lexema += caracter;

                }
                else // si no es una letra o numero debe de terminar el analisis
                {
                  termino = true;
                  if(caracter==34) // esta comilla habria que quitarla -> para extensiones de ruta (revisar)
                      texto->erase(0,1);; // salta a la siguiente direccion de memoria
                }
            }break;

            case 2:{
                if(caracter!=32) // si no es un espacio en blanco debe terminar el analisis
                {
                    termino=true;
                }
            }break;

            case 3:{ //(0-9)+
                if(caracter>=48 && caracter<=57) // reconocera numeros+
                {
                     estado=3;
                     lexema += caracter;
                }
               // else if( ((caracter>=65)&&(caracter<=90)) || ((caracter>=97)&&(caracter<=122)) ) /*Esto fue Agregado para la 2da fase*/
                //{
                  //  estado = 5;
                   // concatenar_texto(&lexema,caracter);
                   // t.num_token = 20;
                //}
                else
                  termino=true; // para que salga del metodo

            }break;

            case 4:{ // concatena TODO (/rutas/)
                if(caracter != 32)//concatena TODO, y termina cuando encuentra un espacio en blanco o fin de cadena
                {
                    estado = 4;
                    lexema += caracter;
                }
                else
                    termino = true;
            }break;

            case 5:{
                if(caracter != 34)
                {
                    estado = 5;
                    //if(caracter == 32) // (espacio)
                    //{
                      //  lexema += "\\"; // agregamos contradiagonal antes de agregar el espacio en blanco para que la consola pueda reconocer la ruta
                    //}
                    lexema += caracter;
                }
                else{
                    termino = true;
                    texto->erase(0,1); // para quitar la ultima ' " ' comillas dobles
                }
            }break;
        }

        if(!termino)
            texto->erase(0,1);// debemos ir quitando los caracteres que ya fueron agregados a lexema
    }
   // Cuando se haya encontrado una coincidencia de lexema debemos retornarlo o retornar un error
    t.lexema = lexema;

 return t;
}
#endif // ANALIZADOR_LEXICO_H
