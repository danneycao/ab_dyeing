MAIN=test
CC=g++ -g -fPIC -O2

GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
ifeq "$(GCCVERSION)" "4.8.5"
	CC += -std=c++11
endif

THRIFT_DIR := /usr/local/include/thrift
BOOST_DIR := /usr/local/include
ifeq ($(yncoolpad), true)
		PGSQL=inv_pgsql.o
endif

CFLAGS := -fno-strict-aliasing -Wall -Wno-deprecated -I ./ -I ./ice/ -I/opt/Ice-3.5.1/include/ \
	-I /usr/local/include/hiredis/ -I /usr/local/include/ImageMagick-6 -I/usr/include/mysql/ \
	-I /usr/include/postgresql -DMAGICKCORE_QUANTUM_DEPTH=16 -DMAGICKCORE_HDRI_ENABLE=1 \
	-I /usr/local/co/include/ -I /usr/local/co/include/ -I /usr/local/qconf/include/

LDFLAGS := -lpthread 

objects = inv_logger_impl.o inv_util.o inv_ex.o inv_file.o inv_logger.o inv_thread_cond.o \
	inv_thread.o inv_thread_mutex.o inv_thread_pool.o inv_timeprovider.o inv_config.o inv_argv.o \
	inv_httpclient.o inv_image.o inv_redis.o inv_coredis.o inv_crc.o inv_md5.o inv_encoder.o inv_keyword.o inv_xxtea.o \
	inv_thread_rwlock.o inv_epoller.o inv_socket.o inv_clientsocket.o inv_coclientsocket.o inv_lzo.o inv_ipsearch.o \
	inv_mysql.o inv_shm.o inv_mem_queue.o inv_webp.o inv_base64.o inv_thrift_service_monitor.o \
	inv_thrift_service_monitor_report.o thrift_service_monitor.o inv_qconfig.o ${PGSQL}

all: $(objects) 

$(objects): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

ar:
	ar r libinvutil.a $(objects)

test:
	$(CC) $(CFLAGS) -c main.cpp
	$(CC) -o ${MAIN} main.o libinvutil.a $(LDFLAGS)

install:
	ar r libinvutil.a $(objects)
	mkdir -p /usr/local/inutil/
	cp -rf libinvutil.a /usr/local/inutil/
	cp -rf  util log /usr/local/inutil/

clean:
	rm -rf *.o
	rm libinvutil.a
	rm ${MAIN}
