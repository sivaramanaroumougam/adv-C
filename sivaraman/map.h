/* 
 * File:   map.h
 * Author: SIVA
 *
 * Created on November 18, 2016
 */

#ifndef CITYMAP_MAP_H
#define CITYMAP_MAP_H

#include "status.h"
#include "List.h"

#define MAX_LENGTH 1024
//using this as default map file name if user not mention
static char* mapFileName = "FRANCE.MAP";

/** The struct holds the information about city in a Map file*/
typedef struct City {
    char* name;
    int lang;
    int lat;
    int g;
    int h;
    List* neighbour;
    struct City* backptr;
}City;

typedef struct Neighbour {
    char* name;
    int distance;
}Neighbour;


List* parseMapFile( char* );

City* findCity( List* , char* );

status findPath( List*, char*, char* );

City* createCity( );

Neighbour* createNeighbour( );

void destroyCities( List* );

void destroyCity( City** );

void destroyNeighbour( Neighbour** );

#endif //CITYMAP_MAP_H