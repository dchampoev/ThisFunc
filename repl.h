#ifndef REPL_H
#define REPL_H
#include <string>

void runRepl();               // Runs the interactive REPL
void executeFile(const std::string& filename); // Executes commands from a file

#endif // REPL_H