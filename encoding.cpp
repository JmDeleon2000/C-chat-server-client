#include <encoding.h>


void consume(int* index, msg_buffer* out, char* buffer)
{
    char id = buffer[0];
    *index++;
    int out_i = (id >> 3) - 1;
    switch (id & 7)
    {
    case 0://8 bytes, expecting a 64 bit int
        out->buffers[out_i] = new char[8];
        break;
    case 1://8 bytes, expecting a 64 bit double
        out->buffers[out_i] = new char[8];
        break;
    case 2://1024 bytes, expecting large data, like a string or a repeated field
        out->buffers[out_i] = new char[1024];
        break;
    case 5://4 bytes, expecting a 32 bit float
        out->buffers[out_i] = new char[4];
        break;
    default:
        break;
    }
    while (buffer[0] & 0x80)
    {
        *index++;
        
    }
    
}


msg_buffer* decode(encoded_msg* buffer)
{
    msg_buffer* out = new msg_buffer();
    char* head;
    int i;
    while (i < buffer->size)
    {
        consume(&i, out, (char*)buffer);
    }
    //TODO identify msg_type
}

