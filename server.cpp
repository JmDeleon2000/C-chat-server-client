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

// struct user_info {
//     string username;
//     int id;
//     char *IP;
//     string status;
    
// }

// vector<user_info> user_reg;
// vector<user_info> active_users;

struct user_info
{
  string username;
  int id;
  char *IP;
  string status;
};


vector<user_info> user_reg;
vector<user_info> active_users;


bool available_user = true;
// bool active = true;


void user_registration(string username, char *IP) {


    for (int i = 0; i < user_reg.size(); i++) {
        if (user_reg[i].username == username) {
            available_user = false; // el nombre de usario esta disponible 
            
        }
        if (user_reg[i].status == "ACTIVE") {
            
            active_users.push_back (user_info()); // se agrega como nuevo usario 
        }

    }

}

void client_connection(string sender_name, string receiver_name, string message) {
    // conexión del cliente
    chat::ServerResponse* servRes = new chat::ServerResponse();
    chat::Message* msg = new chat::Message();

    for (int i = 0; i < active_users.size(); i++) {
        if (active_users[i].username == sender_name) {
            msg->set_sender(sender_name);
        }
        else if (active_users[i].username == receiver_name) {
            msg->set_receiver(receiver_name);
        }
    }

    msg->set_text(message);
    servRes->set_option(chat::ServerResponse_Option_SEND_MESSAGE);

    // crear su sesión en el servidor y manejarla de forma concurrente a las demás sesiones y a la recepción de nuevas conexiones (ergo, multithreading).

}

// void user_release(string username, char *IP) {
//     for (int i = 0; i < active_users.size(); i++) {
//         if (active_users[i].username == username) {

//         }
//     }

// }


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


int main() {

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

    // user_registration("sofia", IP);

    return 0;
}


// no pueden existir dos usuarios conectados con el mismo nombre
// obtener del cliente el nombre del usuario 

// registar nombre del usuario y su IP en la lista de usuarios conectados
