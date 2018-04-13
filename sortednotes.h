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
// Add some support classes for libmdl data handling
// including SortedNotes, PlayerPart

#include <string.h>

#define PROGCNG	0xC0
#define NOTEON 	0x90
#define NOTEOFF	0x80
#define NoteSeqLabelLen 16

#define MAXINSTNAME 128
#define MAXPLAYNAME 128
#define MAXPLAYINST 32

/**
	Basic description of a midi note event
*/
struct Note {
	unsigned char pitch;
	unsigned char velocity;
	unsigned int duration;
	} ;
	
/**
	Used to describe an element in a linked list of midi note events
*/
struct NoteEventList {
	unsigned char status;
	unsigned char pitch;
	unsigned char velocity;
	unsigned long eventtime;
	struct NoteEventList *next;
	} ;

/******************************************************/
/******************* SORTED NOTES CLASS ***************/
/******************************************************/
/**
	Holds notes in a time-sorted sequence for conversion
	to type 1 midi format:
	all notes for all midi channels together in one sequence.
*/
class SortedNotes {
	struct NoteEventList *head, *nextstart;
public:
	SortedNotes();
	~SortedNotes();
	struct NoteEventList *getfirst();
	int tracknotes(Track *);
	int verifynotes();
	int addtolist( unsigned char status, struct Note n, int poff, unsigned long t );	
	int addtolist( int noteon, int channel, int pitch, int velocity, unsigned long t );	
	int numevents();
};

