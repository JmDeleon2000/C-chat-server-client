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
};

class ChangeStatus {
    public:
    std::string username;
    std::string status;
    public:
    void send();
};

class UserRegistration {
    public:
    std::string username;
    std::string ip;
    public:
    void send();
};

class UserInformation {
    public:
    std::string username;
    std::string ip;
    std::string status;
    public:
    void send();
};

class UserRequest {
    public:
    std::string user;
    public:
    void send();
};

class ConnectedUsers {
    public:
    std::vector<UserInformation> users;
    public:
    void send();
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
};