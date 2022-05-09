#include <messages.h>

void encode_int(char* out, long int a, int size = -1)
{
    char* number = (char*)&a;
    int i = 0;
    if (size == -1)
        size = sizeof(a);
    size += size/8+1;
    while (i < size)
    {
        out[i] = (number[i] >> i+1) | 0x80;
        if (i > 0 && i != size-1)
            out[i] |= number[i-1];
        i++;
    }
    out[size-1] &= 0x7F;
}

void push_string(std::string* s, char* in, std::vector<char>* buffer)
{
    encode_int(in, s->size(), 2);
    buffer->push_back(in[0]);
    buffer->push_back(in[1]);
    buffer->push_back(in[2]);
    for (int i = 0; i < s->size()-1; i++)
        buffer->push_back(s->c_str()[i]);
}

void DirectMessage::encode(std::vector<char> *buffer)
{
    char in[3];
    
    buffer->push_back(0x0a);
    push_string(&receiver, in, buffer);
    
    buffer->push_back(0x12);
    push_string(&sender, in, buffer);
    
    buffer->push_back(0x1a);
    push_string(&text, in, buffer);
}

void DirectMessage::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);
    
    //TODO actually send
    delete buffer;
}

void ChangeStatus::encode(std::vector<char> *buffer)
{
    char in[3];
    
    buffer->push_back(0x0a);
    push_string(&username, in, buffer);
    
    buffer->push_back(0x12);
    push_string(&status, in, buffer);
}

void ChangeStatus::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);

    //TODO actually send
    delete buffer;
}

void UserRegistration::encode(std::vector<char>* buffer)
{
    char in[3];
    
    buffer->push_back(0x0a);
    push_string(&username, in, buffer);

    buffer->push_back(0x12);
    push_string(&ip, in, buffer);
}

void UserRegistration::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);

    //TODO actually send
    delete buffer;
}

void UserInformation::encode(std::vector<char>* buffer)
{
    char in[3];
    
    buffer->push_back(0x0a);
    push_string(&username, in, buffer);

    
    buffer->push_back(0x12);
    push_string(&ip, in, buffer);

    buffer->push_back(0x1a);
    push_string(&status, in, buffer);
}

void UserInformation::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);
    
    //TODO actually send
    delete buffer;
}

void UserRequest::encode(std::vector<char>* buffer)
{
    char in[3];
    
    buffer->push_back(0x0a);
    push_string(&user, in, buffer);
}

void UserRequest::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);

    //TODO actually send
    delete buffer;
}

void ConnectedUsers::encode(std::vector<char>* buffer)
{
    std::vector<char>* buff;
    char in[3];
    for (int i = 0; i < users.size()-1; i++)
    {
        buffer->push_back(0x0a);
        users[i].encode(buff);
        
    }
    delete buff;
}

void ConnectedUsers::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);

    //TODO actually send
    delete buffer;
}

void ClientRequest::encode(std::vector<char>* buffer)
{
    char in[3];
    encode_int(in, option, 2);
    switch (option)
    {
    case USER_LOGIN:
        encode_int(in, 0x12, 2);
        newuser.encode(buffer);
        break;
    case CONNECTED_USERS:
        break;
    case USER_INFORMATION:
        encode_int(in, 0x1a, 2);
        user.encode(buffer);
        break;
    case STATUS_CHANGE:
        encode_int(in, 0x22, 2);
        status.encode(buffer);
        break;
    case SEND_MESSAGE:
        encode_int(in, 0x2a, 2);
        message.encode(buffer);
        break;
    default:
        break;
    }
}

void ClientRequest::send()
{
    using namespace std;
    vector<char>* buffer = new vector<char>();
    
    encode(buffer);

    //TODO actually send
    delete buffer;
}

