const multiparty = require('multiparty');
const conexion_consola = require('./conexion_consola'); 
const conexionDB =  require('./conexionDB');
const fs         = require('fs');
const email      = require('./email');

function urls(peticion, respuesta)
{ 
   console.log("Si llego el msg?");
   switch(peticion.url)
   {
      case '/signup':// implica crear una particion en el disco
      {
         if(peticion.method == 'POST')
         {
            console.log("Si se recibio la peticion de post en *signup*");
   
            var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/signup' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
                     let username, nombre, apellido, contrasena, correo, telefono, genero,f_nac,f_reg, direccion, extension;
                     username     = fields.username[0];
                     nombre       = fields.nombre[0];
                     apellido     = fields.apellido[0];
                     contrasena   = fields.contrasena[0];
                     correo       = fields.correo[0];
                     telefono     = parseInt(fields.telefono[0]);
                     genero       = fields.genero[0];
                     f_nac        = fields.fecha_nacimiento[0];
                     f_reg        = fields.fecha_registro[0];
                     direccion    = fields.direccion[0];
                     extension    = fields.extension[0];

                     console.log(username);
                     console.log(nombre);
                     console.log(apellido);
                     console.log(contrasena);
                     console.log(correo);
                     console.log(telefono);
                     console.log(genero);
                     console.log(f_nac);
                     console.log(f_reg);
                     console.log(direccion);


                  //let resp = await
                  conexionDB.Usuario_singup(username, contrasena, nombre, apellido, correo, telefono, genero,f_nac, f_reg, direccion, extension).then( (resp)=>{
                     
                     if(resp.estado === 1)
                     {
                        console.log("Usuario:"+username+" ha sido creado en la base de datos");
                        Object.keys(files).forEach(function(name) { // Lee imagen enviada desde angular
                           //console.log('got file named ' + name);
                           //console.log(files[name]);
                           let fileArray = files[name];   // [  ]
                           let fileObjeto = fileArray[0]; // {  }
         
                           let oldRuta    = fileObjeto['path'];
                           let nombreI    = fileObjeto['originalFilename'];
                           //console.log("no es posible:"+ oldRuta);
                           let newRuta = "imagenes/" + nombreI; // ruta relativa al servidor
         
                           fs.rename(oldRuta, newRuta, (error)=>{ // cambia la locacion de un archivo

                              if(error) console.error(error);
                              console.log("imagen almacenada en el servidor"); 
                           }); 
                         });  
                        
                        let nueva_contrasena = Generar_Contrasena();
                        email.Enviar_Correo(correo, username, nueva_contrasena).catch(console.error);
                        conexionDB.PROCEDURE_CONFIRMACION_CUENTA(username, nueva_contrasena,contrasena);

                     }

                     respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                     respuesta.write( JSON.stringify(resp ) );
                     respuesta.end();

                  });

              /* Object.keys(fields).forEach( (clave)=>{
                  console.log('fields: '+ fields[clave]);
               });*/
                
               }               
            });
         }
      }break;

      case '/syncronice': // c++ envia aqui la informacion cuando realiza syncronice
      {
         console.log("C++ ENVIO INFORMACION PARA SINCRONIZAR CON LA BASE DE DATOS");
         var info = ''; // concatenar todo el texto enviado desde c++
         peticion.on('data', (chunk)=>{

            info += chunk.toString();
         });

         peticion.on('end', ()=>{

            if(info !== '')
               conexion_consola.Sincronizar_SA(info).then(()=>{

                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write('Entrada correcta?');
                  respuesta.end();
               });

            else{
               respuesta.writeHead(404, {'Content-Type': 'text/html'});
               respuesta.write('<h1>404 !!! Pagina no encontrada</h1>');
               respuesta.end();
            }
         });
         
      }break;

      case '/login':
      {
         if(peticion.method == 'POST') // 
         {
            var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/login' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
                  console.log(fields.username[0]);
                  console.log(fields.contrasena[0]);

                  conexionDB.Usuario_Login(fields.username[0], fields.contrasena[0]).then( (resp)=>{

                     respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                     respuesta.write( JSON.stringify(resp ) );
                     respuesta.end();
                  });
               }  
                           
            });
         }
      }break;

      case '/admin/reportes':
      {
         if(peticion.method == 'POST')  
         {
            var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/admin/reportes' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
                  
                 // console.log(fields.consulta);
                 // console.log(typeof fields.f_nac[0]);

                  if(fields.consulta[0] === '1'){

                     conexionDB.REPORTE_NACIMIENTO('', fields.fecha[0]).then( resp => {
                        //console.log("Que se le contestara al servidor?");
                        //console.log(resp);
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(resp ) );
                        respuesta.end();
                     });

                  }else if(fields.consulta[0] === '2'){ // recuperar El sistema de archivos de todos los usuarios

                     
                     conexionDB.REPORTE_SA().then(rsa=>{
                        //console.log(rsa);
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(rsa) );
                        respuesta.end();
                     });

                  }else if(fields.consulta[0] === '3'){

                     conexionDB.REPORTE_MODIFICACION_INODO('', fields.fecha[0],fields.fecha2[0], fields.nombre[0]).then(resp =>{
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(resp ) );
                        respuesta.end();
                     });

                  }else if(fields.consulta[0] === '4'){
                     console.log("Extraer bitacora sapbee");
                     conexionDB.REPORTE_BITACORA().then(rb => {
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(rb ) );
                        respuesta.end();
                     });

                  }else if(fields.consulta[0] === '5'){

                     conexionDB.REPORTE_CLIENTES('', fields.fecha[0]).then(resp =>{
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(resp ) );
                        respuesta.end();
                     });
                  }
               }  
                           
            });
         }

      }break;

      case '/SA':{

         if(peticion.method == 'POST')  
         { 
            var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/SA' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
                  //console.log(fields.usuario);
                  //let usuario = fields.usuario[0];
                  let accion  = fields.accion[0];

                  if(accion == '1'){ // Crear una carpeta o archivo nuevo
                     let nombre = fields.nombre[0];
                     let tipo = fields.tipo[0];
                     let padre = fields.padre[0];

                     conexionDB.CREAR_INODO(nombre, tipo, padre).then( ci_respuesta =>{
                        //console.log(ili_respuesta);
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(ci_respuesta ) );
                        respuesta.end();
                     });

                  }else if(accion == '2'){
                     console.log("permisos para renombrar");
                     let nuevo_nombre = fields.nuevo_nombre[0];
                     let actual = fields.actual[0];

                     conexionDB.RENOMBRAR_INODO(nuevo_nombre, actual).then( ri_respuesta =>{
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                              respuesta.write( JSON.stringify(ri_respuesta ) );
                              respuesta.end();
                     });

                  }else if(accion == '3'){
                     let fuente = fields.fuente[0];
                     let destino = fields.destino[0];

                     conexionDB.MOVER_INODO(fuente, destino).then(mi_respuesta =>{
                        respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                        respuesta.write( JSON.stringify(mi_respuesta ) );
                        respuesta.end();
                     });

                  }else if(accion == 4){

                     let usuario = fields.usuario[0];
                     conexionDB.USUARIO_get(usuario).then( res =>{
                        if(res.length > 0)
                        {
                           arrDisco = res[0]; 
                           //console.log(arrCodigo);
                           //'2-0', 2, 'crol1'
                           let padre, disco;
                           disco = parseInt(arrDisco[0]);
                           padre  = arrDisco[0] + '-0' // ej 2-0
                           conexionDB.Iniciar_LecturaInodos(padre, disco, usuario).then( ili_respuesta =>{
   
                              let arreglo = [];
                              let objeto_string = JSON.parse(ili_respuesta);
                              arreglo.push(objeto_string);
                              respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                              respuesta.write( JSON.stringify(arreglo) );
                              respuesta.end();
                           });
                        }
                     });

                  } 

               }  
                           
            });
         }

      }break;

      case '/gethijos':
      {
         if(peticion.method == 'POST') // 
         {
            var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/gethijos' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
                  //console.log(fields.inodoPadre[0]);
                  let padre = fields.inodoPadre[0];
                  conexionDB.get_hijos_inodo(padre).then( ghi_respuesta =>{
                     respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                              respuesta.write( ghi_respuesta );
                              respuesta.end();
                  });
                 
               }  
                           
            });
         }
      }break;

      case '/prueba':{

         if(peticion.method == 'POST') // 
         {
       
            var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/gethijos' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
               
                  //C1r@f
                 // console.log(fields.username);
                  //console.log(fields.contrasena);
                  conexionDB.BUSCAR_USUARIO(fields.username[0], fields.contrasena[0]).then( bu_respuesta =>{
                     
                     if(bu_respuesta.length > 0)
                     {
                        let tipo_usuario = bu_respuesta[0];
                        let t = '';
                        if(tipo_usuario[0] === 3)
                           t = '3';
                        else {
                           t = '1';
                        }
                        respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                        respuesta.write(t);
                        respuesta.end();
                     }
                     else {
                        respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                        respuesta.write("ACCION DENEGADA");
                        respuesta.end();
                     }

                     
                  });
                  /*conexionDB.get_hijos_inodo(padre).then( ghi_respuesta =>{
                     respuesta.writeHead(200, {'Content-Type': 'text/JSON'});
                              respuesta.write( ghi_respuesta );
                              respuesta.end();
                  });*/
                  /* Object.keys(fields).forEach( (clave)=>{
                  console.log('fields: '+ fields[clave]);
               });*/
               //C1r@f
               
                 
               }  
                           
            });
         }
      }break;

      case '/getParticion':{

         var form = new multiparty.Form();
            form.parse(peticion, async (err, fields, files)=>{
               
               if(err)
               {
                  console.log("Error (catched) en 'rutas.js/gethijos' metodo -> 'form.parse' 'Alguien esta intentanto tronar el servidor'");
                  respuesta.writeHead(200, {'Content-Type': 'text/plain'});
                  respuesta.write("ACCION DENEGADA");
                  respuesta.end();
               }
               else {
               
                  console.log(fields.username);
                  //console.log(fields.contrasena);


               }  
                           
            });
      }break;
   }
}

function Random(max, min)
{
   return Math.floor(Math.random() * (max - min + 1) + min);
}

function Generar_Contrasena()
{
   let contrasena = "";
   let mayusculas = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
   let minusculas = 'abcdefghijklmnopqrstuvwxyz';
   let simbolos = '*-/+?¿¡?&%$#!-_,;<>@|';

   let result  = mayusculas.charAt( Random(0,25));
   let result2 = minusculas.charAt( Random(0,25));
   let result3 = simbolos.charAt( Random(0,20));
   let result4 = "";
   for(let i =0; i < 5; i++){
      result4 += Random(0,9);
   }

   contrasena = result + result2 + result4 + result3;
   //console.log("contrasena:" + contrasena);

 return contrasena;
}


//Generar_Contrasena();
exports.urls = urls; // exportamos la funcion url