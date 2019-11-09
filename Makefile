raytracer : main.o
	g++ -std=c++11 -pthread -o raytracer main.o

main.o : main.cpp
	g++ -c main.cpp

clean :
	rm -f raytracer main.o