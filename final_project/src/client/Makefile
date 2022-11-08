.SUFFIXED:.c .o
INC = ../../include
OBJS = client.o
SRCS = client.cpp
CXX = g++

client: $(OBJS)
	$(CXX) -o client $(OBJS) -lpthread
		cp client ../../bin

.cpp.o:
	$(CXX) -c -g $<

# .cpp.o:
# 	$(CXX) -c $<

clean:
	rm *.o client
