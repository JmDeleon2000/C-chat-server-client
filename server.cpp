#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "proyecto.pb.h"

// para obtener el IP address del usuario 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

using namespace std;
using namespace chat;

const clock_t timeout = 90;

vector<int> socket_list;
vector<chat::UserInformation> user_reg;
vector<clock_t> last_user_valid_request;
vector<bool> has_timedout;
pthread_mutex_t global_mutex;
bool check_for_timeout = true;



bool available_user = true;


bool user_registration(string username, string IP) {
    cout << username << "\t" << IP << endl;
    if (username == "all")
        return false;
    for (int i = 0; i < user_reg.size(); i++) 
        if (user_reg[i].username() == username) 
            return false; // el nombre de usario no esta disponible 
    chat::UserInformation user;
    user.set_ip(IP);
    user.set_status("ACTIVE");
    user.set_username(username);
    user_reg.push_back(user);
    return true;
}



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

void* timeout_manager(void*);

void* user_session(void*);
struct thread_args
{
    int socket_id;
};

int main(int argn, char** argv) 
{

    if (argn != 2)
    {
        cerr << "Expected ./server <port>\n";
        return -1;
    }

    int port;
    sscanf(argv[1], "%d", &port);

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
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(listener, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listener, 10); 

    pthread_t timeout_manager_thread;
    if(pthread_create(&timeout_manager_thread, NULL, timeout_manager, (void*)NULL) == -1)
            return -1;
    
    pthread_t sessions[20];
    int session_count = 0;
    thread_args arguments;
    int read_count;
    char init_buff[512];
    string login_handshake = "";
    chat::ClientRequest* login_request = new chat::ClientRequest();
    chat::ServerResponse response;
    response.set_option(ServerResponse_Option_USER_LOGIN);
    response.set_code(ServerResponse_Code_FAILED_OPERATION);
    string bad_login_msg = response.SerializeAsString();

    while (true)
    {
        cout << "waiting for client...\n";
        int fd = accept(listener, (struct sockaddr*)NULL, NULL);
        memset(init_buff, 0, sizeof(init_buff));

        //while (/*read_count = recv(fd, init_buff, sizeof(init_buff), 0)*/)
        //{
            read_count = recv(fd, init_buff, sizeof(init_buff), 0);
            for (int i = 0; i < read_count; i++)
                login_handshake += init_buff[i];
            
            if (login_request->ParseFromString(login_handshake))
                {
                    pthread_mutex_lock(&global_mutex);
                    if(user_registration(login_request->newuser().username(), login_request->newuser().ip()))
                    {
                        cout << "valid login" << "\n";
                        socket_list.push_back(fd);
                        last_user_valid_request.push_back(0);
                        has_timedout.push_back(false);
                        arguments.socket_id = session_count;
                        
                        if(pthread_create(&sessions[session_count], NULL, user_session, (void*)&arguments) == -1)
                            return -1;
                        session_count++;
                    }
                    else
                        send(fd, bad_login_msg.c_str(), bad_login_msg.size(), 0);
                        
                    pthread_mutex_unlock(&global_mutex);
                }
        //}
        sleep(1);
    }
    
    for (int i = 0; i < session_count; i++)
        pthread_join(sessions[i], NULL);
    check_for_timeout = false;
    pthread_join(timeout_manager_thread, NULL);

    return 0;
}


// no pueden existir dos usuarios conectados con el mismo nombre
// un usuario no se puede llamar all
// obtener del cliente el nombre del usuario 

// registar nombre del usuario y su IP en la lista de usuarios conectados


void* user_session(void* args)
{
    int user_index = ((thread_args*)args)->socket_id;
    char buffer[1024];
    string receive_buffer;
    chat::ClientRequest* request = new chat::ClientRequest();
    chat::ServerResponse* response = new chat::ServerResponse();
    chat::ConnectedUsers* conn_users = new chat::ConnectedUsers();
    chat::Message* msg = new chat::Message();
    chat::UserInformation* userInfo = new chat::UserInformation(); 
    chat::ChangeStatus* status_change = new chat::ChangeStatus();
    response->set_allocated_message(msg);
    response->set_allocated_users(conn_users);
    response->set_allocated_user(userInfo);
    response->set_allocated_status(status_change);
    string send_buffer;

    response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);


    response->set_option(ServerResponse_Option_USER_LOGIN);
    send_buffer = response->SerializeAsString();
    send(socket_list[user_index], send_buffer.c_str(), send_buffer.size(), 0);
    cout << "sent login confirmation\n";
    bool succesful;
    int read_size;
    while (true)
    {
        read_size = recv(socket_list[user_index], buffer, sizeof(buffer), 0);

        if (read_size == 0)
            {
                cout << "Client: " << user_reg[user_index].username() << " disconnected\n";
                pthread_mutex_lock(&global_mutex);
                user_reg[user_index].set_status("INACTIVE");
                pthread_mutex_unlock(&global_mutex);
                break;
            }
        for(int i = 0; i < read_size; i++)
            receive_buffer += buffer[i];
        if (request->ParseFromString(receive_buffer))
        {
            cout << "Valid request from: " << user_reg[user_index].username() << endl;
            pthread_mutex_lock(&global_mutex);
            last_user_valid_request[user_index] = clock();
            if (has_timedout[user_index])
                {
                    cout << user_reg[user_index].username() << " timed out!\n";
                    user_reg[user_index].set_status("INACTIVE");
                    exit(1);
                }
            pthread_mutex_unlock(&global_mutex);
            succesful = false;
            switch (request->option())
            {
            
            case ClientRequest_Option_CONNECTED_USERS:
                response->set_option(ServerResponse_Option_CONNECTED_USERS);
                conn_users->clear_users();
                for (int i = 0; i < user_reg.size(); i++)
                    {
                        chat::UserInformation* this_user_info = conn_users->add_users();
                        this_user_info->set_username(user_reg[i].username());
                        this_user_info->set_status(user_reg[i].status());
                        this_user_info->set_ip(user_reg[i].ip());
                        succesful = true;
                    }
                if (succesful)
                    response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
                else
                    response->set_code(ServerResponse_Code_FAILED_OPERATION);
                send_buffer = response->SerializeAsString();
                send(socket_list[user_index], send_buffer.c_str(), send_buffer.size(), 0);

                break;
            case ClientRequest_Option_USER_INFORMATION:
                response->set_option(ServerResponse_Option_USER_INFORMATION);
                for (int i = 0; i < user_reg.size(); i++)
                    if(user_reg[i].username() == request->user().user())
                        {
                            userInfo->set_username(user_reg[i].username());
                            userInfo->set_ip(user_reg[i].ip());
                            userInfo->set_status(user_reg[i].status());
                            succesful = true;
                        }
                if (succesful)
                    response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
                else
                    response->set_code(ServerResponse_Code_FAILED_OPERATION);
                send_buffer = response->SerializeAsString();
                send(socket_list[user_index], send_buffer.c_str(), send_buffer.size(), 0);
                break;

            case ClientRequest_Option_STATUS_CHANGE:
                response->set_option(ServerResponse_Option_STATUS_CHANGE);
                for (int i = 0; i < user_reg.size(); i++)
                    if(user_reg[i].username() == request->status().username())
                        {
                            pthread_mutex_lock(&global_mutex);
                            user_reg[i].set_status(request->status().status());
                            pthread_mutex_unlock(&global_mutex);
                            succesful = true;
                        }
                if (succesful)
                    response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
                else
                    response->set_code(ServerResponse_Code_FAILED_OPERATION);
                send_buffer = response->SerializeAsString();
                send(socket_list[user_index], send_buffer.c_str(), send_buffer.size(), 0);
                
                break;
            case ClientRequest_Option_SEND_MESSAGE:
                msg->set_sender(request->message().sender());
                msg->set_receiver(request->message().receiver());
                msg->set_text(request->message().text());
                response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
                response->set_option(ServerResponse_Option_SEND_MESSAGE);
                send_buffer = response->SerializeAsString();
                if (request->message().receiver() == "all")
                    for (int i = 0; i < user_reg.size(); i++)
                    {
                        if(user_reg[i].status() == "ACTIVE" && user_reg[i].username() != request->message().sender())
                        {
                            succesful = true;
                            send(socket_list[i], send_buffer.c_str(), send_buffer.size(), 0);
                        }
                    }
                else
                    for (int i = 0; i < user_reg.size(); i++)
                        if(user_reg[i].status() == "ACTIVE" && user_reg[i].username() == request->message().receiver())
                        {
                            succesful = true;
                            send(socket_list[i], send_buffer.c_str(), send_buffer.size(), 0);
                            break;
                        }
                if (!succesful)
                {
                    response->set_code(ServerResponse_Code_FAILED_OPERATION);
                    send_buffer = response->SerializeAsString();
                    send(socket_list[user_index], send_buffer.c_str(), send_buffer.size(), 0);
                }
                break;
            default:
                break;
            }
            receive_buffer.clear();
        }
        else
            cout <<receive_buffer << "\n";
        
    }
    
}


void* timeout_manager(void*)
{
    clock_t now;
    while (check_for_timeout)
    {
        now = clock();
        pthread_mutex_lock(&global_mutex);
        for(int i = 0; i < last_user_valid_request.size(); i++)
            if (last_user_valid_request[i] != 0 && (now - last_user_valid_request[i])/CLOCKS_PER_SEC >= timeout)
                {
                    has_timedout[i] = true;
                }
        pthread_mutex_unlock(&global_mutex);
        sleep(5);
    }
}