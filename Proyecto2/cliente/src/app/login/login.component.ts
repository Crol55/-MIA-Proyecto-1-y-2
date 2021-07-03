import { Component, OnInit } from '@angular/core';
import { DatosService } from '../datos.service';
import { Router } from '@angular/router';


@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {

  constructor(private datosService:DatosService, private router:Router) { }

  ngOnInit() {
  }

  iniciar_sesion(username, contrasena){
    
    var fd = new FormData();
    fd.append('username',username.value);
    fd.append('contrasena',contrasena.value);

    this.datosService.login(fd).subscribe((respuesta)=>{
      console.log(respuesta);
      switch(respuesta['estado'])
      {
        case 1:{
          this.gotoPagina("admin/reportes");
          localStorage.setItem('admin',username.value);
        }break;
        case 2:{}break;

        case 3:{ 
          this.gotoPagina("main/cliente");
          //let credenciales = {username : username.value};
          localStorage.setItem ('credenciales', username.value);

        }break;
        default:{ window.alert(respuesta['msg'])}break;
      }
    });
  }

  gotoPagina(url){
    this.router.navigate([url]).then( (e) =>{
      if(e)
        console.log("Navegacion exitosa");
      else
        console.log("Navegacion ha fallado");
    });
  }
}
