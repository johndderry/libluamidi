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
#include <alsa/asoundlib.h>
#include <lua.h>
#include <lauxlib.h>
#include "seqcontext.h"
#include "chunk.h"
#include "varquantity.h"
#include "midievent.h"
#include "nonmidievent.h"
#include "track.h"
#include "midifile.h"
#include "sortednotes.h"

static int l_MidiFileNew( lua_State *L ) {

    MidiFile **udata;
    udata = (MidiFile **) lua_newuserdata( L, sizeof(MidiFile **) );

    const char *name = lua_tostring(L, 1);
    const char *access = lua_tostring(L, 2);
    
    *udata = new MidiFile((char*)name, (char*)access);
    
    return 1;
}

static int l_TrackNew( lua_State *L ) {

    Track **udata;
    udata = (Track **) lua_newuserdata( L, sizeof(Track **) );

    int count = lua_tointeger( L, 1 );
    
    *udata = new Track( count );
        
    return 1;
}

static int l_SortedNotesNew( lua_State *L ) {

    SortedNotes **udata;
    udata = (SortedNotes **) lua_newuserdata( L, sizeof(SortedNotes **) );
    
    *udata = new SortedNotes();
    
    return 1;
}

static int l_MidiFileDelete( lua_State *L ) {

    MidiFile **mf = (MidiFile **) lua_touserdata( L, 1 );
    delete *mf;
     
    return 0;
}
    
static int l_TrackDelete( lua_State *L ) {

    Track **t = (Track **) lua_touserdata( L, 1 );
    delete *t;
     
    return 0;
}
    
static int l_SortedNotesDelete( lua_State *L ) {

    SortedNotes **sn = (SortedNotes **) lua_touserdata( L, 1 );
    delete *sn;
     
    return 0;
}
    
static int l_MidiFileReadChunk( lua_State *L ) {

    MidiFile **mf = (MidiFile **) lua_touserdata( L, 1 );
    Track **track = (Track **) lua_touserdata( L, 2 );
	(*mf)->readchunk( *track );
	
    return 0;
}
    
static int l_MidiFileWriteChunk( lua_State *L ) {

    MidiFile **mf = (MidiFile **) lua_touserdata( L, 1 );
    Track **track = (Track **) lua_touserdata( L, 2 );
	(*mf)->writechunk( *track );
	
    return 0;
}
    
static int l_SortedNotesTrackNotes( lua_State *L ) {

    SortedNotes **udata = (SortedNotes **) lua_touserdata( L, 1 );
    Track **track = (Track **) lua_touserdata( L, 2 );
    
    (*udata)->tracknotes( *track ); 
    
    return 0;
}

static int l_SortedNotesAddToList( lua_State *L ) {

    SortedNotes **udata = (SortedNotes **) lua_touserdata( L, 1 );
    int control = lua_tonumber( L, 2 );
    int channel = lua_tonumber( L, 3 );
    int pitch = lua_tonumber( L, 4 );
    int velocity = lua_tonumber( L, 5 );
    unsigned long time = lua_tonumber( L, 6 );
    
	(*udata)->addtolist( control, channel, pitch, velocity, time );

    return 0;
}

static int l_SortedNotesCount( lua_State *L ) {

    SortedNotes **udata = (SortedNotes **) lua_touserdata( L, 1 );
	
	lua_pushinteger( L, (*udata)->numevents() );
	
	return 1;
}

static int l_TrackLength( lua_State *L ) {

    Track **udata = (Track **) lua_touserdata( L, 1 );
	
	lua_pushinteger( L, (*udata)->length() );
	
	return 1;
}

static const struct luaL_Reg mylib [] = {
    {"MidiFileNew", l_MidiFileNew },
    {"TrackNew", l_TrackNew },
    {"SortedNotesNew", l_SortedNotesNew },
    {"MidiFileDelete", l_MidiFileDelete },
    {"TrackDelete", l_TrackDelete },
    {"SortedNotesDelete", l_SortedNotesDelete },
	{"MidiFileReadChunk", l_MidiFileReadChunk },
	{"MidiFileWriteChunk", l_MidiFileWriteChunk },
    {"SortedNotesTrackNotes", l_SortedNotesTrackNotes },
    {"SortedNotesAddToList", l_SortedNotesAddToList },
    {"SortedNotesCount", l_SortedNotesCount },
    {"TrackLength", l_TrackLength },
    {NULL, NULL}
};

extern "C" int luaopen_libmidi( lua_State *L ) {
    luaL_newlib(L, mylib);
    return 1;
}

