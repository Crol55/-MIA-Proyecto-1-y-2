#ifndef RMDISK_H
#define RMDISK_H

#include <cstdio> /*puts, getchar, remove , fflush*/
#include <iostream>



bool existe_disco(std::string path) // funcion que verifica si existe el disco en la ruta enviada
{
    FILE * disco;
    if( (disco = fopen(path.c_str(), "r+")) )
    {
        fclose(disco);
        return true;
    }
 return false;
}



void Instruccion_RMDISK(std::string path)
{
 if(path != "")
 {
    if(existe_disco(path))
    {
        printf("Esta seguro que desea eliminar el Disco, S|N: ");
        int c = getchar();
        while(getchar() != '\n'){} // para limpiar la entrada del usuario

           if( (c==83) || (c==115))
           {
             int estado = remove(path.c_str());
               if(estado==0)
                  printf("Disco eliminado correctamente\n");
               else
                  printf("Error, no se pudo eliminar el Disco\n");
           }

    }else {
        puts("Rmdisk: Error -> El Disco que desea eliminar no existe.");
    }

 }else{
    puts("Rmdisk: Error Interno-> El comando RMDISK detecto que el parametro es erroneo.");
 }
}

#endif // RMDISK_H
