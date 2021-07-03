
--===================== TABLA DISCO =======================
CREATE TABLE DISCO(
    codigo   integer not null,
    ruta varchar2(150) not null,
    tamano NUMBER(38), 
    constraint pk_DISCO PRIMARY KEY(codigo),
    constraint u_DISCO  unique(ruta)
);

DROP SEQUENCE seq_DISCO;
 
CREATE SEQUENCE seq_DISCO START WITH 1;

CREATE OR REPLACE TRIGGER AUTOINCREMENTO_DISCO
BEFORE INSERT ON DISCO
FOR EACH ROW
BEGIN
    SELECT seq_DISCO.nextval INTO :NEW.codigo FROM DUAL;
END;


insert into DISCO(ruta, tamano) 
values ('puteee',125829120);


select * from disco;
DROP TABLE DISCO;



/*========================= TABLA PARTICION =============================*/

--                               
CREATE TABLE PARTICION(
    codigo  number(38) not null,
    nombre  varchar2(15) not null,
    tipo    char(1) not null,
    tamano  number(38) not null,
    DISCO_codigo integer not null,
    constraint pk_PARTITION PRIMARY KEY(codigo),
    constraint fk_PARTITION FOREIGN KEY (DISCO_codigo) REFERENCES DISCO(codigo)
);

DROP SEQUENCE seq_PARTICION;
 
CREATE SEQUENCE seq_PARTICION START WITH 1;

CREATE OR REPLACE TRIGGER AUTOINCREMENTO_PARTICION
BEFORE INSERT ON PARTICION
FOR EACH ROW
BEGIN
    SELECT seq_PARTICION.nextval INTO :NEW.codigo FROM DUAL;
END;

insert into PARTICION (nombre,tipo,tamano,DISCO_codigo)
      select 'PRI12','p',1258,1
      from dual
      where not exists(
         select * from PARTICION 
         where nombre='PRI12' AND DISCO_codigo=1
      );


drop table particion;
select * from particion;


/*=========================== TABLA SUPER_BLOQUE============================*/

CREATE TABLE SUPER_BLOQUE(
    codigo                 integer not null,
    total_inodos           integer not null,
    total_bloques          integer not null,
    inodos_disponibles     integer not null,
    bloques_disponibles    integer not null,
    PARTICION_codigo       integer not null,
    CONSTRAINT pk_SUPER_BLOQUE PRIMARY KEY(codigo),
    CONSTRAINT u_SUPER_BLOQUE UNIQUE(PARTICION_codigo)
);

DROP SEQUENCE seq_SUPER_BLOQUE;
 
CREATE SEQUENCE seq_SUPER_BLOQUE START WITH 1;

CREATE OR REPLACE TRIGGER AUTOINCREMENTO_SUPER_BLOQUE
BEFORE INSERT ON SUPER_BLOQUE
FOR EACH ROW
BEGIN
    SELECT seq_SUPER_BLOQUE.nextval INTO :NEW.codigo FROM DUAL;
END;

DROP TABLE SUPER_BLOQUE;



--==============================TABLA TEMPORAL==============================
-- Esta tabla se utilizara al momento de realizar la carga masiva (syncronice)

CREATE TABLE SYNCRONICE_TEMPORAL(
    actual  varchar2(25),
    padre   varchar2(25),
    nombre  varchar2(12),
    particion integer,
    propietario varchar2(15),
    tipo    varchar2(1),
    permiso integer
);

insert into syncronice_temporal(actual, nombre) 
values('3-0', 'mierda');

update syncronice_temporal
SET
    particion = 1,
    propietario='',
    tipo='1',
    permiso =664
WHERE actual = '3-0';

select * from syncronice_temporal;
truncate table syncronice_temporal;
drop table syncronice_temporal;





/*================TABLA DE BLOQUEA_TEMPORAL=======================*/

CREATE TABLE BLOQUEA_TEMPORAL(
    sync_actual varchar2(25),
    contenido   varchar2(64)
);

INSERT INTO BLOQUEA_TEMPORAL(sync_actual, contenido)
values('PRI1-0', 'hola mundo desde c++');

select * from bloquea_temporal;

truncate table bloquea_temporal;






/*================ TABLA TIPO_USUARIO ================*/

CREATE TABLE TIPO_USUARIO(
    codigo  integer not null,
    tipo    varchar2(15) not null,
    descripcion varchar2(100) not null,
    CONSTRAINT pk_TIPO_USUARIO PRIMARY KEY(codigo),
    CONSTRAINT u_TIPO_USUARIO  UNIQUE(tipo)
);

INSERT INTO TIPO_USUARIO (codigo, tipo, descripcion)
values(1,'Administrador','Encargado de dar mantenimiento y agregar funcionalidades al sitio web');
INSERT INTO TIPO_USUARIO (codigo, tipo, descripcion)
values(2,'root','Encargado de dar permisos dentro de todo el sistema de archivos');
INSERT INTO TIPO_USUARIO (codigo, tipo, descripcion)
values(3,'cliente','Posee un espacio de almacenamiento en el sistema');

select * from TIPO_USUARIO;
drop table TIPO_USUARIO;
truncate table TIPO_USUARIO;







/*=================TABLA USUARIO======================*/                  


CREATE TABLE USUARIO(
    id_usuario          int not null,
    nombre              varchar(15),
    apellido            varchar(20),
    username            varchar2(15) not null,
    contrasena          varchar2(9) not null,
    correo              varchar2(100),
    telefono            int,
    fotografia          varchar2(150),
    genero              varchar2(9),
    fecha_nacimiento    date,
    fecha_registro      date not null,
    direccion           varchar2(150),
    estado              integer default 0,
    TIPO_USUARIO_codigo integer not null,
    PARTICION_codigo    integer,
    CONSTRAINT pk_USUARIO primary key(username),
    CONSTRAINT fk_USUARIO foreign key (TIPO_USUARIO_codigo) references TIPO_USUARIO(codigo),
    CONSTRAINT fk2_USUARIO foreign key (PARTICION_codigo) references PARTICION(codigo)
);

DROP SEQUENCE seq_USUARIO;
 
CREATE SEQUENCE seq_USUARIO START WITH 1;

CREATE OR REPLACE TRIGGER AUTOINCREMENTO_USUARIO
BEFORE INSERT ON USUARIO
FOR EACH ROW
BEGIN
    SELECT seq_USUARIO.nextval INTO :NEW.id_usuario FROM DUAL;
END;



INSERT INTO USUARIO(username, contrasena, fecha_registro, TIPO_USUARIO_codigo,PARTICION_codigo)
values('caraj5','fukiu',TO_DATE('24/10/2018 13:35:10','DD/MM/YYYY HH24:MI:SS'),3,null);


select TO_DATE('8/10/2018 8:35:10','DD/MM/YYYY HH24:MI:SS') from dual;
select systimestamp from dual;

SELECT * FROM USUARIO;

DROP TABLE USUARIO;
TRUNCATE TABLE USUARIO;
commit;

CREATE OR REPLACE PROCEDURE CONFIRMACION_CUENTA(usrname in varchar2, nc in varchar2, ca in varchar2)
AS
espera INT := 60; -- en segundos
tiempo DATE;
temp    INT; 
BEGIN

savepoint val;

    update USUARIO
    set contrasena = nc
    ,   estado     = 1 -- Estado de envio de confirmacion
    where username = usrname;
    commit;
    
    select sysdate
    into tiempo from dual;

    LOOP
        exit when tiempo + (espera * (1/86400)) <= SYSDATE;
    END LOOP;
    
    update USUARIO
    set contrasena = ca where username = usrname;
    
    select estado 
    into temp from USUARIO 
    where username = usrname;
    IF temp <> 2 THEN -- implica que el usuario no confirmo al 1(minuto)
        update USUARIO
        set estado = 0 where username = usrname; -- coloca estado en no confirmado
    END IF;
    commit;
rollback;
END;


execute CONFIRMACION_CUENTA('maddie6', 'Piki1!', 'holam');

SELECT id_usuario, nombre, username, correo, PARTICION_codigo, fecha_registro
FROM USUARIO 
WHERE fecha_registro BETWEEN  TO_DATE('15/11/2019 00:00:00','DD/MM/YYYY HH24:MI:SS') AND TO_DATE('15/11/2019 22:59:59','DD/MM/YYYY HH24:MI:SS') -->= TO_DATE('14/10/2019','DD/MM/YYYY')
AND (TIPO_USUARIO_codigo =3 OR TIPO_USUARIO_codigo = 2);







-- ============================= TABLA BITACORA =============================================

CREATE TABLE BITACORA(
    id_bitacora integer not null,
    USUARIO_username     varchar2(15),
    accion      varchar2(150) not null,
    fecha       date default sysdate,
    CONSTRAINT pk_BITACORA primary key(id_bitacora),
    CONSTRAINT fk_BITACORA foreign key (USUARIO_username) references USUARIO(username)
);

DROP SEQUENCE seq_BITACORA;
 
CREATE SEQUENCE seq_BITACORA START WITH 1;

CREATE OR REPLACE TRIGGER AUTOINCREMENTO_BITACORA
BEFORE INSERT ON BITACORA
FOR EACH ROW
BEGIN
    SELECT seq_BITACORA.nextval INTO :NEW.id_bitacora FROM DUAL;
END;

insert into bitacora(USUARIO_username, accion) values('crol1', 'Renombro carpeta');
select * from bitacora;
commit;





-- ============================ TABLA DE INODO ========================================

drop table INODO;

CREATE TABLE INODO (    
    actual           varchar2(25),
    INODO_padre      varchar2(25),
    nombre           varchar2(12),
    PARTICION_codigo integer,
    propietario      varchar2(15),
    tipo             varchar2(1),
    permiso          integer,
    f_modificacion  date default sysdate,
    CONSTRAINT pk_INODO PRIMARY KEY(actual),
    CONSTRAINT fk_INODO FOREIGN key(INODO_padre) references INODO(actual)
);


CREATE OR REPLACE PROCEDURE COPIAR_TEMPORAL_A_INODO
AS
BEGIN
  INSERT INTO INODO(actual,INODO_padre,nombre,PARTICION_codigo,propietario,tipo,permiso)
    SELECT actual,padre,nombre,particion,propietario,tipo,permiso
    FROM syncronice_temporal
    WHERE NOT EXISTS(
        SELECT actual FROM INODO
        WHERE actual=syncronice_temporal.actual 
    );
END ;

EXECUTE COPIAR_TEMPORAL_A_INODO;



select * from INODO;

select * from usuario;
INSERT INTO INODO (actual, INODO_padre, nombre, PARTICION_CODIGO, propietario, tipo, permiso)
values('2-5', '2-3', '/a.txt', 2, 'crol1','1', 664);
commit;

delete from INODO
where actual= '5-0';

select * from inodo 
where inodo_padre is null
AND particion_codigo = 2 AND propietario ='root';

select count(*) as carpeta 
from inodo where propietario='uk' AND tipo='0';

select * from inodo 
where nombre ='/etc' 
AND (f_modificacion BETWEEN TO_DATE('15/11/2019 00:00:00','DD/MM/YYYY HH24:MI:SS') AND TO_DATE('17/11/2019 22:59:59','DD/MM/YYYY HH24:MI:SS') );

select actual, nombre, tipo from inodo where inodo_padre = '2-0';
select propietario  from inodo where actual = '2-2';




-- ======================== TABLA BLOQUE_A =======================
drop table bloque_a;

DROP SEQUENCE seq_BLOQUE_A;

CREATE SEQUENCE seq_BLOQUE_A START WITH 1;

CREATE TABLE BLOQUE_A(
    codigo  integer not null,
    INODO_ACTUAL VARCHAR2(25) not null,
    contenido    VARCHAR2(64) not null,
    CONSTRAINT pk_BLOQUE_A PRIMARY KEY (codigo),
    CONSTRAINT fk_BLOQUE_A FOREIGN KEY (INODO_ACTUAL) REFERENCES INODO(ACTUAL)
);



CREATE OR REPLACE TRIGGER AUTOINCREMENTO_BLOQUE_A
BEFORE INSERT ON BLOQUE_A
FOR EACH ROW
BEGIN
    SELECT seq_BLOQUE_A.nextval INTO :NEW.codigo FROM DUAL;
END;


CREATE OR REPLACE PROCEDURE COPIAR_BLOQUEA_TEMP_A_BLOQUEA
AS
BEGIN
INSERT INTO BLOQUE_A(INODO_actual, contenido)
    SELECT sync_actual, contenido
    FROM BLOQUEA_TEMPORAL
    WHERE NOT EXISTS(
        SELECT * FROM BLOQUE_A
        WHERE INODO_actual LIKE BLOQUEA_TEMPORAL.sync_actual
        AND   contenido LIKE BLOQUEA_TEMPORAL.contenido
    );
END;


select * from bloque_a;
truncate table bloque_a;
EXECUTE COPIAR_BLOQUEA_TEMP_A_BLOQUEA();




