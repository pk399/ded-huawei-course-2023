CC = g++

CFLAGS = -O3

INCLUDE = ../../common/include

quality-test:
	python graph_quality.py

strlist.o: strlist.c
	$(CC) $(CFLAGS) -c strlist.c -I$(INCLUDE)

clean:
	rm -r list pics __pycache__ words.txt
