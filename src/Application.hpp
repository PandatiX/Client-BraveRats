#ifndef CLIENTBRAVERATS_APPLICATION_HPP
#define CLIENTBRAVERATS_APPLICATION_HPP

#include <list>
#include <string>

#include "Window.hpp"
#include "Terminal.hpp"
#include "Game.hpp"

class Application {
private:
    const char* appName = "BraveRats";
    std::list<std::string> history;

    Game game;
    Window window;
    Terminal terminal;

public:
    Application();

    void loop();

    ~Application() = default;

};

#endif //CLIENTBRAVERATS_APPLICATION_HPP
