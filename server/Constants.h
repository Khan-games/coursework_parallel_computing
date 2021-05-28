#pragma once

// network
#define CON_PORT 10105
#define DISCONNECT_TIMEOUT 30 // seconds
#define DISCONNECT_CYCLE 5 // seconds


// constants
#define BUFF_SIZE 1024 // total length of read msg
#define BYTES_PER_READ 128 // one read operation from buffer