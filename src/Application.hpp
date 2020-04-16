#ifndef CLIENTBRAVERATS_APPLICATION_HPP
#define CLIENTBRAVERATS_APPLICATION_HPP

#include <list>
#include <string>
#include <mutex>

#include "Window.hpp"
#include "Terminal.hpp"
#include "Game.hpp"

class Application {
private:
    const char* appName = "BraveRats";
    bool resetConnectionSettings;
    std::mutex mutexrResetConnectionSettings;
    bool refreshOnOpen;
    std::mutex mutexRefeshOnOpen;
    std::list<std::string> history;

    Game game;
    Window window;
    Terminal terminal;

public:
    Application();

    void loop();
    void resetUI();

    ~Application() = default;

};

#endif //CLIENTBRAVERATS_APPLICATION_HPP
