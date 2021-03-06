#pragma once

// network
#define CON_PORT 10105
#define DISCONNECT_CYCLE 5 // seconds


// constants
#define BUFF_SIZE 1024 // total length of read msg
#define BYTES_PER_READ 128 // one read operation from buffer

// for serialization
static auto const flags = boost::archive::no_header | boost::archive::no_tracking;
