.SUFFIXED:.c .o

OBJS = client.o
SRCS = client.cpp
CXX = g++

client: $(OBJS)
	$(CXX) -o client $(OBJS) -lpthread

# .cpp.o:
# 	$(CXX) -c $<

clean:
	rm *.o client

client.o : client.cpp