#include <string>
#include <vector>

class DirectMessage
{
    public:
    std::string receiver;
    std::string sender;
    std::string text;
    public:
    void send();
    void encode(std::vector<char>*);
    DirectMessage();
    DirectMessage(std::string reciever, std::string sender, std::string text);
};

class ChangeStatus 
{
    public:
    std::string username;
    std::string status;
    public:
    void send();
    void encode(std::vector<char>*);
    ChangeStatus();
    ChangeStatus(std::string username, std::string status);
};

class UserRegistration 
{
    public:
    std::string username;
    std::string ip;
    public:
    void send();
    void encode(std::vector<char>*);
    UserRegistration();
    UserRegistration(std::string username, std::string ip);
};

class UserInformation 
{
    public:
    std::string username;
    std::string ip;
    std::string status;
    public:
    void send();
    void encode(std::vector<char>*);
    UserInformation();
    UserInformation(std::string username, std::string ip, std::string status);
};

class UserRequest 
{
    public:
    std::string user;
    public:
    void send();
    void encode(std::vector<char>*);
    UserRequest();
    UserRequest(std::string user);
};

class ConnectedUsers 
{
    public:
    std::vector<UserInformation> users;
    public:
    void send();
    void encode(std::vector<char>*);
    ConnectedUsers();
};

enum Option 
{
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

class ClientRequest 
{
    public:
    short int option;
    UserRegistration newuser;
    UserRequest user;
    ChangeStatus status;
    DirectMessage message;
    public:
    void send();
    void encode(std::vector<char>*);
    ClientRequest();
};

class ServerResponse {
    public:
    short int option;
    short int code;
    std::string response;
    ConnectedUsers users;
    UserInformation user;
    DirectMessage message;
    ChangeStatus status;
    public:
    void send();
    void encode(std::vector<char>*);
    ServerResponse();
};