#ifndef UNMOUNT_H
#define UNMOUNT_H

#include <iostream>
#include "lista_enlazada.h" // para poder desmontar de la lista


void Instruccion_UNMOUNT(std::string id)
{
  if(id != "")
  {
    eliminar(id);
  }
  else
  {
    std::cout<<"Unmount: Error, el valor interno del parametro id= es incorrecto\n";
  }
}

#endif // UNMOUNT_H
