#include <cstdlib>
#include <iostream>
#include "src/Application.hpp"

int main(int argc, char **argv) {

    try {

        Application app;
        app.loop();

    } catch (const std::exception &e) {

        std::cerr << e.what() << std::endl;
        std::cerr << "Exiting due to a fatal and unrecoverable error." << std::endl;
        return EXIT_FAILURE;

    } catch (...) {

        std::cerr << "Exiting due to a fatal and unrecoverable error." << std::endl;
        return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;

}