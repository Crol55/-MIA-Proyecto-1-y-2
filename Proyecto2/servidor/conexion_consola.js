const conexionDB = require('./conexionDB');

function Buscar_Usuario(arreglo, codigo) // buscar un usuario por 'codigo' adentro de arreglo y retorna el nombre si existe, sino retorna ""
{

    if(codigo == 1) return 'root'; // En el arreglo , el primer registro con codigo 1 es root

    let obj_usuario; // compuesto de { codigo: x, nombre: y }
    for(let i =0; i < arreglo.length; i++) // el arreglo internamente tiene un objeto
    {
        obj_usuario = arreglo[i];
        if( obj_usuario['codigo'] == codigo ) // codigo debe ser tipo int
            return obj_usuario['username'];    
    }
 return ""; // solo llega aqui si ninguno coincide, pero es casi imposible
}


async function Sincronizar_SA(rawdata) // sincroniza el|los sistema de archivos hacia la base de datos 
{ 
  // console.time('fin');
  // fs.readFile('/home/carlos/Documentos/Intercambio/cescritura.json', async (err,rawdata)=>{
    //  console.timeEnd('fin');
      let arreglo_u = []; // creamos un arreglo 
      let data = JSON.parse(rawdata);
      let ruta, tamano, particion, super_bloque, inodos;
      
      ruta         = data['ruta'];
      tamano       = data['tamano'];
      particion    = data['particion']
      super_bloque = data['super_bloque'];
      inodos       = data['inodos'];
      

      //---------- Insertar a la base de datos el Disco 
      await conexionDB.INSERT_Disco(ruta, tamano);// devuelve !=-1 si el disco es nuevo

      // ---------- INSERTAR a la base de datos la particion (depende del codigo del disco)
      let codigo_disco = await conexionDB.DISCO_getCodigo(ruta);
      await conexionDB.INSERT_Particion(particion['part_name'],particion['part_type'],particion['part_size'],codigo_disco);
     

      // --------- Recorrer cada objeto(inodo) adentro de 'inodos'
      let  id_inodo_padre;
      let nueva_conexion = await conexionDB.Crear_Conexion();
      let codigo_particion = await conexionDB.PARTICION_getCodigo(particion['part_name'], codigo_disco);// int
      let inodo;
      
      let str_cod = codigo_particion.toString() + '-';

      for(let i =0; i < inodos.length; i++){
         
         inodo = inodos[i];      
         // 1) Realizar un 'UPDATE' de inodo
         id_inodo_padre = inodo['id'];
         if(id_inodo_padre !== 0)
         {
            let usuario = await Buscar_Usuario(arreglo_u, inodo['i_uid']);
            //console.log("Que usuario encontro:" + usuario);
            await conexionDB.UPDATE_SYNCRONICE_TEMPORAL(usuario, inodo['i_type'], inodo['i_iperm'], (str_cod+id_inodo_padre));

         }else{ // este solo servira para INSERTAR EL INODO RAIZ '/' === inodo(0)

            await conexionDB.Insertar_SYNCRONICE_TEMPORAL(nueva_conexion,(str_cod+'0'), null, '/',codigo_particion,'root','0',777); // insercion de inodo raiz
         }

         //2) Realizar un 'INSERT' de inodo a partir de sus bloques
         if(inodo['i_type']=='0') // inodo de tipo carpeta
         {
            let clave_bloque = inodo['bloque_c'];
            let bc;
            for(let j=0; j < clave_bloque.length; j++)
            {
               bc = clave_bloque[j];
               if((bc['nombre']!='.') && (bc['nombre']!='..'))
               {
                 // console.log(bc['nombre']);
                  await conexionDB.Insertar_SYNCRONICE_TEMPORAL(nueva_conexion,str_cod+bc['inodo'],str_cod+id_inodo_padre,bc['nombre'],codigo_particion,null,null,null);
               }
            }
            
         }else{ // inodo de tipo archivo

            let arreglo_bloque = inodo['bloque_a']; // El inodo es de archivo
            //console.log(arreglo_bloque);
            let objeto_contenido; // para almacenr los objetos internos de arreglo_bloque
            let usuarios = '';    // para concatenar los usuarios
            for(let i = 0; i < arreglo_bloque.length; i++)
            {
               objeto_contenido = arreglo_bloque[i];
               if(id_inodo_padre !== 1)
               {
                  await conexionDB.INSERT_BLOQUEA_TEMPORAL((str_cod+id_inodo_padre), objeto_contenido['contenido']);

               }else{ // es el inodo -> users.txt, por lo danto hay que quitarle <br> y tambien aqui se crean los usuarios
                  
                  let modif =  await objeto_contenido['contenido'].replace(/<br\/>/g, " "); //reemplazar <br/>
                  usuarios += objeto_contenido['contenido']; // concatenar los usuarios
                  await conexionDB.INSERT_BLOQUEA_TEMPORAL((str_cod+id_inodo_padre), modif);                  
               }
            } 
            
            if(usuarios !== ''){ // solo ocurre 1 vez en toda la lectura del JSON
               //console.log("Insertando usuarios");
               let registros = usuarios.split("<br/>");
               let registro,psw, username;
               for(let ri = 0; ri < registros.length-1; ri++)
               {   
                  registro = registros[ri].split(",");
                  if(registro.length === 5){ // es un registro de tipo usuario

                     username = registro[3];
                     psw = registro[4];

                     arreglo_u.push( {codigo:registro[0], username:registro[3]} ); // insertar el usuario y su valor , al arreglo de usuarios
                     await conexionDB.INSERT_USUARIO(username, psw, codigo_particion);
                  }
               }
               usuarios = ''; // limpiamos para que no vuelva a ingresar
            }
            
         }

      }
      conexionDB.Cerrar_Conexion(nueva_conexion);
      // 3) TRASLADAR DE LAS TABLAS TEMPORALES HACIA LAS REALES
      await conexionDB.COPIAR_TEMPORAL_A_INODO(); 
      await conexionDB.COPIAR_BLOTEMPORAL_A_BLOQUE_A(); 
      // 4) BORRAR TODOS LOS REGISTROS DE LAS TABLAS TEMPORALES
      await conexionDB.TRUNCATE_TABLE_TEMPORAL();
      console.log("FIN DE SYNCRONICE..");
  // });
}

//exports.Buscar_Usuario = Buscar_Usuario;
exports.Sincronizar_SA = Sincronizar_SA;