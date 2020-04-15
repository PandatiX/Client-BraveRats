#include <arpa/inet.h>
#include <sstream>
#include "Application.hpp"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/imgui_impl_glfw.h"
#include "../lib/imgui/imgui_impl_opengl3.h"

void titleDisplay(const char* title) {
    ImGui::TextColored(ImVec4(0.95f, 0.78f, 0.25f, 1.0f), "%s", title);
}

//TODO parameter pack for multiple details
void textWithDetailDisplay(const char* main, const char* detail) {
    ImGui::Text("%s", main); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.22f, 0.26f, 0.33f, 1.0f), "\t\t%s", detail);
}

Application::Application() : game(), window(&game, appName, 1920, 1080), terminal(&game) {}

void Application::loop() {

    // Decide GL+GLSL versions
    #if __APPLE__
    // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    char bufTerminal[10], bufIP[16], bufPort[8];
    std::list<std::string> games;
    bool resetConnectionSettings = true, refreshOnOpen = true, cantConnectMessage = false;

    while (!window.shouldClose()) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (window.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
            window.close();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!terminal.isConnectedServer()) {

            /***********************/
            /** CONNECTION WINDOW **/
            /***********************/
            if (resetConnectionSettings) {
                sprintf(bufIP, "%s", DEFIP);
                sprintf(bufPort, "%d", DEFPORT);
                resetConnectionSettings = false;
            }

            ImGui::Begin("Connection");
            ImGui::InputTextWithHint("IP", "XXX.XXX.XXX.XXX", bufIP, IM_ARRAYSIZE(bufIP));
            ImGui::InputTextWithHint("Port", "XXXXX", bufPort, IM_ARRAYSIZE(bufPort));
            if (ImGui::Button("Connect")
                && strncmp(bufIP, "", 1) != 0
                && strncmp(bufPort, "", 1) != 0) {
                in_addr_t IP = inet_addr(bufIP);
                uint16_t port = htons(std::stoi(bufPort));
                cantConnectMessage = (terminal.connectServer(IP, port) == EXIT_FAILURE);
                bzero(bufIP, sizeof(bufIP));
                bzero(bufPort, sizeof(bufPort));
            }

            if (cantConnectMessage) {
                ImGui::Text("Can't connect to server");
            }

            ImGui::End();

        } else {

            if (terminal.isInGame()) {

                /*************/
                /** CONSOLE **/
                /*************/
                //Update history for terminal
                history = terminal.getHistory();

                ImGui::Begin("Console", nullptr, 0);

                ImGui::SetWindowPos(ImVec2(40, 40), ImGuiCond_Once);
                ImGui::SetWindowSize(ImVec2(600, 800), ImGuiCond_Once);

                if (ImGui::BeginTabBar("Console")) {
                    /*****CONSOLE TAB*****/
                    if (ImGui::BeginTabItem("Console")) {

                        if (ImGui::Button("Disconnect")) {
                            terminal.disconnectServer();
                            resetConnectionSettings = true;
                            refreshOnOpen = true;
                        }

                        ImGui::BeginChild("Scrolling");
                        for (const auto &h : history) {
                            ImGui::Text("%s", h.c_str());
                            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                                ImGui::SetScrollHere(1.0f);
                        }
                        ImGui::EndChild();

                        //Input fields
                        ImGui::PushItemWidth(-1.0f);
                        ImGui::InputTextWithHint("InputTerminal", "Message to send to the server", bufTerminal, IM_ARRAYSIZE(bufTerminal), 0);
                        ImGui::PopItemWidth();

                        if (ImGui::Button("Clear")) {
                            terminal.clearHistory();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Send")) {
                            terminal.sendMessage(std::string(bufTerminal));
                            bzero(bufTerminal, sizeof(bufTerminal));
                        }

                        ImGui::EndTabItem();

                    }
                    /*****HELP TAB*****/
                    if (ImGui::BeginTabItem("Help")) {

                        titleDisplay("COMMANDS");
                        textWithDetailDisplay("STRT <0/1>", "start a game:\n\t- 0: IA\n\t- 1: other player");
                        textWithDetailDisplay("STOP", "if game creator, stop the game, else, leave it");
                        textWithDetailDisplay("GMLS", "get joinable games");
                        textWithDetailDisplay("JOIN", "join a game based on its ID (see GMLS)");
                        textWithDetailDisplay("LEAV", "if joined a game, leav it, else, stop it");
                        textWithDetailDisplay("CRDD", "draw cards at the beginning of the game");
                        textWithDetailDisplay("CRDP <CARD>", "play a card");
                        textWithDetailDisplay("CRDA", "ask for fighter's card as the spy");
                        textWithDetailDisplay("CRDH [0/1]", "ask for game's history\n\t- [NONE]: ask for all the history\n\t- 0: ask for own history\n\t- 1: ask for fighter's history");
                        textWithDetailDisplay("WONP", "get play winner");
                        textWithDetailDisplay("PNTS", "get points");
                        textWithDetailDisplay("REVD", "acquittal");
                        ImGui::Separator();

                        titleDisplay("CARDS");
                        textWithDetailDisplay("0", "Musician");
                        textWithDetailDisplay("1", "Princess");
                        textWithDetailDisplay("2", "Spy");
                        textWithDetailDisplay("3", "Assassin");
                        textWithDetailDisplay("4", "Ambassador");
                        textWithDetailDisplay("5", "Magician");
                        textWithDetailDisplay("6", "General");
                        textWithDetailDisplay("7", "Prince");
                        ImGui::Separator();

                        titleDisplay("REVD");
                        textWithDetailDisplay("200", "SUCCESS");
                        textWithDetailDisplay("401", "GAME_ENDED");
                        textWithDetailDisplay("402", "GAME_RESTARTED");
                        textWithDetailDisplay("403", "ACCESS_DENIED");
                        textWithDetailDisplay("404", "GAME_STARTED");

                        ImGui::EndTabItem();
                    }
                    /***** CARDS TAB *****/
                    if (ImGui::BeginTabItem("Cards")) {

                        int *cardsSelf = game.getCardsSelf();
                        titleDisplay("SELF CARDS");
                        for (int i = 0; i < 8; i++) {
                            ImGui::Text("%s", std::to_string(cardsSelf[i]).c_str());
                        }

                        int *cardsOther = game.getCardsOther();
                        titleDisplay("OTHER CARDS");
                        for (int i = 0; i < 8; i++) {
                            ImGui::Text("%s", std::to_string(cardsOther[i]).c_str());
                        }

                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }

                ImGui::End();

            } else {

                /****************/
                /** GAMES MENU **/
                /****************/
                ImGui::Begin("Games");

                if (ImGui::Button("Start against IA")) {
                    terminal.sendMessage("STRT 0");
                } ImGui::SameLine();
                if (ImGui::Button("Start joinable game")) {
                    terminal.sendMessage("STRT 1");
                } ImGui::SameLine();
                if (refreshOnOpen || ImGui::Button("Refresh")) {
                    games = terminal.getGames();
                    refreshOnOpen = false;
                }

                //Display games if there are available
                int size = games.size();
                if (size > 0) {
                    const char *listbox_items[size];
                    int i = 0;
                    for (const auto& g : games) {
                        listbox_items[i++] = g.c_str();
                    }
                    static int listbox_item_current = 0;

                    ImGui::ListBox("Joinable games", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items));
                    if (ImGui::Button("Join")) {
                        terminal.joinGame(listbox_items[listbox_item_current]);
                    }
                } else {
                    ImGui::Text("No game available");
                }

                ImGui::End();

            }

        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swapBuffers();
        glfwPollEvents();

    }

}