#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "proyecto.pb.h"
//#include<sys/socket.h> 
//#include<arpa/inet.h>

using namespace std;
using namespace chat;
bool not_out = true;
vector<string> send_buffer;

void* client_sender(void* args);
void* client_receiver(void* args);

int main(int argn, char** argv)
{
    if (argn != 4)
        {
            cerr << "Expected ./client <username> <server ip> <server port>\n";
            return -1;
        }

    string username = argv[1];
    string serverIP = argv[2];
    string serverPort = argv[3];

    //int socket_desc;
	//socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	//
	//if (socket_desc == -1)
	//{
	//	printf("Could not create socket");
	//}

    chat::ClientRequest* req = new chat::ClientRequest();
    chat::Message* msg = new chat::Message();
    chat::ChangeStatus* change = new chat::ChangeStatus();
    chat::UserRequest* userInfoReq = new chat::UserRequest();
    chat::UserRegistration* reg = new chat::UserRegistration();
    msg->set_sender(username);
    reg->set_username(username);
    reg->set_ip("dummy ip"); //TODO get my ip
    req->set_option(ClientRequest_Option_USER_LOGIN);
    req->set_allocated_newuser(reg);
    req->set_allocated_message(msg);
    req->set_allocated_user(userInfoReq);
    req->set_allocated_status(change);
    send_buffer.push_back(req->DebugString());




    string input;
    string command;
    string second_arg;
    string third_arg;

    pthread_t sender_thread, receiver_thread;
    if(pthread_create(&sender_thread, NULL, client_sender, (void*)NULL) == -1)
        return -1;
    if(pthread_create(&receiver_thread, NULL, client_receiver, (void*)NULL) == -1)
        return -1;
    
    while (not_out)
    {
        
        getline(cin, input);
        command = input.substr(0, 3);
        input.erase(0, 4);
        second_arg = input.substr(0, input.find(' '));
        third_arg = input;
        third_arg.erase(0, third_arg.find(' ')+1);


        if (command == "--q" || command == "--Q")
            not_out = false;
        
        if (not_out && input.size() > 0)
        {
            bool valid_op = false;
            if (command == "--h" || command == "--H")
            {
                valid_op = true;
                cout << "Quit: --q or --Q\n";
                cout << "Direct Message: --M <user> <message> or --m <user> <message>\n";
                cout << "Broadcast: --b <message>\n";
                cout << "See connected users: --u or --U\n";
                cout << "Info: --i <user> or --I <user>\n";
                cout << "Status change: --s <status> or --S <status>\n";
            } 
            if (command == "--u" || command == "--U")
            {
                valid_op = true;
                cout << "*** GETTING USERS INFORMATION ***\n";
                req->set_option(ClientRequest_Option_CONNECTED_USERS);
            } 
            if (command == "--i" || command == "--I")
            {
                valid_op = true;
                cout << "*** GETTING " << second_arg << " INFORMATION ***\n";
                userInfoReq->set_user(second_arg);
                req->set_option(ClientRequest_Option_USER_INFORMATION);
            } 
            if (command == "--s" || command == "--S")
            {
                valid_op = true;
                cout << "*** CHANGING STATUS TO " << second_arg <<" ***\n";
                change->set_status(second_arg);
                req->set_option(ClientRequest_Option_STATUS_CHANGE);
            }
            if (command == "--m" || command == "--M")
            {
                valid_op = true;
                cout << "*** SENDING PRIVATE MESSAGE TO " << second_arg <<" ***\n";
                msg->set_receiver(second_arg);
                msg->set_text(third_arg);
                req->set_option(ClientRequest_Option_SEND_MESSAGE);
            }
            if (command == "--b" || command == "--B")
            {
                valid_op = true;
                msg->set_receiver("all");
                msg->set_text(input);
                req->set_option(ClientRequest_Option_SEND_MESSAGE);
            }
            input.clear();
            second_arg.clear();
            third_arg.clear();
            if (valid_op)
                send_buffer.push_back(req->DebugString());
            else 
                cout << "Invalid operation\nUse --h for help\n";
        }
    }
    
    
    pthread_join(sender_thread, NULL);
    pthread_join(receiver_thread, NULL);
    cout << "Bye!\n";
    return 0;
}


void* client_sender(void* args)
{
    while (not_out)
    {
        if (send_buffer.size() > 0)
        {
            cout << send_buffer[0] << endl;
            send_buffer.erase(send_buffer.begin());
        }
        sleep(1);
    }
    
}

void* client_receiver(void* args)
{
    bool keep_reading = false;
    int n = 0;
    chat::ServerResponse* response = new chat::ServerResponse();
    chat::Message* msg = new chat::Message();
    msg->set_sender("sender test");
    msg->set_receiver("reciever test");
    msg->set_text("test text: lorem ipsum");
    response->set_option(ServerResponse_Option_SEND_MESSAGE);
    response->set_allocated_message(msg);
    
    string response_buffer = "";

    char read_buffer[1024];


    while (not_out)
    {
        //use recv or read
        if (n > 0)
            keep_reading = true;
        while (keep_reading)
        {
            for(int i = 0; i < n; i++)
                response_buffer += read_buffer[i];
            
            if (n == 0)
            {
                keep_reading = false;
                if (response->ParseFromString(response_buffer))
                {
                    cout << response->DebugString() << "\n";
                    response_buffer.clear();
                }
            }
            //use recv or read
        }
        
        sleep(1);
    }
    
}