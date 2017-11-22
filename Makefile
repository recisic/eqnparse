rexp :
	g++ -std=c++11 eqnparse.cpp -o eqnparse

all : eqnparse

run : eqnparse
	./eqnparse

clean :
	rm -f eqnparse