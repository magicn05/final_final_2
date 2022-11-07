INC = ../../include
CXX = g++
CXXFLAGS = -fPIC -c  -g
LDFLAGS = -L../libsrc -lmenu -lhash -lpthread -ldl -lboost_serialization
#LDFLAGS = -L../../lib -lmenu -lprofile -lhash -lio  -lpthread
OBJS = add.o sub.o board.o
OBJ1 = server.o user.o manager.o data.o board.o text_reader.o text_writer.o download.o myfile.o reply.o

all: server libadd.so libsub.so libboard.so

server: $(OBJ1)
	$(CXX) -o $@ $(OBJ1) $(LDFLAGS)

# server.o: server.cpp user.cpp manager.cpp
# 	$(CXX) -c -I$(INC) -g $< 

.cpp.o:
	$(CXX) -fPIC -c -I$(INC) -g $< 

libboard.so: board.o
	$(CXX) -shared -fPIC -o $@ $<

libadd.so: add.o
	$(CXX) -shared -fPIC -o $@ $<
#	cp lib$@.so ../../bin/

libsub.so: sub.o
	$(CXX) -shared -fPIC -o $@ $<
#	cp lib$@.so ../../bin/

clean:
	rm *.o lib*.so server 
