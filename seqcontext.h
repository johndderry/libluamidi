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
#define USE_DRAIN 1

struct AddrListElem
{
	snd_seq_addr_t 			addr;
	struct AddrListElem		*next;
};

class AddressList {
private:
	struct AddrListElem		*head;

public:
	AddressList();
	~AddressList();

	void 			append(snd_seq_addr_t *);
	snd_seq_addr_t 	*index(int);
	int 			length();
};

snd_seq_addr_t *parseclientport( char * );

class SeqContext {
private:
	int  			client;		/* The client associated with this context */
	int  			queue; 		/* The queue to use for all operations */
	snd_seq_t		*handle; 	/* The snd_seq handle to /dev/snd/seq */
	snd_seq_addr_t  source;		/* Source for events:  */
	AddressList  *	destlist;	/* Destination list */
#define ctxndest destlist->len
#define ctxdest  destlist->data
	int   port_count;			/* Ports allocated */

public:
	int connected, verbose;
    SeqContext();
    ~SeqContext();

    void name(char *);
	int seq_connect_add(char *);
	int seq_new_port(char);
	void seq_destroy_port(int port);
	int seq_init_tempo(int resolution, int tempo,  int realtime);
	void seq_set_queue(int q);
	int seq_sendto(snd_seq_event_t *ev, int client, int port);
	int seq_send_to_all(snd_seq_event_t *ep);
	snd_seq_addr_t *seq_dev_addr(int dev);
	void seq_start_queue();
	void seq_stop_queue();
	int seq_write(snd_seq_event_t *ep);
	snd_seq_t *seq_handle();
	void set_channel(snd_seq_event_t *ep, int chan);

	void seq_midi_event_init(snd_seq_event_t *ep, unsigned long time, int devchan);
	void seq_midi_note(snd_seq_event_t *ep, int devchan, int note, int vel, int length);
	void seq_midi_note_on(snd_seq_event_t *ep, int devchan, int note, int vel, int length);
	void seq_midi_note_off(snd_seq_event_t *ep, int devchan, int note, int vel, int length);
	void seq_midi_keypress(snd_seq_event_t *ep, int devchan, int note, int value);
	void seq_midi_control(snd_seq_event_t *ep, int devchan, int control, int value);
	void seq_midi_program(snd_seq_event_t *ep, int devchan, int program);
	void seq_midi_chanpress(snd_seq_event_t *ep, int devchan, int pressure);
	void seq_midi_pitchbend(snd_seq_event_t *ep, int devchan, int bend);
	void seq_midi_tempo(snd_seq_event_t *ep, int tempo);
	void seq_midi_sysex(snd_seq_event_t *ep, int status, unsigned char *data, int length);
	void seq_midi_echo(unsigned long time);

};
