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

int main()
{
    pthread_t sender_thread, receiver_thread;

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
    msg->set_sender("sender test");
    msg->set_receiver("reciever test");
    msg->set_text("test text: lorem ipsum");
    req->set_option(ClientRequest_Option_SEND_MESSAGE);
    req->set_allocated_message(msg);
    //send_buffer.push_back(req->SerializeAsString());
    //send_buffer.push_back(req->SerializeAsString());
    //send_buffer.push_back(req->SerializeAsString());


    string input;
    string username;

    if(pthread_create(&sender_thread, NULL, client_sender, (void*)NULL) == -1)
        return -1;
    if(pthread_create(&receiver_thread, NULL, client_receiver, (void*)NULL) == -1)
        return -1;
    
    cout << "Enter username: ";
    cin >> username;
    while (not_out)
    {
        
        cin >> input;
        if (input == "--q" || input == "--Q")
            not_out = false;
        
        if (not_out && input.size() > 0)
            {
                if (input == "--h" || input == "--H"){
                    cout << "Quit: --q or --Q" << endl;
                    cout << "DM: DM <usuario> message" << endl;
                    cout << "Users: --u or --U" << endl;
                    cout << "Info: --i <user> or --I <user>" << endl;
                    cout << "Status change: --s <status> or --S <status>" << endl;
                }

                if (input == "--u" || input == "--U"){
                    cout << "*** GETTING USERS INFORMATION ***" << endl;
                    msg->set_sender(username);
                    req->set_option(ClientRequest_Option_GET_USERS);
                    req->set_allocated_message(msg);
                }
                if (input.substr(0,2) == "--i" || input.substr(0,3) == "--I"){
                    cout << "*** GETTING "<<input.substr(input.find('') + 1)<<" INFORMATION ***" << endl;
                   
                }
                if (input.substr(0,2) == "--s" || input.substr(0,3) == "--S"){
                    cout << "*** CHANGING STATUS TO "<<input.substr(input.find('') + 1)<<" ***" << endl;
                    change->set_status(input.substr(input.find('') + 1));
                    req->set_option(ClientRequest_Option_SET_STATUS);
                    req->set_allocated_change(change);
                    cout << "STATUS CHANGE DONE SUCCESSFULLY" << endl;
                }
                if (input.substr(0,1) == "DM"){
                    cout << "*** SENDING PRIVATE MESSAGE TO "<<input.substr(input.find('') + 1)<<" ***" << endl;
                    msg->set_sender(username);
                    msg->set_receiver(input.substr(input.find('') + 1));
                    size_t pos = input.find(" ");
                    if (pos == std::string::npos)
                        return -1;
                    
                    pos = input.find(" ", pos + 1);
                    if (pos == std::string::npos)
                        return -1;
                    
                    msg->set_text(input.substr(pos, std::string::npos));
                    req->set_option(ClientRequest_Option_SEND_MESSAGE);
                    req->set_allocated_message(msg);
                }

                else{
                    msg->set_sender(username);
                    msg->set_receiver("all");
                    msg->set_text(input);
                    req->set_option(ClientRequest_Option_SEND_MESSAGE);
                    req->set_allocated_message(msg);
                }

                //msg->set_sender(input);
                input.clear();
                
                send_buffer.push_back(req->SerializeAsString());
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

    char read_buffer[256];
    n = test_buffer->size();


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
                    cout << response->DebugString() << endl;
                    response_buffer.clear();
                }
            }
            //use recv or read
        }
        
        sleep(1);
    }
    
}