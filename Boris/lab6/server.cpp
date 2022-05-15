#include <iostream>
#include <string>
#include <vector>
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

    zmq::socket_t LeftChildSocket(context, ZMQ_REQ);
    zmq::socket_t RightChildSocket(context, ZMQ_REQ);

    int LeftPort = bindSocket(LeftChildSocket);
    int RightPort = bindSocket(RightChildSocket);

    int LeftTopVertex = 0;
    int RightTopVertex = 0;

    std::string cmd;
    while(true) {
        std::string request = receiveMessage(ChildSocket);
        std::istringstream cmdStream(request);
        cmdStream >> cmd;
        if (cmd == "create") {
            int NodeId;
            cmdStream >> NodeId;
            if (CurNodeId == NodeId) {
                sendMessage(ChildSocket, "Error: Already exists\n");
            } else {
                if (NodeId < CurNodeId) {
                    if (LeftTopVertex == 0) {
                        int NodePid = fork();
                        if (NodePid == -1) {
                            sendMessage(ChildSocket, "Error: fork() fail\n");
                        } else if (NodePid == 0) {
                            execl("./server", "server", std::to_string(NodeId).c_str(), std::to_string(LeftPort).c_str(), NULL);
                        } else {
                            sendMessage(ChildSocket, "Ok: " + std::to_string(NodePid) + "\n");
                        }
                        LeftTopVertex = 1;
                    } else {
                        sendMessage(LeftChildSocket, "create " + std::to_string(NodeId));
                        std::string AnsFromLeft = receiveMessage(LeftChildSocket);
                        sendMessage(ChildSocket, AnsFromLeft);
                    }
                } else {
                    if (RightTopVertex == 0) {
                        int NodePid = fork();
                        if (NodePid == -1) {
                            sendMessage(ChildSocket, "Error: fork() fail\n");
                        } else if (NodePid == 0) {
                            execl("./server", "server", std::to_string(NodeId).c_str(), std::to_string(RightPort).c_str(), NULL);
                        } else {
                            sendMessage(ChildSocket, "Ok: " + std::to_string(NodePid) + "\n");
                        }
                        RightTopVertex = 1;
                    } else {
                        sendMessage(RightChildSocket, "create " + std::to_string(NodeId));
                        std::string AnsFromRight = receiveMessage(RightChildSocket);
                        sendMessage(ChildSocket, AnsFromRight);
                    }
                }
            }
        } else if (cmd == "exec") {
            int NodeId;
            std::string Text, Pattern;
            cmdStream >> NodeId;
            cmdStream >> Text >> Pattern;
            if (NodeId == CurNodeId) {
                int pos = 0;
                std::vector<int>entiers;
                while (std::string::npos != (pos = Text.find(Pattern, pos))) {
                    entiers.push_back(pos);
                    ++pos;
                }
                std::string Ans;
                if (entiers.size() != 0) {
                    for (int i = 0; i < entiers.size(); i++) {
                        Ans += " " + std::to_string(entiers[i]);
                    }
                } else {
                    Ans += "-1";
                }
                sendMessage(ChildSocket, "Ok: " + std::to_string(NodeId) + ": [" + Ans + "]\n"); // отправляем результат наверх
            } else if (NodeId < CurNodeId) {
                if (LeftTopVertex == 0) {
                    sendMessage(ChildSocket, "Error: " + std::to_string(NodeId) + ": Not found\n");
                } else {
                    sendMessage(LeftChildSocket, "exec " + std::to_string(NodeId) + " " + Text + " " + Pattern);
                    std::string Ans = receiveMessage(LeftChildSocket);
                    sendMessage(ChildSocket, Ans);
                }
            } else {
                if (RightTopVertex == 0) {
                    sendMessage(ChildSocket, "Error: " + std::to_string(NodeId) + ": Not found\n");
                } else {
                    sendMessage(RightChildSocket, "exec " + std::to_string(NodeId) + " " + Text + " " + Pattern);
                    std::string Ans = receiveMessage(RightChildSocket);
                    sendMessage(ChildSocket, Ans);
                }
            }
        } else if (cmd == "heartbit") {
            //std::string Answer, LeftAns, RightAns; // записывать информацию о текущем узле не нужно
            //int time;
            //cmdStream >> time;
            //if ()
            //int start = clock();
            //int end = start;
            //int k = 0; // флаг, что сигнал о работоспособности ещё не послан
            //while (end - start < 4 * time) {
            //    if (end - start == time) {
            //        sendMessage(ChildSocket, "Ok: " + std::to_string(CurNodeId) + " ");
            //    }
            //    end = clock();
            //}
        } else if (cmd == "pingid") {
            int NodeId;
            cmdStream >> NodeId;
            if (NodeId == CurNodeId) {
                sendMessage(ChildSocket, "Ok: 1\n");
            } else if (NodeId < CurNodeId) {
                if (LeftTopVertex == 0) {
                    sendMessage(ChildSocket, "Not found\n");
                } else {
                    sendMessage(LeftChildSocket, "pingid " + std::to_string(NodeId));
                    std::string ans = receiveMessage(LeftChildSocket);
                    if (ans == "Ok: 1\n") {
                        sendMessage(ChildSocket, ans);
                    } else {
                        sendMessage(ChildSocket, "Ok: 0\n");
                    }
                }
            } else {
                if (RightTopVertex == 0) {
                    sendMessage(ChildSocket, "Not found\n");
                } else {
                    sendMessage(RightChildSocket, "pingid " + std::to_string(NodeId));
                    std::string ans = receiveMessage(RightChildSocket);
                    //std::cout << ans << "\n";
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