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

Terminal::Terminal(Application* application, Game* game) : application(application), barrierGMLS(2), game(game), running(true), inGame(false), sockfd(0) {}

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
        running = true;
        thread = std::thread(start, this, &games);
    }

    return EXIT_SUCCESS;

}

void Terminal::disconnectServer() {

    if (isConnectedServer()) {
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        thread.join();
        application->resetUI();
        sockfd = 0;
        inGame = false;
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

            if (nbrecv <= 0) {

                std::cerr << "Terminating play" << std::endl;
                _this->running = false;

            } else {

                std::cout << "Server sent: " << buffer << std::endl;
                _this->addHistory("[Server] " + buffer);

                if (buffer.size() < 4) {

                    std::cerr << "Buffer length too short" << std::endl;

                } else {

                    std::basic_string<char> keyword = buffer.substr(0, 4);

                    if (keyword == "WONP") {

                        bool sendPNTS = true;
                        if (buffer.size() >= 12) {
                            std::cout << buffer.substr(7, 5);
                            if (buffer.substr(7, 5) == "FINAL") {
                                sendPNTS = false;
                            }
                        }
                        if (sendPNTS) {
                            stream << "PNTS" << std::endl;
                            _this->sendMessage(stream.str());
                        }

                    } else if (keyword == "PNTS") {

                        stream << "CRDH 1" << std::endl;
                        _this->sendMessage(stream.str());

                    } else if (keyword == "GMLS") {

                        *games = buffer;
                        _this->barrierGMLS.wait();

                    } else if (keyword == "CRDD") {

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

                            } catch (const nlohmann::json::exception &e) {
                                std::cerr << "Unable to parse " << buffer << std::endl;
                            }

                        }

                    } else if (keyword == "CRDH") {

                        if (buffer.size() > 5) {

                            try {

                                nlohmann::json j = nlohmann::json::parse(buffer.substr(5))["play"][0];
                                int *cardsSelf = _this->game->getCardsSelf();
                                int *cardsOther = _this->game->getCardsOther();

                                int cardPlayedSelf = j["self"];
                                int cardPlayedOther = j["other"];

                                if (cardPlayedSelf >= 0 && cardPlayedSelf < 8
                                    && cardPlayedOther >= 0 && cardPlayedOther < 8
                                    && cardsSelf[cardPlayedSelf] > 0
                                    && cardsOther[cardPlayedOther] > 0) {

                                    cardsSelf[j["self"]]--;
                                    cardsOther[j["other"]]--;

                                    _this->game->setCardsSelf(cardsSelf);
                                    _this->game->setCardsOther(cardsOther);

                                } else
                                    std::cerr << "Bad cards index in CRDH" << std::endl;

                            } catch (const nlohmann::json::exception &e) {
                                std::cerr << "Unable to parse " << buffer << std::endl;
                            }

                        }

                    } else if (keyword == "REVD") {

                        if (buffer.size() > 5) {

                            int code = (int) strtol(buffer.substr(5).c_str(), NULL, 10);
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

    }

}

void Terminal::addHistory(const std::string& s) {
    mutexHistory.lock();
    history.push_back(s);
    mutexHistory.unlock();
}

bool Terminal::isConnectedServer() const {
    return (sockfd != 0);
}

bool Terminal::isInGame() const {
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

Terminal::~Terminal() {
    disconnectServer();
}