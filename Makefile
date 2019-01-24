cruncher: main.c
	$(CC) -Wall -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f cruncher
