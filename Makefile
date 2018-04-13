DBFLAG = -DDEBUG=1
CXX = g++
CPPFLAGS = -g -O0 -fPIC $(DBFLAG)
LDFLAGS = -g -lasound -llua5.3

LIBRARY = libluamidi.so

OBJECTS = seqcontext.o chunk.o varquantity.o midievent.o nonmidievent.o track.o midifile.o lualib.o sortednotes.o

INCLUDES =  seqcontext.h chunk.h varquantity.h midievent.h nonmidievent.h track.h midifile.h lualib.h sortednotes.h

$(LIBRARY): $(OBJECTS)
	g++ -shared -o $(LIBRARY) $(OBJECTS) $(LDFLAGS)
	
lualib.o: lualib.cpp
	g++ -c $(CPPFLAGS) -I/usr/include/lua5.3 lualib.cpp

clean:
	(rm -f *.o $(LIBRARY))

