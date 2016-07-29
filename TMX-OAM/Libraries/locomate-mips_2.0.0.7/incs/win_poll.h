//	This is in -------- <poll.h> ----------- of linux

#ifndef _WIN_POLL_H
#define _WIN_POLL_H
#define POLLIN		0x001
#define POLLPRI		0x002 
#define POLLOUT		0x004


struct pollfd
{
	int fd;					// File Descriptor to poll
	short int events;		//Types of events poller takes care about
	short int revents;		//Types of events that actualy occured
};
#endif

// -------------------- <end> ----------------------//
