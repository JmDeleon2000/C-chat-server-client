#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "proyecto.pb.h"
#include<sys/socket.h> 
#include<arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

#define debug 0

using namespace std;
using namespace chat;
bool not_out = true;
pthread_mutex_t send_queue_mutex;
vector<string> send_buffer;
int socketfd;

void* client_sender(void* args);
void* client_receiver(void* args);

// código extraído de: https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus

// nombre del usuario 
void check_host_name(int hostname) { 
   if (hostname == -1) {
      perror("gethostname");
      exit(1);
   }
}

//información del host
void check_host_entry(struct hostent * hostentry) { 
   if (hostentry == NULL){
      perror("gethostbyname");
      exit(1);
   }
}

//lo convierte al formato de un IP
void IP_formatter(char *IPbuffer) { 
   if (NULL == IPbuffer) {
      perror("inet_ntoa");
      exit(1);
   }
}

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

    pthread_t sender_thread, receiver_thread;
    if(pthread_create(&sender_thread, NULL, client_sender, (void*)NULL) == -1)
        return -1;
    if(pthread_create(&receiver_thread, NULL, client_receiver, (void*)NULL) == -1)
        return -1;

#if debug

#else
    // para obtener el IP address
    char host[256];
    char *IP;
    struct hostent *host_entry;


    int hostname;
    hostname = gethostname(host, sizeof(host)); 
    check_host_name(hostname);


    host_entry = gethostbyname(host); 
    check_host_entry(host_entry);

    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //lo convierte al formato de un IP

    printf("Host IP: %s\n", IP);

    struct sockaddr_in serv_addr; 
    
    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Socket creation failed!\n";
        return -1;
    } 
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    int port;
    sscanf(serverPort.c_str(), "%d", &port);
    serv_addr.sin_port = htons(port); 

    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)
    {
        cerr << "inet_pton error occured\n";
        return 1;
    } 

    if( connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cerr << "\n Error : Connect Failed \n";
        return 1;
    } 
#endif


    chat::ClientRequest* req = new chat::ClientRequest();
    chat::Message* msg = new chat::Message();
    chat::ChangeStatus* change = new chat::ChangeStatus();
    chat::UserRequest* userInfoReq = new chat::UserRequest();
    chat::UserRegistration* reg = new chat::UserRegistration();
    
    string my_ip = "192.168.1.16";
    reg->set_username(username);
    reg->set_ip(my_ip); //TODO get my ip
    req->set_option(ClientRequest_Option_USER_LOGIN);

    req->set_allocated_newuser(reg);
    req->set_allocated_message(msg);
    req->set_allocated_user(userInfoReq);
    req->set_allocated_status(change);
#if debug
    send_buffer.push_back(req->DebugString());
#else
    send_buffer.push_back(req->SerializeAsString());
#endif


    string input;
    string command;
    string second_arg;
    string third_arg;


    
    while (not_out)
    {
        
        getline(cin, input);
        //cout << input << "\n";
        if (input.size() > 3)
        {
            command = input.substr(0, 3);
            input.erase(0, 4);
            second_arg = input.substr(0, input.find(' '));
            third_arg = input;
            third_arg.erase(0, third_arg.find(' ')+1);
#if debug
            cout << command << "\t" << second_arg << "\t" << third_arg << "\t" << input << "\n";
#endif
        }
        else command = input;
        


        bool valid_op = false;

        if (command == "--q" || command == "--Q")
            not_out = false;
        
        if (not_out && input.size() > 0)
        {
            if (command == "--h" || command == "--H")
            {
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
                change->set_username(username);
                req->set_option(ClientRequest_Option_STATUS_CHANGE);
            }
            if (command == "--m" || command == "--M")
            {
                valid_op = true;
                cout << "*** SENDING PRIVATE MESSAGE TO " << second_arg <<" ***\n";
                msg->set_receiver(second_arg);
                msg->set_text(third_arg);
                msg->set_sender(username);
                req->set_option(ClientRequest_Option_SEND_MESSAGE);
            }
            if (command == "--b" || command == "--B")
            {
                valid_op = true;
                msg->set_receiver("all");
                msg->set_text(input);
                msg->set_sender(username);
                req->set_option(ClientRequest_Option_SEND_MESSAGE);
            }
            if (valid_op)
            {
                pthread_mutex_lock(&send_queue_mutex);
#if debug
                send_buffer.push_back(req->DebugString());
#else
                send_buffer.push_back(req->SerializeAsString());
#endif
                pthread_mutex_unlock(&send_queue_mutex);
                req->Clear();
                msg = new chat::Message();
                change = new chat::ChangeStatus();
                userInfoReq = new chat::UserRequest();
                reg = new chat::UserRegistration();
                req->set_allocated_newuser(reg);
                req->set_allocated_message(msg);
                req->set_allocated_user(userInfoReq);
                req->set_allocated_status(change);
            }
            else 
                if (command != "--h" || command != "--H")
                    cout << "Invalid operation\nUse --h for help\n";
            input.clear();
            second_arg.clear();
            third_arg.clear();
        }
    }
    
    
    pthread_join(sender_thread, NULL);
    pthread_cancel(receiver_thread);
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
#if debug
            cout << send_buffer[0] << endl;
#else
            send(socketfd, send_buffer[0].c_str(), send_buffer[0].size(), 0);
#endif
            pthread_mutex_lock(&send_queue_mutex);
            send_buffer.erase(send_buffer.begin());
            pthread_mutex_unlock(&send_queue_mutex);
        }
        sleep(1);
    }
    
}

void* client_receiver(void* args)
{
    sleep(4);
    int n = 0;
    chat::ServerResponse* response = new chat::ServerResponse();
    
    string response_buffer = "";

    char read_buffer[1024];


    while (not_out)
    {
        n = recv(socketfd, read_buffer, sizeof(read_buffer), 0);

        for(int i = 0; i < n; i++)
            response_buffer += read_buffer[i];
        
        if (n == 0)
        {
            cout << "server disconnected" << endl;
            not_out = false;
            break;
        }
        if (response->ParseFromString(response_buffer))
        {

#if debug
            //cout << response->DebugString() << "\n";
#else
            if (response->code() == ServerResponse_Code_FAILED_OPERATION)
            {
                cerr << "Operation: ";
                switch (response->option())
                {
                case ServerResponse_Option_CONNECTED_USERS:
                    cerr << "Get connected users info";
                    break;
                case ServerResponse_Option_USER_LOGIN:
                    cerr << "User Login";
                    not_out = false;
                    break;
                case ServerResponse_Option_USER_INFORMATION:
                    cerr << "Get user information";
                    break;
                case ServerResponse_Option_STATUS_CHANGE:
                    cerr << "User status change";
                    break;
                case ServerResponse_Option_SEND_MESSAGE:
                    cerr << "Send message";
                    break;
                default:
                    break;
                }
                cerr << " failed!\n";

            }
            else
                switch (response->option())
                {
                case ServerResponse_Option_CONNECTED_USERS:
                    //TODO try not to be lazy
                    cout << response->users().DebugString() << "\n";
                    break;
                case ServerResponse_Option_USER_LOGIN:
                    cout << "User Login succesful\n";
                    break;
                case ServerResponse_Option_USER_INFORMATION:
                    if (response->has_user())
                        cout << response->user().DebugString();
                    break;
                case ServerResponse_Option_STATUS_CHANGE:
                    cout << "User status change succesful\n";
                    break;
                case ServerResponse_Option_SEND_MESSAGE:
                    if (response->has_message() && response->message().has_sender() && response->message().has_text())
                    {  
                        cout << "Message from " << response->message().sender() << ":\n";
                        cout << response->message().text() << "\n";
                    }
                    else if (response->has_message() && response->message().has_text())
                    {  
                        cout << "Message :\n";
                        cout << response->message().text() << "\n";
                    }
                    break;
                default:
                    break;
                }
#endif
            response_buffer.clear();
            response->Clear();
        }
        
        //sleep(1);
    }
    
}