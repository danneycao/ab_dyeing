THRIFT=/usr/local/thrift-0.9.2
MAIN=abclient
CC=g++ -Wall -Wno-deprecated -g -O2 -std=c++0x
THRIFT_DIR := $(THRIFT)/include
BOOST_DIR := /usr/local/include

CFLAGS := -I$(THRIFT_DIR) -I$(THRIFT_DIR)/thrift -I$(BOOST_DIR) -I/usr/local/inutil/ -I proto/gen-cpp/ -I/usr/local/include/hiredis/ 
LDFLAGS :=  -L/usr/local/lib/ $(THRIFT)/lib/libthriftnb.a $(THRIFT)/lib/libthrift.a /usr/local/inutil/libinvutil.a -levent -lrt /usr/local/co/lib/libco.a /usr/local/lib/libcityhash.a -lz  -lm -lpthread -lstdc++  -ldl

objects = ../proto/gen-cpp/ab_dyeing_constants.o ../proto/gen-cpp/ab_dyeing_types.o ../proto/gen-cpp/ABDyeingService.o 
exe = main.o 

all: $(objects) $(exe)
	$(CC) -o ../bin/${MAIN} ${exe} ${objects} $(LDFLAGS)  /usr/local/lib/liblzo2.a

$(objects): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(exe): %.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o
	rm -rf ../proto/gen-cpp/*.o
	rm ../bin/${MAIN} 
