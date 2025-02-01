#include "repl.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 2) {
        // If a filename is provided, execute commands from the file
        executeFile(argv[1]);
    } else {
        // Otherwise, start the interactive REPL
        runRepl();
    }

    return 0;
}