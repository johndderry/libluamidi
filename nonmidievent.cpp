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
#include <stdlib.h>
#include <ctype.h>
#include <alsa/asoundlib.h>
#include "seqcontext.h"
#include "chunk.h"
#include "varquantity.h"
#include "nonmidievent.h"

/******************************************************/
/**************** NON-MIDIEVENT CLASS *****************/
/******************************************************/

// nonMidiEvent::nonMidiEvent - start a non midi event with deltatime supplied
nonMidiEvent::nonMidiEvent(unsigned long dt)
{ 
	deltime = new VarQuantity( dt );	// create a delta time with the value
	databyteslen = statusbyteslen = 0;	// zero the storage lengths
	textlen = NULL;					// indicates no text length present
}

nonMidiEvent::~nonMidiEvent()
{ 
	if( textlen ) delete textlen;
	delete deltime;
}
		
// nonMidiEvent::eventstat - supply the status and status length
void nonMidiEvent::eventstat( unsigned char *stat, int statlen )
{
	unsigned char *p = status;	// destination in our class
	statusbyteslen = statlen;
	// make the transfer 
	while( statlen-- ) *p++ = *stat++;
	}

// nonMidiEvent::eventstat - return the status and status length
int nonMidiEvent::eventstat( unsigned char *stat ) 
{
	int statlen = statusbyteslen;
	unsigned char *p = status;	// source in our class
	// make the transfer 
	while( statlen-- ) *stat++ = *p++;
	return( statusbyteslen );
	}

// nonMidiEvent::eventdata - supply the event data and data length
void nonMidiEvent::eventdata( unsigned char *data, int datalen ) 
{
	unsigned char *p;		// our local copy
	databytes = p = new unsigned char [datalen];	// get dynamic storage for this
	databyteslen = datalen;
	// make the transfer
	while( datalen-- ) *p++ = *data++;
}

// nonMidiEvent::eventdata - return the event data and data length
int nonMidiEvent::eventdata( unsigned char *data ) 
{
	int datalen = databyteslen;
	unsigned char *p = databytes;		// from our local copy
	// make the transfer
	while( datalen-- ) *data++ = *p++;
	return( databyteslen );
}

// nonMidiEvent::eventlen - supply the event text length
void nonMidiEvent::eventlen( unsigned long v ) 
{
	textlen = new VarQuantity( v );	// create a variable length value
}

// nonMidiEvent::eventlen - return the event text length
unsigned long nonMidiEvent::eventlen( )
{
	if( textlen ) return( textlen->value() );
	else return 0;
}

// nonMidiEvent::chunkevent - add (this) event to the chunk supplied
int nonMidiEvent::chunkevent( Chunk *ch )
{
	if( !ch->load( deltime->addr(), deltime->len() ) ||
		!ch->load( status, statusbyteslen ) ) return(1);
	if( textlen && !ch->load( textlen->addr(), textlen->len() ))
		return(1);
	if( databyteslen && !ch->load( databytes, databyteslen )) return(1);
	return(0);
} 
		
// nonMidiEvent::getdtlen - return the delta time length 
int nonMidiEvent::dtlen() 
{
	return( deltime->len() );
}

// nonMidiEvent::getlenlen - return the text length 
int nonMidiEvent::txtlen() 
{
	if( textlen ) return( textlen->len() );
	else return( 0 );
}

