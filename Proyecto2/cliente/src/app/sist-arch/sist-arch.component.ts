import { Component, OnInit } from '@angular/core';
import { DatosService } from '../datos.service';
import { createEmbeddedViewAndNode } from '@angular/core/src/render3/instructions';



@Component({
  selector: 'app-sist-arch',
  templateUrl: './sist-arch.component.html',
  styleUrls: ['./sist-arch.component.css']
})
export class SistArchComponent implements OnInit {

  campo: boolean = true;
  inodos = ['carlos','madeline'];

  definitiva:string = `<h1>MOSTRAR SOLO EL ARCHIVO DE TEXTO</h1>`;
  //usuario:string ='root';
  AreaVisualizacion = false;
  Area:boolean = false;
  public list ;
  USUARIOVALIDO = false;

  public listado  ;//= [["2-1","/home","0"],["2-2","/etc","0"],["2-3","ETC2","0"],["2-6","hola1","0"],["2-7","hola2","0"],["2-8","hola3","0"]];

  // variables para el manejo de los archivos
    PADRE: string ;
  accion_actual = '';
    msg_boton = 'sin accion';
    accion_crear = false;
    accion_renombrar = false;  
    habilitar_click = false;
    accion_mover = false;
  accion_copiar = false;
  // variables para mover y copiar 
    fuente  = "";
    destino = "";


  constructor(private datosService:DatosService) 
  { 
    let credenciales = localStorage.getItem('credenciales');
    if(credenciales)
    {
      console.log(credenciales);
      this.USUARIOVALIDO = true;
      var fd = new FormData();
      fd.append('accion','4');
      fd.append('usuario', credenciales);

      datosService.obtener_inodos(fd).subscribe(respuesta =>{
      console.log("respuesta del servidor?????");
      console.log(respuesta);
      this.list = respuesta;
      });
    }
  
  }

  ngOnInit() {
  
  }


  Visualizar()
  {
    window.alert(this.PADRE);

  }

  cambio( valor, elemento:Element){ // Evento carpeta

    //window.alert(elemento.classList);

    /*let elementomsg = document.getElementById('demo');
        elementomsg.innerHTML = elemento.innerHTML
        elementomsg.classList.toggle("active");*/
    let fd = new FormData();
    fd.append('inodoPadre', elemento.id);

    this.datosService.hijos_inodo(fd).subscribe( retorno =>{
      this.listado = retorno;
     // console.log(this.listado);
    });
    this.PADRE = elemento.id;
    this.Area = true;
    //console.log(elemento.id);
    elemento.classList.toggle("caret-down");
    let padre = elemento.parentElement
    let carpetita = padre.querySelector(".carpetita");
    if(carpetita)
      carpetita.classList.toggle("active"); // collapsa a sus hijos
  }


  Evento_archivo(elemento:Element)
  {
      this.Area = false;
      console.log("Se ha clickeado un archivo");
      console.log(elemento.id);   

  }


  addListeners()
  {

    var carps = document.getElementsByClassName('caret');
    //console.log(carps);
    for(let i = 0; i < carps.length; i++)
    {
      //this.cambio(carps[i]);
      carps[i].addEventListener(('dblclick'), this.cambio.bind(this,"carajo si jalo?", carps[i]));
    }

    var archs = document.getElementsByClassName('archivo');
    //console.log(archs);

    for(let i = 0; i < archs.length; i++)
    {
      archs[i].addEventListener(('dblclick'), this.Evento_archivo.bind(this, archs[i]));
    }
  // agregado ahorita sapbee
    var botones_vista = document.getElementsByClassName('btn-outline-primary');
    console.log("super imporantes");
    console.log(botones_vista);
  }

  
  porfavordios(boton:Element)
  {
    if(this.habilitar_click)
    {
      if(this.accion_renombrar)
        this.fuente = boton.id;

      else if(this.accion_mover){
        console.log("cada click llega aqui");
        if(this.fuente === '')
          this.fuente = boton.id;
        else if(this.destino === '')
          this.destino = boton.id;
      }
    }
     // window.alert("Esta habilitado para clickear ");
    //window.alert(boton.classList);
    //window.alert(boton.id);
  }

  msg(val){
    //window.alert("si jala" + val.value);
    this.accion_crear = false;
    this.accion_renombrar = false;
    this.msg_boton = 'Sin accion';
    this.habilitar_click = false;
    this.accion_mover =  false;
    this.fuente = "";
    this.destino = "";

    switch(val.value){
      case '1':{ // crear
        this.accion_crear = true;
        this.msg_boton = 'CREAR';
      }break;

      case '2':{ // mover
       this.accion_mover = true;
       this.habilitar_click = true;
       this.msg_boton = 'Mover';
      }break;

      case '3':{
        window.alert("CLICKE SOBRE LA FUENTE Y LUEGO SOBRE EL DESTINO");
      }break;

      case '4':{ // renombrar
        this.accion_renombrar = true;
        this.msg_boton = "Renombrar";
        this.habilitar_click = true;
      }break;
    }
  }

  accion_boton(tipo, nombre){ // boton para crear archivos en la base de datos

    console.log("ahora si realizaremos la accion?"+ tipo.value + nombre.value);
    if(nombre.value !== '')
    { console.log("esperando la respuesta del servidor");
      var fd = new FormData();
      fd.append('accion','1');
      fd.append('nombre',nombre.value);
      fd.append('tipo',tipo.value);
      fd.append('padre', this.PADRE);

      this.datosService.obtener_inodos(fd).subscribe((ob_respuesta)=>{
        //console.log("Que contesto el servidor");
        window.alert(ob_respuesta.toString());
      });

    }else{
      window.alert("EL CAMPO NOMBRE NO PUEDE ESTAR VACIO");
    }
  }

  boton_Renombrar(nombre){
    
    if(this.fuente === '')
    { window.alert("NO HA SELECCIONADO QUE CARPETA O ARCHIVO, VA A RENOMBRAR"); return true;}

    if(nombre.value === '')
    {window.alert("NO HA COLOCADO EL NUEVO NOMBRE"); return true;}

    // Si llega aqui, si selecciono carpeta y si coloco nombre
    var fd = new FormData();
    fd.append('accion','2');
    fd.append('nuevo_nombre',nombre.value);
    fd.append('actual', this.fuente);

    this.datosService.obtener_inodos(fd).subscribe( ob_respuesta =>{
      window.alert(ob_respuesta.toString());
    });
  }
 
  boton_mover(){
    //console.log(this.fuente);
   // console.log(this.destino);
    if(this.fuente === ''){window.alert("No ha seleccionado archivo a mover"); return true; }
    if(this.destino === ''){window.alert("No ha seleccionado hacia donde se movera"); return true; }

    var fd = new FormData();
    fd.append('accion','3');
    fd.append('fuente', this.fuente);
    fd.append('destino', this.destino);

    this.datosService.obtener_inodos(fd).subscribe( ob_respuesta =>{
      window.alert(ob_respuesta.toString());
    });

  }

  boton_mover_cancelar()
  {
    this.fuente = '';
    this.destino = '';
  }
}


