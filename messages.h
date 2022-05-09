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
    DirectMessage();
};

class ChangeStatus {
    public:
    std::string username;
    std::string status;
    public:
    void send();
    ChangeStatus();
};

class UserRegistration {
    public:
    std::string username;
    std::string ip;
    public:
    void send();
    UserRegistration();
};

class UserInformation {
    public:
    std::string username;
    std::string ip;
    std::string status;
    public:
    void send();
    UserInformation();
};

class UserRequest {
    public:
    std::string user;
    public:
    void send();
    UserRequest();
};

class ConnectedUsers {
    public:
    std::vector<UserInformation> users;
    public:
    void send();
    ConnectedUsers();
};

class ClientRequest {
    public:
    int option;
    UserRegistration newuser;
    UserRequest user;
    ChangeStatus status;
    DirectMessage message;
    public:
    void send();
    ClientRequest();
};

class ServerResponse {
    public:
    int option;
    int code = 2;
    std::string response;
    ConnectedUsers users;
    UserInformation user;
    DirectMessage message;
    ChangeStatus status;
    public:
    void send();
    ServerResponse();
};