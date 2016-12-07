/* 
 * File:   map.c
 * Author: SIVA
 *
 * Created on November 18, 2016.
 */

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "map.h"

static int compareCities(void * s1, void * s2) {

    City* firstCity = ( City* )s1;
    City* secondCity = ( City* )s2;
    return strcasecmp( firstCity->name, secondCity->name  );
}

static int compareCitiesDistance(void * s1, void * s2) {

    City* firstCity = ( City* )s1;
    City* secondCity = ( City* )s2;
    if ( firstCity->g == secondCity->g )
    {
        return 0;
    }
    else if ( firstCity->g < secondCity->g )
    {
        return -1;
    }

    return 2 ;
}

static void displayCity( void * s ) {
    printf(" %s :\n",( ( City* )s )->name );
    printf(" %d :\n",( ( City* )s )->g );
}

static int compareNeighbours( void * s1, void * s2 ) {
    return strcmp( ( ( Neighbour* )s1 )->name, ( ( ( Neighbour* )s2 )->name ) );
}

static void displayNeighbour( void * s ) {
    printf(" %s :\n",( ( Neighbour* )s )->name );
    printf(" %d:\n",( ( Neighbour* )s )->distance );
}

static int get_heuristic_cost( City* startCity, City* goalCity )
{
    return ( abs( startCity->lat - goalCity->lat ) + abs( startCity->lang - goalCity->lang ) ) / 4;
}

static void displaySearchResults( City* currCity )
{
    List* disList = newList( compareCitiesDistance, displayCity );
    while( currCity )
    {
        addListAt( disList,1, currCity );
        currCity = currCity->backptr;
    }
    displayList(disList);
    delList(disList);
}

static City* getLowestfValueCity( List* openList )
{
    Node* currentNode = openList->head;
    if( !currentNode )
    {
        return 0;
    }
    City* highestFCity = ( City* )currentNode->val;
    int highestFvalue = highestFCity->g + highestFCity->h;
    currentNode = currentNode->next;

    while( currentNode )
    {
        City* crntCity = ( City* )currentNode->val;
        if( highestFvalue > ( crntCity->g + crntCity->h ) )
        {
            highestFCity = crntCity;
        }
        currentNode = currentNode->next;
    }
    return highestFCity ;
}


List* parseMapFile( char*  filename )
{
    FILE* fileptr;
    //open file
    fileptr = fopen( filename, "r" );
    if( !fileptr )
    {
        return 0;
    }
    List* cityList = newList( compareCities,displayCity );
    char* cityName = ( char* )malloc(MAX_LENGTH);
    City* newCity = 0;
    // read each line from map
    while ( !feof ( fileptr ) )
    {

        int nLat=-1;
        int nLong=-1;
        fscanf( fileptr, "%s %d %d", cityName, &nLat, &nLong );
        if( -1 != nLong  )
        {
            if( 0 != newCity )
            {
                addList( cityList, newCity );
            }
            newCity = createCity();
            newCity->neighbour = newList( compareNeighbours, displayNeighbour );
            strncpy( newCity->name , cityName, MAX_LENGTH );
            newCity->lat = nLat;
            newCity->lang = nLong;
        }
        else if( 0 != newCity )
        {
            Neighbour* neighbour = 0;
            neighbour = createNeighbour();
            strncpy( neighbour->name, cityName, MAX_LENGTH );
            neighbour->distance = nLat;
            addList( newCity->neighbour, neighbour );
        }
    }
    if( 0 != newCity )
    {
        addList( cityList, newCity );
    }
    fclose( fileptr );
    return cityList;
}

status findPath( List* cityList, char* srcCity, char* destCity )
{
    City* startCity = findCity( cityList, srcCity );
    City* goalCity = findCity( cityList, destCity );
    if( !cityList || !startCity || !goalCity )
    {
        return ERRCITYNOTFOUND;
    }
    List* closedList = newList( compareCities, displayCity );

    List* openList = newList( compareCities, displayCity );
    // The cost of going from start to start is zero.
    startCity->g = 0;
    startCity->h = get_heuristic_cost( startCity, goalCity );
    // directly adding start city to open list
    addList( openList, startCity );
    //loop all the cities in open list
    while( lengthList( openList ) > 0 )
    {
        City* currentCity = getLowestfValueCity( openList );
        remFromList( openList, currentCity );
        addList( closedList, currentCity );      
        if( cityList->comp( currentCity , goalCity ) == 0  )
        {
            //we found the solution
            displaySearchResults( currentCity );
            break;
        }
        Node* neighbourNode = currentCity->neighbour->head;
        while( neighbourNode )
        {
            Neighbour* nextNeighbour = ( Neighbour* )neighbourNode->val;
            City* succCity = findCity( cityList, nextNeighbour->name );
            int sucNodeCost = currentCity->g + nextNeighbour->distance;
            City* nodeSucc = findCity( openList, succCity->name );
            if( nodeSucc!=0 )
            {       
                if( sucNodeCost >= nodeSucc->g )
                {
                    neighbourNode = neighbourNode->next;
                    continue;
                }
                remFromList( openList, succCity );
            }
            nodeSucc = findCity( closedList, succCity->name );
            if( nodeSucc != 0 )
            {
                /// if successor city is on the CLOSED list but the existing
                //one is as good or better then discard this successor and continue
                if( sucNodeCost >= nodeSucc->g )
                {
                    neighbourNode = neighbourNode->next;
                    continue;
                }
                remFromList( closedList, succCity );
            }
            //Set the parent of successor city  to current city
            succCity->backptr = currentCity;
            // set g value of successor city  as new successor Node Cost
            succCity ->g = sucNodeCost;
            // Set h to be the estimated distance to goal city(Using the heuristic function)
            succCity->h = get_heuristic_cost( succCity, goalCity );
            addList( openList, succCity );
            neighbourNode = neighbourNode->next;
        }

    }
    delList( openList );
    delList( closedList );
    return OK;
}

City* findCity( List* cityList, char* name )
{
    Node* currentNode = cityList->head;
    while( currentNode )
    {
        City* crntCity = ( City* )currentNode->val;
        if( strcasecmp( crntCity->name, name ) == 0)
        {
            return crntCity;
        }
        currentNode = currentNode->next;
    }
    return 0;
}

City* createCity( )
{
    City* res = ( City* )malloc( sizeof( City ) );
    res->name = ( char* )malloc( MAX_LENGTH  );
    res->backptr = 0;
    res->g = 0;
    res->h = 0;
    res->lat = 0;
    res->lang = 0;
    res->neighbour = 0;
    return res;
}
Neighbour* createNeighbour( )
{
    Neighbour* res = ( Neighbour* )malloc( sizeof( Neighbour ) );
    res->name = ( char* )malloc( MAX_LENGTH );
    res->distance = 0;
    return res;

}
void 	destroyCities( List* listCities )
{
    Node* node  = listCities->head;
    while( node )
    {
        City* tempCity = node->val;
        Node* nodeN = tempCity->neighbour->head;
        while( nodeN )
        {
            Neighbour* tempNeigh = nodeN->val;
            destroyNeighbour( &tempNeigh );
            nodeN = nodeN->next;
        }
        destroyCity( &tempCity );
        node = node->next;
    }
    delList( listCities );
}

void destroyCity( City** city )
{
    free( ( *city )->name );
    delList( ( *city )->neighbour );
    free( *city );
}

void destroyNeighbour( Neighbour** neighbour )
{
    free( ( *neighbour)->name );
    free( *neighbour );
}