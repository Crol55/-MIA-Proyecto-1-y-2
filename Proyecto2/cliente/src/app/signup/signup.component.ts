import { Component, OnInit } from '@angular/core';
import { DatosService } from '../datos.service';



@Component({
  selector: 'app-signup',
  templateUrl: './signup.component.html',
  styleUrls: ['./signup.component.css']
})
export class SignupComponent implements OnInit {

  imgURL:any;
  public message:string;
  hiddenAlert:Boolean;
  errMensaje:string;
  imagen:any;
  imgExtension: string;

  constructor(private datosservice: DatosService) {
    // inicializar variables
    this.hiddenAlert = true;
    this.errMensaje = '';
   }

  ngOnInit() {
  }


  metodo_agregarU (username, nombre, apdo, correo,pwd, tel, genero, dir, fecha)
  {
    let error:Boolean =  false;
    this.errMensaje = '';
    this.hiddenAlert = true;
    // realizar las verificaciones
    
    if((nombre.value).length > 15){ 
      this.errMensaje += 'El nombre no puede ser mayor a 15 (caracteres)\n' 
      error = true;
    }
    if((apdo.value).length > 20){
      this.errMensaje += 'El apellido no puede ser mayor a 20 (caracteres)\n';
      error = true;
    }
    if((correo.value).length > 100){
      this.errMensaje += 'El correo no puede ser mayor de 100 (caracteres)\n';
      error = true;
    }
    if( (pwd.value).length > 9){
      this.errMensaje += 'La contraseña no puede ser mayor a 9 (caracteres)\n';
      error = true;
    }
    if( (dir.value).length > 150){
      this.errMensaje += 'La Direccion no puede ser mayor a 150 (caracteres)\n';
      error = true;
    }
    if(/\s/.test(pwd.value)){
      this.errMensaje += 'La contraseña ingresada NO puede contener espacios en Blanco\n'
      error = true;
    }
    if( (username.value).length >15 ){
      this.errMensaje += 'El Username no puede ser mayor a 15 (caracteres)\n'
      error = true;
    }
    //console.log(nombre.value+','+apdo.value+','+correo.value+','+pwd.value+','+tel.value+','+genero.value+','+dir.value+','+fecha.value+',');
    
    if(!error)
    {
      let fd = new FormData(); // se envia como 'clave-valor'
        fd.append('username', username.value);
        fd.append('nombre', nombre.value);
        fd.append('apellido', apdo.value);
        fd.append('contrasena', pwd.value);
        fd.append('correo', correo.value);
        fd.append('telefono', tel.value);
        fd.append('genero', genero.value);
        fd.append('fecha_nacimiento',fecha.value);
        var fechas = new Date();
        var strFecha =  (fechas.getDate().toString())  +'/'  + (fechas.getMonth()+1).toString() + '/' + (fechas.getFullYear()).toString();
        let hora     =  (fechas.getHours()).toString() + ':' + (fechas.getMinutes()).toString() + ':' + (fechas.getSeconds()).toString();
        let fecha_hora =  strFecha + ' ' + hora ;
        fd.append('fecha_registro',fecha_hora);
        fd.append('direccion',dir.value);
        fd.append('extension', this.imgExtension);
        fd.append('imagen', this.imagen , ((username.value).toString() + this.imgExtension) );
  
        this.datosservice.post_SignUp(fd).subscribe( respuesta =>{
          console.log(respuesta);
        });
    }else{
      this.hiddenAlert = false; // muestra la alerta de error
    }
    
    return false;
  }

  preview(files){

    if(files.length === 0)
       return;
    var mimeType = files[0].type
    //console.log(mimeType);
    if(mimeType.match(/image\/*/) == null){
       this.message = "Solo puede agregar IMAGENES.";
       return;
    }
    var reader = new FileReader();
    //this.imagePath = files;
    reader.readAsDataURL(files[0]);
    reader.onload = (_event) => {
      this.imgURL = reader.result;    
      this.imagen = files[0];
      if(mimeType == "image/png")
        this.imgExtension = ".png";
      else 
        this.imgExtension = ".jpg";
    };
  }
}
