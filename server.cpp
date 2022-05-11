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

using namespace std;
using namespace chat;



vector<int> socket_list;
vector<chat::UserInformation> user_reg;



bool available_user = true;


bool user_registration(string username, string IP) {

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
}



    // crear su sesión en el servidor y manejarla de forma concurrente a las demás sesiones y a la recepción de nuevas conexiones (ergo, multithreading)


// chat::ServerResponse* servRes = new chat::ServerResponse();
// chat::UserRegistration* userReg = new chat::UserRegistration();
// chat::UserInformation* userInfo = new chat::UserInformation();
// chat::Message* msg = new chat::Message();
// chat::ConnectedUsers* conUser = new chat::ConnectedUsers();

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

    pthread_t sessions[20];
    int session_count = 0;
    thread_args arguments;
    while (true)
    {
        cout << "waiting for client...\n";
        socket_list.push_back(accept(listener, (struct sockaddr*)NULL, NULL));

        arguments.socket_id = session_count;
        if(pthread_create(&sessions[session_count], NULL, user_session, (void*)&arguments) == -1)
            return -1;
        session_count++;
        sleep(1);
    }
    
    for (int i = 0; i < session_count; i++)
        pthread_join(sessions[i], NULL);

    return 0;
}


// no pueden existir dos usuarios conectados con el mismo nombre
// un usuario no se puede llamar all
// obtener del cliente el nombre del usuario 

// registar nombre del usuario y su IP en la lista de usuarios conectados


void* user_session(void* args)
{
    int my_socket = ((thread_args*)args)->socket_id;
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

    //send(socket_list[my_socket], buffer, sizeof(buffer), NULL);

    //suponiendo que recv funciona:

    bool valid_login = false;
    if (request->ParseFromString(receive_buffer))
    {
        if (request->option() == ClientRequest_Option_USER_LOGIN)
            {
                if(request->has_newuser() && request->newuser().has_username() && request->newuser().has_ip())
                    {
                        if (user_registration(request->newuser().username(), request->newuser().ip()))
                                valid_login = true;
                    }
            }
    }
    if (valid_login)
        response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
    else
        response->set_code(ServerResponse_Code_FAILED_OPERATION);

    response->set_option(ServerResponse_Option_USER_LOGIN);
    response->SerializeToString(&send_buffer);
    send(socket_list[my_socket], send_buffer.c_str(), send_buffer.size()+1, 0);


    int read_size;
    while (true)
    {
        read_size = recv(socket_list[my_socket], buffer, sizeof(buffer), 0);

        if (read_size == 0)
            {
                cout << "Client: " << user_reg[my_socket].username() << " disconnected\n";
                user_reg[my_socket].set_status("INACTIVE");
                exit(1);
            }
        
        receive_buffer = buffer;
        if (request->ParseFromString(receive_buffer))
        {
            bool succesful = false;
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
                send(my_socket, send_buffer.c_str(), send_buffer.size()+1, 0);

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
                send(my_socket, send_buffer.c_str(), send_buffer.size()+1, 0);
                break;

            case ClientRequest_Option_STATUS_CHANGE:
                response->set_option(ServerResponse_Option_STATUS_CHANGE);
                for (int i = 0; i < user_reg.size(); i++)
                    if(user_reg[i].username() == request->status().username())
                        {
                            user_reg[i].set_status(request->status().status());
                            succesful = true;
                        }
                if (succesful)
                    response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
                else
                    response->set_code(ServerResponse_Code_FAILED_OPERATION);
                send(my_socket, send_buffer.c_str(), send_buffer.size()+1, 0);
                
                break;
            case ClientRequest_Option_SEND_MESSAGE:
                msg->set_sender(request->message().sender());
                msg->set_receiver(request->message().receiver());
                msg->set_text(request->message().text());
                response->set_code(ServerResponse_Code_SUCCESSFUL_OPERATION);
                response->set_option(ServerResponse_Option_SEND_MESSAGE);
                if (request->message().receiver() == "all")
                    for (int i = 0; i < user_reg.size(); i++)
                        if(user_reg[i].status() == "ACTIVE")
                            send(socket_list[i], send_buffer.c_str(), send_buffer.size()+1, 0);
                else
                    for (int i = 0; i < user_reg.size(); i++)
                        if(user_reg[i].status() == "ACTIVE" && user_reg[i].username() == request->message().receiver())
                            send(socket_list[i], send_buffer.c_str(), send_buffer.size()+1, 0);
                break;
            default:
                break;
            }
        }

        // ver si el el username está activo
        // TODO verificar el timeout del usuario. Si se pasa, ponerlo inactivo

        
    }
    
}