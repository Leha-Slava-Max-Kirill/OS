#include <string>

void send_message_to_server(int fd, std::string curlogin, std::string user, std::string message) {
    std::string text = curlogin + "$" + user + "$" + message;
    int k = text.size();
    write(fd, &k, sizeof(k));
    char messagec[k];
    for (int i = 0; i < k; ++i) {
        messagec[i] = text[i];
    }
    write(fd, messagec, k);
}
void send_message_to_client(int fd, std::string message) {
    std::string text = message;
    int k = text.size();
    write(fd, &k, sizeof(k));
    char messagec[k];
    for (int i = 0; i < k; ++i) {
        messagec[i] = text[i];
    }
    write(fd, messagec, k);
}

std::string recieve_message_client(int fd) {
    int size;
    read(fd, &size, sizeof(size));
    char messagec[size];
    read(fd, messagec, size);
    std::string recv;
    for (int i = 0; i < size; ++i) {
        if (messagec[i] != '$') {
            recv.push_back(messagec[i]);
        }
        else {
            recv = recv + ": ";
        }
    }
    return recv;
}
std::string recieve_message_server(int fd) {
    int size;
    read(fd, &size, sizeof(size));
    char messagec[size];
    read(fd, messagec, size);
    std::string recv;
    for (int i = 0; i < size; ++i) {
        recv.push_back(messagec[i]);
    }
    return recv;
}

std::string extract_login(std::string message) {
    std::string login;
    int i = 0;
    while (message[i] != '$') {
        login.push_back(message[i]);
        ++i;
    }
    return login;
}

std::string extract_message(std::string message) {
    std::string text, text1, text2;
    int i = 0;
    while (message[i] != '$') {
        text1.push_back(message[i]);
        ++i;
    }
    ++i;
    while (message[i] != '$') {
        ++i;
    }
    while (i < message.size()) {
        text2.push_back(message[i]);
        ++i;
    }
    text = text1 + text2;
    return text;
}

std::string extract_addressee(std::string message) {
    std::string text;
    int i = 0;
    while (message[i] != '$') {
        ++i;
    }
    ++i;
    while (message[i] != '$') {
        text.push_back(message[i]);
        ++i;
    }
    return text;
}
std::string extract_text(std::string message) {
    std::string text;
    int i = 0;
    while (message[i] != '$') {
        ++i;
    }
    ++i;
    while (message[i] != '$') {
        ++i;
    }
    ++i;
    ++i;
    while (i < message.size()) {
        text.push_back(message[i]);
        ++i;
    }
    return text;
}

bool search(std::string text, std::string pattern) {
    if (pattern.size() <= text.size()) {
        for (int i = 0; i <= text.size() - pattern.size(); ++i) {
            std::string pat;
            for (int z = 0; z < pattern.size(); ++z) {
                if (text[i + z] == pattern[z])
                    pat.push_back(text[i + z]);
            }
            if (pat == pattern) {
                return true;
            }
            pat.clear();
        }
    }
    return false;
}
