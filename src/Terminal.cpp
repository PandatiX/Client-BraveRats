#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <nlohmann/json.hpp>

#include "Terminal.hpp"

void Terminal::sendMessage(int fd, const std::string& outputString) {
    //Make sure outputString isn't n*MAXDATASIZE (could crash client)
    std::string sMsg = outputString;
    if (sMsg.length() % MAXDATASIZE == 0)
        sMsg.append("");
    std::cout << "Client send: " << sMsg.c_str() << std::endl;
    addHistory("[Client] " + sMsg);
    send(fd, sMsg.c_str(), sMsg.length()+1, MSG_NOSIGNAL);
}

Terminal::Terminal(Game* game) : game(game), running(true), inGame(false), sockfd(0) {}

void Terminal::sendMessage(const std::string& outputString) {
    if (isConnectedServer()) {
        sendMessage(sockfd, outputString);
        //Ensure that app won't crash if user submit GMLS from terminal
        if (outputString == "GMLS")
            barrierGMLS.wait();
    }
}

void Terminal::joinGame(const char* gameID) {

    sendMessage("JOIN " + std::string(gameID));
    inGame = true;

}

int Terminal::connectServer(in_addr_t IP, uint16_t port) {

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "[-] Error in connection: ";
        return EXIT_FAILURE;
    }
    std::cout << "[+] Client Socket is created." << std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = IP;
    bzero(&(server_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "[-] Error in connection.";
        sockfd = 0;
        return EXIT_FAILURE;
    } else {
        std::cout << "[+] Connected to Server." << std::endl;
        std::thread(start, this, &games).detach();
    }

    return EXIT_SUCCESS;

}

void Terminal::disconnectServer() {

    if (isConnectedServer()) {
        std::cout << "Disconnected" << std::endl;
        close(sockfd);
        sockfd = 0;
        inGame = false;
        running = false;
    }

}

void Terminal::start(Terminal *_this, std::string *games) {

    if (_this->isConnectedServer()) {

        int nbrecv;
        char tmpBuffer[MAXDATASIZE + 1];
        std::ostringstream stream;
        std::string buffer;

        while (_this->running) {

            buffer = std::string("");
            stream.str("");

            do {
                bzero(tmpBuffer, sizeof(tmpBuffer));
                nbrecv = recv(_this->sockfd, tmpBuffer, MAXDATASIZE, 0);
                if (buffer.size() < buffer.max_size() - MAXDATASIZE)
                    buffer.append(tmpBuffer);
            } while (nbrecv == MAXDATASIZE);
            std::cout << "Server sent: " << buffer << std::endl;
            _this->addHistory("[Server] " + buffer);

            if (nbrecv <= 0) {

                std::cerr << "Terminating play" << std::endl;
                _this->running = false;

            } else if (buffer.size() < 4) {

                std::cerr << "Buffer length too short" << std::endl;

            } else if (buffer.substr(0, 4) == "WONP") {

                stream << "PNTS" << std::endl;
                _this->sendMessage(stream.str());

            } else if (buffer.substr(0, 4) == "GMLS") {

                *games = buffer;
                _this->barrierGMLS.wait();

            } else if (buffer.substr(0, 4) == "CRDD") {

                if (buffer.size() > 5) {

                    try {

                        nlohmann::json j = nlohmann::json::parse(buffer.substr(5)),
                                jArrSelf = j["self"],
                                jArrOther = j["other"];

                        if (jArrSelf.size() == 8 && jArrOther.size() == 8) {
                            int cardsSelf[8], cardsOther[8];
                            for (int i = 0; i < 8; i++) {
                                cardsSelf[i] = jArrSelf[i];
                                cardsOther[i] = jArrOther[i];
                            }

                            _this->game->setCardsSelf(cardsSelf);
                            _this->game->setCardsOther(cardsOther);
                        } else {
                            std::cerr << "Not enough cards" << std::endl;
                        }


                    } catch (const nlohmann::json::exception& e) {
                        std::cerr << "Unable to parse " << buffer << std::endl;
                    }

                }

            } else if (buffer.substr(0, 4) == "REVD") {

                if (buffer.size() > 5) {

                    int code = (int)strtol(buffer.substr(5).c_str(), NULL, 10);
                    switch (code) {
                        case 401:
                            _this->inGame = false;
                            break;
                        case 404:
                            _this->inGame = true;
                            break;
                        default:
                            break;
                    }

                }

            }

        }

    }

}

void Terminal::addHistory(const std::string& s) {
    mutexHistory.lock();
    history.push_back(s);
    mutexHistory.unlock();
}

bool Terminal::isConnectedServer() {
    return (sockfd != 0);
}

bool Terminal::isInGame() {
    return inGame;
}

std::list<std::string> Terminal::getHistory() {
    mutexHistory.lock();
    auto h = history;
    mutexHistory.unlock();
    return h;
}

std::list<std::string> Terminal::getGames() {
    //Ask for games
    sendMessage(sockfd, "GMLS");
    barrierGMLS.wait();

    std::cout << games << std::endl;
    nlohmann::json j = nlohmann::json::parse(games.substr(5));
    nlohmann::json jArr = j["players"];

    int size = jArr.size();
    std::list<std::string> out;
    for (int i = 0; i < size; i++) {
        out.emplace_back(to_string(jArr[i]));
    }

    return out;
}

void Terminal::clearHistory() {
    mutexHistory.lock();
    history.clear();
    mutexHistory.unlock();
}

Terminal::~Terminal() = default;