/*
 * JRandom.c
 *
 * "JamieRandom": A hacked-up non-OOP version of version 1.1.1 of CRandom.
 * CRandom is available by ftp from ftp.brown.edu in /pub/tcl/classes.
 *
 * JRandom.c and JRandom.h are in the public domain.
 *
 * MLH: Adapted the code to make it POSIX compliant
 */



/******************************/

#include "JRandom.h"

/******************************/

// MLH: Changed includes
//#include <Global.h>
#include <time.h>

/******************************/

#define MULTIPLICAND (4096)
#define MULTIPLY(x) ((x) << 12)

#define ADDEND (150889)
#define ADD(x) ((x) + ADDEND)

	/* this probably isn't really a word, but what the hell */
#define MODULEND (NDISTINCTRANDOMVALUES)
#define MODULO(x) ((x) % MODULEND)

#define NEXTVAL(x) (MODULO(ADD(MULTIPLY(x))))

/******************************/



void IJRandom(jrPtr theJRPtr)
{
//	unsigned long theDateTime;
//	GetDateTime(&theDateTime);
//	jrSeed(theJRPtr, theDateTime ^ TickCount());

//	MLH: changed the initialization to posix style

	time_t t;
	time(&t);
	
	jrSeed(theJRPtr, t ^ clock());
}

#define abs(a) ((a)<0?-(a):(a))

void jrSeed(register jrPtr theJRPtr,
	unsigned long seedVal)
{
	register short j;
	
//	theJRPtr->dummy = MODULO(Abs(ADDEND - seedVal));
// MLH: Changed Abs to abs
	theJRPtr->dummy = MODULO(abs(ADDEND - seedVal));
	for (j = NRANDOMSLOTS-1; j >= 0; j--) {
		theJRPtr->seeds[j] = theJRPtr->dummy = NEXTVAL(theJRPtr->dummy);
	}
	theJRPtr->value = theJRPtr->dummy = NEXTVAL(theJRPtr->dummy);
}



short jrLinearShort(register jrPtr theJRPtr,
	register short lowest, short highest)
{
	register long interval = highest - lowest + 1;
	register long partitionSize;
	register short j;
	
	if (interval <= 0) return lowest;
	partitionSize = (NDISTINCTRANDOMVALUES-1)/interval;
	while (true)
	{
		j = (NRANDOMSLOTS*theJRPtr->value) / NDISTINCTRANDOMVALUES;
		theJRPtr->value = theJRPtr->seeds[j];
		theJRPtr->seeds[j] = theJRPtr->dummy = NEXTVAL(theJRPtr->dummy);
		if (theJRPtr->value < (unsigned long)(partitionSize * interval)) {
			return lowest + theJRPtr->value/partitionSize;
		}
	}
}

