#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <Lista.h>
#include <Structs.h>
#include <Acciones.h>

//METODOS
void Automata_analizador(char *comando);
void restartVariables();
void Ejecutar();
void Reportes(char *name, char *path, char *id, char *ruta);
void crearid(char *path, char *name);
int File_Exists(const char *ruta);



//VARIABLES GLOBALES
listatokens LTokens;
listamontados Montados;


char entrada[400];
int ejecucion = 1;
int estado = 0;
int state=0 ;

int letramontar = 97;
int numeromontar = 48;
int indexabc = 0;
int discoencontrado = 0;
char letraactual[1];
char numeroactual[1];
char num[1];

const char *auxletraactual;
const char *auxnumeroactual;


char path[200];
char pathDisco[200]; //Se utiliza en método de Reportes
char pathFile[200];
char namePartition[50];
char unit[1];
char type[1];
char fit[1];
char del[20];
char name[30];
char id[10];

int add = 0;
int size = 0;


/* Banderas para manejo de parametros opcionales y obligatorios */
int banderaerrorparametro = 0;
int banderap = 0;
int banderasize = 0;
int banderapath = 0;
int banderacreacion = 0;
int banderaname = 0;
int banderaid= 0;
int banderaprimerofdisk = 0;    //Me indica que función viene primero en el comando fdisk (size, delete, add)



int main()
{
    Ejecutar();
    return 0;
}

//Método para analizar comando en consola.
void Automata_analizador(char *comando){

    int state = 0;
    char actual;
    char lectura[200] = {0};
    int i = 0;
    int longitud = strlen(comando);

    for (i= 0; i <= longitud; i++) {
        actual = comando[i];

        switch (state) {

        case 0:

            if(isalpha(actual)){
                strncat(lectura,&actual,1);
                state = 1;
            }else if(actual == '-' ){
                strncat(lectura,&actual,1);
                state = 2;
            }else if(actual == '\"'){
                state = 3;
            }else if(isdigit(actual)){
                strncat(lectura,&actual,1);
                state = 4;
            }else if(actual == '/'){
                strncat(lectura,&actual,1);
                state = 5;
            }else if(actual == '\\'){
                insertarToken(&LTokens,"doblelinea","\\");
                memset(lectura,0,sizeof(lectura));
                state = 7;
            }else if(actual == '#'){
                state = 8;
            }else if((actual == '+')){
                strncat(lectura,&actual,1);
                state = 9;
            }else if(actual == ' ' || actual == '\t' || actual == '\n' || actual == '_' || actual == ':'){
                state = 0;
            }else if(actual == '\0'){
                memset(lectura,0,sizeof(lectura));
                state = 0;
            }else{
                printf("\nERROR: Unknown symbol '%s'.",lectura);
                memset(lectura,0,sizeof(lectura));
                state = 0;
            }
            break;

        case 1:

            if(isalpha(actual)){
                strncat(lectura,&actual,1);
                state = 1;
            }else if(isdigit(actual)){
                strncat(lectura,&actual,1);
                state = 1;
            }else{

                if(strncasecmp(lectura, "mkdisk", strlen("mkdisk")) == 0){
                    insertarToken(&LTokens,"resmkdisk","mkdisk");
                    state=0;
                }else if(strncasecmp(lectura, "rmdisk", strlen("rmdisk")) == 0){
                    insertarToken(&LTokens,"resrmdisk","rmdisk");
                    state=0;
                }else if(strncasecmp(lectura, "fdisk", strlen("fdisk")) == 0){
                    insertarToken(&LTokens,"resfdisk","fdisk");
                    state=0;;
                }else if(strncasecmp(lectura, "mount", strlen("mount")) == 0){
                    insertarToken(&LTokens,"resmount","mount");
                    state=0;
                }else if(strncasecmp(lectura, "umount", strlen("umount")) == 0){
                    insertarToken(&LTokens,"resumount","umount");
                    state=0;
                }else if(strncasecmp(lectura, "rep", strlen("rep")) == 0){
                    insertarToken(&LTokens,"resrep","rep");
                    state=0;
                }else if(strncasecmp(lectura, "exec", strlen("exec")) == 0){
                    insertarToken(&LTokens,"resexec","exec");
                    state=0;
                }else{
                    insertarToken(&LTokens,"cadena",lectura);
                    state=0;
                }
                memset(lectura,0,sizeof(lectura));
            }
            break;

        case 2:
            if(isalpha(actual)){
                strncat(lectura,&actual,1);
                state = 2;
            }else if(isdigit(actual)){
                strncat(lectura,&actual,1);
                state = 4;
            }else{

                if(strncasecmp(lectura, "-size", strlen("-size")) == 0){
                    insertarToken(&LTokens,"ressize","-size");
                    state=0;
                }else if(strncasecmp(lectura, "-path", strlen("-path")) == 0){
                    insertarToken(&LTokens,"respath","-path");
                    state=0;
                }else if(strncasecmp(lectura, "-name", strlen("-name")) == 0){
                    insertarToken(&LTokens,"resname","-name");
                    state=0;
                }else if(strncasecmp(lectura, "-id", strlen("-id")) == 0){
                    insertarToken(&LTokens,"resid","-id");
                    state=0;
                }else{
                    insertarToken(&LTokens,"error",lectura);
                    state=0;
                }
                memset(lectura,0,sizeof(lectura));
            }
            break;

        case 3:

            if((actual == '_') ||(actual == ' ') || isalpha(actual) || isdigit(actual) || (actual == '.')){
                state = 3;
                strncat(lectura,&actual,1);
            }else if((actual == '/') ){
                state = 6;
                strncat(lectura,&actual,1);
            }else  if(actual == '\"'){
                insertarToken(&LTokens,"cadena",lectura);
                memset(lectura,0,sizeof(lectura));
                state=0;
            }
            break;

        case 4:

            if(isdigit(actual)){
                strncat(lectura,&actual,1);
                state = 4;
            }else if(actual == ':'){
                insertarToken(&LTokens,"resid","-id");
                memset(lectura,0,sizeof(lectura));
                state=0;
            }else{
                insertarToken(&LTokens,"numero",lectura);
                memset(lectura,0,sizeof(lectura));
                state=0;
            }
            break;

        case 5:

            if( (actual == '/') || (actual == '.') || isalpha(actual) || isdigit(actual) ){
                strncat(lectura,&actual,1);
                state = 5;
            }else{
                insertarToken(&LTokens,"ruta",lectura);
                memset(lectura,0,sizeof(lectura));
                state=0;
            }
            break;

        case 6:

            if((actual == ' ') || isalpha(actual) || isdigit(actual) || (actual == '/') || (actual == '.')){
                strncat(lectura,&actual,1);
                state = 6;
            }else if(actual == '\"'){
                insertarToken(&LTokens,"ruta",lectura);
                memset(lectura,0,sizeof(lectura));
                state=0;
            }
            break;

        case 7:

            //if(actual == '\n'){
            //    printf("DOBLE LINEA");
                gets(comando);
                Automata_analizador(comando);
            //}
            break;


        case 8:

            if((actual == ' ') || isalpha(actual) || isdigit(actual)){
                state = 8;
                strncat(lectura,&actual,1);
            }else if((actual == '/') || (actual == '.')){
                state = 8;
                strncat(lectura,&actual,1);
            }else if(actual == '\0' || actual == '\n'){
                insertarToken(&LTokens,"comentario",lectura);
                memset(lectura,0,sizeof(lectura));
                state = 0;
            }
            break;

        case 9:
            if(isalpha(actual)){
                strncat(lectura,&actual,1);
                state = 9;
            }else if(isdigit(actual)){
                strncat(lectura,&actual,1);
                state = 4;
            }else{

                if(strncasecmp(lectura, "+unit", strlen("+unit")) == 0){
                    insertarToken(&LTokens,"resunit","+unit");
                    state=0;
                }else if(strncasecmp(lectura, "+type", strlen("+type")) == 0){
                    insertarToken(&LTokens,"restype","+type");
                    state=0;
                }else if(strncasecmp(lectura, "+fit", strlen("+fit")) == 0){
                    insertarToken(&LTokens,"resfit","+fit");
                    state=0;
                }else if(strncasecmp(lectura, "+delete", strlen("+delete")) == 0){
                    insertarToken(&LTokens,"resdelete","+delete");
                    state=0;
                }else if(strncasecmp(lectura, "+add", strlen("+add")) == 0){
                    insertarToken(&LTokens,"resadd","+add");
                    state=0;
                }else if(strncasecmp(lectura, "+ruta", strlen("+ruta")) == 0){
                    insertarToken(&LTokens,"resruta","+ruta");
                    state=0;
                }else{
                    insertarToken(&LTokens,"error",lectura);
                    state=0;
                }
                memset(lectura,0,sizeof(lectura));
            }
            break;

        default:
            break;
        }
    }
    return;
}


//Método para verificar si existe el archivo.
int File_Exists(const char * ruta)
{
    FILE *file = fopen(ruta, "r");
    if (file )
    {
        fclose(file);
        return 1;
    }
    return 0;
}

//Método para regresar variables a valor default.
void restartVariables(){

    memset(path,0,sizeof(path));
    memset(unit,0,sizeof(unit));
    memset(fit,0,sizeof(fit));
    memset(type,0,sizeof(type));
    memset(name,0,sizeof(name));
    memset(del,0,sizeof(del));

    /* Valores default */
    strcpy(type,"P");
    strcpy(unit,"M");
    strcpy(fit,"W");

    size = 0;
    add = 0;

    banderapath = 0;
    banderasize = 0;
    banderaname = 0;
    banderaprimerofdisk = 0;
    banderaid = 0;
    estado = 0;

}



void Reportes(char *name, char *path, char *id, char *ruta){

    memset(pathDisco,0,sizeof(pathDisco));
    memset(namePartition,0,sizeof(namePartition));
    disco *aux = Montados.primero;
    while(aux!=NULL){
        if(strcasecmp(aux->id,id)==0){
            strcpy(pathDisco,aux->path);
            strcpy(namePartition,aux->name);
            break;
        }
        aux = aux->siguiente;
    }

    FILE* disk;
    disk = fopen(pathDisco,"rb+");
    if(disk == NULL){
        printf("\nERROR: Disk %s not mounted.\n", pathDisco);
        return;
    }

    if(strncasecmp(name,"mbr",strlen("mbr")) == 0){
        printf("- MBR Report\n");
        ReporteMBR(path,pathDisco);

    }else if(strncasecmp(name, "disk", strlen("disk")) == 0){
        printf("- Disk Report\n");
        ReporteDisk(path,pathDisco);

    }else{
        printf("\nERROR: This Report does not exist.\n");
        printf("Existing reports: -mbr -disk\n");

    }

}

void crearid(char *path, char *name){

    letraactual[0] = 'a';
    numeroactual[0] = '1';

    FILE* disk;
    disk = fopen(path,"rb+");
    if(disk == NULL){
        printf("\nERROR: No existe disco %s",path);
        return;
    }

    int i_disco = 97;
    int i_part = 49;

    MBR lectura;
    EBR ebr,auxebr;
    fseek(disk,0,SEEK_SET);
    fread(&lectura,sizeof(MBR),1,disk);

    int a=0;
    int encontrado=0;
    int byte_inicio=0;



    for(a = 0; a < 4; a++){
        if(strcasecmp(name,lectura.mbr_particion[a].part_name)==0){
            byte_inicio = lectura.mbr_particion[a].part_start;
            encontrado = 1;
            break;
        }

        if(lectura.mbr_particion[a].part_type == 'E'){

            fseek(disk,lectura.mbr_particion[a].part_start,SEEK_SET);
            fread(&ebr,sizeof(EBR),1,disk);

            while((ebr.part_status == '1')){

                if(strcasecmp(ebr.part_name,name)==0){
                    encontrado = 1;
                    break;
                }

                fseek(disk,ebr.part_next,SEEK_SET);
                fread(&auxebr,sizeof(EBR),1,disk);

                ebr.part_fit = auxebr.part_fit;
                strcpy(ebr.part_name,auxebr.part_name);
                ebr.part_next = auxebr.part_next;
                ebr.part_size = auxebr.part_size;
                ebr.part_start = auxebr.part_start;
                ebr.part_status = auxebr.part_status;
            }
        }
    }

    int verify = verifyPartitionName(path,name);
    if(verify == 0){
        fclose(disk);
        printf("\nERROR: No existe partición %s",name);
        return;
    }

    fclose(disk);



    memset(id,0,sizeof(id));
    int nuevodisco = 1;

    if(Montados.primero != NULL){

        disco *aux = Montados.primero;
        while( aux != NULL )
        {
            if(strcasecmp(aux->path,path)==0){
                i_part = aux->numeromount;
                if(strcasecmp(aux->name,name)==0){
                    printf("\nPartición %s ya se encuentra montada.",name);
                    return;
                }
                i_part = i_part + 1;
                i_disco = aux->letra;
                nuevodisco = 0;
                break;
            }
            i_disco = aux->letra;
            aux = aux->siguiente;
        }

        if(nuevodisco == 1){
            i_disco = i_disco + 1;
            strcat(id,"vd");

            id[2] = i_disco;
            id[3] = i_part;

            montarDisco(&Montados,name,id,path,i_disco,i_part);
            printf("\nPartición %s montada exitosamente.",name);
            return;

        }else if (nuevodisco == 0){

            disco *aux = Montados.primero;
            while( aux != NULL )
            {

                if(strcasecmp(aux->path,path)==0){

                    i_part = aux->numeromount;

                    if(strcasecmp(aux->name,name)==0){
                        printf("\nPartición %s ya se encuentra montada.",name);
                        return;
                    }
                    i_part = i_part + 1;
                }

                aux = aux->siguiente;
            }
            strcat(id,"vd");
            id[2] = i_disco;
            id[3] = i_part;
            montarDisco(&Montados,name,id,path,i_disco,i_part);
            printf("\nPartición %s montada exitosamente.",name);
            return;
        }
    }else{

        strcat(id,"vd");
        id[2] = i_disco;
        id[3] = i_part;

        montarDisco(&Montados,name,id,path,i_disco,i_part);
        printf("\nPartición %s montada exitosamente.",name);
        return;
    }

    disco *auxprint = Montados.primero;
    while(auxprint!=NULL){
        printf("\tId: %s \n",auxprint->id);
        printf("\t\tDisk: %s \n",auxprint->path);
        printf("\t\tName: %s \n",auxprint->name);

        auxprint = auxprint->siguiente;
    }

}




//Método para la ejecución del programa.
void Ejecutar(){

    restartVariables();
    ejecucion = 1;
    while (ejecucion != 0) {

        memset(&LTokens,0,sizeof(LTokens));
        banderaerrorparametro = 0;
        printf("christian@201314041:~$");
        gets(entrada);
        Automata_analizador(entrada);

        int todobien = 1;
        while (todobien == 1){

            token *aux = LTokens.primero;
            while( aux != NULL ){
                switch (estado){

                case 0:

                    if(strncasecmp(aux->tipo, "resmkdisk", strlen("resmkdisk")) == 0){

                        if(aux->siguiente != NULL){
                            strcpy(unit,"m");
                            aux =aux->siguiente;
                            estado = 1;

                        }else{

                            printf("\nERROR: Missing 'mkdisk' parameters.");
                            printf("\n'mkdisk' mandatory parameters: -size -path -name\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resrmdisk", strlen("resrmdisk")) == 0){

                        if(aux->siguiente !=NULL){
                            aux = aux->siguiente;
                            estado = 2;

                        }else{
                            printf("\nERROR: Missing 'rmdisk' parameters.\n");
                            printf("\n'rmdisk' mandatory parameters: -path\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resfdisk", strlen("resfdisk")) == 0){

                        if(aux->siguiente !=NULL){
                            strcpy(unit,"k");
                            aux =aux->siguiente;
                            estado = 3;

                        }else{
                            printf("\nERROR: Missing 'fdisk' parameters.\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resmount", strlen("resmount")) == 0){

                        if(aux->siguiente !=NULL){
                            aux =aux->siguiente;
                            estado = 4;

                        }else{

                            /*Mostrar montados.*/
                            disco *auxprint = Montados.primero;
                            while(auxprint!=NULL){
                                printf("\tId: %s \n",auxprint->id);
                                printf("\t\tDisk: %s \n",auxprint->path);
                                printf("\t\tName: %s \n",auxprint->name);

                                auxprint = auxprint->siguiente;
                            }
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resumount", strlen("resumount")) == 0){

                        if(aux->siguiente !=NULL){
                            aux =aux->siguiente;
                            estado = 5;

                        }else{

                            printf("\nERROR: Missing 'umount' parameters.\n");
                            printf("\n'umount' mandatory parameters: -id\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resrep", strlen("resrep")) == 0){

                        if(aux->siguiente !=NULL){
                            aux =aux->siguiente;
                            estado = 6;

                        }else{

                            printf("\nERROR: Missing 'rep' parameters.\n");
                            printf("\n'rep' mandatory parameters: -path -name -id\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resexec", strlen("resexec")) == 0){
                        if(aux->siguiente != NULL){
                            aux =aux->siguiente;
                            estado = 7;

                        }else{

                            printf("\nERROR: Missing 'exec' parameters.\n");
                            printf("\n'exec' mandatory parameters: -path\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }
                    }else{
                        printf("\nERROR: Command not found.\n");
                        restartVariables();
                        todobien = 0;
                        aux = NULL;
                    }
                    break;


                case 1: //PARAMETROS MKDISK

                    while(aux != NULL){

                        if(strncasecmp(aux->tipo, "ressize", strlen("ressize")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "numero", strlen("numero")) != 0){

                                printf("\nERROR: Not a valid 'size'.\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;

                            }else{

                                size =  strtol(aux->siguiente->valor, NULL, 10);
                                if(size > 0 ){

                                    banderasize = 1;

                                }else{

                                    printf("\nERROR: 'Size' can't be 0 or less.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;

                                    break;
                                }
                            }

                        }else if(strncasecmp(aux->tipo, "respath", strlen("respath")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "ruta", strlen("ruta")) == 0){

                                strcpy(path, aux->siguiente->valor);
                                banderapath = 1;

                            }else{

                                printf("\nERROR: Not a valid path '%s'\n", aux->siguiente->valor);
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;

                            }

                        }else if(strncasecmp(aux->tipo, "resunit", strlen("resunit")) == 0){

                            if(strncasecmp(aux->siguiente->valor, "m", strlen("m")) == 0) {
                                strcpy(unit,"M");
                            }
                            else if(strncasecmp(aux->siguiente->valor, "k", strlen("k")) == 0) {
                                strcpy(unit,"K");

                            }else{
                                printf("\nERROR: Not a valid 'unit'.\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "resname", strlen("resname")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "cadena", strlen("cadena")) == 0){

                                banderaname = 1;
                                strcpy(name,aux->siguiente->valor);

                                int longitudName = strlen(name);
                                printf("%c",name[1]);

                                if((name[longitudName-4] == '.') && (name[longitudName-3] == 'd' || name[longitudName-3] == 'D') && (name[longitudName-2] == 's' || name[longitudName-2] == 'S') && (name[longitudName-1] == 'k' || name[longitudName-1] == 'K')){

                                    //printf("%s","Extension bien");

                                }else{
                                    printf("\nERROR: Not a valid extension, verify that the extension is '.dsk'.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;

                                    break;

                                }

                                //printf("LONGITUD %d",strlen(name));

                            }else{

                                printf("\nERROR: Unknown error un parameter 'name'.\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "error", strlen("error")) == 0){

                            printf("\nERROR: Unknown parameter '%s'\n", aux->valor);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            banderaerrorparametro = 1;
                            break;

                        }

                        aux = aux->siguiente;
                    }

                    if(banderaerrorparametro == 1){
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else if(banderapath == 0){
                        printf("\nERROR: Missing parameter 'path'.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else if(banderasize == 0){

                        printf("\nERROR: Missing parameter 'size'.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else if(banderaname == 0){

                        printf("\nERROR: Missing parameter 'name'.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else{

                        printf("Creating disk %s at %s ...\n", name, path);
                        crearDisco(size,path,name,unit[0]);
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }

                    break;

                case 2: //PARAMETROS RMDISK

                    while(aux != NULL){

                        if(strncasecmp(aux->tipo, "respath", strlen("respath")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "ruta", strlen("ruta")) == 0){

                                memset(path,0,sizeof(path));
                                strcpy(path, aux->siguiente->valor);

                                if(File_Exists(path)==1){
                                    printf("Are you sure you want to remove this disk? [y/n]");

                                    if(fgetc(stdin)=='y'){
                                        remove(path);
                                        printf("\nDisk %s removed.\n", path);
                                    }else if(fgetc(stdin)=='n'){

                                    }else{
                                        printf("\nERROR");
                                    }

                                }else{

                                    printf("\nERROR: Disk does not exist.\n");

                                }

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                break;


                            }else{

                                printf("\nERROR: Not a valid path '%s'\n", aux->siguiente->valor);
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                break;

                            }
                        }else if(strncasecmp(aux->tipo, "error", strlen("error")) == 0){

                            printf("\nERROR: Unknown parameter '%s'\n", aux->valor);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            banderaerrorparametro = 1;
                            break;

                        }
                        aux = aux->siguiente;
                    }


                    break;

                case 3: //PARAMETROS FDISK

                    while(aux != NULL){

                        if(strncasecmp(aux->tipo, "ressize", strlen("ressize")) == 0){

                            if(banderaprimerofdisk == 0){

                                banderaprimerofdisk = 1;

                                if(strncasecmp(aux->siguiente->tipo, "numero", strlen("numero")) != 0){

                                    printf("\nERROR: Not a valid 'size'.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;

                                    break;

                                }else{

                                    size =  strtol(aux->siguiente->valor, NULL, 10);
                                    if(size > 0 ){

                                        banderasize = 1;

                                    }else{

                                        printf("\nERROR: 'Size' can't be 0 or less.\n");
                                        restartVariables();
                                        aux = NULL;
                                        todobien = 0;
                                        banderaerrorparametro = 1;

                                        break;
                                    }
                                }
                            }

                        }else if(strncasecmp(aux->tipo, "resdelete", strlen("resdelete")) == 0){

                            if(banderaprimerofdisk == 0){

                                banderaprimerofdisk = 1;

                                if(strncasecmp(aux->siguiente->valor, "fast", strlen("fast")) == 0) {
                                    strcpy(del,"FAST");
                                }else if(strncasecmp(aux->siguiente->valor, "full", strlen("full")) == 0) {
                                    strcpy(del,"FULL");
                                }else{
                                    printf("\nERROR: Not a valid value for 'delete'.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;
                                    break;
                                }
                            }

                        }else if(strncasecmp(aux->tipo, "resadd", strlen("resadd")) == 0){

                            if(banderaprimerofdisk == 0){

                                banderaprimerofdisk = 1;

                                if(strncasecmp(aux->siguiente->tipo, "numero", strlen("numero")) != 0){

                                    printf("\nERROR: Not a valid value for 'add'.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;
                                    break;

                                }else{

                                    add =  strtol(aux->siguiente->valor, NULL, 10);

                                    if(add == 0){

                                        printf("\nERROR: Add value can't be 0.\n");
                                        restartVariables();
                                        aux = NULL;
                                        todobien = 0;
                                        banderaerrorparametro = 1;

                                        break;

                                    }
                                }
                            }

                        }else if(strncasecmp(aux->tipo, "respath", strlen("respath")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "ruta", strlen("ruta")) == 0){

                                memset(path,0,sizeof(path));
                                strcpy(path, aux->siguiente->valor);

                                if(File_Exists(path)==1){

                                    banderapath = 1;

                                }else{

                                    printf("\nERROR: This disk doesn't exist.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;
                                    break;
                                }
                            }else{

                                printf("\nERROR: Not a valid path '%s'\n", aux->siguiente->valor);
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "resunit", strlen("resunit")) == 0){

                            if(strncasecmp(aux->siguiente->valor, "b", strlen("b")) == 0) {
                                strcpy(unit,"B");
                            }else if(strncasecmp(aux->siguiente->valor, "k", strlen("k")) == 0) {
                                strcpy(unit,"K");
                            }else if(strncasecmp(aux->siguiente->valor, "m", strlen("m")) == 0) {
                                strcpy(unit,"M");
                            }else{

                                //Unidad incorrecta.
                                printf("\nERROR: Unit has to be in -kilobytes[k], -megabytes[m]  or -bytes[b].\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;

                            }

                        }else if(strncasecmp(aux->tipo, "restype", strlen("restype")) == 0){

                            if(strncasecmp(aux->siguiente->valor, "p", strlen("p")) == 0) {
                                strcpy(type,"P");
                            }else if(strncasecmp(aux->siguiente->valor, "e", strlen("e")) == 0) {
                                strcpy(type,"E");
                            }else if(strncasecmp(aux->siguiente->valor, "l", strlen("l")) == 0) {
                                strcpy(type,"L");
                            }else{
                                printf("\nERROR: %s is not a valid type .\n",aux->siguiente->valor);

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "resfit", strlen("resfit")) == 0){

                            if((strncasecmp(aux->siguiente->valor, "FF", strlen("FF")) == 0) || (strncasecmp(aux->siguiente->valor, "F", strlen("F")) == 0)) {
                                strcpy(fit,"F");
                            }else if((strncasecmp(aux->siguiente->valor, "WF", strlen("WF")) == 0) || (strncasecmp(aux->siguiente->valor, "W", strlen("W")) == 0)) {
                                strcpy(fit,"W");
                            }else if((strncasecmp(aux->siguiente->valor, "BF", strlen("BF")) == 0) || (strncasecmp(aux->siguiente->valor, "B", strlen("B")) == 0)) {
                                strcpy(fit,"B");
                            }else{
                                printf("\nERROR: Not a valid 'Fit'.\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "resname", strlen("resname")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "cadena", strlen("cadena")) == 0){

                                banderaname = 1;
                                strcpy(name,aux->siguiente->valor);

                            }else{

                                printf("\nERROR: Please check 'name' format.\n");

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "error", strlen("error")) == 0){

                            printf("\nERROR: Unknown parameter '%s'\n", aux->valor);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            banderaerrorparametro = 1;

                            break;


                        }
                        aux = aux->siguiente;
                    }

                    if(banderaerrorparametro == 1){

                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;
                    }else if(banderapath == 0){
                        printf("\nERROR: Missing 'path' parameter.\n");

                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else if(banderaname == 0){

                        printf("\nERROR: Missing 'name' parameter.\n");

                        restartVariables();
                        aux = NULL;
                        todobien = 0;

                        break;

                    }else{

                        if(size != 0){
                            //CREAR PARTICION

                            printf("Creating partition in disk %s ...\n",path);
                            Particionar(size, unit[0], fit[0], name, path, type[0]);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;

                            break;


                        }else if(add != 0){



                            printf("Changing capacity...\n");
                            printf("Add %d\n",add);
                            if(add < 0 ){
                                printf("Removing space...\n");
                            }
                                else if(add > 0){
                                printf("Adding space...n");

                            }

                            // ACCION DE ADD AQUI

                        }else if(del != NULL){

                            if(strncasecmp(del, "full", strlen("full")) == 0) {
                                printf("Full delete.\n");
                            }
                            else if(strncasecmp(del, "fast", strlen("fast")) == 0) {
                                printf("Fast delete.\n");

                            }

                           // ACCION DE DELETE AQUI
                        }
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                    }
                    break;

                case 4: //PARAMETROS MOUNT

                    while(aux != NULL){
                        if(strncasecmp(aux->tipo, "resname", strlen("resname")) == 0){
                            if(strncasecmp(aux->siguiente->tipo,"cadena",strlen("cadena")) == 0){
                                banderaname = 1;
                                strcpy(name,aux->siguiente->valor);
                            }else{
                                printf("\nERROR: Please check 'name' format.\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }
                        }else if(strncasecmp(aux->tipo, "respath", strlen("respath")) == 0){

                            if(strncasecmp(aux->siguiente->tipo,"ruta",strlen("ruta")) == 0){
                                strcpy(path, aux->siguiente->valor);
                                banderapath = 1;
                            }else{
                                printf("\nERROR: Not a valid path '%s'\n", aux->siguiente->valor);
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }
                        }else if((strncasecmp(aux->tipo, "error", strlen("error")) == 0)){
                            printf("\nERROR: Unknown parameter '%s'\n", aux->valor);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            banderaerrorparametro = 1;
                            break;
                        }
                        aux = aux->siguiente;
                    }

                    if(banderaerrorparametro == 1){
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else if(banderapath == 0){
                        printf("\nERROR: Missing 'path' parameter.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else if(banderaname == 0){

                        printf("\nERROR: Missing 'name' parameter.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else{

                        crearid(path, name);

                        disco *auxprint = Montados.primero;
                        while(auxprint!=NULL){
                            printf("\tId: %s \n",auxprint->id);
                            printf("\t\tDisk: %s \n",auxprint->path);
                            printf("\t\tName: %s \n",auxprint->name);

                            auxprint = auxprint->siguiente;
                        }

                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                    }
                    break;

                case 5: // PARAMETROS UMOUNT


                    while(aux != NULL){

                        if(strncasecmp(aux->tipo,"resid",strlen("resid"))== 0 ){

                            if(strncasecmp(aux->siguiente->tipo,"cadena", strlen("cadena"))==0){

                                banderaid = 1;
                                strcpy(id,aux->siguiente->valor);

                                if(banderaerrorparametro == 1){
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    break;
                                }else if(banderaid == 0){
                                    printf("\nERROR: Missing 'id' parameter.\n");

                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    break;

                                }else{

                                    if(Montados.primero == NULL){

                                        printf("\nERROR: There are no mounted partitions.\n");

                                        restartVariables();
                                        aux = NULL;
                                        todobien = 0;
                                        break;

                                    }else{

                                        int banderaexiste = 0;

                                        disco *temp = Montados.primero;
                                        while (temp != NULL){

                                            if(strcasecmp(temp->id,id)==0){

                                                desmontardisco(&Montados,id);
                                                printf("\nPartition %s unmount.\n",id);

                                                restartVariables();
                                                aux = NULL;
                                                todobien = 0;
                                                banderaexiste = 1;

                                                disco *auxprint = Montados.primero;
                                                while(auxprint!=NULL){
                                                    printf("\tId: %s \n",auxprint->id);
                                                    printf("\t\tDisk: %s \n",auxprint->path);
                                                    printf("\t\tName: %s \n",auxprint->name);
                                                    auxprint = auxprint->siguiente;
                                                }

                                                break;
                                            }

                                            temp = temp->siguiente;
                                        }
                                        if(banderaexiste == 0){
                                            printf("\nERROR: This id doesn't exist %s.\n", id);

                                            restartVariables();
                                            aux = NULL;
                                            todobien = 0;
                                            break;
                                        }
                                    }
                                }


                            }else{

                                printf("\nERROR: Incorrect id.\n");

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;

                            }

                        }else if(strncasecmp(aux->tipo,"error", strlen("error"))==0){

                            printf("\nERROR: Unknown parameter '%s'\n", aux->valor);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            banderaerrorparametro = 1;
                            break;


                        }

                        aux = aux->siguiente;
                    }



                    break;

                case 6: // PARAMETROS REP

                    while(aux != NULL){

                        if((strncasecmp(aux->tipo, "respath", strlen("respath")) == 0)){

                            if(strncasecmp(aux->siguiente->tipo,"ruta",strlen("ruta"))==0){

                                memset(path,0,sizeof(path));
                                strcpy(path, aux->siguiente->valor);

                                if(File_Exists(replace_str(path,".png",".dot"))==1){

                                    banderapath = 1;

                                }else{

                                    char comando[600] = {0};

                                    /* Creacion de directorio y archivo para reportes. */
                                    strcat(comando, "mkdir -p ");
                                    strcat(comando, pathReplace(replace_str(path,".png",".dot")));// Cambio .png por .dot para escribir el .dot.
                                    system(comando);

                                    memset(comando,0,sizeof(comando));
                                    strcat(comando, "rmdir ");
                                    strcat(comando, pathReplace(replace_str(path,".png",".dot")));// Cambio .png por .dot para escribir el .dot.
                                    system(comando);

                                    memset(comando,0,sizeof(comando));
                                    strcat(comando, "touch ");
                                    strcat(comando, pathReplace(replace_str(path,".png",".dot")));// Cambio .png por .dot para escribir el .dot.
                                    system(comando);

                                    banderapath = 1;

                                }

                            }else{

                                printf("\nERROR: Not a valid path '%s'\n", aux->siguiente->valor);

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;

                            }


                        }else  if(strncasecmp(aux->tipo,"resname",strlen("resname"))==0){

                            if(strncasecmp(aux->siguiente->tipo,"cadena",strlen("cadena"))==0){

                                banderaname = 1;
                                strcpy(name,aux->siguiente->valor);

                            }else{

                                printf("\nERROR: Please check 'name' parameter.\n");

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }

                        }else if((strncasecmp(aux->tipo, "resid", strlen("resid")) == 0)){

                            if(strncasecmp(aux->siguiente->tipo,"cadena",strlen("cadena"))==0){

                                banderaid = 1;
                                strcpy(id,aux->siguiente->valor);

                            }else{

                                printf("\nERROR: Incorrect id.\n");

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;
                                break;
                            }
                        }
                        aux = aux->siguiente;
                    }

                    if(banderaerrorparametro == 1){
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else if(banderapath == 0){
                        printf("\nERROR: Missing 'path' parameter.\n");

                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else if(banderaname == 0){

                        printf("\nERROR: Missing 'name' parameter.\n");

                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else if(banderaid == 0){

                        printf("\nERROR: Missing 'id' parameter.\n");

                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        break;

                    }else{

                        Reportes(name,path,id,pathFile);
                        restartVariables();
                        aux = NULL;
                        todobien = 0;

                    }

                    break;


                default:
                    break;
                }
            }

            memset(entrada,0,sizeof(entrada));
            memset(&LTokens,0,sizeof(LTokens));
            LTokens.primero = NULL;
            LTokens.ultimo = NULL;
            todobien = 0;

        }
    }
}





