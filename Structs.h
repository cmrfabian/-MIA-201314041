#ifndef STRUCTS_H
#define STRUCTS_H

#include <time.h>

typedef struct Particiones{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
}Particion;

typedef struct MasterBootRecord{
    int mbr_tamanio;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    Particion mbr_particion[4];
}MBR;

typedef struct ExtendenBootRecord{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
}EBR;

#endif // STRUCTS_H
