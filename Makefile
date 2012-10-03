OPTS = -O8
CC = gcc -g

driver: driver.c init_field.o get_msg.o encode.o lose_packets.o decode.o compare_msg.o
	$(CC) $(OPTS) -o driver init_field.o get_msg.o encode.o lose_packets.o decode.o compare_msg.o driver.c

init_field.o: init_field.c parameters.h init_field.h
	$(CC) $(OPTS) -c init_field.c

get_msg.o: get_msg.c parameters.h get_msg.h
	$(CC) $(OPTS) -c get_msg.c

encode.o: encode.c parameters.h encode.h
	$(CC) $(OPTS) -c encode.c

lose_packets.o: lose_packets.c parameters.h lose_packets.h
	$(CC) $(OPTS) -c lose_packets.c

decode.o: decode.c parameters.h decode.h
	$(CC) $(OPTS) -c decode.c

compare_msg.o: compare_msg.c parameters.h compare_msg.h
	$(CC) $(OPTS) -c compare_msg.c

test: driver
	./driver

clean:
	/bin/rm -f *.o core
