#include <vector>




enum MSG_TYPE
{
    UserRegistration,
    ChangeStatus,
    Message,
    UserInformation,
    UserRequest,
    ConnectedUsers,
    ClientRequest,
    ServerResponse
};


struct msg_buffer
{
    int type = -1;
    void* buffers[10];
};

struct encoded_msg
{
    int size;
    void* buffer;
};


msg_buffer* decode(encoded_msg*);
