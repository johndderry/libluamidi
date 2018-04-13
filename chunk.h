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
/* 
	Chunk type enumeration
*/
enum chunktype { ct_undef, ct_header, ct_track  };

/********************************************************/
/***************** CHUNK CLASS	*************************/
/********************************************************/
/** 
  Handle the file chunking. Used by Track primariy 
*/
class Chunk {
private:
	struct {
		unsigned char ID[4];
		unsigned char length[4];
		} header;					// build our header here
	unsigned char *start, *current;	// read/write pointers
	unsigned long length;			// keep track of length

	// low level copy in/out
	void copybytes( unsigned char *, unsigned char *, unsigned );
	void headerlen( unsigned char *, unsigned ) ;
public:
	Chunk(enum chunktype type, unsigned len);
	~Chunk();
	unsigned load(unsigned char *p, unsigned int l);
	unsigned unload(unsigned char *p, unsigned int l);
	unsigned len();
	int adjust( unsigned );
	int write(FILE *s);
	int read(FILE *s);
	int write(SeqContext *c, snd_seq_addr_t *s);
	int read(SeqContext *c, snd_seq_addr_t *s);
	unsigned char *currentdata();
	enum chunktype type();
	};

