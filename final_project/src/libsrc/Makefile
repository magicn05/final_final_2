INC = ../../include
CFLAGS = -fPIC -c  -g
OBJS = menu.o profile.o input.o
TARGET = libmenu.so libhash.so libarray.so 

all: libmenu.so libhash.so libarray.so

libmenu.so: $(OBJS)
	$(CXX) -shared -fPIC -o $@ $(OBJS)
#	cp lib$@.so ../../bin/

libhash.so: hash.o 
	$(CXX) -shared -fPIC -o $@ $<
#	cp lib$@.so ../../bin/

libarray.so: array.o
	$(CXX) -shared -fPIC -o $@ $<

.cpp.o:
	$(CXX) -fPIC -c -g -I$(INC) $< 

clean:
	rm *.o lib*.so
