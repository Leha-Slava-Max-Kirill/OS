#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <vector>
#include <fcntl.h>
#include "funcs.hpp"

int in(std::vector<std::string> logins, std::string str) {
    for (int i = 0; i < logins.size(); ++i) {
        if (logins[i] == str)
            return i;
    }
    return -1;
}

int main() {
    std::vector<std::vector<std::string>> history;
    std::vector<std::string> logins;
    std::string command;
    std::string login;

    std::cout << "Enter all user's logins. Insert 'end' to stop:\n";
    while (1) {
        std::cin >> login;
        if(login == "end") break;
        std::vector<std::string> vec;
        if (in(logins, login) == -1)
            logins.push_back(login);
        else {
            std::cout << "This user is already exists!\n";
            continue;
        }
        vec.push_back(login);
        history.push_back(vec);
    }
    if (login == "end") 
        std::cout << "All users was succesfully writed to the system\n" 
                  << "Now you can chat" 
                  <<std::endl;

    for (int i = 0; i < logins.size(); ++i) {
        if (mkfifo(logins[i].c_str(), 0777) == -1) {
            if (errno != EEXIST) {
                std::cout << "ERROR: Pipes for clients wasn't created\n";
                exit(EXIT_FAILURE);
            }
        }
    }
    if (mkfifo("input", 0777) == -1) {
        std::cout << "ERROR: main pipe wasn't create\n";
        exit(1);
    }
    int fd_recv = open("input", O_RDWR);
    if (fd_recv == -1) {
        std::cout << "ERROR: pipe wasn't open\n";
        exit(EXIT_FAILURE);
    }

    int fd[logins.size()];
    for (int i = 0; i < logins.size(); ++i) {
        fd[i] = open(logins[i].c_str(), O_RDWR);
    }

    while (1) {
        std::string message;
        message = recieve_message_server(fd_recv);
        std::cout << message << std::endl;
        std::string rcvd_usr = extract_login(message);          //от кого
        std::string rcvd_adressee = extract_addressee(message); //кому
        std::string rcvd_message = extract_message(message);    //что
        int fd_repl = in(logins, rcvd_adressee);                //id получателя
        int fd_usr = in(logins, rcvd_usr);                      //id отправителя
        int pos = -1;
        bool flag = 0;
        
        if (rcvd_adressee == "history") {
            // if (history.size() > 0) {
            //     for(int i = 0; i < history.size(); i++) {
            //         std::cout << "USER: " << logins[i] << std::endl;
            //         for(int j = 0; j < history[i][j].size(); j++) {
            //         std::cout << '\t' << history[i][j] << std::endl;
            //         }
            //     }
            // }
            std::string reply = "No matches found\n";
            for (int j = 0; j < history[fd_usr].size(); ++j) {
                if ((search(history[fd_usr][j], extract_text(message)))) {
                    reply = "[" + logins[fd_usr] + "] " + history[fd_usr][j];
                    std::cout << history[fd_usr][j] << std::endl;
                    send_message_to_client(fd[fd_usr], reply);
                    flag = 1;
                }
            }
            if(flag == 0) send_message_to_client(fd[fd_usr], reply);
        }
        else {
            for (int i = 0; i < history.size(); ++i) {
                if (logins[i] == rcvd_usr)
                    history[i].push_back(extract_text(message));
                if (logins[i] == rcvd_adressee && rcvd_usr != rcvd_adressee)
                    history[i].push_back(extract_text(message));
            }
            if (in(logins, rcvd_adressee) == -1) {
                send_message_to_client(fd[fd_usr], "Login does not exists!\n");
            }
            else {
                send_message_to_client(fd[fd_repl], rcvd_message);
            }
        }
    }
}