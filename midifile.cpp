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

/******************************************************/
/****************** MIDIFILE CLASS ********************/
/******************************************************/

// midifile::midifile - create, initialize the file
MidiFile::MidiFile( char *filename, char *access ) 
{
	if( strchr( access, 'r' ) )
	{
		accessmode = 0;
		midiin = fopen( filename, "r" );
		if( !midiin )
		{
			fprintf(stderr, "error opening midifile %s for input\n", filename );
			exit(1);
		}
	}	
	else if( strchr( access, 'w' ) ) 
	{
		accessmode = 1;
		midiout = fopen( filename, "w");
		if( !midiout  ) 
		{
			fprintf(stderr, "error opening midifile %s for output\n", filename );
			exit(2);
		}
	}
	else 
	{
		fprintf(stderr, "invalid accessmode %s\n", access );
		exit(2);
	}
				
	if( accessmode )
	{	
		// initialize the actual header storage with defaults
		hdrbytes[0] = hdrbytes[2] = hdrbytes[4] = 0; // upper byte zeroed
		// presently, only do midi type 1
		hdrbytes[1] = hdrbytes[3] = 1; 	// type 1, 1 track */
		hdrbytes[5] = 0x60;  /* = 96 ppqn */
	}
	else 
	{
		readheader();
#if DEBUG
		fprintf(stderr,"Header is read: type=%x tracks=%x timing=%x\n",
			type(), tracks(), timing() );
#endif			
	}	
}
	
// these allow modification of the defaults
void MidiFile::type( unsigned char v ) 
{
	hdrbytes[1] = v;
}
	
void MidiFile::tracks( unsigned char v )
{
	hdrbytes[3] = v;
}
	
void MidiFile::timing( unsigned char v ) 
{
	hdrbytes[5] = v;
}
	
// these return the current header values
unsigned char MidiFile::type( )
{
	return hdrbytes[1];
}
	
unsigned char MidiFile::tracks( ) {
	return hdrbytes[3];
	}
	
unsigned char MidiFile::timing( )
{
	return hdrbytes[5];
}
	
int MidiFile::access( ) {
	return accessmode;
	}
	
// midifile::writeheader - chunk and flush the header 
void MidiFile::writeheader() 
{
	Chunk header(ct_header,6);
	header.load(hdrbytes, 6);
	header.write(midiout);
}

// midifile::readheader - get the header chunk from file 
void MidiFile::readheader() 
{
	Chunk header(ct_header,6);
	header.read(midiin);
	header.unload(hdrbytes, 6);
}

// midifile::~midifile - close the file stream
MidiFile::~MidiFile() 
{
	if( accessmode ) fclose( midiout );
	else			 fclose( midiin );
}

FILE *MidiFile::ostream() 
{
	// return a stream reference to use with chunk::write()
	return( midiout );
}

FILE *MidiFile::istream() 
{
	// return a stream reference to use with chunk::read()
	return( midiin );
}

int MidiFile::readchunk( Track *track )
{
	return track->chunk()->read( midiin );
}

int MidiFile::writechunk( Track *track )
{
	writeheader();
	track->finish();
	return track->chunk()->write( midiout );
}

