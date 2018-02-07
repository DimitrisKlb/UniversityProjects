#ifndef PING_PONG_H
#define PING_PONG_H

typedef nx_struct RadioMsg {
	nx_uint16_t moteID;
	nx_uint8_t data;
} RadioMsg;

enum {
	AM_RADIO = 6,
	TIMER_FREQ = 4000
};

enum{ // MESSAGE CODES
	MESSAGE_START = 0,
	MESSAGE_PLAY,
	MESSAGE_STOP	
};

#endif /* PING_PONG_H */
