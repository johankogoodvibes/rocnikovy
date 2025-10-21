# Compiler and flags
CXXFLAGS = -O2 -g -std=c++17 -Wall -Wextra -Wconversion -Wfatal-errors -fsanitize=undefined,address
CXX = g++

# Compile source files to object files
checker-3n.o: checker-3n.cpp ../template.h
	$(CXX) -I.. -r checker-3n.cpp -o checker-3n.o

checker-sat.o: checker-sat.cpp ../template.h ../zChaff/libsat.a
	$(CXX) -I.. -r checker-sat.cpp ../zChaff/libsat.a -o checker-sat.o

checker-cadical.o: checker-cadical.cpp ../template.h ../cadical_compiled/libcadical.a
	$(CXX) -I.. -r checker-cadical.cpp ../cadical_compiled/libcadical.a -o checker-cadical.o

checker-kissat.o: checker-kissat.cpp ../template.h ../kissat_compiled/libkissat.a
	$(CXX) $(CXXFLAGS) -I.. -I../kissat_compiled -c checker-kissat.cpp -o checker-kissat.tmp.o
	ld -r -o checker-kissat.o checker-kissat.tmp.o ../kissat_compiled/libkissat.a
	rm -f checker-kissat.tmp.o
# Clean up
clean:
	rm -f color-3n.o
	rm -f color-sat.o
	rm -f color-cadical.o
	rm -f color-kissat.o
