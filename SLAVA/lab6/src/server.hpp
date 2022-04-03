#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>
#include <zmq.hpp>

std::vector<size_t> prefix_function(std::string s) 
{
    size_t n = s.length();
    // в i-м элементе (его индекс i-1) количество 
    // совпавших символов в начале и конце для подстроки длины i.
    std::vector<size_t> pi(n);  
	// p[0]=0 всегда, p[1]=1, если начинается с двух одинаковых 
    for (size_t i=1; i<n; ++i) 
    {
       // ищем, какой префикс-суффикс можно расширить
        size_t j = pi[i-1]; // длина предыдущего префикса-суффикса, возможно нулевая
        while ((j > 0) && (s[i] != s[j])) // этот нельзя расширить,
            j = pi[j-1];   // берем длину меньшего префикса-суффикса

        if (s[i] == s[j]) 
            ++j;  // расширяем найденный (возможно пустой) префикс-суффикс
        pi[i] = j;
     }
     return pi;
}

std::vector<size_t> KMP(std::string pattern, std::string text)
{
    int n = text.length();
    int m = pattern.length();

    std::vector<size_t> Lps = prefix_function(pattern); // применяем префиекс функцию
    std::vector<size_t> out; // вектор с индексами вхождений
        
    int i = 0, j = 0;
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++; j++;
        } // если совпало, продолжаем
        if (j == m) { // если j==m это подтверждает то, что мы нашли образец в тексте
            out.push_back(i - m); // добавляем этот индекс минус длина образца в вектор out  
            j = Lps[j - 1]; // обновляем j как префикс последнего совпавшего символа 
        } else if (i < n && pattern[j] != text[i]) {  // если не совпало
            if (j == 0) {
                i++; // если j становится равным нулю, делаем инкремент индекса i
            } else {
                j = Lps[j - 1]; // обновляем j как префикс последнего совпавшего символа
            }
        }
    }
    return out;
}

// send message to the particular socket
bool sendMessage(zmq::socket_t &socket, const std::string &message_string)
{
    // message size init
    zmq::message_t message(message_string.size());
    // message content init
    memcpy(message.data(), message_string.c_str(), message_string.size());
    return socket.send(message);
}

std::string receiveMessage(zmq::socket_t &socket)
{
    zmq::message_t message;
    int recResult;
    // receiving message from socket 
    try {
        recResult = (int)socket.recv(&message);
        if (recResult < 0) {
            perror("socket.recv()");
            exit(1);
        }
    }
    catch (...) {
        recResult = 0;
    }
    // transform to string
    std::string recieved_message((char *)message.data(), message.size());
    if (recieved_message.empty() || !recResult) {
        return "Error: Node is not available";
    }
    return recieved_message;
}

std::string getPortName(int port)
{
    return "tcp://127.0.0.1:" + std::to_string(port);
}

int bindSocket(zmq::socket_t &socket)
{
    int port = 8080;
    // create endpoint and bind it to the socket
    while (true) {
        try {
            socket.bind(getPortName(port)); // создаёт сокет
            break;
        }
        catch (...) { // в случае неудачи используем другой сокет
            port++;
        }
    }
    return port;
}

