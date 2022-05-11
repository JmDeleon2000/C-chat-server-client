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

void client_connection(string sender_name, string receiver_name, string message) {
    // conexión del cliente
    chat::ServerResponse* servRes = new chat::ServerResponse();
    chat::Message* msg = new chat::Message();
    
    for (int i = 0; i < user_reg.size(); i++) {
        if (user_reg[i].username()== sender_name) {
            msg->set_sender(sender_name);
        }
        else if (user_reg[i].username()== receiver_name) {
            msg->set_receiver(receiver_name);
        }
    }

    msg->set_text(message);
    servRes->set_option(chat::ServerResponse_Option_SEND_MESSAGE);
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


    chat::ConnectedUsers* conn_users = new chat::ConnectedUsers();
    int read_size;
    while (true)
    {
        read_size = recv(socket_list[my_socket], receive_buffer, sizeof(receive_buffer), 0);

        if (read_size == 0)
            {
                cout << "Client: " << user_reg[my_socket].username() << " disconnected\n";
                user_reg[my_socket].set_status("INACTIVE");
                exit(1);
            }

        // ver si el el username está activo
        // verificar el timeout del usuario. Si se pasa, ponerlo inactivo
        //conn_users->set_users(user_reg);
        //conn_users->add_users(user_reg);

        response->set_allocated_users(conn_users);
    }
    
}