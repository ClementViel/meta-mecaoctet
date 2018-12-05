all: wsclient artnet

artnet:
	$(CC) artnet-flood-tx.c -o artnet -lartnet

wsclient: wsclient.o cJSON.o
	$(CC) wsclient.o cJSON.o -o wsclient -L/opt/marvell/2.1/sysroots/aarch64-marvell-linux/usr/lib
wsclient.o: wsclient.c
	$(CC) -c wsclient.c

cJSON.o: cJSON.c
	$(CC) -c cJSON.c

clean:
