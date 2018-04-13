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
/******************************************************/
/******************* TRACK CLASS **********************/
/******************************************************/
/**
  Used to create and maintain midi track data
*/
class Track {
	int trackfinished;			// completion flag
	unsigned long runninglen;	// track running length
	Chunk *bigchunk;			// pointer to our accumulation or load chunk
	int varQtyLen(unsigned char *);		// determine byte count in varQ
public:
	Track( unsigned long );
	Track();
	~Track();
	int recordMevent( unsigned long, unsigned char, unsigned char *, int ) ;
	int recordNONMevent( unsigned long, unsigned char *, int, unsigned long, unsigned char *, int ) ;
	int nextEvent() ;
	nonMidiEvent *nextnonMidi();	
	MidiEvent *nextMidi();
	void finish();
	void finish(unsigned long, SeqContext *);
	Chunk *chunk();
	int length();
};
