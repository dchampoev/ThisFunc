CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: thisFuncInterpreter

thisFuncInterpreter: main.o repl.o parser.o lexer.o interpreter.o
	$(CXX) $(CXXFLAGS) -o thisFuncInterpreter main.o repl.o parser.o lexer.o interpreter.o

main.o: main.cpp repl.h interpreter.h
	$(CXX) $(CXXFLAGS) -c main.cpp

repl.o: repl.cpp repl.h interpreter.h
	$(CXX) $(CXXFLAGS) -c repl.cpp

parser.o: parser.cpp parser.h
	$(CXX) $(CXXFLAGS) -c parser.cpp

lexer.o: lexer.cpp lexer.h
	$(CXX) $(CXXFLAGS) -c lexer.cpp

interpreter.o: interpreter.cpp interpreter.h parser.h lexer.h
	$(CXX) $(CXXFLAGS) -c interpreter.cpp

clean:
	rm -f *.o thisFuncInterpreter