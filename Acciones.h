#ifndef ACCIONES_H
#define ACCIONES_H

#include <Structs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *pathReplace(char *originalpath);
void crearDisco(int size, char *path, char *name, char unit);
char *unirPathName(char *path, char *name);


char *pathReplace(char* originalpath){

    char newpath[150];
    strcpy(newpath,"'");
    strcat(newpath,originalpath);
    strcat(newpath,"'");
    return newpath;
}


char *unirPathName(char* originalpath, char* name){

    char newpath[150];
    strcpy(newpath,"'");
    strcat(newpath,originalpath);
    strcat(newpath,"/");
    strcat(newpath,name);
    strcat(newpath,"'");
    return newpath;
}

/* Inicio REPORTES */

/* Fin REPORTES */



/* ACCIONES DE LOS COMANDOS */

void crearDisco(int size, char* path, char* name, char unit){

    char comando[600];
    strcpy (comando, "");
    strcat(comando, "mkdir -p ");
    strcat(comando, pathReplace(path));
    printf("%s\n", comando);
    system(comando);

    int disksize;

    if (unit=='m' || unit =='M'){
        disksize = size*1024;
    }else if(unit=='k' || unit =='K'){
        disksize = size;
    }

    char bsdev[50];
    strcpy(bsdev,"");

    strcpy(comando,"");
    strcat(comando, "dd if=/dev/zero of=");
    strcat(comando, unirPathName(path,name));
    strcat(comando, " bs=");
    snprintf(bsdev, sizeof(bsdev) ,"%d",disksize);
    strcat(comando,bsdev);
    strcat(comando, " count=1024");
    printf("%s\n",comando);
    system(comando);


    MBR initmbr;

    /* Tamaño MBR*/
    if(unit == 'm' || unit == 'M'){
        initmbr.mbr_tamanio = size*1048576;
    }
    else if(unit == 'k' || unit == 'K'){
        initmbr.mbr_tamanio = size*1024;
    }

    /* Fecha creacion MBR */
    initmbr.mbr_fecha_creacion = time (NULL);

    /* Particiones en el MBR */
    initmbr.mbr_disk_signature = (int)time(NULL);
    int a = 0;
    for(a = 0; a < 4; a++){

        initmbr.mbr_particion[a].part_status ='0';
        initmbr.mbr_particion[a].part_type = '\0';
        initmbr.mbr_particion[a].part_fit = '\0';
        initmbr.mbr_particion[a].part_start = 0;
        initmbr.mbr_particion[a].part_size = 0;
        strcpy(initmbr.mbr_particion[a].part_name,"");

    }

    char ruta[150] = {0};
    strcat(ruta,path);
    strcat(ruta,"/");
    strcat(ruta,name);

    FILE *disk;
    disk = fopen(ruta,"rb+");
    if(disk == NULL){
        printf("%s", "UNKNOWN ERROR WHILE CREATING DISK.");
        return;
    }
    fseek(disk,0,SEEK_SET);
    fwrite(&initmbr,sizeof(MBR),1,disk);
    fclose(disk);
    system("clear");

}




#endif // ACCIONES_H
