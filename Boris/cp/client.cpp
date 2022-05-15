#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include <fcntl.h>
#include "funcs.hpp"
#include <thread>

void func(int fd_recv, std::string login) {
    while (1) {
        std::string reply = recieve_message_client(fd_recv);
        std::cout << reply << "\n";
        std::cout.flush();
        std::cout << login << ">";
        std::cout.flush();
    }
}
int main() {
    int fd_send = open("input", O_RDWR);
    if (fd_send == -1) {
        std::cout << "ERROR: main pipe was failed\n";
        exit(1);
    }

    std::cout << "Wellcome to chat.\nTo create accounts launch ./server and insert logins.\nThan relaunch this application and enter your login.\n";
    std::cout << "Input: login message. Example: anton\n hey, how are you?\n";
    std::cout << "Insert your login: ";
    std::string login;

    int fd_recv = -1;
    while (fd_recv == -1) {
        std::cin >> login;
        fd_recv = open(login.c_str(), O_RDWR);
        if (fd_recv == -1) {
            std::cout << "Wrong login!\nTry another: ";
        }
    };

    std::string adressee, message;
    std::cout << "Right login! You have signed in chat. Now you can send messages!\n";
    std::thread thr_recieve(func, fd_recv, login);

    while (1) {
        std::cout << login << "> ";
        std::cin >> adressee;
        if (adressee == "history") {
            std::string pattern;
            std::getline(std::cin, pattern);
            send_message_to_server(fd_send, login, adressee, pattern);
        } else {
            if (adressee == "quit")
                break;
            std::getline(std::cin, message);
            send_message_to_server(fd_send, login, adressee, message);
        }
    }
    thr_recieve.detach();
}