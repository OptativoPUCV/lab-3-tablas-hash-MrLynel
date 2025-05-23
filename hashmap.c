#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long pos = hash(key, map->capacity); //Se usa función hash para encontrar la posición

    while (map->buckets[pos] != NULL && map->buckets[pos]->key != NULL) { 
        //Si la casilla está ocupada y no es NULL, avanzamos a la siguiente casilla (resolución de colisiones)
        if (is_equal(map->buckets[pos]->key, key)) return; //Si ya existe la clave, no insertamos
        pos = (pos + 1) % map->capacity; //Se avanza circularmente
    }

    map->buckets[pos] = createPair(key, value); //Inserta el par (clave, valor) en la posición encontrada
    map->size++; //Incrementa el tamaño del mapa
    map->current = pos; //Actualiza el índice actual
}


void enlarge(HashMap * map) {
    
    enlarge_called = 1; //no borrar (testing purposes)
     //a - Crear una variable auxiliar para mantener el arreglo actual de buckets
    Pair ** oldBuckets = map->buckets;
    long oldCapacity = map->capacity; // b - Guardar la capacidad anterior

    //c - Duplicar la capacidad
    map->capacity *= 2;

    //Asignar un nuevo arreglo con la nueva capacidad
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *)); // Asigna nuevo espacio para los buckets

    map->size = 0; //d - Inicializar el tamaño a 0

    //e - Reinsertar los elementos del arreglo antiguo en la nueva tabla
    for (long i = 0; i < oldCapacity; i++) {
        if (oldBuckets[i] != NULL && oldBuckets[i]->key != NULL) {
            insertMap(map, oldBuckets[i]->key, oldBuckets[i]->value); //Reinsertamos el par en la nueva tabla
        }
    }

    //Liberar el arreglo antiguo después de haber reinsertado todos los pares
    free(oldBuckets);
}


HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *)malloc(sizeof(HashMap)); //Se asigna memoria
    if (map == NULL) return NULL; //Verifica la asignacion de memoria
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *)); //Inicializa los buckets con NULL
    if (map->buckets == NULL) { free(map); return NULL; } //Verifica la reserva de memoria
    map->size = 0; //Asigna el tamaño en 0
    map->capacity = capacity; //Asigna la capacidad
    map->current = -1; // Inicializa el índice
    return map;
}


void eraseMap(HashMap * map, char * key) {
    Pair * pair = searchMap(map, key); //Se busca el par con la clave

    if (pair != NULL) { //En caso de que lo encuentre
        pair->key = NULL; //Invalida el par (no lo elimina, solo marca la clave como NULL)
        map->size--; //Decrementa el tamaño
    }
}

Pair * searchMap(HashMap * map, char * key) {
    long pos = hash(key, map->capacity); //Usamos funcion hash

    while (map->buckets[pos] != NULL) { //Se busca hasta encontrar un null
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            map->current = pos; //Actualiza indice
            return map->buckets[pos]; //Si la clave es igual se retorna el par
        }
        pos = (pos + 1) % map->capacity; //Avanza circularmente
    }

    return NULL; //NULL si no encuentra la clave
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) { //Encuentra el primer par valido
            map->current = i; //Actualiza el indice actual
            return map->buckets[i]; //Retorna el par
        }
    }
    return NULL; //Si no encuentra pares, retorna NULL
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {// Encuentra el siguiente par valido
            map->current = i; //Actualiza el indice actual
            return map->buckets[i]; //Retorna el par
        }
    }
    return NULL; //Si no hay mas pares, retorna NULL
}
