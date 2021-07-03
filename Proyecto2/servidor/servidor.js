const http  = require('http');
const rutas = require('./rutas');
//const fs    = require('fs');
//const conexionDB = require('./conexionDB');
//const conexion_consola = require('./conexion_consola'); 

var puerto = 5000;

 // configurando el servidor
const servidor = http.createServer( (req, res) =>{

   res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Access-Control-Request-Method', '*');
   res.setHeader('Access-Control-Allow-Methods', 'OPTIONS, GET,POST');
   res.setHeader('Access-Control-Allow-Headers', 'authorization, content-type');

   // let url_ = url.parse(req.url); // extrae toda la informacion de la url 
   rutas.urls(req, res); // Aqui se responderan las peticiones realizadas por medio de su url...
});
//192.168.43.21 | 
servidor.listen(puerto, '0.0.0.0', ()=>{

   console.log('Servidor escuchando en el puerto', puerto);
});


 /*console.log("Si fijo llega hasta aqui");
 fs.writeFile('/home/carlos/Escritorio/compartido.txt','1', (err)=>{

   if(err){
      return console.log(err);
   }
   console.log("fijo creo esa damier");
 })*/

/*
 {
   title: '/',
   children:[
            {
            title: '/home',
            children: [
                     {
                     title: '/etc2',
                     children: []
                     },
            ]
            },
            {
               title: '/etc',
               children: []
            },
            {
               title: '/etc2',
               children: [
               {
                  title: '/a.txt',
                  children: []
               },
               ]
            }
   ]
  }*/