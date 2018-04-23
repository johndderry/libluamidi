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
#include "midifile.h"
#include "sortednotes.h"

/******************************************************/
/******************* SORTED NOTES CLASS ***************/
/******************************************************/

SortedNotes::SortedNotes() 
{
	head = nextstart = (struct NoteEventList *) 0;
}
	
SortedNotes::~SortedNotes() 
{
	struct NoteEventList *nexthead;
	while( head ) {
		nexthead = head->next;
		delete head;
		head = nexthead;
		}
}

int SortedNotes::tracknotes( Track *t )
{	
	struct NoteEventList *next = head;
	// track all the notes in the sequence, begin from T=0
	unsigned char buff[2];
	unsigned long deltatime, runningtime = 0;
	int totalN = 0;
	while( next ) 
	{
		if( next->eventtime < runningtime ) 
		{
			fprintf(stderr, "tracknotes problem: eventtime less than running\n");
			return 0;
		}
		deltatime = next->eventtime - runningtime;
		buff[0] = next->pitch;
		buff[1] = next->velocity;
		if( (next->status & 0xf0) == PROGCNG )
			t->recordMevent(deltatime, next->status, buff, 1 );
		else
			t->recordMevent(deltatime, next->status, buff, 2 );
#if DEBUG
		fprintf(stderr, "MidiEvent: dT %x status %x Note %d Veloc %d\n",
			(unsigned)deltatime, next->status, next->pitch, next->velocity );
#endif
		runningtime = next->eventtime;
		if( (next->status & 0xf0) == NOTEOFF ) totalN++;
		next = next->next;
	}
	return totalN;
}

int SortedNotes::verifynotes( )
{	
	struct NoteEventList *next = head;
	// verify the sequence for time order
	unsigned long runningtime = 0;
	while( next )
	{
		if( next->eventtime < runningtime ) 
		{
			fprintf(stderr, "verifynotes: eventtime less than running\n");
			return 0;
		}
		runningtime = next->eventtime;
		next = next->next;
	}
	return 1;
}

struct NoteEventList *SortedNotes::getfirst()
{
	return( head );
}

struct NoteEventList *srchnext(struct NoteEventList *test, 
			unsigned long time ) 
{ 
	struct NoteEventList *lasttest = test;

	if( test->eventtime > time )
		// the test slot time is already past us
		 return(NULL);

	if( test->eventtime == time )
		// the test slot time is the same as us
		 return(test);

	// go until we are before
	while( test && test->eventtime < time )
	{
			lasttest = test;
			test = test->next;
		}
	if( !test || test->eventtime > time ) // too far
		return( lasttest );
	// else right here
	return( test );
}

int SortedNotes::addtolist( unsigned char status, struct Note n, int poff,
		 unsigned long time )
{
	struct NoteEventList *p, *temp, *appendpoint;

	if( !this->verifynotes() ) return -1;
	// create our data item
	if( !(p = new struct NoteEventList)) return 1;
	p->status = status;
	p->pitch = n.pitch + poff;
	p->velocity = n.velocity;
	p->eventtime = time;
	p->next = (struct NoteEventList *)0;
		
	// make this the head of the list if no head
	if( !head ) 
	{
		head = nextstart = p;
		appendpoint = 0;
	}
	// look for insert point from nextstart
	else if( (appendpoint = srchnext( nextstart, time )) )
	{
		// use it
		temp = appendpoint->next;
		appendpoint->next = p;
		p->next = temp;
	}
	// look for insert point from head
	else if( (appendpoint = srchnext( head, time )) )
	{
		// use it
		temp = appendpoint->next;
		appendpoint->next = p;
		p->next = temp;
	}
	else 
	{
		fprintf(stderr, "*opps- can't find ordered place for note *");
		return -1;
	}
#if DEBUG
	fprintf(stderr, "note(s=%x p=%d v=%x) pof=%d tm=%x ADD at %x\n",
		0xff&status, n.pitch, n.velocity, poff,
		(unsigned)time, (long unsigned)appendpoint) ;
#endif
	return( 0 );
}

int SortedNotes::addtolist( int control, int channel, int pitch, int velocity, unsigned long time )
{
	struct NoteEventList *p, *temp, *appendpoint;

	if( !this->verifynotes() ) return -1;
	// create our data item
	if( !(p = new struct NoteEventList)) return 1;
  
	switch( control ) {
		case 2: p->status = PROGCNG | channel; break;
		case 1: p->status = NOTEON | channel; break;
		case 0: p->status = NOTEOFF | channel; break;
	}
    p->pitch = pitch;
	p->velocity = velocity;
	p->eventtime = time;
	p->next = (struct NoteEventList *)0;
		
	// make this the head of the list if no head
	if( !head ) 
	{
		head = nextstart = p;
		appendpoint = NULL;
	}
	// look for insert point from nextstart
	else if( (appendpoint = srchnext( nextstart, time )) )
	{
		// use it
		temp = appendpoint->next;
		appendpoint->next = p;
		p->next = temp;
	}
	// look for insert point from head
	else if( (appendpoint = srchnext( head, time )) )
	{
		// use it
		temp = appendpoint->next;
		appendpoint->next = p;
		p->next = temp;
	}
	else 
	{
		fprintf(stderr, "*opps- can't find ordered place for note *");
		return -1;
	}
#if DEBUG
	fprintf(stderr, "note(s=%x p=%d v=%x) tm=%x ADD at %x\n",
		0xff&p->status, p->pitch, p->velocity,
		(unsigned)p->eventtime, (unsigned long)appendpoint) ;
#endif
	return( 0 );
}

int SortedNotes::numevents( ) {
	int count = 0;
	struct NoteEventList *p = head;
	while( p ) {
		count++;
		p = p->next;
	}
	return count;
}
