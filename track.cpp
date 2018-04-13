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
#include "nonmidievent.h"
#include "track.h"

/******************************************************/
/******************* TRACK CLASS **********************/
/******************************************************/

// Track::Track - initialize the track which can be used
//   for creating or storing track data
Track::Track( ) 
{
	runninglen = 0;
	trackfinished = 0;
	bigchunk = new Chunk(ct_track, 0 );
	}	

Track::Track(  unsigned long startlen ) 
{
	runninglen = 0;
	trackfinished = 0;
#if 0
	mfile = mf;
	if( mf->access() ) 
	{  // for writing
		mf->writeheader(); 
	}
#endif
	bigchunk = new Chunk(ct_track, startlen );
}	

// Track::~Track - finish up and flush the track if necessary, 
//   return chunk memory storage
Track::~Track() 
{
//	if( runninglen && !trackfinished ) finish();
	if (bigchunk) delete bigchunk;
}

// Track::recordMevent - record a midi event in the track
int Track::recordMevent( unsigned long dtime, unsigned char status, unsigned char *data, int datalen )
{
	int ret;
	MidiEvent mtemp( dtime );
	mtemp.event( status, data, datalen );
	if( (ret = mtemp.chunkevent( bigchunk ) )) return(ret);
	runninglen += mtemp.dtlen() + 1 + datalen;
	return(0);
}		

// Track::recordNONMevent - record a non midi event in the track
int Track::recordNONMevent( unsigned long dtime, unsigned char *status, int statlen, 
		unsigned long eventlen, unsigned char *data, int datalen )
{
	int ret;	
	nonMidiEvent mtemp( dtime );
	mtemp.eventstat( status, statlen );
	if( eventlen ) mtemp.eventlen( eventlen );
	if( datalen ) mtemp.eventdata( data, datalen );
	if( (ret = mtemp.chunkevent( bigchunk ) )) return(ret);
	runninglen += mtemp.dtlen() + statlen + datalen 
			+ mtemp.txtlen();
	return(0);
}		

void Track::finish(long unsigned end, SeqContext *seqcontext)
{
	snd_seq_event_t *ep;

	seqcontext->seq_midi_echo( end );
	/* else alsa stream close */

	snd_seq_drain_output( seqcontext->seq_handle() );
	snd_seq_event_input( seqcontext->seq_handle(), &ep );

	sleep(1);
	seqcontext->seq_stop_queue();
}

// Track::finish - finish up the track, flush it out
void Track::finish()
{
	// we create a end-of-track marker
	unsigned char eotstatus[3] = { 0xff, 0x2F, 0x00 };
#if DEBUG
	fprintf(stderr, "Track:Creating EndOfTrack marker\n");
#endif
	// end-of-track is a non-midi event
	nonMidiEvent eot( 0 );
	eot.eventstat( eotstatus, 3 );
	eot.chunkevent( bigchunk );
	// adjust our track running length
	runninglen += eot.dtlen() + 3;
	bigchunk->adjust( runninglen );

#if 0
	// now we can flush the chunk out
#if DEBUG
	fprintf(stderr, "Track writing out chunk of len %d(%x)\n", 
		runninglen, runninglen );
#endif
	bigchunk->write( mfile->ostream() );
#endif
	trackfinished = 1;	// we are done
}

int Track::varQtyLen( unsigned char *p )
{
	int n = 1;
	while( *p & 0x80 ) n++;
	return n;
}

// nextEvent() determine next event in the chunk
int Track::nextEvent() 
{
	unsigned char status;
	if( !bigchunk->currentdata() ) 
//		bigchunk->read(mfile->istream());
		return -1;
	status = *(bigchunk->currentdata()+varQtyLen(bigchunk->currentdata() ));
	status = 0xff & status;
#if DEBUG
	fprintf(stderr, "nextEvent looking at status %x\n", status );
#endif
	if( status >= 0xf0 ) return 1; // non midi event
	return 0;  // else it's midi
}

nonMidiEvent *Track::nextnonMidi() 
{
	int statuslen, datalen, varQL;
	unsigned status;
	unsigned char statusbytes[4], *bigbuffer;
	// create event, transfer from chunk in 
	nonMidiEvent *evptr;
	VarQuantity VQ;
	// the first thing is delta time represented as a midi variable quatity
	// determine length of varquantity, then transfer
	varQL = varQtyLen( bigchunk->currentdata() );
	bigchunk->unload( VQ.addr(), varQL );
	VQ.len( varQL );
	evptr = new nonMidiEvent( VQ.value() );
	// can have up to three status bytes here, must look at the first
	status = (unsigned) *(bigchunk->currentdata()) ;
	status = 0xff & status;
	// now we case on this
	switch( status )
	{
	case 0xf0:  // sysex message
		statuslen = 2; break;
	case 0xff:  // this form has three status bytes	
		statuslen = 3; break;
	default:
		fprintf(stderr,"unknown status encountered: %x\n", status );
		exit(3); break;
	}		
	bigchunk->unload( statusbytes, statuslen );
	evptr->eventstat( statusbytes, statuslen );
	datalen = 0;
	if( status == 0xf0 )
	{
		// sysex message length found at second byte
		datalen = (int)statusbytes[1];
		if( datalen )
		{
			// alloocate buffer and transfer
			bigbuffer = new unsigned char[datalen];	
			bigchunk->unload( bigbuffer, datalen );
			evptr->eventdata( bigbuffer, datalen );
			delete bigbuffer;
		}
	}	
	if( status == 0xff ) {
		// other message length found at third byte
		datalen = (int)statusbytes[2];
		if(	datalen )
		{
			// alloocate buffer and transfer
			bigbuffer = new unsigned char[datalen];	
			bigchunk->unload( bigbuffer, datalen );
			evptr->eventdata( bigbuffer, datalen );
			delete bigbuffer;
		}
	}
	evptr->eventlen( statuslen + datalen );			
	// completed non-midi event
	return(evptr);
}	

MidiEvent *Track::nextMidi()
{
	int varQL;
	unsigned char status, databytes[4];
	// create event, transfer from chunk in 
	MidiEvent *evptr;
	VarQuantity VQ;
	// the first thing is delta time represented as a midi variable quatity
	// determine length of varquantity, then transfer
	varQL = varQtyLen( bigchunk->currentdata() );
	bigchunk->unload( VQ.addr(), varQL );
	VQ.len( varQL );
	evptr = new MidiEvent( VQ.value() );
	status = 0;
	if( (unsigned)*( bigchunk->currentdata() ) >= 0x80 ) 
		bigchunk->unload( &status, 1 );
	// assumming here always two data bytes!
	bigchunk->unload( databytes, 2 );
	// fill out event
	evptr->event( status, databytes, 2 ); 
	// completed midi event
	return(evptr);
}	

Chunk *Track::chunk()
{
	return bigchunk;
}

int Track::length()
{
	return runninglen;
}
