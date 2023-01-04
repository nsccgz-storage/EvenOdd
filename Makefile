CXX = g++
CFLAGS = -I./src/ -I./ -fopenmp -lpthread -lrt
VPATH = src:util
all: evenodd time_check
evenodd: decoding.o encoding.o repair.o log.o
	$(CXX) -O3 -o evenodd src/evenodd.cpp decoding.o encoding.o repair.o log.o $(CFLAGS)

time_check: decoding.o encoding.o
	$(CXX) -O3 -o time_check src/time_check.cpp decoding.o encoding.o repair.o log.o $(CFLAGS)

decoding.o: decoding.cpp decoding.h log.o
	$(CXX) -O3 -c src/decoding.cpp log.o $(CFLAGS)

encoding.o: encoding.cpp encoding.h log.o
	$(CXX) -O3 -c src/encoding.cpp log.o $(CFLAGS)

repair.o: repair.cpp repair.h log.o
	$(CXX) -O3 -c src/repair.cpp log.o $(CFLAGS)

log.o: log.cpp log.h
	$(CXX) -O3 -c util/log.cpp $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o