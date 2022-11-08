all : server client
server:
	cd src/server; $(MAKE)

client:
	cd src/client; $(MAKE)

clean:
	cd src/client; rm *.o client
	cd src/server; rm *.o *.so server
	cd lib; rm *.so
	cd bin; rm client server