const oracledb =  require('oracledb');


var mypw = "adminoracle";
//oracledb.autoCommit = true;

async function conectar()
{
    let connection;
    try {
        connection = await oracledb.getConnection({
          user          : "carlos",
          password      : mypw,
          connectString : "crol/XE"
        });
        var result = await connection.execute(
          `SELECT * FROM USUARIO`
            // bind value for :id
        ); 

        console.log(result.rows);
    } catch (err) {
        console.error(err);
      } finally {
        if (connection) {
          try {
            await connection.close();
          } catch (err) {
            console.error(err);
          }
        }
      }
  return result;
}


async function INSERT_Disco(ruta, tamano)
{
    let connection;
    let estado =  true; // la transaccion inicia en un estado correcto

    try {
        connection = await oracledb.getConnection({
          user          : "carlos",
          password      : mypw,
          connectString : "crol/XE"
        });
        let result;
          
        result = await connection.execute(
          `INSERT INTO DISCO(ruta, tamano)
           VALUES (:id1,:id2)`,[ruta, tamano], { autoCommit:true}
        );
        
    } catch (err) {
        if(err['errorNum'] === 1)
          console.log("Advertencia: El Disco con la ruta = '"+ruta+"' ya esta montada en la base de datos");
        else{
          console.log("Error en conexionDB.INSERT_Disco()");
          console.error(err);
          estado = false;
        }

      } finally {
        if (connection) {
          try {
            await connection.close();
          } catch (err) {
            console.error(err);
          }
        }
      }
  return estado; // true (transaccion correcta) | false (transaccion incorrecta)
}


async function DISCO_getCodigo(ruta)
{
    let connection;
    let codigo; // para almacenar el codigo retornado al realizar la consulta
    try {
        connection = await oracledb.getConnection({
          user          : "carlos",
          password      : mypw,
          connectString : "crol/XE"
        });
        let result;

        result = await connection.execute(
            `SELECT codigo FROM DISCO WHERE ruta = :id1`,[ruta]
        ); 
          codigo = result.rows[0]; // almacenar el objeto { }
          codigo = codigo[0];       // extraer del objeto su valor en la posicion 0
          
    } catch (err) {
          console.error(err);
          console.log("Error en 'DISCO_getCodigo'");

      } finally {
        if (connection) {
          try {
            await connection.close();
          } catch (err) {
            console.error(err);
          }
        }
      }
  return codigo;
}


async function INSERT_Particion(nombre, tipo, tamano, DISCO_codigo)
{
  let connection;
  let estado = true; // transaccion correcta al inicio
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      let result;

      result = await connection.execute(
        `INSERT INTO PARTICION (nombre, tipo, tamano, DISCO_codigo)
        SELECT :id1,:id2,:id3,:id4
        from dual
        where not exists(
        select * from PARTICION 
        where nombre=:id1 AND DISCO_codigo=:id4
        )`,[nombre, tipo, tamano, DISCO_codigo]
        ,{ autoCommit:true}
        );

  } catch (err) {
      console.log("Error en conexionDB.INSERT_Particion()");
      console.error(err);
      estado = false;
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
  return estado; 
}


async function PARTICION_getCodigo(nombre, DISCO_codigo)
{
  let connection;
  let codigo; // Para almacenar el codigo luego de ejecutar la consulta
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      let result;
      result = await connection.execute(
          `SELECT codigo from PARTICION
          WHERE nombre=:id1 AND DISCO_codigo=:id2`,[nombre, DISCO_codigo] 
      ); 

      codigo = result.rows[0]; // almacenar el objeto { }
      codigo = codigo[0];       // extraer del objeto su valor en la posicion 0

  } catch (err) {
      console.error(err);
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
  return codigo; 
}


async function Crear_Conexion() // crea una conexion y retorna el objeto
{
  let connection;
  try {
    connection = await oracledb.getConnection({
      user          : "carlos",
      password      : mypw,
      connectString : "crol/XE"
    });

  } catch (error) {
    console.error(error);
  }
  //console.log(connection);
 return connection;
}


async function Cerrar_Conexion(connection){ // cierra la conexion indicada en la variable 'connection'

  if (connection) 
  {
    try {
      await connection.close();
    } catch (err) {
      console.error(err);
    }
  }
}


async function Insertar_SYNCRONICE_TEMPORAL(connection,actual,padre,nombre,particion,propietario,tipo,permiso)
{ // inserta en la tabla temporal, solo ciertos valores, los otros se ingresan por update

    try {          
        let result = await connection.execute(
          `INSERT INTO SYNCRONICE_TEMPORAL(actual,padre,nombre,particion,propietario,tipo,permiso)
           VALUES (:id1,:id2,:id3,:id4,:id5,:id6,:id7)`
           , [actual, padre, nombre, particion, propietario, tipo, permiso]
           ,{ autoCommit:true }
        );
           //console.log(result);
    } catch (err) {
        console.error(err);
      } 
  return true; 
}


async function UPDATE_SYNCRONICE_TEMPORAL(propietario, tipo, permiso, actual)
{
  let connection;
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      let result, contador;

      result = await connection.execute(
          `UPDATE syncronice_temporal
          SET
              propietario=:id1,
              tipo=:id2,
              permiso =:id3
          WHERE actual = :id4`,[propietario,tipo,permiso,actual]
          , { autoCommit:true }
      ); 

  } catch (err) {
      console.error(err);
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return true;
}

async function INSERT_BLOQUEA_TEMPORAL(sync_actual, contenido)
{
  let connection;
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      let result;

      result = await connection.execute(
          `INSERT INTO BLOQUEA_TEMPORAL(sync_actual, contenido)
          values(:id1, :id2)`,[sync_actual, contenido]
          ,{ autoCommit:true }
      ); 

  } catch (err) {
      console.error(err);
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return true;
}


async function INSERT_USUARIO(username, contrasena, PARTICION_codigo)
{
  let objeto_date = new Date();
  let fecha = ''+objeto_date.getDate()+'/'+ (objeto_date.getMonth()+1)+'/'+objeto_date.getFullYear();
  let hora  = ''+objeto_date.getHours()+':'+objeto_date.getMinutes()+':'+objeto_date.getSeconds();
  let fecha_hora =  fecha + ' ' + hora ;

  let connection;
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      await connection.execute(
          `INSERT INTO USUARIO(username, contrasena, fecha_registro, TIPO_USUARIO_codigo, PARTICION_codigo)
          values(:id1, :id2, TO_DATE(:id3,'DD/MM/YYYY HH24:MI:SS'), 3, :id4)`
          , [username, contrasena, fecha_hora, PARTICION_codigo]
          , { autoCommit:true }
      ); 

  } catch (err) {

      if(err['errorNum'] === 1){ // errorNum = 1 -> Es un unique constraint repetido
        console.log("Advertencia: El usuario '"+username+"' esta repetido, por lo tanto NO se insertara");

      }else{
        console.error(err);
      }
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return true;
}


async function COPIAR_TEMPORAL_A_INODO() // EJECUTA EL PROCEDIMIENTO DE COPIAR REGISTROS DE UNA TABLA A OTRA
{
  let connection;
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      await connection.execute(
      `CALL COPIAR_TEMPORAL_A_INODO()`, []
      , { autoCommit:true }
      ); 

  } catch (err) {
    console.log("Error en 'COPIAR_TEMPORAL_A_INODO()'->");
        console.error(err);
      
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return true;
}


async function COPIAR_BLOTEMPORAL_A_BLOQUE_A() // EJECUTA EL PROCEDIMIENTO DE COPIAR REGISTROS DE UNA TABLA A OTRA
{
  let connection;
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      await connection.execute(
      `CALL COPIAR_BLOQUEA_TEMP_A_BLOQUEA()`, []
      , { autoCommit:true }
      ); 

  } catch (err) {
        console.log("Error en 'COPIAR_BLOTEMPORAL_A_BLOQUE_A()'->");
        console.error(err);
      
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return true;
}


async function TRUNCATE_TABLE_TEMPORAL() // EJECUTA EL PROCEDIMIENTO DE COPIAR REGISTROS DE UNA TABLA A OTRA
{
  let connection;
  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      await connection.execute(
      `TRUNCATE TABLE SYNCRONICE_TEMPORAL`,[]
      , { autoCommit:true }
      ); 

      await connection.execute(
        `TRUNCATE TABLE BLOQUEA_TEMPORAL`,[]
        , { autoCommit:true }
      ); 

  } catch (err) {
        console.log("Error en 'TRUNCATE_TABLE_TEMPORAL()'->");
        console.error(err);
      
    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return true;
}



async function Usuario_singup(username, pwd, nombre, apdo, correo, tel, genero, f_nac, f_reg, dir, extensionI) // Peticion desde el cliente para insertar un usuario
{
  let connection;
  let msgRetorno = { estado : -1, mensaje : "Error al crear usuario"};  // mensaje DEFAULT
  let ruta_foto = username + extensionI;

  try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      let resultado; // objeto
      resultado = await connection.execute(
      `SELECT username,contrasena,correo FROM USUARIO 
      WHERE username=:id1`,[username]
      ); 
      console.log("usuario busqueda:"+ resultado.rows);
        
      if(resultado.rows.length > 0) // El usuario si existe
      {
        let arr_usuario = resultado.rows[0];
        if(arr_usuario[2] === null) // El usuario es del 'Sistema de archivos' antiguo (correo===null)
        {       
          if(arr_usuario[1] == pwd)
          {
            await connection.execute(
            `UPDATE USUARIO 
            SET 
            nombre=:id1, apellido=:id2, correo=:id3, telefono=:id4,
            fotografia=:id5, genero=:id6, fecha_nacimiento=TO_DATE(:id7,'YYYY/MM/DD'), 
            fecha_registro=TO_DATE(:id8,'DD/MM/YYYY HH24:MI:SS'), direccion=:id9
            WHERE username=:id10`,[nombre, apdo, correo, tel, ruta_foto, genero, f_nac, f_reg, dir, username]
            , { autoCommit:true }
            );
          }
          else
            msgRetorno = { estado : -1, mensaje : "Su usuario es del sistema de archivos, pero la contraseña no coincide"};

        }else{ // El usuario ya existe en el sistema
          msgRetorno = { estado : -1, mensaje : "El Username ya existe, escoja otro nombre de usuario."}; 
        } 

      }else{ // El usuario no existe

        let regex = new RegExp("^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[!@#\$%\^&\*+=])");
        if( regex.test(pwd) ) // si la contraseña es correcta
        {
          let codigo_disco, codigo_particion, transaccion;
          // CREAR EL NUEVO DISCO EN LA BASE DE DATOS
          let nuevaRuta = 'vd_'+username+'.disk';
          let tamano = 256*(1024*1024); // 256 MB (cada Disco) 

          transaccion = await INSERT_Disco(nuevaRuta, tamano);
          if(transaccion) // realizo correctamente INSERT_Disco
          {
            // CREAR LA NUEVA PARTICION EN LA BASE DE DATOS
            tamano = 250*(1024*1024); // 250 MB (particion)
            codigo_disco = await DISCO_getCodigo(nuevaRuta);
            if(codigo_disco) // realizo correctamente DISCO_getcodigo
            {
              transaccion  = await INSERT_Particion('virtual', 'p', tamano, codigo_disco);
              if(transaccion) // Realizo correctamente INSERT_Particion
              {
                //REGISTRAR EL NUEVO USUARIO EN LA BASE DE DATOS
                codigo_particion = await PARTICION_getCodigo('virtual',codigo_disco);
                if(codigo_particion) // Realizo correctamente PARTICION_getCodigo
                {
                  await connection.execute(
                    `INSERT INTO USUARIO(nombre,apellido,username,contrasena,correo,telefono,fotografia,genero,
                    fecha_nacimiento,fecha_registro,direccion,TIPO_USUARIO_codigo,PARTICION_codigo)
                    VALUES(:id1,:id2,:id3,:id4,:id5,:id6,:id7,:id8,TO_DATE(:id9,'YYYY/MM/DD'),
                    TO_DATE(:id10,'DD/MM/YYYY HH24:MI:SS'),:id11,3,:id12)`
                    ,[nombre, apdo, username, pwd, correo, tel, ruta_foto, genero, f_nac, f_reg, dir,codigo_particion]
                    ,{ autoCommit:true }
                  );

                  //CREAR UNA RAIZ (/) AL NUEVO SISTEMA DE ARCHIVOS EN LA BASE DE DATOS
                  let actual_;
                  actual_ = codigo_particion + '-0';
                  INSERT_INODO(actual_, null, "/", codigo_particion, "root", 777, "0","");
                  msgRetorno =  { estado : 1, mensaje : "Creacion de Usuario exitosa! Se le enviara un correo para confirmar su cuenta (vigencia 1 minuto)"}; 
                }

              }

            }
            
          }
     
        }
        else
          msgRetorno =  { estado : -1, mensaje : "Error: La contraseña no tiene el formato correcto, debe contener almenos(1 'Mayuscula', 1 'Minuscula', 1 'numero', 1 simbolo '#@#$%&^&*')"};
      }


  } catch (err) {
        console.log("Error en 'Usuario_singup()':");
        console.error(err);
        msgRetorno = { estado : -1, mensaje : "Error al intentar crear su usuario."};

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
return msgRetorno;
}


async function INSERT_INODO(actual, padre, nombre, PARTICION_codigo, propietario, permiso, tipo, contenido) // pARA INSERTAR CARPETA O ARCHIVO
{
  let connection, msg;
  if(tipo === '0') // carpeta
  {
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      await connection.execute(
      `INSERT INTO INODO (actual, INODO_padre, nombre, PARTICION_CODIGO, propietario, tipo, permiso)
      values(:id1, :id2, :id3, :id4, :id5, :id6, :id7)`,[actual, padre, nombre, PARTICION_codigo, propietario, tipo, permiso]
      ,{ autoCommit:true }
      ); 
        msg = "CARPETA '"+ nombre +"', Ha Sido creada correctamente";
        
    } catch (err) {
        console.error(err);
        msg = "Error al crear CARPETA : "+ nombre ;

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }

  }else if(tipo === '1'){ // archivo
      //Extreaer aqui el contenido
  }
 return msg;
}


async function PROCEDURE_CONFIRMACION_CUENTA(username, nc, ac) // pARA INSERTAR CARPETA O ARCHIVO
{
  let connection;
  var transaccion = false; // saber si la transaccion se ejecuto de manera correcta
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      await connection.execute(
      `CALL CONFIRMACION_CUENTA(:id1, :id2, :id3)`,[username, nc, ac,]
      ); 
      transaccion = true; 
        
    } catch (err) {
        console.log("Error en conexionDB.PROCEDURE_CONFIRMACION_CUENTA():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return transaccion;
}



async function Usuario_Login(username, contrasena) // pARA INSERTAR CARPETA O ARCHIVO
{
  let connection;
  var transaccion = { estado : -1, msg : "login error" }; // saber si la transaccion se ejecuto de manera correcta

    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      var resultado;
      resultado = await connection.execute(
      `SELECT nombre, estado, TIPO_USUARIO_CODIGO, PARTICION_CODIGO
      FROM USUARIO 
      WHERE username=:id1 AND contrasena=:id2`,[username, contrasena]
      ); 
      //console.log(resultado.rows);

      if(resultado.rows.length > 0)
      {
        let registro = resultado.rows[0]; 
        if(registro[1] === 2) // usuario confirmado previamente
        {
          let msg_ = "mostar la pagina que le corresponde por su tipo";
          let tipo_usuario = registro[2];
          transaccion = { estado : tipo_usuario, msg : msg_ };

        }else if(registro[1] === 1){ // Usuario confirmara con contraseña temporal

          resultado = await connection.execute(
            `UPDATE USUARIO
            SET estado = 2 
            WHERE username=:id1`,[username], {autoCommit : true}
          );
          //console.log(resultado.rowsAffected);
          if(resultado.rowsAffected > 0){

            let msg_ = "Su usuario ha sido confirmado correctamente, ya puede iniciar sesion con su contraseña Inicial";
            transaccion = { estado : 0, msg : msg_ };

          }else{
            let msg_ = "Error al confirmar su usuario!, comuniquece con un administrador";
            transaccion = { estado : -1, msg : msg_ };
          }

        }else{ // usuario quedo inhabilitado..
          let msg_ = "Su usuario quedo inhabilitado";
          transaccion = { estado : -1, msg : msg_ };
        }

      }else{

        let msg_ = "Su usuario o contraseña incorrectos, o su tiempo de confirmacion ha caducado!";
        transaccion = { estado : -1, msg : msg_ };
      }
        
    } catch (err) {
        console.log("Error en conexionDB.Usuario_Login():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return transaccion;
}



async function REPORTE_NACIMIENTO(username, fecha_nacimiento) 
{
  let connection;

    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });
      var result;
      result = await connection.execute(
      `SELECT * FROM USUARIO 
      WHERE fecha_nacimiento >= TO_DATE(:id1,'YYYY/MM/DD')`,[fecha_nacimiento]
      ); 
        
    } catch (err) {
        console.log("Error en conexionDB.REPORTE_NACIMIENTO():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}


async function REPORTE_CLIENTES(username, fecha) 
{
  let connection;

    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      fecha1 = fecha + ' 00:00:00'
      fecha2 = fecha + ' 23:59:59';
      var result;
      result = await connection.execute(
      `SELECT id_usuario, nombre, username, correo, PARTICION_codigo, fecha_registro
      FROM USUARIO 
      WHERE fecha_registro BETWEEN  TO_DATE(:id1,'YYYY/MM/DD HH24:MI:SS') AND TO_DATE(:id2,'YYYY/MM/DD HH24:MI:SS')
      AND (TIPO_USUARIO_codigo =3 OR TIPO_USUARIO_codigo = 2)`,[fecha1, fecha2]
      ); 

      //console.log(result.rows);

      // ver cantidad de archivos o carpetas a las que tiene acceso
      for(let i = 0; i < result.rows.length; i++)
      { 
        let registro = result.rows[i]; // es un objeto
        let resultado;
        //console.log("usuario a buscar: "+ registro[2]);
        resultado = await connection.execute(
          `select count(*) as carpetas
          from inodo where propietario=:id1 AND tipo='0'`,[registro[2]]
        ); 
        //console.log(resultado.rows);
        if(resultado.rows.length > 0)
        {
          let carp = resultado.rows[0];
          //console.log("Esto es lo que deseo almacenar: "+ carp[0]);
          registro.push(carp[0]);
        }

        resultado = await connection.execute(
          `select count(*) as archivos 
          from inodo where propietario=:id1 AND tipo='1'`,[registro[2]]
        );   
        //console.log(resultado.rows);
        if(resultado.rows.length > 0)
        {
          let arch =  resultado.rows[0];
          //console.log("archivo: "+arch[0]);
          registro.push(arch[0]);
        }
      }
      //console.log(result.rows);

    } catch (err) {
        console.log("Error en conexionDB.REPORTE_NACIMIENTO():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}


async function REPORTE_MODIFICACION_INODO(username, fecha_inicio, fecha_final, nombre_inodo) 
{
  let connection;

    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      let fecha1 = fecha_inicio + ' 00:00:00';
      let fecha2 = fecha_final  + ' 23:59:59';
      var result;
      result = await connection.execute(
      `select * from inodo 
      where nombre =:id1
      AND (f_modificacion BETWEEN TO_DATE(:id2,'YYYY/MM/DD HH24:MI:SS') AND TO_DATE(:id3,'YYYY/MM/DD HH24:MI:SS') )`
      ,[nombre_inodo, fecha1, fecha2]
      ); 
      //console.log(result.rows); 

    } catch (err) {
        console.log("Error en conexionDB.REPORTE_MODIFICACION_INODO():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}


async function INODO_getinodo(padre, PARTICION_codigo, propietario) // pARA INSERTAR CARPETA O ARCHIVO
{
  let connection;
  var transaccion = false; // saber si la transaccion se ejecuto de manera correcta
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      if(padre == null) // recuperar la raiz
      { 
        result = await connection.execute(
          `select actual, nombre, tipo from inodo 
          where inodo_padre is null
          AND particion_codigo = :id1 AND propietario ='root'`,[PARTICION_codigo]
          ); 

      }else{

        result = await connection.execute(
          `select actual, nombre, tipo from inodo 
          where inodo_padre = :id1
          AND particion_codigo = :id2 AND propietario = :id3`,[padre, PARTICION_codigo, propietario]
          ); 

      }
      console.log(result.rows);
      
      transaccion = true; 
        
    } catch (err) {
        console.log("Error en conexionDB.PROCEDURE_CONFIRMACION_CUENTA():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}



async function recursiva(padre, disco, usuario)
{
  let jsonf = '';
  let arreglo = [];

  let res = await INODO_getinodo(padre, disco, usuario);
   // console.log(res);
     
  for(let i=0; i < res.length; i++)
  {
    arreglo.push(res[i]);
  }
 
  if(res.length > 0){ // Si tenia hijos el padre

    let ino = res[0];
    //jsonf += "<ul class=\"carpetita active\">\n";

    for(let i=0; i < arreglo.length; i++)
    { //class="archivo"
      let inodo = arreglo[i];
      //console.log(inodo);
      jsonf +="{";
      if(inodo[2] == '0')
      {
        //jsonf += "<li>\n";
        //jsonf += "<span class=\"caret\">"+inodo[1]+"</span>\n";
        jsonf +=`"title": "`+inodo[1]+`","algo":"caret","actual":"`+inodo[0]+`",`;
      }else{
        jsonf += '"title":"'+inodo[1]+`", "algo":"archivo", "actual":"`+inodo[0] +`",`;
        //jsonf += "<li> <span class=\"archivo\" id=\""+inodo[0]+"\">"+inodo[1]+"</span>\n";
        //jsonf += inodo[1];
        //jsonf += "hijo:" + inodo[1]+ ", padre:" + padre + "\n";
      }      
      //console.log("Ahora hay que volver a llamar recursiva? " + jsonf);
      jsonf +='"children":[';
      jsonf += await recursiva(inodo[0], disco, usuario);
      jsonf +="]";
      //jsonf += "</li>\n";
      if( i == arreglo.length -1) jsonf +="}";
      else jsonf +="},";
    }
    //jsonf += "</ul>";
  }

 return jsonf;
}


async function Iniciar_LecturaInodos(padre, disco, usuario)
{
  var jsonf = '';
  jsonf += "{";
  //jsonf += "title: '/', algo: 'caret', actual : '" +padre+ "',\n children:[" 
  jsonf += '"title": "/", "algo": "caret", "actual" : "' +padre+ '","children":['
  jsonf += await recursiva(padre, disco, usuario);
  jsonf += "]";
  jsonf += "}";
  //console.log("Esto es lo que hay que probar puta madre");
  //console.log(jsonf);

 return jsonf;
}


async function USUARIO_get(username) 
{
  let connection;

    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `select PARTICION_codigo from usuario
      WHERE username = :id1`
      ,[username]
      ); 
      //console.log(result.rows); 

    } catch (err) {
        console.log("Error en conexionDB.USUARIO_get():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}


async function CREAR_INODO(nombre, tipo, padre) 
{
  let connection;
  let msg_respuesta = '';
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `select nombre, propietario, tipo from inodo
      where INODO_padre = :id1`,[padre]
      ); 
      //console.log(result.rows); 

      let existe = false;
      let usuario;
      if(result.rows.length > 0) // verificar si se puede crear
      {
        let registro;
        for(let i =0; i<result.rows.length; i++){ // verificar que la carpeta o archivo no exista ya en la carpeta padre

          registro = result.rows[i];
          if( (registro[0]==nombre) && (registro[2]==tipo) )
          { //console.log("no llega?");
            existe = true;
            break;
          }
          usuario = registro[1];
        }
      }
      
      if(!existe)
      {
        //console.log("si se puede insertar: "+ padre.charAt(0));
        let codigo_particion = padre.charAt(0);
        
        result = await connection.execute(
          `select count(actual) from inodo where particion_codigo = :id1`,[codigo_particion]
        );
        let sig_inodo_libre = result.rows[0];
        //console.log(sig_inodo_libre[0] );
        let actual = codigo_particion + '-' + sig_inodo_libre[0] ;
        /*console.log("actual:" + actual);
        console.log("inodo_padre:"+ padre);
        console.log("nombre:"+ nombre);
        console.log("particion:"+ codigo_particion);
        console.log("propietario:"+usuario);
        console.log("tipo:" + tipo);
        console.log("permiso" +664);
        console.log("f_modificacion: sysdate" );*/

        result = await connection.execute(
          `INSERT INTO INODO (actual, INODO_padre, nombre, PARTICION_CODIGO, propietario, tipo, permiso)
          values(:id1, :id2, :id3, :id4, :id5,:id6, 664)`,[actual, padre, nombre, codigo_particion, usuario, tipo]
          ,{autoCommit : true}
        );
        //  console.log("Que putas?");
         // console.log(result.rowsAffected);
        msg_respuesta = "HA SIDO INSERTADO CON EXITO";
        BITACORA_insert(actual, 'Creo un archivo o carpeta nuevo');
      }
      else {msg_respuesta = "NO SE PUEDE INSERTAR LA CARPETA O ARCHIVO PORQUE YA EXISTE";}

    } catch (err) {
        console.log("Error en conexionDB.CREAR_INODO():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return msg_respuesta;
}


async function RENOMBRAR_INODO(nuevo_nombre, actual) 
{
  let connection;
  let msg_respuesta = '';
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `UPDATE INODO SET nombre = :id1 
      WHERE actual = :id2`,[nuevo_nombre, actual],{autoCommit : true}
      ); 
      //console.log(result.rows); 
      msg_respuesta = "SE ACTUALIZO EL NOMBRE";
      console.log(msg_respuesta);
      BITACORA_insert(actual, 'Renombro un INODO');

    } catch (err) {
        console.log("Error en conexionDB.RENOMBRAR_INODO():");
        console.error(err);


    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return msg_respuesta;
}


async function MOVER_INODO(fuente, destino) 
{
  let connection;
  let msg_respuesta = '';
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `UPDATE INODO SET INODO_padre = :id1
      WHERE actual = :id2`,[destino, fuente],{autoCommit : true}
      ); 
      console.log(result.rowsAffected); 
      msg_respuesta = "Se movio de lugar correctamente";
      console.log(msg_respuesta);
      BITACORA_insert(fuente, 'MOVIO INODO DE LUGAR');

    } catch (err) {
        console.log("Error en conexionDB.RENOMBRAR_INODO():");
        console.error(err);


    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return msg_respuesta;
}


async function get_hijos_inodo(inodoPadre) 
{
  let connection;
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `select actual, nombre, tipo from inodo
       where inodo_padre = :id1`,[inodoPadre]
      ); 
      
      console.log(result.rows);
      let obj;
      for(let i =0; i < result.rows.length; i++){
        obj = result.rows[i];
        if(obj[2] =='0')
          obj[2] = 'folder';
        else 
          obj[2] = 'assignment';
      }
      var retorno = JSON.stringify(result.rows);

    } catch (err) {
        console.log("Error en conexionDB.RENOMBRAR_INODO():");
        console.error(err);


    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return retorno;
}



async function REPORTE_SA() 
{
  let connection;
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `select username,PARTICION_codigo from usuario`
      ); 
      
      console.log(result.rows);
      if(result.rows.length > 0) // Si recupero usuarios
      {
        let reg_usuario;
        let usuario, padre, particion;
        var arreglo = [];
        for(let i = 0; i < result.rows.length; i++)
        {
          reg_usuario = result.rows[i];
          particion = reg_usuario[1]; // number
          padre = particion +'-0';
          usuario = reg_usuario[0] ; // string
          let tree = await Iniciar_LecturaInodos(padre, particion, usuario);
          let objeto_string = JSON.parse(tree); // string -> {object}

          arreglo.push(objeto_string);          
        }
        //console.log("UUAA");
        //console.log(arreglo[0]);
        //console.log(arreglo[1]);
        //console.log("final");
        //console.log(typeof JSON.stringify(arreglo));
        
        //let b = { n : '', val : {}};
      }

    } catch (err) {
        console.log("Error en conexionDB.RENOMBRAR_INODO():");
        console.error(err);


    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return arreglo;
}


async function REPORTE_BITACORA() 
{
  let connection;
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `SELECT * FROM BITACORA`
      ); 
      
     // console.log(result.rows);
      

    } catch (err) {
        console.log("Error en conexionDB.REPORTE_BITACORA():");
        console.error(err);


    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}


async function BUSCAR_USUARIO(username, contrasena) 
{
  let connection;
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
      `SELECT tipo_usuario_codigo FROM USUARIO WHERE username=:id1 AND contrasena=:id2`,[username, contrasena]
      ); 
      
     // console.log(result.rows);
      

    } catch (err) {
        console.log("Error en conexionDB.BUSCAR_USUARIO():");
        console.error(err);


    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return result.rows;
}


async function BITACORA_insert(inodo_afectado, accion_realizada) 
{
  let connection;
    try {
      connection = await oracledb.getConnection({
        user          : "carlos",
        password      : mypw,
        connectString : "crol/XE"
      });

      var result;
      result = await connection.execute(
        `select propietario  from inodo where actual = :id1`,[inodo_afectado]
        );
      let obj_usuario = result.rows[0];

      await connection.execute(
      `INSERT INTO BITACORA (USUARIO_USERNAME,accion)
      VALUES(:id1, :id2)`,[obj_usuario[0], accion_realizada],{autoCommit : true}
      ); 
      //console.log(result.rowsAffected); 


    } catch (err) {
        console.log("Error en conexionDB.BITACORA_insert():");
        console.error(err);

    } finally {
      if (connection) {
        try {
          await connection.close();
        } catch (err) {
          console.error(err);
        }
      }
    }
 return true;
}


//TRUNCATE_TABLE_TEMPORAL();
//BITACORA_insert('2-2','Saber que hizo con la carpeta');

//REPORTE_BITACORA().then(r =>{console.log(r)});

//REPORTE_SA().then(s=>{console.log(s);});


/*let str = `{"title": "/", "algo": "caret", "actual" : "2-0","children":[{"title": "/home","algo":"caret","actual":"2-1","children":[{"title": "/etc2","algo":"caret","actual":"2-4","children":[]}]},{"title": "/etc","algo":"caret","actual":"2-2","children":[]},{"title": "ETC2","algo":"caret","actual":"2-3","children":[{"title":"/a.txt", "algo":"archivo", "actual":"2-5","children":[]}]},{"title": "hola1","algo":"caret","actual":"2-6","children":[]},{"title": "hola2","algo":"caret","actual":"2-7","children":[]},{"title": "hola3","algo":"caret","actual":"2-8","children":[]}]}`;
let abc = `{"title": "/", "algo": "caret", "actual" : "2-0","children":[{"title": "/home","algo":"caret","actual":"2-1","children":[{"title": "/etc2","algo":"caret","actual":"2-4","children":[]}]},{"title": "/etc","algo":"caret","actual":"2-2","children":[]},{"title": "ETC2","algo":"caret","actual":"2-3","children":[{"title":"/a.txt", "algo":"archivo", "actual":"2-5","children":[]}]},{"title": "hola1","algo":"caret","actual":"2-6","children":[]},{"title": "hola2","algo":"caret","actual":"2-7","children":[]},{"title": "hola3","algo":"caret","actual":"2-8","children":[]}]}`
//let str2 = '{"title":"/","algo":"caret"}';              
let obj = [];
obj.push(str);
console.log(obj);
console.log(JSON.stringify(str));
//let final =  JSON.parse(`{ "hello":"world" }`);
let final =   JSON.parse(abc);
console.log(final);*/
/*USUARIO_get('crol1').then( res =>{
  if(res.length > 0)
  {
     arrDisco = res[0]; 
     //console.log(arrCodigo);
     //'2-0', 2, 'crol1'
     let padre, disco;
     disco = parseInt(arrDisco[0]);
     padre  = arrDisco[0] + '-0' // ej 2-0
     Iniciar_LecturaInodos(padre, disco, 'crol1').then( ili_respuesta =>{
        console.log("solo esto me interesa");
        console.log(ili_respuesta);
      
      //console.log(JSON.stringify(ili_respuesta ) );
      
     });
  }
});*/



/*get_hijos_inodo('2-0').then(puta=>{
  console.log("fuck");
  console.log(puta);
});*/

//MOVER_INODO('2-3', '2-0');


//RENOMBRAR_INODO('/etcc2', '2-3');

//CREAR_INODO('hola2','0','2-0').then(r=>{console.log(r)});

//Iniciar_LecturaInodos('2-0', 2, 'crol1');


  //preciosa();
/*REPORTE_MODIFICACION_INODO('','15/11/2019','17/11/2019','/etc');*/

/*REPORTE_CLIENTES('','2019/11/15').then(res=>{
  console.log("Esto se le contestara el servidor: ");
  console.log(res);
});*/


/*Usuario_Login('crol0', 'C1r@f').then((respuesta)=>{

});*/

/*Usuario_singup('maddi2', 'Piki1@', 'madeline','noriega','carlosorantes@gmail',1234,'Femenino','15/9/1994'
,'10/10/2019 07:12:10','5ta av B', '.png').then((ret)=>{
  console.log("la respuesta del signup fue:" + ret.mensaje);
});*/

/*INSERT_INODO('5-0', null, '/', 5, 'maddie2', 777, '0', '').then((resp)=>{
  console.log(resp);
});*/

//conectar();

exports.BUSCAR_USUARIO = BUSCAR_USUARIO;
exports.REPORTE_BITACORA = REPORTE_BITACORA;
exports.REPORTE_SA = REPORTE_SA; // RECUPERA TODOS LOS SISTEMAS DE ARCHIVOS DE TODOS LOS USUARIOS
exports.get_hijos_inodo = get_hijos_inodo;
exports.MOVER_INODO = MOVER_INODO; //esta
exports.RENOMBRAR_INODO = RENOMBRAR_INODO; //esta
exports.CREAR_INODO = CREAR_INODO;  //esta
exports.Iniciar_LecturaInodos = Iniciar_LecturaInodos;
exports.USUARIO_get = USUARIO_get;   // RECUPERA INFORMACION DEL USUARIO POR MEDIO DE SU USERNAME
exports.REPORTE_MODIFICACION_INODO = REPORTE_MODIFICACION_INODO;
exports.REPORTE_CLIENTES = REPORTE_CLIENTES;
exports.REPORTE_NACIMIENTO = REPORTE_NACIMIENTO;
exports.Usuario_Login = Usuario_Login;
exports.PROCEDURE_CONFIRMACION_CUENTA = PROCEDURE_CONFIRMACION_CUENTA;
exports.Usuario_singup          = Usuario_singup;
exports.TRUNCATE_TABLE_TEMPORAL = TRUNCATE_TABLE_TEMPORAL;
exports.COPIAR_BLOTEMPORAL_A_BLOQUE_A = COPIAR_BLOTEMPORAL_A_BLOQUE_A;
exports.COPIAR_TEMPORAL_A_INODO = COPIAR_TEMPORAL_A_INODO;
exports.PARTICION_getCodigo = PARTICION_getCodigo;
exports.DISCO_getCodigo = DISCO_getCodigo;
exports.INSERT_USUARIO = INSERT_USUARIO;
exports.INSERT_BLOQUEA_TEMPORAL = INSERT_BLOQUEA_TEMPORAL;
exports.UPDATE_SYNCRONICE_TEMPORAL = UPDATE_SYNCRONICE_TEMPORAL;
exports.Cerrar_Conexion = Cerrar_Conexion;
exports.Crear_Conexion = Crear_Conexion;
exports.Insertar_SYNCRONICE_TEMPORAL = Insertar_SYNCRONICE_TEMPORAL;
exports.conectar = conectar;
exports.INSERT_Particion = INSERT_Particion;
exports.INSERT_Disco = INSERT_Disco;