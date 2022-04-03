#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>
#include "server.hpp"

int main(int argc, char *argv[])
{   
    int CurNodeId = atoi(argv[1]);
    int ParentPort = atoi(argv[2]);

    zmq::context_t context(2);
    zmq::socket_t ChildSocket(context, ZMQ_REP);
    ChildSocket.connect(getPortName(ParentPort));

    zmq::socket_t req_socket(context, ZMQ_REQ);

    int ChildPort = bindSocket(req_socket);

    int ChildTopVertex = 0;

    std::string cmd;
    while(true) {
        std::string request = receiveMessage(ChildSocket); // receive message from parent (client) 
        std::istringstream cmdStream(request); // thread to read data from accepted string
        cmdStream >> cmd;
        if (cmd == "create") {
            int NodeId;
            cmdStream >> NodeId;
            if (CurNodeId == NodeId) { // if node has already been created
                sendMessage(ChildSocket, "Error: Already exists\n");
            } else { 
                if (ChildTopVertex == 0) {
                    int NodePid = fork();
                    if (NodePid == -1) {
                        sendMessage(ChildSocket, "Error: fork() fail\n");
                    } else if (NodePid == 0) {
                        execl("./server", "server", std::to_string(NodeId).c_str(), std::to_string(ChildPort).c_str(), NULL);
                    } else {
                        sendMessage(ChildSocket, "Ok: " + std::to_string(NodePid) + "\n");
                    }
                    ChildTopVertex = 1;
                } else {
                    sendMessage(req_socket, "create " + std::to_string(NodeId));
                    std::string AnsFromChild = receiveMessage(req_socket);
                    sendMessage(ChildSocket, AnsFromChild);
                }

            }
        } else if (cmd == "exec") {
            int NodeId;
            std::string Text, Pattern;
            cmdStream >> NodeId;
            cmdStream >> Text >> Pattern;
            if (NodeId == CurNodeId) {
                int pos = 0;
                std::vector<size_t> entries = KMP(Pattern, Text);
                std::string Ans;
                if (entries.size() != 0) {
                    for (int i = 0; i < entries.size(); i++) {
                        if (i == 0) Ans += std::to_string(entries[i]);
                        else  Ans += " " + std::to_string(entries[i]);
                    }
                } else {
                    Ans += "-1";
                }
                sendMessage(ChildSocket, "Ok: " + std::to_string(NodeId) + ": [" + Ans + "]\n"); // send result up
            } else {
                if (ChildTopVertex == 0) { // there is no node with this nodeid
                    sendMessage(ChildSocket, "Error: " + std::to_string(NodeId) + ": Not found\n");
                } else { // send message to next node 
                    sendMessage(req_socket, "exec " + std::to_string(NodeId) + " " + Text + " " + Pattern);
                    std::string Ans = receiveMessage(req_socket); // receive and send message to next node
                    sendMessage(ChildSocket, Ans);
                }
            }
        } else if (cmd == "pingid") {
            int NodeId;
            cmdStream >> NodeId;
            if (NodeId == CurNodeId) {
                sendMessage(ChildSocket, "Ok: 1\n");
            } else {
                if (ChildTopVertex == 0) {
                    sendMessage(ChildSocket, "Not found\n");
                } else {
                    sendMessage(req_socket, "pingid " + std::to_string(NodeId));
                    std::string ans = receiveMessage(req_socket);
                    if (ans == "Ok: 1\n") {
                        sendMessage(ChildSocket, ans);
                    } else {
                        sendMessage(ChildSocket, "Ok: 0\n");
                    }
                }
            }
        }
    }
    return 0;
}