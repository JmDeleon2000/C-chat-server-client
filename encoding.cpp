#include <encoding.h>

using namespace std;
void consume(int* index, msg_buffer* out, char* buffer, bool* set_mask)
{
    char id = buffer[0];
    buffer += 1;
    *index++;
    int out_i = (id >> 3) - 1;
    switch (id & 0x07)
    {
    case 0://8 bytes, expecting a 64 bit int
        
        char read_buff[9];
        int num_size = 0;
        while (buffer[0] & 0x80)
        {
            *index++;
            read_buff[num_size] = buffer[0];
            buffer += 1;
            num_size++;
        }
        *index++;
        read_buff[num_size] = buffer[0];
        buffer += 1;
        num_size++;
        int i = 0, j = num_size;
        
        
        if (set_mask[out_i])
            out->buffers[out_i] = new vector<int>();
        while (i < num_size)
        {
            read_buff[--j];
            i++;
        }
        

        break;
    case 1://8 bytes, expecting a 64 bit double
        out->buffers[out_i] = new char[8];
        break;
    case 2://1024 bytes, expecting large data, like a string or a repeated field
        //out->buffers[out_i] = new char[1024]; TODO
        break;
    case 5://4 bytes, expecting a 32 bit float
        out->buffers[out_i] = new char[4];
        break;
    default:
        break;
    }
}

