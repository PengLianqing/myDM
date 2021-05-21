TARGET = test
CC =g++ -L../src -g -O3 -Wall -std=c++17
INCLUDE =$(shell find ./ -name "*.h*")
SOURCE =$(shell find ./ -name "*.cpp")
OBJS =$(SOURCE:%.cpp=%.o)
LIBS =-pthread -latomic -I/usr/include/mysql
CFLAGS =-I./

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.cpp $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
	
all:$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
