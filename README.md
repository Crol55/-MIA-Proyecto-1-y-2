# Manejo e imp de archivos 
## proyecto 1 - (Implementacion de discos duros virtuales e implementacion de un sistema de archivos tipo linux ( inodos-EXT2 y EXT3) )
El proyecto se dividio en 2 fases. 
    * El framework e ide utlizado fue QT-creator.
    * Se utilizo el lenguaje c/c++ enfatizando las estructuras de datos. 
    * Una aplicacion de comandos (terminal).
    * Manejo de discos duros virtuales.
    * Para reconocer los comandos se creó un analizador lexico y sintactico desde cero.

### FASE 1 
El enunciado de la primera fase se encuentra en este repositorio bajo el nombre "[MIA]Proyecto1_Fase1.pdf".
Entre todo lo implementado lo mas importante fue: 
1. Un RAID1 - Mirroring. 
2. Comandos implementados (Ver los parametros de cada comando en el enunciado): 
* MKDISK
* RMDISK
* FDISK 
* MOUNT 
* UNMOUNT
* REP
3. Reportes, se crearon reportes para ver el estado de los discos 
    * Reportes de MBR y EBR
    * Reporte del estado del disco virtual (particiones, espacio libre, etc).

### FASE 2
El enunciado de la segunda fase se encuentra en este repositorio bajo el nombre "[MIA]Proyecto1_Fase2.pdf".
Entre todo lo implementado lo mas importante fue: 

1. Comandos implementados para la administracion de los discos duros: 
* MKFS
* LOGIN 
* LOGOUT 
* MKGRP
* RMGRP 
* MKUSR
* RMUSR 

2. Comandos implementados para ADMINISTRACION de carpetas archivos y permisos: 
* CHMOD 
* MKFILE 
* CAT 
* REM 
* MKDIR 
* CP 
* MV 
* CHOWN 
* PAUSE 
3. Recuperacion del sistema de archivos. (comando) 
* RECOVERY 
    
4. Simulacion de perdida de informacion. (Comando
* LOSS
5. Reportes 
    * Reporte de super bloque. 
    * Reporte de tabla de inodos. 
    * Reporte de bloques de carpetas. 
    * Reporte de bloques de archivos.
    * Reporte de bloque de Apuntadores. 
    * Reporte de journaling.
    * Reporte de tree. 
    * reporte de BITMAP_inode. 
 

