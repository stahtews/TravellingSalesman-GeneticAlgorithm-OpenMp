#include <iostream>  // cout
#include <fstream>   // ifstream
#include <string.h>  // strncpy
#include <stdlib.h>  // rand
#include <math.h>    // sqrt, pow
#include <omp.h>     // OpenMP
#include "Timer.h"   //elapsed time time.lap()
#include "Trip.h"    // constants and trip class
#include <algorithm> // g++ sort algorithm
#include <stdbool.h> // bool datatype


using namespace std;

void initialize( Trip trip[CHROMOSOMES], int coordinates[CITIES][2] ); // read already existing chromosome.txt into trip objects and itinerary members
void select( Trip trip[CHROMOSOMES], Trip parents[TOP_X] ); // select top half of the minimum distance best chromosomes for the purpose of spawning the offsprings
void populate( Trip trip[CHROMOSOMES], Trip offsprings[TOP_X] ); // append offsprings to top half of trip
int findCharVal(char);


// swap 2 genes in each chromosome at 50% probability

void mutate( Trip offsprings[TOP_X] )
{
  for(int i=0 ; i < TOP_X ; i++)
  {
    int randIndex ;
    randIndex = rand() % 100; // to get random numbers between 0 to 100

    if(randIndex < MUTATE_RATE )
    {
       int temp;
       int randIndexGene1;
       randIndexGene1 = rand() % CITIES;
       int randIndexGene2;
       randIndexGene2 = rand() % CITIES;

       while(randIndexGene1 == randIndexGene2)
       randIndexGene2 = rand() % CITIES;

       temp = offsprings[i].itinerary[randIndexGene1];
       offsprings[i].itinerary[randIndexGene1]= offsprings[i].itinerary[randIndexGene2] ;
       offsprings[i].itinerary[randIndexGene2]= temp;
  }
  }
}


// operator for sort
bool operator<(Trip const & a, Trip const & b)
{
  return a.fitness <b.fitness;
}

//generate compliment char of each gene in chromosome, example 9 is the compliment of A ,8 is the compliment of B etc
void getCompliment(char *a,char *b,char *reference)
{
  for(int i = 0; i < CITIES; i++)
  {
    b[i] = reference[35 - findCharVal(a[i])];
  }
}


//finds the city or position equivalent of each character. example (A=0,B=1 .... 9=35)
int findCharVal(char c)
{
  if(c >= 'A')
  {
    return (int)(c -'A') ;
  }
  else
  {
    return ((int)(c) - 22);
  }
 // char *index = strchr(reference,c);
 // return index != NULL ? index-reference : -1;

}

//finds distance between two points using the formula sqrt(pow(xdistance,2)+pow(ydistance,2))
float findDistance(char a , char b,int coordinates[CITIES][2] )
{
 int x1 = 0;

 x1 = coordinates[findCharVal(a)][0];
 int y1 =0 ;
 y1 = coordinates[findCharVal(a)][1];
 int x2 ;
 x2 = coordinates[findCharVal(b)][0];
 int y2 ;
 y2 = coordinates[findCharVal(b)][1];
 float distanceX ;
 distanceX = pow((x2-x1),2);
 float distanceY ;
 distanceY = pow((y2-y1),2);
 float result = sqrt(distanceX + distanceY);
 return result;
}


//finds the position of char c in array a
int findPos(const char *a , char c)
{
        for(int i=0 ; i < strlen(a) ; i++)
        {
          if(a[i] == c)
          {
          return int(i);
          }

        }
return -1;
}


// creates offspirngs using 2 chromosomes from parents using greedy crossover algorithm
 void crossover( Trip *parents, Trip *offsprings, int coordinates[CITIES][2], char *reference )
 {
       // iterate through all the parents . takes 2 parents at a time
       #pragma omp parallel for
       for(int chromosomeIndex = 0 ;  chromosomeIndex < TOP_X; chromosomeIndex += 2  )
       {
          char tempChildIti1[37];
          char tempChildIti2[37];
          tempChildIti1[36]='\0';
          tempChildIti2[36]='\0';
          bool leftOverChar[37] = { false };
          leftOverChar[36] = '\0';
    int indexP1;
          indexP1 = 0;  // index of next element in parent one
          int indexP2;
          indexP2 = 0;  // index of next element in parent two
          char nextCharInP1 = '\0';
          char nextCharInP2= '\0';
          int distance1=0;
          int distance2 = 0;

          //iterate through all genes or cities in a given chromosome
         for(int index = 1 ; index < CITIES; index++)
         {
           char temp;
           tempChildIti1[0]= parents[chromosomeIndex].itinerary[0];
           char  currTempVar;
           currTempVar = tempChildIti1[index - 1];
           leftOverChar[findCharVal(currTempVar)] = true;

           indexP1 = (findPos(parents[chromosomeIndex].itinerary ,currTempVar) + 1) % CITIES;
           indexP2 = (findPos(parents[chromosomeIndex + 1].itinerary , currTempVar) + 1 ) % CITIES;
           nextCharInP1 = parents[chromosomeIndex].itinerary[indexP1];
           nextCharInP2 = parents[chromosomeIndex + 1].itinerary[indexP2];
           bool present1 = leftOverChar[findCharVal(nextCharInP1)] ; // checks if next char is present in parent1
           bool present2 = leftOverChar[findCharVal(nextCharInP2)] ;// checks if next char is present in parent2

           if((present1 == false) && (present2 == false) ) // both the characters are not present in parent
           {

           temp = (findDistance(currTempVar, nextCharInP1,coordinates) < findDistance(currTempVar, nextCharInP2,coordinates)) ? nextCharInP1 : nextCharInP2 ;
           }
           else if((present1 == false) &&  (present2 = true)) // next char in parent 1 is not  present in child but next char in parent 2 is present in child
           {
             temp =  nextCharInP1;

           }
           else if((present1 == true) &&  (present2 == false)) // next char in parent 1 is present in child but next char in parent 2 is not present in child
           {
             temp = nextCharInP2;

           }
           else //next char from both parent1 and parent2 are present in child hence choose random
           {
             int randIndex = rand() % CITIES;
  while(leftOverChar[randIndex] == true )
             randIndex = (randIndex + 1) % 36;
             temp = (char) ( (randIndex < 26) ? randIndex + 65 : randIndex + 22) ;

           }

           leftOverChar[findCharVal(temp)]= true;
           tempChildIti1[index] = temp;
         }
          // gets the compliment of each chromosome or itinerary
         getCompliment(tempChildIti1, tempChildIti2 , reference);
         strcpy(offsprings[chromosomeIndex].itinerary, tempChildIti1);
         strcpy(offsprings[chromosomeIndex+1].itinerary, tempChildIti2);

       }
}

//finds the distance between all the cities from first to last.
void evaluate(Trip trip[CHROMOSOMES], int coordinates[CITIES][2])
{
  #pragma omp parallel for
  for(int i = 0 ;i < CHROMOSOMES ; i++)
  {
    float  sum =0;
//iterate through all the genes in a given chromosome
    for(int  index = 0 ; index < CITIES - 1 ; index++)
    {
       sum = sum + findDistance(trip[i].itinerary[index], trip[i].itinerary[index+1],coordinates );
    }
    trip[i].fitness = sum;
  }
}

/*
 * MAIN: usage: Tsp #threads
 */
int main( int argc, char* argv[] ) {

 char reference[37] ={'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9','\0'};

  Trip trip[CHROMOSOMES];       // all 50000 different trips (or chromosomes)


  Trip shortest;                // the shortest path so far
  int coordinates[CITIES][2];   // (x, y) coordinates of all 36 cities:
  int nThreads = 1;

  // verify the arguments
  if ( argc == 2 )
    nThreads = atoi( argv[1] );
  else {
    cout << "usage: Tsp #threads" << endl;
    if ( argc != 1 )
      return -1; // wrong arguments
  }
  cout << "# threads = " << nThreads << endl;

  // shortest path not yet initialized
  shortest.itinerary[CITIES] = 0;  // null path
  shortest.fitness = -1.0;         // invalid distance

  // initialize 5000 trips and 36 cities' coordinates
  initialize( trip, coordinates );

  // start a timer
  Timer timer;
  timer.start( );

  // change # of threads
  omp_set_num_threads( nThreads );

  // find the shortest path in each generation
  for ( int generation = 0; generation < MAX_GENERATION ; generation++ ) {

    // evaluate the distance of all 50000 trips
    evaluate( trip, coordinates );

//    constexpr std::size_t size = sizeof trip / sizeof *trip;
//    std::qsort( trip, size , sizeof(char *),(int(*)(const void *, const void *)) cmp );
    std::sort( trip, trip + 50000 );


//     just print out the progress
    if ( generation % 20 == 0 )
    cout << "generation: " << generation << endl;
 // whenever a shorter path was found, update the shortest path
    if ( shortest.fitness < 0 || shortest.fitness > trip[0].fitness ) {

      strncpy( shortest.itinerary, trip[0].itinerary, CITIES );
      //shortest.itinerary = trip[0].itinerary;
      shortest.fitness = trip[0].fitness;

      cout << "generation: " << generation
           << " shortest distance = " << shortest.fitness
           << "\t itinerary = " << shortest.itinerary << endl;
    }

    // define TOP_X parents and offsprings.
    Trip parents[TOP_X], offsprings[TOP_X];

    // choose TOP_X parents from trip
    select( trip, parents );

    // generates TOP_X offsprings from TOP_X parenets
    crossover( parents, offsprings, coordinates,reference );

    // mutate offsprings
   mutate( offsprings );

  //   populate the next generation.
   populate( trip, offsprings );
  }

  // stop a timer
  cout << "elapsed time = " << timer.lap( ) << endl;
  return 0;
}



/*
 * Initializes trip[CHROMOSOMES] with chromosome.txt and coordiantes[CITIES][2] with cities.txt
 *
 * @param trip[CHROMOSOMES]:      50000 different trips
 * @param coordinates[CITIES][2]: (x, y) coordinates of 36 different cities: ABCDEFGHIJKLMNOPQRSTUVWXYZ
 */
void initialize( Trip trip[CHROMOSOMES], int coordinates[CITIES][2] ) {
  // open two files to read chromosomes (i.e., trips)  and cities
  ifstream chromosome_file( "chromosome.txt" );
  ifstream cities_file( "cities.txt" );

  // read data from the files
  // chromosome.txt:
  //   T8JHFKM7BO5XWYSQ29IP04DL6NU3ERVA1CZG
  //   FWLXU2DRSAQEVYOBCPNI608194ZHJM73GK5T
  //   HU93YL0MWAQFIZGNJCRV12TO75BPE84S6KXD
  for ( int i = 0; i < CHROMOSOMES; i++ ) {
    chromosome_file >> trip[i].itinerary;
    trip[i].fitness = 0.0;
  }

  // cities.txt:
  // name    x       y
  // A       83      99
  // B       77      35
  // C       14      64
  for ( int i = 0; i < CITIES; i++ ) {
    char city;
    cities_file >> city;
    int index = ( city >= 'A' ) ? city - 'A' : city - '0' + 26;
    cities_file >> coordinates[index][0] >> coordinates[index][1];

  }

  // close the files.
  chromosome_file.close( );
  cities_file.close( );

  // just for debugging
  if ( DEBUG ) {
    for ( int i = 0; i < CHROMOSOMES; i++ )
      cout << trip[i].itinerary << endl;
    for ( int i = 0; i < CITIES; i++ )
      cout << coordinates[i][0] << "\t" << coordinates[i][1] << endl;
  }
}

/*
 * Select the first TOP_X parents from trip[CHROMOSOMES]
 *
 * @param trip[CHROMOSOMES]: all trips
 * @param parents[TOP_X]:    the firt TOP_X parents
 */
void select( Trip trip[CHROMOSOMES], Trip parents[TOP_X] ) {
  // just copy TOP_X trips to parents
  for ( int i = 0; i < TOP_X; i++ )
    strncpy( parents[i].itinerary, trip[i].itinerary, CITIES + 1 );
}

/*
 * Replace the bottom TOP_X trips with the TOP_X offsprings
 */
void populate( Trip trip[CHROMOSOMES], Trip offsprings[TOP_X] ) {
  // just copy TOP_X offsprings to the bottom TOP_X trips.
  for ( int i = 0; i < TOP_X; i++ )
{    strncpy( trip[ CHROMOSOMES - TOP_X + i ].itinerary, offsprings[i].itinerary, CITIES + 1 );


}
  // for debugging
  if ( DEBUG ) {
    for ( int chrom = 0; chrom < CHROMOSOMES; chrom++ )
      cout << "chrom[" << chrom << "] = " << trip[chrom].itinerary
           << ", trip distance = " << trip[chrom].fitness << endl;
  }
}
