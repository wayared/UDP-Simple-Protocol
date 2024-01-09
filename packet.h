#ifndef PACKET_H
#define PACKET_H

#define MAX_SEGMENT_SIZE 1024

struct Packet {
	    int seq_num;
	        int sender_id;
		    char data[MAX_SEGMENT_SIZE];
};

#endif /* PACKET_H */
