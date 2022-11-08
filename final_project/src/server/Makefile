INC = ../../include
CXX = g++
CXXFLAGS = -fPIC -c  -g
LDFLAGS = -L../../lib -lboard -lmanager -ldownload   -ldl -lpthread -lboost_serialization
#LDFLAGS = -L../libsrc -lmenu -lhash -lpthread -ldl -lboost_serialization
#LDFLAGS = -L../../lib -lmenu -lprofile -lhash -lio  -lpthread
#OBJS = add.o sub.o board.o
OBJ1 = server.o
#OBJ1 = server.o user.o manager.o data.o board.o text_reader.o text_writer.o download.o myfile.o reply.o

all: libboard.so libdownload.so libmanager.so server
#libadd.so libsub.so libboard.so

server: $(OBJ1)
	$(CXX) -o $@ $(OBJ1) $(LDFLAGS)

# server.o: server.cpp user.cpp manager.cpp
# 	$(CXX) -c -I$(INC) -g $< 

.cpp.o:
	$(CXX) -fPIC -c -I$(INC) -g $< 


libboard.so: board.o text_reader.o text_writer.o
	$(CXX) -shared -fPIC -o $@ board.o text_reader.o text_writer.o
	cp libboard.so ../../lib/

libdownload.so: download.o
	$(CXX) -shared -fPIC -o $@ $<
	cp libdownload.so ../../lib/

libmanager.so: data.o myfile.o user.o reply.o manager.o
	$(CXX) -shared -fPIC -o $@ data.o myfile.o user.o reply.o manager.o
	cp libmanager.so ../../lib/

clean:
	rm *.o lib*.so server 
