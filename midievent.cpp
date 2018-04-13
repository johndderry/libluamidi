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
#include "midievent.h"

/******************************************************/
/******************* MIDIEVENT CLASS ******************/
/******************************************************/

// MidiEvent::MidiEvent - start a midi event with deltatime supplied
MidiEvent::MidiEvent(unsigned long dt)
{ 
	deltime = new VarQuantity( dt );	// create a delta time with the value
}

MidiEvent::~MidiEvent() 
{
	delete deltime;
}
		
// MidiEvent::event - indicate the status, and tranfer midi event data
void MidiEvent::event( unsigned char stat, unsigned char *data, int datalen )
{
	unsigned char *p = databytes;		// pointer to destination in our class
	status = stat;						// update status
	if( datalen > 2 ) return;	  		// sanity check 
	databyteslen = datalen;				// update length
	while( datalen-- ) *p++ = *data++;	// tranfer bytes
}

// MidiEvent::chunkevent - add (this) event to the chunk supplied
int MidiEvent::chunkevent( Chunk *ch )
{
	// give some verbage
#if DEBUG
	fprintf(stderr, "ChunkingEvent:DeltaTime=");
    deltime->showtime();
	fprintf(stderr, " Status=%x %x:%x\n", 0xff & status, 
		0xff & databytes[0], 0xff & databytes[1] );
#endif
	// now make the tranfer
	if( !ch->load( deltime->addr(), deltime->len() ) ||
		!ch->load( &status, 1) || !ch->load( databytes, databyteslen ) )
		return(1);
	// normal return
	return(0);
}

// MidiEvent::getdtlen - return the delta time length 
int MidiEvent::dtlen()
{
	return( deltime->len() );
}

