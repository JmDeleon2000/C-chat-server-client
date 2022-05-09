#include <vector>


enum Option {
USER_LOGIN = 0,
CONNECTED_USERS = 1,
USER_INFORMATION = 2,
STATUS_CHANGE = 3,
SEND_MESSAGE = 4
};

enum Code
{
FAILED_OPERATION = 0,
SUCCESSFUL_OPERATION = 1
};

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
encoded_msg* encode(msg_buffer*);