/*
MIT License

Copyright (c) 2018 John D. Derry

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <stdio.h>
#include "varquantity.h"

/******************************************************/
/***************** VARQUANTITY CLASS ******************/
/******************************************************/

VarQuantity::VarQuantity(unsigned long val )
{
	// VarQuantity::VarQuantity - 1st form converts passed value to midi variable-len value
	unsigned long buffer = val & 0x7f;	// first 7-bit value can be had immediately

	varquantitylen = 0;
	// stuff off each 7-bit value from input into unsigned long word (buffer)
	while( (val >>= 7 ) ) 
	{	// while more 7-bitters
		buffer <<= 8;
		buffer |= ((val & 0x7f) | 0x80 );
	}
	while( 1 )
	{	// now count the length as we copy back to varquantity
		varquantity[varquantitylen] = (char) (0xff & buffer);
		varquantitylen++;
		if( buffer & 0x80)	buffer >>=8;
		else				break;
	}
}

// 2nd form converts converts variable-len value in stream to a binary value
VarQuantity::VarQuantity()
{
	varquantitylen = 0;
}

unsigned long VarQuantity::value()
{
	// this resembles MostSignicantByteFirst
	unsigned long buffer;	
	int x = 0;
	// extract true byte, throw away the MSBit, shift up
	while( x < varquantitylen ) 
	{	// while more bytes
		buffer <<= 7;
		buffer |= (varquantity[x] & 0x7f);
		x++;
	}
	if( varquantity[--x] & 0x80 )
	{
		fprintf(stderr, "variable-length value inconstency found: ");
		for( x=0; x < varquantitylen; x++ ) fprintf(stderr, "%x ", (unsigned) varquantity[x] );
		fprintf(stderr, "\n");
	}  
	return( buffer );
}
		  
VarQuantity::~VarQuantity() { };

// len - set the length after copy stream into buffer
void VarQuantity::len( int len )
{
	varquantitylen = len;
}

// VarQuantity::getaddr ::getlen ::showtime - return various stuff
unsigned char *VarQuantity::addr()
{
	return(  varquantity );
}

int VarQuantity::len()
{
	return(  varquantitylen );
}

void VarQuantity::showtime()
{
	unsigned char *p = varquantity;
	int i = varquantitylen;
	fprintf(stderr,"(%x)", varquantitylen );
	while( i-- ) fprintf(stderr,"%x ", 0xff & *p++);
};

// note here
// we have no dump method, instead raw copy is used with getaddr()
