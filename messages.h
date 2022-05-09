#include <string>
#include <vector>

class DirectMessage
{
    std::string receiver;
    std::string sender;
    std::string text;
    void send();
};

class ChangeStatus {
    std::string username;
    std::string status;
    void send();
};

class UserRegistration {
    std::string username;
    std::string ip;
    void send();
};

class UserInformation {
    std::string username;
    std::string ip;
    std::string status;
    void send();
};

class UserRequest {
    std::string user;
    void send();
};

class ConnectedUsers {
    std::vector<UserInformation> users;
    void send();
};

class ClientRequest {
    int option;
    UserRegistration newuser;
    UserRequest user;
    ChangeStatus status;
    DirectMessage message;
    void send();
};

class ServerResponse {
    int option;
    int code = 2;
    std::string response;
    ConnectedUsers users;
    UserInformation user;
    DirectMessage message;
    ChangeStatus status;
    void send();
};