import { Injectable } from '@angular/core';
import { HttpClient} from '@angular/common/http'; // revisar app.module.ts para que funcione httpclient

@Injectable({
  providedIn: 'root'
})
export class DatosService {

  
  constructor(private httpcliente:HttpClient) { 
    console.log("Servicio Funcionando");
  }

  public post_SignUp(formulario:FormData) // Metodo para indicarle al servidor que debe crear un usuario
  {
    return this.httpcliente.post('http://localhost:5000/signup',formulario);
  }

  public login(formulario:FormData)
  {
    return this.httpcliente.post('http://localhost:5000/login',formulario);
  }

  public obtener_inodos(formulario:FormData)
  {
    return this.httpcliente.post('http://localhost:5000/SA',formulario);
  }
  
  public realizar_consulta(formulario:FormData)
  {
    return this.httpcliente.post('http://localhost:5000/admin/reportes',formulario);
  }

  public hijos_inodo(formulario:FormData)
  {
    return this.httpcliente.post('http://localhost:5000/gethijos',formulario);
  }

 
}


