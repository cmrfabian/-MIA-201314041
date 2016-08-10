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
void DeleteParticion(char *name, char *path, char *del);
void AddPartition(char *name, char *path, int add, char unit);

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

    MBR tempMBR;
    EBR ebr,auxebr;
    struct tm *loctime;

    if(pathDisco!=NULL){

        printf("RUTA=%s\n",dir);
        FILE *leer = fopen(pathDisco,"rb+");
        fseek(leer,0,SEEK_SET);
        fread(&tempMBR,sizeof(MBR),1,leer);
        loctime = localtime (&tempMBR.mbr_fecha_creacion);

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

                while((ebr.part_status != '0')){

                    if(ebr.part_next != -1){

                        sprintf(label,"%s\n <TR>\n\t <TD colspan=\"2\"><b>EBR_%d</b></TD></TR>",label,indebr);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%c</TD> \n</TR>",label,"  \tpart_status: ",ebr.part_status);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%c</TD> \n</TR>",label,"  \tpart_fit: ",ebr.part_fit);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_start: ",ebr.part_start);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_size: ",ebr.part_size);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_next: ",ebr.part_next);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%s</TD> \n</TR>",label,"  \tpart_name: ",ebr.part_name);

                        fseek(leer,ebr.part_next,SEEK_SET);
                        fread(&auxebr,sizeof(EBR),1,leer);
                        ebr.part_next = auxebr.part_next;
                        ebr.part_fit = auxebr.part_fit;
                        strcpy(ebr.part_name,auxebr.part_name);
                        ebr.part_next = auxebr.part_next;
                        ebr.part_size = auxebr.part_size;
                        ebr.part_start = auxebr.part_start;
                        ebr.part_status = auxebr.part_status;
                        indebr++;

                    }else if(ebr.part_next == -1){

                        sprintf(label,"%s\n <TR>\n\t <TD colspan=\"2\"><b>EBR_%d</b></TD></TR>",label,indebr);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%c</TD> \n</TR>",label,"  \tpart_status: ",ebr.part_status);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%c</TD> \n</TR>",label,"  \tpart_fit: ",ebr.part_fit);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_start: ",ebr.part_start);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_size: ",ebr.part_size);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%d</TD> \n</TR>",label,"  \tpart_next: ",ebr.part_next);
                        sprintf(label,"%s\n <TR>\n\t <TD align=\"right\"><i>%s</i></TD>\n\t <TD align=\"left\">\t%s</TD> \n</TR>",label,"  \tpart_name: ",ebr.part_name);

                        break;
                    }
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


        }else if(i == 3 && temporal.mbr_particion[i].part_status == '1'){

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


        }

    }
    for(i = 0; i < 4 ; i++){

        if(i != 0 && temporal.mbr_particion[i].part_status == '0'){
            espacioentreparticiones = temporal.mbr_tamanio - (temporal.mbr_particion[i-1].part_start+temporal.mbr_particion[i-1].part_size);
            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
                break;
            }
            break;
        }

        if(i == 3 && temporal.mbr_particion[i].part_status == '1'){
            espacioentreparticiones = temporal.mbr_tamanio - (temporal.mbr_particion[i].part_start+temporal.mbr_particion[i].part_size);
            if (espacioentreparticiones > 0){
                sprintf(label,"%s%s",label,"\t\t\t<TD ROWSPAN=\"2\">Libre</TD>\n");
                break;
            }
            break;
        }

    }



    sprintf(label,"%s%s",label,"\t\t\t</TR>\n");

    if(existeextendida == 1 ){
        sprintf(label,"%s%s",label,"\t\t\t<TR>\n");

        fseek(disco,temporal.mbr_particion[posicionextendida].part_start,SEEK_SET);
        fread(&ebr,sizeof(EBR),1,disco);

        int indebr = 1;

        if(ebr.part_status == '0'){
            sprintf(label,"%s%s",label,"\t\t\t<TD>EBR</TD>\n");
            sprintf(label,"%s%s",label,"\t\t\t<TD>Libre</TD>\n");
        }

        while((ebr.part_status != '0')){

            if(ebr.part_next != -1){

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
                    sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_next:",ebr.part_next,"</TD></TR>\n");
                    sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",ebr.part_name,"</TD></TR>\n");
                    sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
                }


                if(ebr.part_status == '1' ){
                        espacioEntreLogicas =  (ebr.part_next) - (ebr.part_start + ebr.part_size);
                    if(espacioEntreLogicas > 0){
                        sprintf(label,"%s%s",label,"\t\t\t<TD>Libre</TD>\n");
                    }
                }


                fseek(disco,ebr.part_next,SEEK_SET);
                fread(&auxebr,sizeof(EBR),1,disco);
                ebr.part_next = auxebr.part_next;
                ebr.part_fit = auxebr.part_fit;
                strcpy(ebr.part_name,auxebr.part_name);
                ebr.part_next = auxebr.part_next;
                ebr.part_size = auxebr.part_size;
                ebr.part_start = auxebr.part_start;
                ebr.part_status = auxebr.part_status;
                indebr++;

            }else if(ebr.part_next == -1){

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
                    sprintf(label,"%s%s%s%d%s",label,"\t\t\t<TR><TD align=\"left\">","part_next:",ebr.part_next,"</TD></TR>\n");
                    sprintf(label,"%s%s%s%s%s",label,"\t\t\t<TR><TD align=\"left\">","part_name:",ebr.part_name,"</TD></TR>\n");
                    sprintf(label,"%s%s",label,"\t\t\t</TABLE></TD>\n");
                }

                if(ebr.part_status == '1' ){
                        espacioEntreLogicas =  (temporal.mbr_particion[posicionextendida].part_start + temporal.mbr_particion[posicionextendida].part_size) - (ebr.part_start + ebr.part_size);
                    if(espacioEntreLogicas > 0){
                        sprintf(label,"%s%s",label,"\t\t\t<TD>Libre</TD>\n");
                    }
                }
                break;
            }
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


void AddPartition(char* name, char* path, int add, char unit){

    int sizeAfterPartition = 0;
    int sizeAfterLogicPartition = 0;

    int existeextendida = 0;
    int posicionextendida = 0;

    int esLogica = 0;
    int posicionlogica = 0;

    int sizeParticion = 0; //Tamaño de la partición a modificar.
    int sizeParticionLogica = 0;

    FILE* disco;
    disco = fopen(path,"rb+");
    if(disco== NULL){
        printf("\nERROR: UNKNOWN ERROR WHILE TRYING TO READ DISK.\n");
        return;
    }

    if(verifyPartitionName(path,name) == 0){
        fclose(disco);
        printf("\nERROR: Partition %s does not exist\n", name);
        return;
    }

    MBR modificar;
    fseek(disco,0,SEEK_SET);
    fread(&modificar,sizeof(MBR),1,disco);

    int indiceauxiliar = 0;
    int a = 0;
    for(a = 0; a < 4; a++){
        if(modificar.mbr_particion[a].part_type == 'E'){
            existeextendida = 1;
            posicionextendida = modificar.mbr_particion[a].part_start;

        }
        if(strcasecmp(modificar.mbr_particion[a].part_name,name) == 0){
            indiceauxiliar = a;
            sizeParticion = modificar.mbr_particion[a].part_size;
            break;
        }

    }


    EBR tempEBR,auxEBR;
    int lastLogica = 0; /* Posicion final de la ultima logica para verificacion de add negativo en EXTENDIDA */

    if(existeextendida == 1){

        fseek(disco,posicionextendida,SEEK_SET);
        fread(&tempEBR,sizeof(EBR),1,disco);

        while((tempEBR.part_status != '0')){

            if(tempEBR.part_next != -1){

                if(strcasecmp(tempEBR.part_name,name) == 0){
                    esLogica = 1;
                    sizeParticionLogica = tempEBR.part_size;
                    posicionlogica = tempEBR.part_start;

                    break;
                }

                fseek(disco,tempEBR.part_next,SEEK_SET);
                fread(&auxEBR,sizeof(EBR),1,disco);
                tempEBR.part_next = auxEBR.part_next;
                tempEBR.part_fit = auxEBR.part_fit;
                strcpy(tempEBR.part_name,auxEBR.part_name);
                tempEBR.part_next = auxEBR.part_next;
                tempEBR.part_size = auxEBR.part_size;
                tempEBR.part_start = auxEBR.part_start;
                tempEBR.part_status = auxEBR.part_status;


            }else if(tempEBR.part_next == -1){
                lastLogica == tempEBR.part_start + tempEBR.part_size;

                if(strcasecmp(tempEBR.part_name,name) == 0){
                    esLogica = 1;
                    sizeParticionLogica = tempEBR.part_size;
                    posicionlogica = tempEBR.part_start;

                    break;
                }

                break;
            }

        }

    }


    if((unit=='B')||(unit=='b')){
        add = add;
    }else if((unit == 'K') || (unit == 'k')){
        add = add*1024;
    }else if((unit=='M')||(unit=='m')){
        add = add*1024*1024;
    }
    //<>
    /* Se quitará espacio de la partición */
    if(add < 0){
        add = add * -1;
        if(esLogica == 0){
            if(add > sizeParticion - 2097152){
                fclose(disco);
                printf("\nERROR: The partition has to have al least 2 Mb.\n");
                printf("\nERROR: You can only remove %d bytes or less from this partition.\n", sizeParticion - 2097152);
                return;
            }else{
                if(modificar.mbr_particion[indiceauxiliar].part_type == 'E' || modificar.mbr_particion[indiceauxiliar].part_type == 'e'){
                    int finalExt = modificar.mbr_particion[indiceauxiliar].part_start + modificar.mbr_particion[indiceauxiliar].part_size;
                    if(add <= (finalExt - lastLogica)){
                        modificar.mbr_particion[indiceauxiliar].part_size = modificar.mbr_particion[indiceauxiliar].part_size - add;
                    }else{
                        printf("\nERROR: Can't remove space because of an existing logical partition.\n");

                    }
                }else{


                    modificar.mbr_particion[indiceauxiliar].part_size = modificar.mbr_particion[indiceauxiliar].part_size - add;

                }

            }
        }else if(esLogica == 1){
            if(add > sizeParticionLogica - 2097152){
                fclose(disco);
                printf("\nERROR: The partition has to have al least 2 Mb.\n");
                printf("\nERROR: You can only remove %d bytes or less from this partition.\n", sizeParticion - 2097152);
                return;
            }else{

                fseek(disco,posicionlogica,SEEK_SET);
                fread(&tempEBR,sizeof(EBR),1,disco);

                tempEBR.part_size = tempEBR.part_size - add;

                fseek(disco,posicionlogica,SEEK_SET);
                fwrite(&tempEBR,sizeof(EBR),1,disco);

            }
        }


    /* Se agregaŕa espacio a la partición */
    }else if(add > 0){
        if(esLogica == 0){

            if(indiceauxiliar == 3){
                sizeAfterPartition = modificar.mbr_tamanio - (modificar.mbr_particion[indiceauxiliar].part_start + modificar.mbr_particion[indiceauxiliar].part_size);
                if(sizeAfterPartition >= add){
                    modificar.mbr_particion[indiceauxiliar].part_size = modificar.mbr_particion[indiceauxiliar].part_size + add ;
                }else{
                    fclose(disco);
                    printf("\nERROR: The size you want to add es bigger than the free space available.\n");
                    return;
                }
            }else{
                if(modificar.mbr_particion[indiceauxiliar+1].part_status == '1'){
                    sizeAfterPartition = modificar.mbr_particion[indiceauxiliar+1].part_start - (modificar.mbr_particion[indiceauxiliar].part_start + modificar.mbr_particion[indiceauxiliar].part_size);
                    if(sizeAfterPartition >= add){
                        modificar.mbr_particion[indiceauxiliar].part_size = modificar.mbr_particion[indiceauxiliar].part_size + add ;
                    }else{
                        fclose(disco);
                        printf("\nERROR: The size you want to add es bigger than the free space available.\n");
                        return;
                    }
                }else{
                    sizeAfterPartition = modificar.mbr_tamanio - (modificar.mbr_particion[indiceauxiliar].part_start + modificar.mbr_particion[indiceauxiliar].part_size);
                    if(sizeAfterPartition >= add){
                        modificar.mbr_particion[indiceauxiliar].part_size = modificar.mbr_particion[indiceauxiliar].part_size + add ;
                    }else{
                        fclose(disco);
                        printf("\nERROR: The size you want to add es bigger than the free space available.\n");
                        return;
                    }
                }
            }
        }else if(esLogica == 1){

            fseek(disco,posicionlogica,SEEK_SET);
            fread(&tempEBR,sizeof(EBR),1,disco);

            fseek(disco,tempEBR.part_next,SEEK_SET);
            fread(&auxEBR,sizeof(EBR),1,disco);


            sizeAfterLogicPartition = tempEBR.part_next - (tempEBR.part_start + tempEBR.part_size);
            if(sizeAfterLogicPartition >= add){
                tempEBR.part_size = tempEBR.part_size + add ;
            }else{
                fclose(disco);
                printf("\nERROR: The size you want to add es bigger than the free space available.\n");
                return;
            }

            fseek(disco,posicionlogica,SEEK_SET);
            fwrite(&tempEBR,sizeof(EBR),1,disco);

        }
    }

    fseek(disco,0, SEEK_SET);
    fwrite(&modificar,sizeof(MBR),1,disco);
    fclose(disco);

    printf("\nModification complete.\n");
    return;
}





void DeleteParticion(char* name, char* path, char* del){

    FILE* disco;
    disco = fopen(path,"rb+");
    if(disco== NULL){
        printf("\nERROR: UNKNOWN ERROR WHILE TRYING TO READ DISK.\n");
        return;
    }

    if(verifyPartitionName(path,name) == 0){
        fclose(disco);
        printf("\nERROR: Partition %s does not exist.\n", name);
        return;
    }

    //Para FULL
    int punteroext;
    int finext;
    char cadena[0] = {"\0"};


    MBR borrar;
    fseek(disco,0,SEEK_SET);
    fread(&borrar,sizeof(MBR),1,disco);

    int esLogica = 0; // 0 para no, 1 para si

    int existePartition = 0;
    int existeextendida = 0;
    int posicionextendida = 0;
    int posicionlogica = 0;
    int esPrimerLogica = 0;
    int esUltimaLogica = 0;
    int posLogicaAnterior = 0;

    int i = 0;
    for(i = 0; i < 4; i++){
        if(strcasecmp(borrar.mbr_particion[i].part_name,name) == 0){
            existePartition = 1;
            break;
        }
        if(borrar.mbr_particion[i].part_type == 'E'){
            existeextendida = 1;
            posicionextendida = borrar.mbr_particion[i].part_start;

        }
    }


    EBR tempEBR,previousEBR,aux2EBR;

    if(existeextendida == 1){
        fseek(disco,posicionextendida,SEEK_SET);
        fread(&tempEBR,sizeof(EBR),1,disco);

        while((tempEBR.part_status != '0')){
            if(tempEBR.part_next != -1){
                if(strcasecmp(tempEBR.part_name,name) == 0)
                {
                    existePartition = 1;
                    esLogica = 1;
                    posicionlogica = tempEBR.part_start;
                    if(posicionlogica == posicionextendida){
                        esPrimerLogica = 1;
                    }

                    break;
                }
                posLogicaAnterior = tempEBR.part_start;
                fseek(disco,tempEBR.part_next,SEEK_SET);
                fread(&aux2EBR,sizeof(EBR),1,disco);
                tempEBR.part_next = aux2EBR.part_next;
                tempEBR.part_fit = aux2EBR.part_fit;
                strcpy(tempEBR.part_name,aux2EBR.part_name);
                tempEBR.part_next = aux2EBR.part_next;
                tempEBR.part_size = aux2EBR.part_size;
                tempEBR.part_start = aux2EBR.part_start;
                tempEBR.part_status = aux2EBR.part_status;


            }else if(tempEBR.part_next == -1){
                if(strcasecmp(tempEBR.part_name,name) == 0)
                {
                    existePartition = 1;
                    esLogica = 1;
                    posicionlogica = tempEBR.part_start;
                    esUltimaLogica = 1;

                    break;
                }
                break;
            }
        }
    }





    if(existePartition == 1){
        printf("\nDesea eliminar partición %s? [y/n]",name);
        if(fgetc(stdin)=='y'){
            if(strncasecmp(del, "fast", strlen("fast")) == 0){
                if(esLogica == 1){

                    if(esPrimerLogica == 1){

                        fseek(disco,posicionlogica,SEEK_SET);
                        fread(&tempEBR,sizeof(EBR),1,disco);

                        memset(tempEBR.part_name,0,sizeof(tempEBR.part_name));
                        strcpy(tempEBR.part_name,"");
                        tempEBR.part_fit = '\0';
                        tempEBR.part_size = 0;
                        tempEBR.part_start = 0;
                        tempEBR.part_status = '0';


                        fseek(disco,posicionlogica,SEEK_SET);
                        fwrite(&tempEBR,sizeof(EBR),1,disco);

                    }else{

                        fseek(disco,posLogicaAnterior,SEEK_SET);
                        fread(&previousEBR,sizeof(EBR),1,disco);

                        fseek(disco,posicionlogica,SEEK_SET);
                        fread(&tempEBR,sizeof(EBR),1,disco);

                        previousEBR.part_next = tempEBR.part_next;

                        fseek(disco,posLogicaAnterior,SEEK_SET);
                        fwrite(&previousEBR,sizeof(EBR),1,disco);


                        memset(tempEBR.part_name,0,sizeof(tempEBR.part_name));
                        strcpy(tempEBR.part_name,"");
                        tempEBR.part_fit = '\0';
                        tempEBR.part_size = 0;
                        tempEBR.part_start = 0;
                        tempEBR.part_status = '0';
                        tempEBR.part_next = -1;


                    }


                }else{

                    memset(borrar.mbr_particion[i].part_name,0,sizeof(borrar.mbr_particion[i].part_name));
                    strcpy(borrar.mbr_particion[i].part_name,"");
                    borrar.mbr_particion[i].part_status ='0';
                    borrar.mbr_particion[i].part_type = '\0';
                    borrar.mbr_particion[i].part_fit = '\0';
                    borrar.mbr_particion[i].part_start = 0;
                    borrar.mbr_particion[i].part_size = 0;


                }

            }else if(strncasecmp(del, "full", strlen("full")) == 0){

                if(esLogica == 0){

                    punteroext = borrar.mbr_particion[i].part_start;
                    finext = borrar.mbr_particion[i].part_size;
                    fseek(disco,punteroext,SEEK_SET);
                    int c = 0;
                    for(c = 0; c < finext; c++){
                        fwrite(cadena,sizeof(char), 1, disco);

                    }

                    memset(borrar.mbr_particion[i].part_name,0,sizeof(borrar.mbr_particion[i].part_name));
                    strcpy(borrar.mbr_particion[i].part_name,"");
                    borrar.mbr_particion[i].part_status ='0';
                    borrar.mbr_particion[i].part_type = '\0';
                    borrar.mbr_particion[i].part_fit = '\0';
                    borrar.mbr_particion[i].part_start = 0;
                    borrar.mbr_particion[i].part_size = 0;

                    fseek(disco,0, SEEK_SET);
                    fwrite(&borrar,sizeof(MBR),1,disco);

                }else if(esLogica == 1){

                    if(esPrimerLogica == 1){



                        fseek(disco,posicionlogica,SEEK_SET);
                        fread(&tempEBR,sizeof(EBR),1,disco);

                        punteroext = posicionlogica + sizeof(EBR);
                        finext = tempEBR.part_start + tempEBR.part_size;
                        fseek(disco,punteroext,SEEK_SET);
                        int c = 0;
                        for(c = 0; c < finext; c++){
                            fwrite(cadena,sizeof(char), 1, disco);

                        }

                        memset(tempEBR.part_name,0,sizeof(tempEBR.part_name));
                        strcpy(tempEBR.part_name,"");
                        tempEBR.part_fit = '\0';
                        tempEBR.part_size = 0;
                        tempEBR.part_start = 0;
                        tempEBR.part_status = '0';


                        fseek(disco,posicionlogica,SEEK_SET);
                        fwrite(&tempEBR,sizeof(EBR),1,disco);




                    }else{

                        fseek(disco,posLogicaAnterior,SEEK_SET);
                        fread(&previousEBR,sizeof(EBR),1,disco);

                        fseek(disco,posicionlogica,SEEK_SET);
                        fread(&tempEBR,sizeof(EBR),1,disco);

                        punteroext = posicionlogica + sizeof(EBR);
                        finext = tempEBR.part_start + tempEBR.part_size;
                        fseek(disco,punteroext,SEEK_SET);
                        int c = 0;
                        for(c = 0; c < finext; c++){
                            fwrite(cadena,sizeof(char), 1, disco);

                        }

                        previousEBR.part_next = tempEBR.part_next;

                        fseek(disco,posLogicaAnterior,SEEK_SET);
                        fwrite(&previousEBR,sizeof(EBR),1,disco);

                        memset(tempEBR.part_name,0,sizeof(tempEBR.part_name));
                        strcpy(tempEBR.part_name,"");
                        tempEBR.part_fit = '\0';
                        tempEBR.part_size = 0;
                        tempEBR.part_start = 0;
                        tempEBR.part_status = '0';
                        tempEBR.part_next = -1;


                    }
                }
            }

            fseek(disco,0, SEEK_SET);
            fwrite(&borrar,sizeof(MBR),1,disco);


            MBR ordenar;
            fseek(disco,0,SEEK_SET);
            fread(&ordenar,sizeof(MBR),1,disco);


            for(i = 0; i < 4; i++){

                if(i != 0 && ordenar.mbr_particion[i-1].part_status == '0'){

                    while(ordenar.mbr_particion[i].part_status == '1' && ordenar.mbr_particion[i-1].part_status == '0'){

                        strcpy(ordenar.mbr_particion[i-1].part_name,ordenar.mbr_particion[i].part_name);
                        ordenar.mbr_particion[i-1].part_fit = ordenar.mbr_particion[i].part_fit;
                        ordenar.mbr_particion[i-1].part_size = ordenar.mbr_particion[i].part_size;
                        ordenar.mbr_particion[i-1].part_start = ordenar.mbr_particion[i].part_start;
                        ordenar.mbr_particion[i-1].part_status = ordenar.mbr_particion[i].part_status;
                        ordenar.mbr_particion[i-1].part_type = ordenar.mbr_particion[i].part_type;

                        memset(ordenar.mbr_particion[i].part_name,0,sizeof(ordenar.mbr_particion[i].part_name));
                        strcpy(ordenar.mbr_particion[i].part_name,"") ;
                        ordenar.mbr_particion[i].part_fit = '\0';
                        ordenar.mbr_particion[i].part_size = 0;
                        ordenar.mbr_particion[i].part_start = 0;
                        ordenar.mbr_particion[i].part_status = '0';
                        ordenar.mbr_particion[i].part_type = '\0';

                    }
                }
            }

            fseek(disco,0, SEEK_SET);
            fwrite(&ordenar,sizeof(MBR),1,disco);
            fclose(disco);

            printf("\nPartition %s deleted succesfully.\n", name);
            return;
        }else if(fgetc(stdin)=='n'){
            printf("\nDelete cancelled.\n");
            return;

        }else{
            printf("\nERROR\n");
            return ;
        }
    }
}


void Particionar(int size, char unit, char fit, char* name, char* path, char type){

    FILE* disk;
    disk = fopen(path,"rb+");
    if(disk == NULL){
        printf("\nERROR: UNKNOWN ERROR WHILE TRYING TO READ DISK.\n");
        return;
    }

    int i = 0;
    int lengthname = strlen(name);
    if(lengthname > 16){

        printf("\nERROR: Name can't have more than 16 characters.\n");
        return;

    }

    int SizeInBytes=0;
    if((unit=='B')||(unit=='b')){
        SizeInBytes = size;
    }else if((unit == 'K') || (unit == 'k')){
        SizeInBytes = size*1024;
    }else if((unit=='M')||(unit=='m')){
        SizeInBytes = size*1048576;
    }

    if((SizeInBytes) < 2097152){
        printf("\nERROR: Minimum size for a partition is 2 Mb.\n");
        return;
    }

    MBR mbr_DiscoAParticionar;
    fseek(disk,0,SEEK_SET);
    fread(&mbr_DiscoAParticionar,sizeof(MBR),1,disk);

    int particionestotalsize = 0;
    for(i = 0; i < 4; i++){
        if(mbr_DiscoAParticionar.mbr_particion[i].part_status == '1'){
            particionestotalsize = particionestotalsize + mbr_DiscoAParticionar.mbr_particion[i].part_size;
        }
    }

    int freespace = 0;
    freespace = mbr_DiscoAParticionar.mbr_tamanio  - particionestotalsize;
     if(type != 'L' ){
        if(SizeInBytes > freespace){
            fclose(disk);
            printf("\nERROR: There is not enough space to create the requested partition.\n");
            return;
        }

    }


    int ParticionesPrimarias = 0;
    int flag_ExistsExtendida = 0;
    int inicioExtendida = 0;
    int finalExtendida = 0;

    for(i = 0; i < 4; i++){
        if((mbr_DiscoAParticionar.mbr_particion[i].part_type == 'P' || mbr_DiscoAParticionar.mbr_particion[i].part_type == 'p')){
            ParticionesPrimarias++;
        }else if((mbr_DiscoAParticionar.mbr_particion[i].part_type == 'E' || mbr_DiscoAParticionar.mbr_particion[i].part_type == 'e')){
            flag_ExistsExtendida = 1;
            inicioExtendida = mbr_DiscoAParticionar.mbr_particion[i].part_start;
            finalExtendida = mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size;
        }
    }




    if(ParticionesPrimarias == 4){
        fclose(disk);
        printf("\nERROR: Can't create any more partitions, there are already 4 primary partitions.\n");
        return;
    }else if((ParticionesPrimarias == 3) && (flag_ExistsExtendida == 1) && ((type == 'E' || type == 'e')  || (type == 'P' || type == 'p')) ){
        fclose(disk);
        printf("\nERROR: Can't create any more partitions.\n");
        return;
    }else if((type == 'E' || type == 'e') && flag_ExistsExtendida == 1){
        fclose(disk);
        printf("\nERROR: Can't create extended partition, there is already 1 extended partition.\n");
        return;
    }else if((type == 'L' || type == 'l') && flag_ExistsExtendida == 0){
        fclose(disk);
        printf("\nERROR: Can't create logical partition because there is no extended partition.\n");
        return;
    }


    if(verifyPartitionName(path,name) == 1){
        printf("\nERROR: There is already a partition with this name.\n");
        return;
    }


    int espacioentreparticiones = 0;
    int sizelibrealfinal = 0;

       if((type=='E')||(type=='P')){

        EBR initebr;
        for(i = 0; i < 4; i++){

            if(i == 0 && mbr_DiscoAParticionar.mbr_particion[i].part_status == '0'){

                strcpy(mbr_DiscoAParticionar.mbr_particion[0].part_name,name);
                mbr_DiscoAParticionar.mbr_particion[0].part_status = '1';
                mbr_DiscoAParticionar.mbr_particion[0].part_type = type;
                mbr_DiscoAParticionar.mbr_particion[0].part_fit = fit;
                mbr_DiscoAParticionar.mbr_particion[0].part_start = sizeof(MBR);
                mbr_DiscoAParticionar.mbr_particion[0].part_size = SizeInBytes;

                if(type == 'E'){

                    strcpy(initebr.part_name,"");
                    initebr.part_status = '0';
                    initebr.part_fit = '\0';
                    initebr.part_next = -1;
                    initebr.part_size = 0;
                    initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                    fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                    fwrite(&initebr,sizeof(EBR),1,disk);

                }

                break;

            }else if(i == 0 && mbr_DiscoAParticionar.mbr_particion[i].part_status == '1'){

                espacioentreparticiones = mbr_DiscoAParticionar.mbr_particion[i].part_start - sizeof(MBR);

                if (SizeInBytes <= espacioentreparticiones){

                    if(mbr_DiscoAParticionar.mbr_particion[3].part_status == '0' && mbr_DiscoAParticionar.mbr_particion[2].part_status == '1'){

                        int index = 0;
                        for(index = 3; index > 0; index--){

                            mbr_DiscoAParticionar.mbr_particion[index] = mbr_DiscoAParticionar.mbr_particion[index - 1];

                        }

                        strcpy(mbr_DiscoAParticionar.mbr_particion[0].part_name,name);
                        mbr_DiscoAParticionar.mbr_particion[0].part_status = '1';
                        mbr_DiscoAParticionar.mbr_particion[0].part_type = type;
                        mbr_DiscoAParticionar.mbr_particion[0].part_fit = fit;
                        mbr_DiscoAParticionar.mbr_particion[0].part_start = sizeof(MBR);
                        mbr_DiscoAParticionar.mbr_particion[0].part_size = SizeInBytes;

                        if(type == 'E'){

                            strcpy(initebr.part_name,"");
                            initebr.part_status = '0';
                            initebr.part_fit = '\0';
                            initebr.part_next = -1;
                            initebr.part_size = 0;
                            initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                            fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                            fwrite(&initebr,sizeof(EBR),1,disk);

                        }

                        break;


                    }else if(mbr_DiscoAParticionar.mbr_particion[2].part_status == '0' && mbr_DiscoAParticionar.mbr_particion[1].part_status == '1'){

                        int index = 0;
                        for(index = 2; index > 0; index--){

                            mbr_DiscoAParticionar.mbr_particion[index] = mbr_DiscoAParticionar.mbr_particion[index - 1];

                        }

                        strcpy(mbr_DiscoAParticionar.mbr_particion[0].part_name,name);
                        mbr_DiscoAParticionar.mbr_particion[0].part_status = '1';
                        mbr_DiscoAParticionar.mbr_particion[0].part_type = type;
                        mbr_DiscoAParticionar.mbr_particion[0].part_fit = fit;
                        mbr_DiscoAParticionar.mbr_particion[0].part_start = sizeof(MBR);
                        mbr_DiscoAParticionar.mbr_particion[0].part_size = SizeInBytes;

                        if(type == 'E'){

                            strcpy(initebr.part_name,"");
                            initebr.part_status = '0';
                            initebr.part_fit = '\0';
                            initebr.part_next = -1;
                            initebr.part_size = 0;
                            initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                            fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                            fwrite(&initebr,sizeof(EBR),1,disk);

                        }

                        break;


                    }else if(mbr_DiscoAParticionar.mbr_particion[1].part_status == '0' && mbr_DiscoAParticionar.mbr_particion[0].part_status == '1'){

                        mbr_DiscoAParticionar.mbr_particion[1] = mbr_DiscoAParticionar.mbr_particion[0];

                        strcpy(mbr_DiscoAParticionar.mbr_particion[0].part_name,name);
                        mbr_DiscoAParticionar.mbr_particion[0].part_status = '1';
                        mbr_DiscoAParticionar.mbr_particion[0].part_type = type;
                        mbr_DiscoAParticionar.mbr_particion[0].part_fit = fit;
                        mbr_DiscoAParticionar.mbr_particion[0].part_start = sizeof(MBR);
                        mbr_DiscoAParticionar.mbr_particion[0].part_size = SizeInBytes;

                        if(type == 'E'){

                            strcpy(initebr.part_name,"");
                            initebr.part_status = '0';
                            initebr.part_fit = '\0';
                            initebr.part_next = -1;
                            initebr.part_size = 0;
                            initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                            fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                            fwrite(&initebr,sizeof(EBR),1,disk);

                        }

                        break;
                    }

                }else{

                    sizelibrealfinal = mbr_DiscoAParticionar.mbr_tamanio - mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size;
                    if(SizeInBytes <= sizelibrealfinal) {

                        if(mbr_DiscoAParticionar.mbr_particion[i + 1].part_status == '0'){

                            strcpy(mbr_DiscoAParticionar.mbr_particion[i + 1].part_name,name);
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_status = '1';
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_type = type;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_fit = fit;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size ;//+ 1;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_size = SizeInBytes;

                            if(type == 'E'){

                                strcpy(initebr.part_name,"");
                                initebr.part_status = '0';
                                initebr.part_fit = '\0';
                                initebr.part_next = -1;
                                initebr.part_size = 0;
                                initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i + 1].part_start;

                                fseek(disk,mbr_DiscoAParticionar.mbr_particion[i + 1].part_start,SEEK_SET);
                                fwrite(&initebr,sizeof(EBR),1,disk);

                            }

                            break;
                        }

                    }else{
                        printf("Ya no hay espacio al final.");
                    }
                }
            }else if(i == 1 && mbr_DiscoAParticionar.mbr_particion[i].part_status == '1'){
                espacioentreparticiones = mbr_DiscoAParticionar.mbr_particion[i].part_start - (mbr_DiscoAParticionar.mbr_particion[i - 1].part_start+mbr_DiscoAParticionar.mbr_particion[i - 1].part_size);

                if (SizeInBytes <= espacioentreparticiones){

                    if(mbr_DiscoAParticionar.mbr_particion[3].part_status == '0' && mbr_DiscoAParticionar.mbr_particion[2].part_status == '1'){


                        int index = 0;
                        for(index = 3; index > 1; index--){

                            mbr_DiscoAParticionar.mbr_particion[index] = mbr_DiscoAParticionar.mbr_particion[index - 1];
                        }

                        strcpy(mbr_DiscoAParticionar.mbr_particion[1].part_name,name);
                        mbr_DiscoAParticionar.mbr_particion[1].part_status = '1';
                        mbr_DiscoAParticionar.mbr_particion[1].part_type = type;
                        mbr_DiscoAParticionar.mbr_particion[1].part_fit = fit;
                        mbr_DiscoAParticionar.mbr_particion[1].part_start = mbr_DiscoAParticionar.mbr_particion[i - 1].part_start + mbr_DiscoAParticionar.mbr_particion[i - 1].part_size ;//+ 1;
                        mbr_DiscoAParticionar.mbr_particion[1].part_size = SizeInBytes;




                        if(type == 'E'){

                            strcpy(initebr.part_name,"");
                            initebr.part_status = '0';
                            initebr.part_fit = '\0';
                            initebr.part_next = -1;
                            initebr.part_size = 0;
                            initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                            fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                            fwrite(&initebr,sizeof(EBR),1,disk);

                        }

                        break;


                    }else if(mbr_DiscoAParticionar.mbr_particion[2].part_status == '0' && mbr_DiscoAParticionar.mbr_particion[1].part_status == '1'){

                        mbr_DiscoAParticionar.mbr_particion[2] = mbr_DiscoAParticionar.mbr_particion[1];

                        strcpy(mbr_DiscoAParticionar.mbr_particion[1].part_name,name);
                        mbr_DiscoAParticionar.mbr_particion[1].part_status = '1';
                        mbr_DiscoAParticionar.mbr_particion[1].part_type = type;
                        mbr_DiscoAParticionar.mbr_particion[1].part_fit = fit;
                        mbr_DiscoAParticionar.mbr_particion[1].part_start = mbr_DiscoAParticionar.mbr_particion[i - 1].part_start + mbr_DiscoAParticionar.mbr_particion[i - 1].part_size ;//+ 1;
                        mbr_DiscoAParticionar.mbr_particion[1].part_size = SizeInBytes;


                        if(type == 'E'){

                            strcpy(initebr.part_name,"");
                            initebr.part_status = '0';
                            initebr.part_fit = '\0';
                            initebr.part_next = -1;
                            initebr.part_size = 0;
                            initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                            fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                            fwrite(&initebr,sizeof(EBR),1,disk);

                        }

                        break;

                    }

                }else{

                    sizelibrealfinal = mbr_DiscoAParticionar.mbr_tamanio - mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size;
                    if(SizeInBytes <= sizelibrealfinal) {

                        if(mbr_DiscoAParticionar.mbr_particion[i + 1].part_status == '0'){

                            strcpy(mbr_DiscoAParticionar.mbr_particion[i + 1].part_name,name);
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_status = '1';
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_type = type;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_fit = fit;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size ;//+ 1;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_size = SizeInBytes;


                            if(type == 'E'){

                                strcpy(initebr.part_name,"");
                                initebr.part_status = '0';
                                initebr.part_fit = '\0';
                                initebr.part_next = -1;
                                initebr.part_size = 0;
                                initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i + 1].part_start;

                                fseek(disk,mbr_DiscoAParticionar.mbr_particion[i + 1].part_start,SEEK_SET);
                                fwrite(&initebr,sizeof(EBR),1,disk);

                            }
                            break;
                        }

                    }else{
                        printf("Ya no hay espacio al final.");
                    }
                }

            }else if(i == 2 && mbr_DiscoAParticionar.mbr_particion[i].part_status == '1'){
                espacioentreparticiones = mbr_DiscoAParticionar.mbr_particion[i].part_start - (mbr_DiscoAParticionar.mbr_particion[i - 1].part_start+mbr_DiscoAParticionar.mbr_particion[i - 1].part_size);

                if (SizeInBytes <= espacioentreparticiones){

                    if(mbr_DiscoAParticionar.mbr_particion[3].part_status == '0' && mbr_DiscoAParticionar.mbr_particion[2].part_status == '1'){

                        mbr_DiscoAParticionar.mbr_particion[3] = mbr_DiscoAParticionar.mbr_particion[2];

                        strcpy(mbr_DiscoAParticionar.mbr_particion[2].part_name,name);
                        mbr_DiscoAParticionar.mbr_particion[2].part_status = '1';
                        mbr_DiscoAParticionar.mbr_particion[2].part_type = type;
                        mbr_DiscoAParticionar.mbr_particion[2].part_fit = fit;
                        mbr_DiscoAParticionar.mbr_particion[2].part_start = mbr_DiscoAParticionar.mbr_particion[i - 1].part_start + mbr_DiscoAParticionar.mbr_particion[i - 1].part_size  ;//+ 1;
                        mbr_DiscoAParticionar.mbr_particion[2].part_size = SizeInBytes;


                        if(type == 'E'){

                            strcpy(initebr.part_name,"");
                            initebr.part_status = '0';
                            initebr.part_fit = '\0';
                            initebr.part_next = -1;
                            initebr.part_size = 0;
                            initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                            fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                            fwrite(&initebr,sizeof(EBR),1,disk);

                        }

                        break;
                    }

                }else{

                    sizelibrealfinal = mbr_DiscoAParticionar.mbr_tamanio - mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size;
                    if(SizeInBytes <= sizelibrealfinal) {


                        if(mbr_DiscoAParticionar.mbr_particion[i + 1].part_status == '0'){

                            strcpy(mbr_DiscoAParticionar.mbr_particion[i + 1].part_name,name);
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_status = '1';
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_type = type;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_fit = fit;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start + mbr_DiscoAParticionar.mbr_particion[i].part_size ;//+ 1;
                            mbr_DiscoAParticionar.mbr_particion[i + 1].part_size = SizeInBytes;

                            if(type == 'E'){

                                strcpy(initebr.part_name,"");
                                initebr.part_status = '0';
                                initebr.part_fit = '\0';
                                initebr.part_next = -1;
                                initebr.part_size = 0;
                                initebr.part_start = mbr_DiscoAParticionar.mbr_particion[i].part_start;

                                fseek(disk,mbr_DiscoAParticionar.mbr_particion[i].part_start,SEEK_SET);
                                fwrite(&initebr,sizeof(EBR),1,disk);

                            }

                            break;

                        }

                    }else{
                        printf("Ya no hay espacio al final.");
                    }
                }
            }
        }

    }else if((type=='L')||(type=='l')){

        int espacioentrelogicas = 0;
        int posNextEmptyEBR = 0;
        int posFinalEBR = 0;
        int posActualEBR = 0;


        EBR tempEBR,auxEBR,aux2EBR;
        fseek(disk,inicioExtendida,SEEK_SET);
        fread(&tempEBR,sizeof(EBR),1,disk);
        if(tempEBR.part_status == '0' && tempEBR.part_next == -1){

            strcpy(tempEBR.part_name,name);
            tempEBR.part_status = '1';
            tempEBR.part_fit = fit;
            tempEBR.part_start = inicioExtendida;
            tempEBR.part_size = SizeInBytes;
            tempEBR.part_next = -1;

            fseek(disk,inicioExtendida,SEEK_SET);
            fwrite(&tempEBR,sizeof(EBR),1,disk);

            posNextEmptyEBR =  tempEBR.part_start + tempEBR.part_size;

            if((posNextEmptyEBR > (finalExtendida - sizeof(EBR))) && (posNextEmptyEBR <= finalExtendida)){
                printf("...");
                //No escribir ultimo EBR vacio, se saldria de la extendida.
            }else{


                strcpy(auxEBR.part_name,"");
                auxEBR.part_status= '0';
                auxEBR.part_fit = '\0';
                auxEBR.part_start = tempEBR.part_start + tempEBR.part_size;
                auxEBR.part_size = 0;
                auxEBR.part_next = -1;

                posNextEmptyEBR =  tempEBR.part_start + tempEBR.part_size;

                fseek(disk,posNextEmptyEBR,SEEK_SET);
                fwrite(&auxEBR,sizeof(EBR),1,disk);
            }

        }else if(tempEBR.part_status == '0' && tempEBR.part_next != -1){ /*Caso en que la primera logica fue eliminada. */

            espacioentrelogicas = tempEBR.part_next - inicioExtendida;

            if(SizeInBytes <= espacioentrelogicas ){

                strcpy(tempEBR.part_name,name);
                tempEBR.part_status = '1';
                tempEBR.part_fit = fit;
                tempEBR.part_start = inicioExtendida;
                tempEBR.part_size = SizeInBytes;
                //tempEBR.part_next = -1;

                fseek(disk,inicioExtendida,SEEK_SET);
                fwrite(&tempEBR,sizeof(EBR),1,disk);
            }

        }else if((tempEBR.part_status != '0')){

            while((tempEBR.part_status != '0')){

                if(tempEBR.part_next != -1){

                    espacioentrelogicas = tempEBR.part_next - (tempEBR.part_start + tempEBR.part_size);
                    posActualEBR = tempEBR.part_start;
                    posFinalEBR = tempEBR.part_start + tempEBR.part_size;

                    if(SizeInBytes <= espacioentrelogicas ){

                        strcpy(auxEBR.part_name,name);
                        auxEBR.part_status = '1';
                        auxEBR.part_fit = fit;
                        auxEBR.part_start = tempEBR.part_start + tempEBR.part_size;
                        auxEBR.part_size = SizeInBytes;
                        auxEBR.part_next = tempEBR.part_next;

                        fseek(disk,posFinalEBR,SEEK_SET);
                        fwrite(&auxEBR,sizeof(EBR),1,disk);

                        tempEBR.part_next = auxEBR.part_start;
                        fseek(disk,posActualEBR,SEEK_SET);
                        fwrite(&tempEBR,sizeof(EBR),1,disk);
                        break;

                    }

                    fseek(disk,tempEBR.part_next,SEEK_SET);
                    fread(&aux2EBR,sizeof(EBR),1,disk);
                    tempEBR.part_next = aux2EBR.part_next;
                    tempEBR.part_fit = aux2EBR.part_fit;
                    strcpy(tempEBR.part_name,aux2EBR.part_name);
                    tempEBR.part_next = aux2EBR.part_next;
                    tempEBR.part_size = aux2EBR.part_size;
                    tempEBR.part_start = aux2EBR.part_start;
                    tempEBR.part_status = aux2EBR.part_status;


                }else if(tempEBR.part_next == -1){

                    espacioentrelogicas = finalExtendida - (tempEBR.part_start + tempEBR.part_size); //Espacio entre final de Extendida y ultima logica.
                    posActualEBR = tempEBR.part_start;
                    posFinalEBR = tempEBR.part_start + tempEBR.part_size;

                    if(SizeInBytes <= espacioentrelogicas ){

                        strcpy(auxEBR.part_name,name);
                        auxEBR.part_status = '1';
                        auxEBR.part_fit = fit;
                        auxEBR.part_start = tempEBR.part_start + tempEBR.part_size;
                        auxEBR.part_size = SizeInBytes;
                        auxEBR.part_next = -1;

                        fseek(disk,posFinalEBR,SEEK_SET);
                        fwrite(&auxEBR,sizeof(EBR),1,disk);

                        tempEBR.part_next = tempEBR.part_next;
                        tempEBR.part_fit = tempEBR.part_fit;
                        strcpy(tempEBR.part_name,tempEBR.part_name);
                        tempEBR.part_next = auxEBR.part_start;
                        tempEBR.part_size = tempEBR.part_size;
                        tempEBR.part_start = tempEBR.part_start;
                        tempEBR.part_status = tempEBR.part_status;
                        fseek(disk,posActualEBR,SEEK_SET);
                        fwrite(&tempEBR,sizeof(EBR),1,disk);
                        break;


                    }else{

                        fclose(disk);
                        printf("\nERROR: There is not enough space to create the requested partition.\n");
                        return;

                    }
                    break;
                }
            }
        }
    }

    fseek(disk,0,SEEK_SET);
    fwrite(&mbr_DiscoAParticionar,sizeof(MBR),1,disk);
    fclose(disk);

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


    /* Verificacion de tamaño minimo de disco */
    if((disksize - sizeof(MBR)) < 10485760){
        printf("\nERROR: Minimum size for a disk is 10 Mb.\n");
        return;

    }

    /* Creación de directorio. */
    strcpy(comando, "");
    strcat(comando, "mkdir -p ");
    strcat(comando, pathReplace(path));
    printf("\n%s\n", comando);
    system(comando);

    /* Creación de archivo binario. */
    strcpy(comando,"");
    strcat(comando, "dd if=/dev/zero of=");
    strcat(comando, unirPathName(path,name));
    strcat(comando, " bs=");
    snprintf(bsdev, sizeof(bsdev) ,"%d",disksize);
    strcat(comando,bsdev);
    strcat(comando, " count=1");
    printf("\n%s\n",comando);
    system(comando);


    MBR initmbr;
    /* Tamaño MBR*/
    initmbr.mbr_tamanio = disksize;

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
        printf("\n%s\n", "UNKNOWN ERROR WHILE CREATING DISK.");
        return;
    }
    fseek(disk,0,SEEK_SET);
    fwrite(&initmbr,sizeof(MBR),1,disk);
    fclose(disk);
    //system("clear");
    printf("\nDisk '%s' created.\n", name);

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
