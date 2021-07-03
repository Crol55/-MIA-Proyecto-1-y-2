import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-user-cliente',
  templateUrl: './user-cliente.component.html',
  styleUrls: ['./user-cliente.component.css']
})
export class UserClienteComponent implements OnInit {

  usuario_logeado =  false;
  
  constructor() {
    let credencial = localStorage.getItem('credenciales');
    if(credencial)
      this.usuario_logeado = true;
  }

  ngOnInit() {
  }


  cerrar_sesion()
  {
    localStorage.removeItem('credenciales');
  }
}
