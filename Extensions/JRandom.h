/*
 * JRandom.h
 *
 * "JamieRandom": A hacked-up non-OOP version of version 1.1.1 of CRandom.
 * CRandom is available by ftp from ftp.brown.edu in /pub/tcl/classes.
 *
 * JRandom.c and JRandom.h are in the public domain.
 *
 */



/******************************/

#ifndef JRANDOM_H
#define JRANDOM_H

/******************************/

#define NRANDOMSLOTS (97)
#define NDISTINCTRANDOMVALUES (714025)

/******************************/

typedef struct {
	unsigned long		seeds[NRANDOMSLOTS];
	unsigned long		dummy;
	unsigned long		value;
} jrStruct, *jrPtr;

/******************************/



extern void IJRandom(jrPtr theJRPtr);
extern void jrSeed(jrPtr theJRPtr, unsigned long seedVal);
extern short jrLinearShort(jrPtr theJRPtr, short lowest, short highest);

#endif
