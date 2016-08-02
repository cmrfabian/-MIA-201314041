#ifndef ACCIONES_H
#define ACCIONES_H

#include <Structs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ESTE HEADER INCLUYE METODOS PARA LOS REPOTES y METODOS PARA LAS ACCIONES DE LOS COMMANDOS */

char *pathReplace(char *originalpath);
void crearDisco(int size, char *path, char *name, char unit);
char *unirPathName(char *path, char *name);

void Particionar(int size, char unit, char fit, char* name, char* path, char type);
int verifyPartitionName(char *path, char* name);
void ReporteMBR(char* dir , char* pathDisco);
void ReporteDisk(char *dir, char *pathDisco);

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

char *pathReplaceImagen(char* originalpath){

    char newpath[100];
    strcpy(newpath,"\"");
    strcat(newpath,originalpath);
    strcat(newpath,"\"");
    return newpath;
}


char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}
void ReporteMBR(char* dir , char* pathDisco){

    printf("Reporte MBR\n");
    char *dotCommand;
    char rep[400];
    char *shot;
    shot=malloc(2000);
    dotCommand=malloc(2000);
    char label[20000];
    char graphviz[20000];

    memset(label,0,sizeof(label));
    memset(graphviz,0,sizeof(graphviz));

    //char* ruta = "";

    MBR tempMBR;
    EBR ebr,auxebr;
    struct tm *loctime;

    if(pathDisco!=NULL){

        printf("RUTA=%s\n",dir);
        FILE *leer = fopen(pathDisco,"rb+");
        fseek(leer,0,SEEK_SET);
        fread(&tempMBR,sizeof(MBR),1,leer);
        loctime = localtime (&tempMBR.mbr_fecha_creacion);
        printf("\tFecha de Creación MBR: %s \n", asctime (loctime));


        strcpy(rep,replace_str(dir,".png",".dot"));

        FILE *reporte=fopen(rep,"w+");
        fprintf(reporte,"digraph test { \n");
        fprintf(reporte,"graph [ratio=fill]; \n");
        fprintf(reporte,"node [label=\"N\", shape=plaintext, fontsize=12, fontname=\"Ubuntu\", style=filled ,fillcolor=grey88, height = 0.7]; \n");
        fprintf(reporte,"arset [label=< \n <TABLE ALIGN=\"LEFT\">");

        sprintf(label,"%s<TR>\n\t<TD align=\"left\">%s</TD> \n\t <TD align=\"left\">%i</TD> \n</TR> ",label," mbr_tamanio: ",tempMBR.mbr_tamanio);
        sprintf(label,"%s<TR>\n\t<TD align=\"left\">%s</TD> \n\t <TD align=\"left\">%s</TD> \n</TR> ",label," mbr_fecha_creacion: ",asctime (loctime));
        sprintf(label,"%s<TR>\n\t<TD align=\"left\">%s</TD> \n\t <TD align=\"left\">%i</TD> \n</TR> ",label," mbr_disk_signature: ",tempMBR.mbr_disk_signature);

        int i=0;

        for(i = 0; i < 4 ; i++){
            if(tempMBR.mbr_particion[i].part_status == '1'){
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\" colspan=\"2\"><b>Particion[%d]</b></TD></TR>",label,i);
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\">%s%i%s</TD>\n\t <TD align=\"left\">%c</TD> \n</TR>",label,"  part_status_",i,": ",tempMBR.mbr_particion[i].part_status);
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\">%s%i%s</TD>\n\t <TD align=\"left\">%c</TD> \n</TR>",label,"  part_type_",i,": ",tempMBR.mbr_particion[i].part_type);
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\">%s%i%s</TD>\n\t <TD align=\"left\">%c</TD> \n</TR>",label,"  part_fit_",i,": ",tempMBR.mbr_particion[i].part_fit);
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\">%s%i%s</TD>\n\t <TD align=\"left\">%d</TD> \n</TR>",label,"  part_start_",i,": ",tempMBR.mbr_particion[i].part_start);
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\">%s%i%s</TD>\n\t <TD align=\"left\">%d</TD> \n</TR>",label,"  part_size_",i,": ",tempMBR.mbr_particion[i].part_size);
                sprintf(label,"%s\n <TR>\n\t <TD align=\"left\">%s%i%s</TD>\n\t <TD align=\"left\">%s</TD> \n</TR>",label,"  part_name_",i,": ",tempMBR.mbr_particion[i].part_name);

            }
            if(tempMBR.mbr_particion[i].part_type == 'E'){

                fseek(leer,tempMBR.mbr_particion[i].part_start,SEEK_SET);
                fread(&ebr,sizeof(EBR),1,leer);
                int indebr = 1;

                while((ebr.part_next != -1)){

                    if(ebr.part_status == '1'){
                        sprintf(label,"%s\n <TR>\n\t <TD colspan=\"2\"><b>EBR_%d</b></TD></TR>",label,indebr);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%c</TD> \n</TR>",label,"  \tpart_status: ",ebr.part_status);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%c</TD> \n</TR>",label,"  \tpart_fit: ",ebr.part_fit);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_start: ",ebr.part_start);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_size: ",ebr.part_size);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_next: ",ebr.part_next);
                        //sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_previous: ",ebr.part_previous);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%s</TD> \n</TR>",label,"  \tpart_name: ",ebr.part_name);

                    }

                    fseek(leer,ebr.part_next,SEEK_SET);
                    fread(&auxebr,sizeof(EBR),1,leer);
                    ebr.part_fit = auxebr.part_fit;
                    strcpy(ebr.part_name,auxebr.part_name);
                    ebr.part_next = auxebr.part_next;
                    //ebr.part_previous = auxebr.part_previous;
                    ebr.part_size = auxebr.part_size;
                    ebr.part_start = auxebr.part_start;
                    ebr.part_status = auxebr.part_status;
                    indebr++;

                }
            }
         }

        sprintf(graphviz,"%s%s",graphviz,label);
        sprintf(graphviz,"%s%s",graphviz,"</TABLE> \n >, ]; \n }");
        fprintf(reporte,"%s",graphviz);
        fclose(reporte);
        fclose(leer);

    }
    strcpy(dotCommand,"dot -Tpng ");
    strcat(dotCommand,pathReplaceImagen(replace_str(dir,".png",".dot")));
    strcat(dotCommand," -o ");
    strcat(dotCommand,pathReplaceImagen(dir));
    system(dotCommand);

    strcpy(shot, "shotwell ");
    strcat(shot, pathReplaceImagen(dir));
    strcat(shot, " &");
    system(shot);
}

void ReporteDisk(char* dir,char* pathDisco){

    char rep[150];
    char *dotCommand;
    dotCommand = malloc(200);
    char *shot;
    shot = malloc(200);
    char label[20000];
    char graphviz[20000];
    int posicionextendida = 0;
    int existeextendida = 0;


    memset(label,0,sizeof(label));
    memset(graphviz,0,sizeof(graphviz));
    int espacioentreparticiones = 0;

    FILE* disco = fopen(pathDisco,"rb+");
    fseek(disco,0,SEEK_SET);

    MBR temporal;
    EBR ebr,auxebr, tempebr;
    fread(&temporal,sizeof(MBR),1,disco);
    int espacioEntreLogicas = 0;

    strcpy(rep,replace_str(dir,".png",".dot"));

    FILE *reporte=fopen(rep,"w+");

    fprintf(reporte, "digraph ReporteDisk {");
    fprintf(reporte, "\t\ndisco [shape=none, margin=0, label=<");
    fprintf(reporte, "\t\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">");

    sprintf(label,"%s%s",label,"\t\t\n<TR>\n\t\t\t<TD ROWSPAN=\"2\">MBR</TD>\n");

    int i = 0;
    for(i = 0; i < 4 ; i++){

        if(i == 0 && temporal.mbr_particion[i].part_status == '0'){
            sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            break;
        }
        else if(i == 0 && temporal.mbr_particion[i].part_status == '1'){

            espacioentreparticiones = temporal.mbr_particion[i].part_start - sizeof(MBR);

            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            }
            if(temporal.mbr_particion[i].part_type == 'P' || temporal.mbr_particion[i].part_type == 'p'){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Primaria</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }else if(temporal.mbr_particion[i].part_type == 'E' || temporal.mbr_particion[i].part_type == 'e'){
                posicionextendida = i;
                existeextendida = 1;
                sprintf(label,"%s%s",label,"\t\t\t<TD COLSPAN=\"40\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Extendida</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }

        }else if(i == 1 && temporal.mbr_particion[i].part_status == '0'){
            sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            break;
        }else if(i == 1 && temporal.mbr_particion[i].part_status == '1'){

            espacioentreparticiones = temporal.mbr_particion[i].part_start - (temporal.mbr_particion[i-1].part_start+temporal.mbr_particion[i-1].part_size);

            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            }

            if(temporal.mbr_particion[i].part_type == 'P' || temporal.mbr_particion[i].part_type == 'p'){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Primaria</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }else if(temporal.mbr_particion[i].part_type == 'E' || temporal.mbr_particion[i].part_type == 'e'){
                posicionextendida = i;
                existeextendida = 1;
                sprintf(label,"%s%s",label,"\t\t\t<TD COLSPAN=\"40\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Extendida</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }
        }else if(i == 2 && temporal.mbr_particion[i].part_status == '0'){
            sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            break;
        }else if(i == 2 && temporal.mbr_particion[i].part_status == '1'){

            espacioentreparticiones = temporal.mbr_particion[i].part_start - (temporal.mbr_particion[i-1].part_start+temporal.mbr_particion[i-1].part_size);

            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            }

            if(temporal.mbr_particion[i].part_type == 'P' || temporal.mbr_particion[i].part_type == 'p'){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Primaria</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }else if(temporal.mbr_particion[i].part_type == 'E' || temporal.mbr_particion[i].part_type == 'e'){
                posicionextendida = i;
                existeextendida = 1;
                sprintf(label,"%s%s",label,"\t\t\t<TD COLSPAN=\"40\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Extendida</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }
        }else if(i == 3 && temporal.mbr_particion[i].part_status == '0'){
            sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            break;
        }else if(i == 3 && temporal.mbr_particion[i].part_status == '1'){

            espacioentreparticiones = temporal.mbr_particion[i].part_start - (temporal.mbr_particion[i].part_start+temporal.mbr_particion[i].part_size);

            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            }

            if(temporal.mbr_particion[i].part_type == 'P' || temporal.mbr_particion[i].part_type == 'p'){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Primaria</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");


            }else if(temporal.mbr_particion[i].part_type == 'E' || temporal.mbr_particion[i].part_type == 'e'){
                posicionextendida = i;
                existeextendida = 1;

                sprintf(label,"%s%s",label,"\t\t\t<TD COLSPAN=\"40\"><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Extendida</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",temporal.mbr_particion[i].part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_type:",temporal.mbr_particion[i].part_type,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",temporal.mbr_particion[i].part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",temporal.mbr_particion[i].part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",temporal.mbr_particion[i].part_size,"</TD></TR>\n");
                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",temporal.mbr_particion[i].part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");

            }

            espacioentreparticiones = temporal.mbr_tamanio - (temporal.mbr_particion[i].part_start+temporal.mbr_particion[i].part_size);

            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
            }
        }

    }
    sprintf(label,"%s%s",label,"\t\t\t</TR>\n");

    if(existeextendida == 1 ){
        sprintf(label,"%s%s",label,"\t\t\t<TR>\n");

        fseek(disco,temporal.mbr_particion[posicionextendida].part_start,SEEK_SET);
        fread(&ebr,sizeof(EBR),1,disco);
        int indebr = 1;

        while((ebr.part_next != -1)){
            sprintf(label,"%s%s",label,"\t\t\t<TD>EBR</TD>\n");
            if(ebr.part_status == '0'){
                sprintf(label,"%s%s",label,"\t\t\t<TD>Libre</TD>\n");
            }else{
                sprintf(label,"%s%s",label,"\t\t\t<TD><TABLE BORDER=\"0\" >\n");
                sprintf(label,"%s%s",label,"\t\t\t<TR><TD>Logica</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_status:",ebr.part_status,"</TD></TR>\n");
                sprintf(label,"%s%s%s%c%s",label,"\t\t\t<TR><TD align=\"left\">","part_fit:",ebr.part_fit,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_start:",ebr.part_start,"</TD></TR>\n");
                sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_size:",ebr.part_size,"</TD></TR>\n");

                fseek(disco,ebr.part_next,SEEK_SET);
                fread(&tempebr,sizeof(EBR),1,disco);
                if(tempebr.part_status == 0){
                    sprintf(label,"%s%s",label,"\t\t\t<TR><TD align=\"left\">part_next:-1 </TD></TR>\n");

                }else{
                    sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_next:",ebr.part_next,"</TD></TR>\n");

                }

                sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",ebr.part_name,"</TD></TR>\n");
                sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
            }



            fseek(disco,ebr.part_next,SEEK_SET);
            fread(&auxebr,sizeof(EBR),1,disco);

            if(ebr.part_status == '1' ){

                    espacioEntreLogicas =  (ebr.part_next) - (ebr.part_start + ebr.part_size - sizeof(EBR));

                if(  espacioEntreLogicas > 0 ){
                    sprintf(label,"%s%s",label,"\t\t\t<TD>Libre</TD>\n");

                }
            }

            ebr.part_fit = auxebr.part_fit;
            strcpy(ebr.part_name,auxebr.part_name);
            ebr.part_next = auxebr.part_next;

            ebr.part_size = auxebr.part_size;
            ebr.part_start = auxebr.part_start;
            ebr.part_status = auxebr.part_status;
            indebr++;

        }

        sprintf(label,"%s%s",label,"\t\t\t</TR>\n");
        existeextendida = 0;

    }
    sprintf(graphviz,"%s%s",graphviz,label);
    sprintf(graphviz,"%s%s",graphviz,"</TABLE> \n >]; \n }");

    fprintf(reporte,"%s",graphviz);
    fclose(reporte);
    fclose(disco);

    strcpy(dotCommand,"dot -Tpng ");
    strcat(dotCommand,pathReplaceImagen(replace_str(dir,".png",".dot")));
    strcat(dotCommand," -o ");
    strcat(dotCommand,pathReplaceImagen(dir));
    system(dotCommand);

    strcpy(shot, "shotwell ");
    strcat(shot, pathReplaceImagen(dir));
    strcat(shot, " &");
    system(shot);
}




/* Fin REPORTES */



/* ACCIONES DE LOS COMANDOS */

void Particionar(int size, char unit, char fit, char* name, char* path, char type){


    printf("\nPartition created succesfully.\n");
    return;

}


void crearDisco(int size, char* path, char* name, char unit){

    char ruta[150] = {0};
    char comando[600] = {0};
    strcat(ruta,path);
    strcat(ruta,"/");
    strcat(ruta,name);

    FILE *temdisk;
    temdisk = fopen(ruta,"rb+");
    if(temdisk != NULL){
        printf("\nERROR: Disk '%s already exists in '%s'.\n",name,path);
        return;
    }

    int disksize;
    char bsdev[50];
    strcpy(bsdev,"");

    if (unit=='m' || unit =='M'){
        disksize = (size*1024*1024) + sizeof(MBR);
    }else if(unit=='k' || unit =='K'){
        disksize = size*1024 + sizeof(MBR);
    }

    /* Creación de directorio. */
    strcpy(comando, "");
    strcat(comando, "mkdir -p ");
    strcat(comando, pathReplace(path));
    printf("%s\n", comando);
    system(comando);

    /* Creación de archivo binario. */
    strcpy(comando,"");
    strcat(comando, "dd if=/dev/zero of=");
    strcat(comando, unirPathName(path,name));
    strcat(comando, " bs=");
    snprintf(bsdev, sizeof(bsdev) ,"%d",disksize);
    strcat(comando,bsdev);
    strcat(comando, " count=1");
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
    initmbr.mbr_disk_signature = (int)time(NULL);

    /* Particiones en el MBR */
    int a = 0;
    for(a = 0; a < 4; a++){

        initmbr.mbr_particion[a].part_status ='0';
        initmbr.mbr_particion[a].part_type = '\0';
        initmbr.mbr_particion[a].part_fit = '\0';
        initmbr.mbr_particion[a].part_start = 0;
        initmbr.mbr_particion[a].part_size = 0;
        strcpy(initmbr.mbr_particion[a].part_name,"");

    }

    FILE *disk;
    disk = fopen(ruta,"rb+");
    if(disk == NULL){
        printf("%s", "UNKNOWN ERROR WHILE CREATING DISK.");
        return;
    }
    fseek(disk,0,SEEK_SET);
    fwrite(&initmbr,sizeof(MBR),1,disk);
    fclose(disk);
    //system("clear");
    printf("Disk '%s' created.\n", name);

}

/* Método para verificar si ya existe alguna partición con el mismo nombre. */
int verifyPartitionName(char *path, char *name){

    FILE* disk;
    disk = fopen(path,"rb+");
    if(disk == NULL){
        printf("\nERROR: UNKNOWN ERROR WHILE TRYING TO READ DISK.\n");
        return 5;
    }

    MBR temporal;
    EBR ebr,auxebr;
    fseek(disk,0,SEEK_SET);
    fread(&temporal,sizeof(MBR),1,disk);

    int i = 0;
    for(i = 0; i < 4; i++){
        if(strcasecmp(name,temporal.mbr_particion[i].part_name) == 0){
            fclose(disk);
            return 1; /* Una partición ya tiene el mismo nombre. */
            break;
        }

        if(temporal.mbr_particion[i].part_type == 'E'){

            fseek(disk,temporal.mbr_particion[i].part_start,SEEK_SET);
            fread(&ebr,sizeof(EBR),1,disk);

            while((ebr.part_next != -1)){

                if(ebr.part_status == '1'){
                    if(strcasecmp(name,ebr.part_name) == 0){
                        fclose(disk);
                        return 1; /* Una partición ya tiene el mismo nombre. */
                    }
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
    return 0; /* No hay partición con este nombre. */
}













#endif // ACCIONES_H
