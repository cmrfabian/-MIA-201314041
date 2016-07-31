#ifndef LISTA_H
#define LISTA_H

#include <string.h>

typedef struct Token
{
   char* tipo;
   char valor[150];

   struct Token *siguiente;
   struct Token *anterior;

}token;

typedef struct ListaTokens
{
    token *primero;
    token *ultimo;

}listatokens;

token* crearToken(char *tipo, char valor[])
{
    token* nVA;
    nVA = (token*)malloc(sizeof(token));
    nVA->tipo = tipo;
    strcpy(nVA->valor,valor);
    //nVA->valor = valor;
    nVA->siguiente = NULL;
    nVA->anterior = NULL;

    return nVA;
}


void insertarToken(listatokens *lv,  char *tipo, char valor[])
{
    token* nv;
    nv = crearToken(tipo, valor);
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

#endif // LISTA_H
