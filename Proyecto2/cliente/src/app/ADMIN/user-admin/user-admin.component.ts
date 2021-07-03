import { Component, OnInit } from '@angular/core';
import { DatosService } from 'src/app/datos.service';

@Component({
  selector: 'app-user-admin',
  templateUrl: './user-admin.component.html',
  styleUrls: ['./user-admin.component.css']
})
export class UserAdminComponent implements OnInit {

  divReporte1 = false;
  divReporte2 = false;
  divReporte3 = false;
  divReporte4 = false;
  divReporte5 = false;

  arreglo :object;

  activo_fecha1 = false;
  activo_fecha2 = false;
  activo_nombre = false;

  public list;
  admin_logeado = false;

  constructor(private datosService:DatosService) {

    
    let admin = localStorage.getItem('admin');
    if(admin){
      this.activo_fecha1 = true; // para el primer reporte por defecto
      this.admin_logeado = true;
    }
  }

  ngOnInit() {

  }

  consultar(itemS)
  {
    //window.alert("Si que fui clickeado sapbeeee "+ fecha.value); *ngIf="divReporte1"
    if(itemS.value == '2') // listado de carpetas y archivos
    { 
      this.alternar_vista_reportes('2');
      var fd = new FormData();
      fd.append('consulta',itemS.value);

      this.datosService.realizar_consulta(fd).subscribe (res =>{
        console.log("El servidor respondio: ");
        console.log(res);
        this.list = res;
      });

    }
    else {

      var fd = new FormData();
      fd.append('consulta',itemS.value);

      if(itemS.value == '1')
      {
       
        let inputvalue = (<HTMLInputElement>document.getElementById('myDate')).value;
        fd.append('fecha', inputvalue);
        
      }else if(itemS.value == '3'){

        let inputvalue = (<HTMLInputElement>document.getElementById('myDate')).value;
        let inputvalue2 = (<HTMLInputElement>document.getElementById('fecha2')).value;
        let inputvalue3 = (<HTMLInputElement>document.getElementById('nombre')).value;
        fd.append('fecha', inputvalue);
        fd.append('fecha2', inputvalue2);
        fd.append('nombre',inputvalue3);
        if((itemS.value == '3') && (inputvalue3 == ''))
        {
          window.alert("Para realizar esa consulta, el campo nombre NO debe estar vacio ");
          return;
        }

      }else if(itemS.value == '5'){
        let inputvalue = (<HTMLInputElement>document.getElementById('myDate')).value;
        fd.append('fecha', inputvalue);
      }
    
      this.alternar_vista_reportes(itemS.value);
       this.datosService.realizar_consulta(fd).subscribe(res =>{
        console.log("El servidor respondio: ");
        console.log(res);
        this.arreglo = res;
      });

    }
    
  }


  alternar_vista_reportes(id_reporte:string)
  {
    this.divReporte1 = false;
    this.divReporte2 = false;
    this.divReporte3 = false;
    this.divReporte4 = false;
    this.divReporte5 = false;
    if(id_reporte === '1') this.divReporte1 = true;
    else if(id_reporte === '2') this.divReporte2 = true;
    else if(id_reporte === '3') this.divReporte3 = true;
    else if(id_reporte === '4') this.divReporte4 = true;
    else if(id_reporte === '5') this.divReporte5 = true;
    
  }

  dropdown(itemS){
    this.divReporte1 = false;
    this.divReporte2 = false;
    this.divReporte3 = false;
    this.divReporte4 = false;
    this.divReporte5 = false;
    //window.alert("si juncio");
    this.activo_fecha1 = false;
    this.activo_fecha2 =  false;
    this.activo_nombre = false;

    switch(itemS.value){
      case '1':{this.activo_fecha1 = true;}break;
      case '2':{}break;
      case '3':{
        this.activo_fecha1 = true;
        this.activo_fecha2 = true;
        this.activo_nombre = true;
      }break;
      case '4':{}break;
      case '5':{ this.activo_fecha1 = true; }break;
    }
  }
}
