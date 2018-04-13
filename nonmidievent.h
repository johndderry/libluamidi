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
/**************** NON-MIDIEVENT CLASS *****************/
/******************************************************/
/**
  Used to create non-midi (sysex) event data in a track
*/
class nonMidiEvent {
	int statusbyteslen;	// actual length of statusbytes 
	int databyteslen;	// actual length of databytes 
	unsigned char *databytes;	// pointer to data bytes
	unsigned char status[4];		// at most three status bytes
	VarQuantity *deltime;	// pointer to deltatime
	VarQuantity *textlen;	// pointer to textlen if we create
public:
	nonMidiEvent(unsigned long dt);
	~nonMidiEvent();
	void eventstat( unsigned char *, int );
	int eventstat( unsigned char * );
	void eventlen( unsigned long  );
	unsigned long eventlen( );
	void eventdata( unsigned char *, int );
	int eventdata( unsigned char * );
	int chunkevent( Chunk * );  
	int dtlen();
	int txtlen();
};

