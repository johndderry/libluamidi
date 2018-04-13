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
#include <alsa/asoundlib.h>
#include "seqcontext.h"
#include "chunk.h"

/******************************************************/
/******************** CHUNK CLASS *********************/
/******************************************************/

#if 1
void Chunk::copybytes( unsigned char *d, unsigned char *s, unsigned int len )
{
	// copy bytes as unsigned characters, no null terminator
	while( len-- ) *d++ = *s++;
}
#endif

void Chunk::headerlen( unsigned char *s, unsigned len ) 
{
	// build a four byte header length at address
	s += 3;
	*s-- = (char)(len & 0xff);
	len = len >> 8;
	*s-- = (char)(len & 0xff);
	len = len >> 8;
	*s-- = (char)(len & 0xff);
	len = len >> 8;
	*s   = (char)(len & 0xff);
}

// chunk::chunk - initialize the chunk header, get ready for loading
Chunk::Chunk(	enum chunktype type, 		// pass the type
				unsigned len  				// and the overall (max)chunk length 
				) 
{
	// two cases to consider: midifile is reading, chunk storage space
	// is allocated after reading the header; or midifile is writing
	// and chuck space is created here because size is known
	if( type == ct_header )			copybytes(header.ID, (unsigned char *)"MThd", 4);
	else if ( type == ct_track )	copybytes(header.ID, (unsigned char *)"MTrk",  4);
	// else we don't know yet
	length = len;	
	start = current = NULL;
	// should be 0 for reading case
	if( length ) {
		// if length is passed, create the buffer now
		headerlen(header.length, length );
		start = current = new unsigned char [length];
		if( start == NULL ) exit(3);
	}
	return;
}

// chuck::~chuck - free up dynamic storage
Chunk::~Chunk() 
{
	if( length ) delete start;
}

// chunk::load - load the chunk with data from somewhere, updating pointers
unsigned Chunk::load(	unsigned char *p, 		// "load from" pointer
						unsigned l 				// length of load data
					 	) 
{
	// make sure we don't overwrite buffer
	if( current + l > start + length ) {
		fprintf(stderr, "BUFFER OVERWRITE, %d over\n", current + l - start - length ); 
		return(0);	
	}
	
	copybytes( current, p, l );
#if DEBUG
		int l2=l; unsigned char *p2=p;
		fprintf(stderr, "ChnkLoad(a=%x,l=%x):", current - start, l );
		while (l2--) fprintf(stderr, "%x ", 0xff & *p2++ );
		fprintf(stderr, "\n");
#endif
	current += l;	
	return(l);
	}

// chunk::unload - unload the chunk data to another area
unsigned Chunk::unload(	unsigned char *p, 		// "load to" pointer
						unsigned l 				// length of load data
					 	) 
{
	if( current + l > start + length ) // can't overread buffer
		fprintf(stderr, "BUFFER OVERREAD, %d over\n", current + l - start - length ); 
		return(0);
	copybytes( p, current, l );
#if DEBUG
		int l2=l; unsigned char *p2=p;
		fprintf(stderr, "ChnkUnload(a=%x,l=%x):", current - start, l );
		while (l2--) fprintf(stderr, "%x ", 0xff & *p2++ );
		fprintf(stderr, "\n");
#endif
	current += l;	
	return(l);
}

unsigned Chunk::len()
{
	return length;
}

// chunk::adjustlen - fix the length of the chunk, return old length
int Chunk::adjust( unsigned len )
{
	int oldlen = length;
	length = len;
	headerlen(header.length, length );
	return(oldlen);
}

// chunk::write - flush the chunk to output
int Chunk::write( FILE *s )
{
#if DEBUG
	fprintf(stderr, "ChnkWrite(to file) len of %d(%x)\n", length, length );
#endif
	if(	fwrite( (char *) &header, 1, 8, s ) < 8 ||
		fwrite( (char *) start, 1, length, s ) < length ) 
		return 1;
	return(0);
}

int Chunk::write( SeqContext *c, snd_seq_addr_t *s )
{
#if DEBUG
	fprintf(stderr, "ChnkWrite(to port) len of %d(%x)\n", length, length );
#endif
//	if(	al_fwrite( s,  (char *) &header, 8) < 8 ||
//		al_fwrite( s,  (char *) start, length ) < length ) 
//		return 1;
	fprintf(stderr, "**Not Supported: ChnkWrite(to port) len of %d(%x)\n", length, length );
	return(0);
}

// chunk::read - fetch the chunk from source
int Chunk::read( FILE *s ) 
{
	unsigned oldlen;
	oldlen = length; // sometimes we know read length, use this to verify
	fread( (char *) &header, 1, 8, s );
	length = (unsigned)(header.length[3]) + 
		((unsigned)(header.length[2])<<8) +
		((unsigned)(header.length[1])<<16) + 
		((unsigned)(header.length[0])<<24) ; 
#if DEBUG
	fprintf(stderr, "ChnkRead(from file) oldlen %x new %x\n", oldlen, length );
#endif
	if( oldlen > 0 && oldlen != length )
	{
		fprintf(stderr,
			"Consistency error:read chunk oldlen=%d newlen=%d, quitting\n",
			(unsigned)oldlen, (unsigned)length );
		exit(2);
	}
	// now we can allocate the space
	if( oldlen == 0 ) 
	{
		start = current = new unsigned char [length];
		if( start == NULL ) exit(3);
	}
	// and read it in		
	if( fread( (char *)start, 1, length, s ) < length )
		return  1;
	return(0);
}
	
int Chunk::read( SeqContext *c, snd_seq_addr_t *s ) 
{
	unsigned oldlen;
	oldlen = length; // sometimes we know read length, use this to verify
//	al_fread( s, (char *) &header, 8 );
	length = (unsigned)(header.length[3]) + 
		((unsigned)(header.length[2])<<8) +
		((unsigned)(header.length[1])<<16) + 
		((unsigned)(header.length[0])<<24) ; 
#if DEBUG
	fprintf(stderr, "ChnkRead(from port) oldlen %x new %x\n", oldlen, length );
#endif
	if( oldlen > 0 && oldlen != length )
	{
		fprintf(stderr,
			"Consistency error:read chunk oldlen=%d newlen=%d, quitting\n",
			(unsigned)oldlen, (unsigned)length );
		exit(2);
	}
	// now we can allocate the space
	if( oldlen == 0 ) 
	{
		start = current = new unsigned char [length];
		if( start == NULL ) exit(3);
	}
	// and read it in		
//	if( al_fread( s, (char *)start, length ) < length )
//		return  1;
	return(0);
}
	
unsigned char *Chunk::currentdata() 
{
	return current;
}	

enum chunktype Chunk::type()
{
	if( !strncmp( (char *)header.ID, "MThd", 4) ) return( ct_header );
	if( !strncmp( (char *)header.ID, "MTrk", 4) ) return( ct_track );
	return ct_undef;
}	
