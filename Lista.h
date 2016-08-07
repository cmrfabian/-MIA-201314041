#ifndef LISTA_H
#define LISTA_H

#include <string.h>


/* Nodo para lista de Tokens.*/
typedef struct Token
{
   char valor[150];
   char* tipo;

   struct Token *siguiente;
   struct Token *anterior;

}token;

/* Nodo para lista de Particiones Montadas. */
typedef struct Disco {

   char id[50];
   char path[150];
   char name[50];
   int letra;
   int numeromount;
   int estado;

   struct Disco *siguiente;
   struct Disco *anterior;

}disco;

/* Lista de tokens */
typedef struct ListaTokens
{
    token *primero;
    token *ultimo;

}listatokens;


/* Lista Particiones Montadas */
typedef struct ListaMontados{
    disco *primero;
    disco *ultimo;
}listamontados;


token* crearToken(char *tipo, char valor[])
{
    token* nVA = (token*)malloc(sizeof(token));
    strcpy(nVA->valor,valor);
    nVA->tipo = tipo;

    nVA->siguiente = NULL;
    nVA->anterior = NULL;
    return nVA;
}

/* Insertar en lista como en EDD */
void insertarToken( listatokens *lv, char *tipo, char valor[])
{
    token* nv = crearToken(tipo, valor);
    if (lv->primero == NULL)
    {
        lv->primero = nv;
        lv->ultimo = nv;
    }
    else
    {
        lv->ultimo->siguiente = nv;
        nv->anterior = lv->ultimo;
        nv->siguiente = NULL;
    }
    lv->ultimo = nv;
}


disco* creardisco(char name[], char id[], char path[], int letra, int numeromount)
{
    disco* nVA;
    nVA = (disco*)malloc(sizeof(disco));
    strcpy(nVA->name,name);
    strcpy(nVA->id,id);
    strcpy(nVA->path,path);
    nVA->letra = letra;
    nVA->numeromount = numeromount;

    nVA->siguiente = NULL;
    nVA->anterior = NULL;
    return nVA;
}


void montarDisco(listamontados *lm,  char name[], char id[], char path[],  int letra, int numeromount)
{
    disco* nv;
    nv = creardisco(name,id,path,letra,numeromount);
    if (lm->primero == NULL)
    {
        lm->primero = nv;
        lm->ultimo = nv;
    }
    else
    {
        lm->ultimo->siguiente = nv;
        nv->anterior = lm->ultimo;
        nv->siguiente = NULL;
    }
    lm->ultimo = nv;
}


void desmontardisco(listamontados *lista, char id[])
{
    disco* aux;
    aux = lista->primero;
    while (aux != NULL)
    {
        if(strcasecmp(aux->id,id)==0)
        {

            if(aux == lista->primero && aux != lista->ultimo)
            {
                lista->primero = aux->siguiente;
                lista->primero->anterior = NULL;
                free(aux);
            }
            else if(aux== lista->ultimo &&  aux != lista->primero)
            {
                lista->ultimo = aux->anterior;
                lista->ultimo->siguiente = NULL;
                free(aux);
            }
            else if (aux == lista->primero && aux == lista->ultimo)
            {
                lista->primero = NULL;
                lista->ultimo = NULL;
                free(aux);
            }
            else
            {
                aux->anterior->siguiente = aux->siguiente;
                aux->siguiente->anterior = aux->anterior;
                free(aux);
            }
        break;
        }
        aux = aux->siguiente;
    }
}

void borrarToken (listatokens *lista, char *tipo)
{
    token* aux;
    aux = lista->primero;
    while (aux != NULL)
    {
        if(strcasecmp(aux->tipo,tipo)==0)
        {

            if(aux == lista->primero && aux != lista->ultimo)
            {
                lista->primero = aux->siguiente;
                lista->primero->anterior = NULL;
                free(aux);
            }
            else if(aux== lista->ultimo &&  aux != lista->primero)
            {
                lista->ultimo = aux->anterior;
                lista->ultimo->siguiente = NULL;
                free(aux);
            }
            else if (aux == lista->primero && aux == lista->ultimo)
            {
                lista->primero = NULL;
                lista->ultimo = NULL;
                free(aux);
            }
            else
            {
                aux->anterior->siguiente = aux->siguiente;
                aux->siguiente->anterior = aux->anterior;
                free(aux);
            }
        break;
        }
        aux = aux->siguiente;
    }
}



#endif // LISTA_H
