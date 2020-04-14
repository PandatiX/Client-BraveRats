#ifndef CLIENTBRAVERATS_TERMINAL_HPP
#define CLIENTBRAVERATS_TERMINAL_HPP

#define DEFPORT 3490
#define DEFIP "127.0.0.1"
#define MAXDATASIZE 1024

#include <thread>
#include <string>
#include <list>
#include <mutex>
#include <netinet/in.h>
#include "barriers/BarrierGMLS.hpp"
#include "Game.hpp"

class Terminal {
private:
    Game* game;

    BarrierGMLS barrierGMLS;
    std::string games;
    std::thread thread;
    std::mutex mutexHistory;
    std::list<std::string> history;
    bool running, inGame;
    struct sockaddr_in server_addr{};
    int sockfd;

    void sendMessage(int, const std::string&);

public:
    Terminal(Game* game);

    static void start(Terminal*, std::string *);
    int connectServer(in_addr_t, uint16_t);
    void disconnectServer();
    void sendMessage(const std::string&);
    void joinGame(const char*);
    void addHistory(const std::string&);
    void clearHistory();

    bool isConnectedServer();
    bool isInGame();
    std::list<std::string> getHistory();
    std::list<std::string> getGames();

    ~Terminal();
};

#endif //CLIENTBRAVERATS_TERMINAL_HPP