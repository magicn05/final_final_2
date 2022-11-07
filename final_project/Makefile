all : server client libsrc

server:
	cd src/server; $(MAKE)

client:
	cd src/client; $(MAKE)

libsrc:
	cd src/libsrc; $(MAKE)

clean:
	cd src/server; rm *.o server
	cd src/client; rm *.o client
	cd src/libsrc; rm *.o lib*.so 