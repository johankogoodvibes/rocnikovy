# Compiler and flags
CXXFLAGS = -O2 -g -std=c++17 -Wall -Wextra -Wconversion -Wfatal-errors -fsanitize=undefined,address
CXX = g++

# Compile source files to object files
checker-3n.o: checker-3n.cpp ../template.h
	$(CXX) -I.. -r checker-3n.cpp -o checker-3n.o

checker-sat.o: checker-sat.cpp ../template.h ../zChaff/libsat.a
	$(CXX) -I.. -r checker-sat.cpp ../zChaff/libsat.a -o checker-sat.o

# Clean up
clean:
	rm -f color-3n.o
	rm -f color-sat.o
