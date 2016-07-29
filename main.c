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



//VARIABLES GLOBALES
listatokens LTokens;


char entrada[300];
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


char path[150];
char pathDisco[150]; //Se utiliza en método de Reportes
char namePartition[50];
char unit[1];
char type[1];
char fit[1];
char del[20];
char name[30];
char id[10];

int add = 0;
int size = 0;



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
            }else if(actual == ':'){
                //insertarToken(&LTokens,"dospuntos",":");
            }else if(actual == ' ' || actual == '\t' || actual == '\n' || actual == '_'){
                state = 0;
            }else if(actual == '\0'){
                memset(lectura,0,sizeof(lectura));
                state = 0;
            }else{
                printf("\nERROR: Símbolo desconocido '%s'.",lectura);
                memset(lectura,0,sizeof(lectura));
                state = 0;
            }
            break;

        case 1:

            if(isalpha(actual)){
                strncat(lectura,&actual,1);
                state = 1;
            }else{

                if(strncasecmp(lectura, "mkdisk", strlen("mkdisk")) == 0){
                    insertarToken(&LTokens,"resmkdisk","mkdisk");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "rmdisk", strlen("rmdisk")) == 0){
                    insertarToken(&LTokens,"resrmdisk","rmdisk");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "fdisk", strlen("fdisk")) == 0){
                    insertarToken(&LTokens,"resfdisk","fdisk");
                    memset(lectura,0,sizeof(lectura));
                    state=0;;
                }else if(strncasecmp(lectura, "mount", strlen("mount")) == 0){
                    insertarToken(&LTokens,"resmount","mount");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "unmount", strlen("unmount")) == 0){
                    insertarToken(&LTokens,"resunmount","unmount");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "rep", strlen("rep")) == 0){
                    insertarToken(&LTokens,"resrep","rep");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "exec", strlen("exec")) == 0){
                    insertarToken(&LTokens,"resexec","exec");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else{
                    insertarToken(&LTokens,"cadena",lectura);
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }
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
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "-path", strlen("-path")) == 0){
                    insertarToken(&LTokens,"respath","-path");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "-name", strlen("-name")) == 0){
                    insertarToken(&LTokens,"resname","-name");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "-id", strlen("-id")) == 0){
                    insertarToken(&LTokens,"resid","-id");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else{
                    insertarToken(&LTokens,"error",lectura);
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }
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

            if(actual == '\n'){
                printf("DOBLE LINEA");
                gets(comando);
                Automata_analizador(comando);
            }
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
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "+type", strlen("+type")) == 0){
                    insertarToken(&LTokens,"restype","+type");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "+fit", strlen("+fit")) == 0){
                    insertarToken(&LTokens,"resfit","+fit");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "+delete", strlen("+delete")) == 0){
                    insertarToken(&LTokens,"resdelete","+delete");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "+add", strlen("+add")) == 0){
                    insertarToken(&LTokens,"resadd","+add");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else if(strncasecmp(lectura, "+ruta", strlen("+ruta")) == 0){
                    insertarToken(&LTokens,"resruta","+ruta");
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }else{
                    insertarToken(&LTokens,"error",lectura);
                    memset(lectura,0,sizeof(lectura));
                    state=0;
                }
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

    size = 0;
    memset(path,0,sizeof(path));
    memset(unit,0,sizeof(unit));
    strcpy(unit,"M");

    memset(type,0,sizeof(type));
    strcpy(type,"P");
    memset(fit,0,sizeof(fit));
    strcpy(fit,"W");
    memset(name,0,sizeof(name));
    memset(del,0,sizeof(del));
    add = 0;
    memset(id,0,sizeof(id));

    banderapath = 0;
    banderasize = 0;
    banderaname = 0;
    banderaprimerofdisk = 0;
    banderaid = 0;
    estado = 0;

}

//Método para la ejecución del programa.
void Ejecutar(){

    ejecucion = 1;
    while (ejecucion != 0) {

        memset(&LTokens,0,sizeof(LTokens));
        banderaerrorparametro = 0;
        printf("christian@201314041:~$");
        gets(entrada);
        Automata_analizador(entrada);

        token *tempprint = LTokens.primero;
            while (tempprint != NULL){
            if(strncasecmp(tempprint->tipo, "doblelinea", strlen("doblelinea")) == 0){
                gets(entrada);
                Automata_analizador(entrada);
                break;
            }
            tempprint = tempprint->siguiente;
        }

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

                            printf("\nERROR: Faltan parametros de 'mkdisk'.");
                            printf("\nParametros obligatorios de 'mkdisk': -size -path -name\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resrmdisk", strlen("resrmdisk")) == 0){

                        if(aux->siguiente !=NULL){
                            aux =aux->siguiente;
                            estado = 2;

                        }else{
                            printf("\nERROR: Faltan parametros de 'rmdisk'.\n");
                            printf("\nParametros obligatorios de 'rmdisk': -path\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resfdisk", strlen("resfdisk")) == 0){
                        strcpy(unit,"k");
                        if(aux->siguiente !=NULL){
                            aux =aux->siguiente;
                            estado = 3;

                        }else{
                            printf("\nERROR: Faltan parametros de 'fdisk'.\n");
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

                            printf("\nERROR: Faltan parametros de 'mount'.\n");
                            printf("\nParametros obligatorios de 'mkdisk': -path -name\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }

                    }else if(strncasecmp(aux->tipo, "resunmount", strlen("resunmount")) == 0){

                        if(aux->siguiente !=NULL){
                            aux =aux->siguiente;
                            estado = 5;

                        }else{

                            printf("\nERROR: Faltan parametros de 'unmount'.\n");
                            printf("\nParametros obligatorios de 'mkdisk': -id\n");
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

                            printf("\nERROR: Faltan parametros de 'rep'.\n");
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

                            printf("\nERROR: Faltan parametros de 'exec'.\n");
                            printf("----------------------------------------------------------------------------\n");
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            break;
                        }
                    }else{
                        printf("\nERROR: Este comando no existe.\n");
                        restartVariables();
                        todobien = 0;
                        aux = NULL;
                    }
                    break;


                case 1: //PARAMETROS MKDISK

                    while(aux != NULL){

                        if(strncasecmp(aux->tipo, "ressize", strlen("ressize")) == 0){

                            if(strncasecmp(aux->siguiente->tipo, "numero", strlen("numero")) != 0){

                                printf("\nERROR: Valor de 'size' no válido.\n");
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

                                    //Tamaño invalido.
                                    printf("\nERROR: Valor de 'size' no puede ser menor a 0.\n");
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

                                printf("\nERROR: Ruta no valida '%s'\n", aux->siguiente->valor);
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;

                            }

                        }else if(strncasecmp(aux->tipo, "resunit", strlen("resunit")) == 0){

                            if(strncasecmp(aux->siguiente->valor, "m", strlen("m")) == 0) {

                                printf("Unit is in megabytes.\n");
                                strcpy(unit,"M");
                            }
                            else if(strncasecmp(aux->siguiente->valor, "k", strlen("k")) == 0) {
                                printf("Unit is in kilobytes.\n");
                                strcpy(unit,"K");

                            }else{

                                //Unidad incorrecta.
                                printf("\nERROR: Valor para 'unit' no valido.\n");
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



                                if((name[longitudName-3] == 'd' || name[longitudName-3] == 'D') && (name[longitudName-2] == 's' || name[longitudName-2] == 'S') && (name[longitudName-1] == 'k' || name[longitudName-1] == 'K')){

                                    //printf("%s","Extension bien");

                                }else{
                                    printf("\nERROR: Extensión no válida, verificar que sea extensión '.dsk'.\n");
                                    restartVariables();
                                    aux = NULL;
                                    todobien = 0;
                                    banderaerrorparametro = 1;

                                    break;

                                }

                                //printf("LONGITUD %d",strlen(name));

                            }else{

                                printf("\nERROR: Formato de nombre incorrecto..\n");
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                banderaerrorparametro = 1;

                                break;
                            }

                        }else if(strncasecmp(aux->tipo, "error", strlen("error")) == 0){

                            printf("\nERROR: Parametro no valido. '%s'\n", aux->valor);
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
                        printf("\nERROR: Falta parametro 'path'.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else if(banderasize == 0){

                        printf("\nERROR: Falta parametro 'size'.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else if(banderaname == 0){

                        printf("\nERROR: Falta parametro 'name'.\n");
                        restartVariables();
                        aux = NULL;
                        todobien = 0;
                        banderaerrorparametro = 0;
                        break;

                    }else{

                        printf("Creating disk %s at %s ...\n", name, path);

                        crearDisco(size,path,name,unit[0]);

                        printf("Disk '%s' of size %d%c created.\n", name, size, unit[0]);
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
                                    printf("Si existe disco.");
                                    printf("Desea eliminar este disco? [y/n]");

                                    if(fgetc(stdin)=='y'){
                                        remove(path);
                                        printf("\nDisk %s removed.\n", path);
                                    }else if(fgetc(stdin)=='n'){
                                        //No se elimina el disco
                                    }else{
                                        printf("\nERROR");
                                    }

                                }else{

                                    printf("\nERROR: No existe este disco.\n");

                                }

                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                break;


                            }else{

                                printf("\nERROR: Ruta no valida '%s'\n", aux->siguiente->valor);
                                restartVariables();
                                aux = NULL;
                                todobien = 0;
                                break;

                            }
                        }else if(strncasecmp(aux->tipo, "error", strlen("error")) == 0){

                            printf("\nERROR: Parametro no valido. '%s'\n", aux->valor);
                            restartVariables();
                            aux = NULL;
                            todobien = 0;
                            banderaerrorparametro = 1;
                            break;

                        }
                        aux = aux->siguiente;
                    }


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



