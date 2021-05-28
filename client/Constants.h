#pragma once

// network
#define CON_PORT 10105
#define CON_IP "127.0.0.1"

// constants
#define BUFF_SIZE 1024
#define BYTES_PER_READ 128 // one read operation from buffer

// for serialization
static auto const flags = boost::archive::no_header | boost::archive::no_tracking;
