#ifndef INTERPRETE_H
#define INTERPRETE_H

#include <iostream>
#include "analizador_lexico.h"
#include "mkdisk.h"
#include "rmdisk.h"
#include "fdisk.h"
#include "mount.h"
#include "rep.h"
#include "exec.h"
#include "unmount.h"

using namespace std;



string StoMinusculas(string texto)
{
    string nuevo="";
    int tamano = texto.length();
    for(int i  = 0;  i < tamano; i++) // para recorrer todo el texto entrante.
    {
        nuevo += (char) tolower(texto.at(i));
    }

 return nuevo;
}


void Interprete(string texto)
{
 // cout<<"Recibido en el interprete:"<< texto <<endl;
  int estado = 0;
  bool error = false;
  char parametro = '\0'; // para ver que parametro esta activo en ese momento
  //============= Variables globales utilizadas en parametros de una instruccion =============================
  int tamano = 0;
  char unidad = '\0';
  char tipo_ajuste = '\0'; // utilizado por fit
  char tipo_particion = '\0'; // utilizado por type
  string nombre_particion = "";
  char tipo_borrado = '\0';
  int espacio = 0;
  char operacion = '+';  /*para utilizarlo en reduccion o agregacion de espacio de una particion*/
  string ruta = "";
  string id_disco = "";
  char instruccion = '\0';

  string jerarquia = ""; // utilizado para fdisk, para saber que comandos ignorar

    while( (texto.length() >0) && (!error) )
    {
        Token t =  analizador_lexico(&texto); //Enviar la cadena multiples veces para que reconozca un token
       // cout<<"lexema:"<<t.lexema<<",token:"<<t.num_token<<endl;

        switch(estado) // El switch trabajara como un analizador sintactico
        {
            case 0:{ // Se esperaran comandos: mkdisk, fdisk, rmdisk, mount, unmound, exec, rep
                string lex_m = StoMinusculas( t.lexema );

                if(lex_m.compare("mkdisk") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'm';
                    tipo_ajuste = 'f'; /* por defecto tiene primer ajsute(FF)*/
                    unidad = 'm'; /*Por defecto tendra Megabytes (m) */

                }else if(lex_m.compare("rmdisk") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'r';

                }else if(lex_m.compare("fdisk") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'f';
                    tipo_ajuste = 'w'; // tiene peor ajsute(WF) por defecto
                    tipo_particion = 'p'; // tiene primaria por defecto
                    unidad = 'k';   /*Por defecto sera kilobytes(k) */

                }else if(lex_m.compare("mount") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'M';

                }else if(lex_m.compare("unmount") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'u';

                }else if(lex_m.compare("rep") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'R';

                }else if(lex_m.compare("exec") == 0){

                    estado = 1; // ir a case 1
                    instruccion = 'e';

                }else{
                    cout<<"Error, se esperaba(mkdisk|rmdisk|fdisk|mount|unmount|rep|exec) y se obtuvo:"<<t.lexema<<endl;

                    error = true;
                }
            }break;

            case 1:{ // se espera un (espacio) o multiples

                if(t.num_token==2) /*Es un espacio o multiples espacios*/
                   estado=2; // ir a case 2
                else{
                    cout<<"ERROR!! Se esperaba espacio en blanco y se obtuvo:"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 2:{ // se espera un guion -

                if(t.num_token==3) // espera el guion -
                    estado=3; // ir a case 3
                else
                {
                    cout<<"Error se esperaba '-' y se obtuvo:"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 3: // Aqui se espera, size, fit, unit, path, delete,name, etc......
            {
                 string lex_m = StoMinusculas( t.lexema );
                 estado = 4; // ir a case 4

                 if(lex_m.compare("size") == 0){
                     parametro = 's';
                     if(jerarquia == "") // usado en fdisk
                        jerarquia = "size";

                 }else if(lex_m.compare("unit") == 0){
                     parametro = 'u';

                 }else if(lex_m.compare("fit") == 0){
                     parametro = 'f';

                 }else if(lex_m.compare("type") == 0){
                     parametro = 't';

                 }else if(lex_m.compare("delete") == 0){
                     parametro = 'd';
                     if(jerarquia == "")// usado en fdisk
                        jerarquia = "delete";

                 }else if(lex_m.compare("add") == 0){
                     parametro = 'a';
                     if(jerarquia == "")// usado en fdisk
                        jerarquia = "add";

                 }else if(lex_m.compare("name") == 0){
                     parametro = 'n';

                 }else if(lex_m.compare("id") == 0){
                     parametro = 'i';

                 }else if(lex_m.compare("path") == 0){
                     parametro = 'p';

                 }else{
                     cout<<"Error se esperaba (size|unit|path|fit|type|delete|add|name|id|fs|usr|pwd|grp) y se obtuvo"<<t.lexema<<endl;
                     error = true;
                 }


            }break;

            case 4: // Se espera que venga ' = '
            {
                if(t.num_token == 4) // ' = '
                {
                    if(parametro == 's')
                       estado=5;
                    else if(parametro == 'u')
                       estado=6;
                    else if(parametro == 'f')
                       estado=7;
                    else if(parametro == 't')
                       estado=8;
                    else if(parametro == 'n')
                       estado=9;
                    else if(parametro == 'd')
                       estado=10;
                    else if(parametro == 'a')
                       estado=11;
                    else if(parametro == 'p')
                       estado=13;
                   else if(parametro == 'i')
                       estado=14;
                }
                else
                {
                    cout<<"Error se esperaba ' = ' y se obtuvo"<< t.lexema<< endl;
                    error = true;
                }
            }break;

            case 5:
            {
                estado = 1; // Regresamos para poder leer mas posibles parametros
                    if(t.num_token==5)
                    {
                      tamano = stoi(t.lexema);
                     // cout<<"El tamaño que se almaceno en size es"<< tamano<< endl;
                    }
                    else
                    {
                      cout<<"Error se esperaba '(numero)' y se obtuvo "<< t.lexema<< endl;
                      error = true;
                    }
            }break;

            case 6: // parametro de unit
            {
                estado=1; /*volvemos para recibir mas parametros*/
                    string lex_m = StoMinusculas( t.lexema );

                    if(lex_m.compare("k") == 0)
                        unidad='k';
                    else if(lex_m.compare("m") == 0)
                        unidad='m';
                    else if(lex_m.compare("b") == 0)
                        unidad='b';
                    else
                    {
                        cout<<"Error en los parametros de unit se esperaba 'b'o'k'o'm' y se obtuvo:"<<t.lexema<<endl;
                        error= true;
                    }
            }break;

            case 7: // parametro de fit
            {
                estado=1; /*regresamos por si hay que leer mas parametros*/
                    string lex_m = StoMinusculas(t.lexema);

                      if(lex_m.compare("bf") == 0)
                          tipo_ajuste = 'b';
                      else if(lex_m.compare("ff") == 0)
                          tipo_ajuste = 'f';
                      else if(lex_m.compare("wf") == 0)
                          tipo_ajuste = 'w';
                      else{
                          cout<<"Error se esperaba 'bf,ff,wf' y se obtuvo:"<<t.lexema<<endl;
                          error= true;
                      }
            }break;

            case 8: // parametro de type
            {
                estado=1;
                string lex_m = StoMinusculas(t.lexema);

                     if(lex_m.compare("p") == 0)
                         tipo_particion = 'p';
                     else if(lex_m.compare("e") == 0)
                         tipo_particion = 'e';
                     else if(lex_m.compare("l") == 0)
                         tipo_particion = 'l';
                     else{
                         cout<<"Error se esperaba 'p,l,e' y se obtuvo:"<<t.lexema<<endl;
                         error = true;
                     }
            }break;

            case 9: // parametro de name
            {
                estado=1;

                if(t.num_token == 1) // se reciben letras
                {
                        nombre_particion += t.lexema;
                }
                else if(t.num_token == 7) // se reciben letras con espacios intercalados
                {

                    string texto = "";
                    int longitud = t.lexema.length();
                        for(int i = 0; i < longitud; i++)
                        {
                            char c = t.lexema[i];
                                if(c != '\\') // para quitarle estos caracteres ya que viene asi la cadena usualmente
                                {
                                    texto += c;
                                }

                        }
                    nombre_particion = texto;

                }else{
                    cout<<"Error se esperaba name y se obtuvo"<<t.lexema<<endl;
                    error = true;
                }


            }break;

            case 10:// parametro de delete
            {
                estado=1;
                string lex_m = StoMinusculas(t.lexema);

                if(lex_m.compare("fast") == 0)
                {
                    tipo_borrado='r'; // r -> rapido

                }else if(lex_m.compare("full") == 0)
                {
                    tipo_borrado='c'; // c-> completo
                }
                 else
                 {
                       cout<< "Error se esperaba 'fast|full' y se obtuvo:"<< t.lexema;
                       error = true;
                 }

            }break;

            case 11:// Parametro de add
            {
                estado=1; /*Regresamos para poder leer mas parametros*/

                 if(t.num_token==5) // numero positivo
                 {
                   espacio = stoi(t.lexema);
                   //printf("El tamaño que se almaceno en add es %d\n",espacio);

                 }
                 else if(t.num_token==3) // ' - ' -> para numero negativo
                 {
                     estado=12; // ir a case 12
                    operacion = '-'; /*nos indica que es un valor negativo*/
                 }
                  else
                  {
                    cout<<"Error se esperaba '(numero)' | '-' y se obtuvo"<<t.lexema<<endl;
                    error = true;
                  }
            }break;

            case 12:
            {
                estado = 1;

                if(t.num_token==5) // numero positivo
                {
                  espacio = stoi(t.lexema);
                  printf("El tamaño que se almaceno en add es %d\n",espacio);

                }else {
                    cout<<"Error se esperaba '(numero)' y se obtuvo"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 13: // parametro de path
            {
                estado = 1;

                if((t.num_token == 6) || (t.num_token == 7))
                {
                    ruta = t.lexema;
                }else {
                    cout<<"Error: Se esperaba path y se obtuvo->"<<t.lexema<<endl;
                    error = true;
                }
            }break;

            case 14:
            {
                estado=1;

                if(t.num_token == 1) // se reciben letras
                {

                        id_disco = t.lexema;
                        cout<<"el id de la particion es :"<<t.lexema<<endl;
                }
                else{
                    cout<<"Error! Se esperaba letras y se obtuvo:"<<t.lexema<<endl;
                    error = true;
                }
            }break;
        }
    }


    if(!error) // Si no hay errores en el analisis se procede a ejecutar las instrucciones
    {

      if( (estado != 1) && (estado != 2)) // implica que un parametro quedo inconcluso (ej: -fit = )
      {
          puts("Interprete: Error-> Se detecto que un parametro no tiene asignado un valor.");
          return;
      }

      switch(instruccion) // Aqui ya sabemos que instruccion se quiere ejecutar
      {
          case 'm':// Quiere ejecutar 'MKDISK'
          {
            if((tipo_particion=='\0') && (tipo_borrado=='\0') && (espacio==0) && (nombre_particion=="") && (id_disco=="")) // Si alguno de estos valores por defecto cambia, significa que ingreso un parametro que no pertenece a mkdisk
            {
              puts("===========================Instruccion MKDISK====================================\n");
                Instruccion_MKDISK(tamano,tipo_ajuste,unidad,ruta);
              puts("=================================FIN MKDISK======================================\n\n\n");
              puts("\n\n\n\n\n\n\n\n");
            }else {
              cout<<"Error en la instruccion 'MKDISK': se ingreso un parametro erroneo"<<endl;
            }

          }break;

          case 'r': // Quiere ejecutar 'RMDISK'
          {
            if((tamano==0)&&(unidad=='\0')&&(tipo_ajuste=='\0')&&(tipo_particion=='\0')&&(tipo_borrado=='\0')&&(espacio==0)&&(nombre_particion=="")&&(id_disco=="")) // valores por defecto
            {
                puts("===========================Instruccion RMDISK===================================");
                    Instruccion_RMDISK(ruta);
                puts("==================================FIN RMDISK====================================\n\n\n\n");
                puts("\n\n\n\n\n\n\n\n");
            }else {
                cout<<"Error en la instruccion 'RMDISK': se ingreso un parametro erroneo"<<endl;
            }
          }break;

          case 'f': // Quiere ejecutar un 'FDISK'
          {
            if(id_disco == "") // este parametro es el unico que no viene, por eso debe ser vacio -> ""
            {
                puts("===========================Instruccion FDISK====================================\n");
                  Instruccion_FDISK(operacion, tamano, ruta, tipo_particion, tipo_ajuste,tipo_borrado,nombre_particion, espacio, unidad);
                puts("\n=================================FIN FDISK=====================================\n\n\n\n");
                puts("\n\n\n\n\n\n\n\n");
            }else {
                cout<<"Error en la instruccion 'FDISK': se ingreso un parametro erroneo"<<endl;
            }

          }break;

          case 'M': // Quiere ejecutar 'Mount'
          {
            puts("===========================Instruccion MOUNT===================================");

            if((ruta!="" && nombre_particion!=""))
                Instruccion_MOUNT(ruta, nombre_particion); /*metodo de comandos.h*/
            else
                printf("Los parametros de MOUNT son erroneos");
            puts("=================================FIN MOUNT=====================================\n\n\n\n");
            puts("\n\n\n\n\n\n\n\n");
          }break;

          case 'u': // Quiere ejecutar 'Unmount'
          {
            if((tamano==0)&&(unidad=='\0')&&(tipo_ajuste=='\0')&&(tipo_particion=='\0')&&(tipo_borrado=='\0')&&(espacio==0)&&(nombre_particion=="")&&(ruta=="")) // valores por defecto
            {
                puts("===========================Instruccion UNMOUNT===================================");
                    Instruccion_UNMOUNT(id_disco);
                puts("==================================FIN UNMOUNT====================================\n\n\n");
                puts("\n\n\n\n\n\n\n\n");
            }else {
                cout<<"Interprete: Error en la instruccion 'UNMOUNT', se ingreso un parametro erroneo\n";
            }

          }break;

          case 'R': // Quiere ejecutar 'REP'
          {
            puts("===========================Instruccion REP===================================");
            if( (nombre_particion!="") && (ruta!="") && (id_disco!="")  )
            {
                    Instruccion_REP(ruta, nombre_particion, id_disco);
            }
            else
                printf("Los parametros de el comando REP son erroneos \n");

            puts("=================================FIN REP====================================\n\n\n");
            puts("\n\n\n\n\n\n\n\n");
          }break;

          case 'e': // Quiere ejecutar 'Exec'
          {
            puts("===========================Instruccion EXEC===============================");
                if(ruta!="")
                   Instruccion_EXEC(ruta);
                else
                   printf("LOS PARAMETROS DEL COMANDO EXEC SON ERRONEOS por lo tanto no se puede ejecutar...\n");

            puts("=================================FIN EXEC=================================\n\n\n");
          }break;
      }

    }
    else {
        cout<<"ERROR! No se pudo ejecutar la instruccion ya que tiene errores."<<endl;
    }
}










#endif // INTERPRETE_H
