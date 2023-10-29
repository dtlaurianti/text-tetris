# set gcc as c compiler
CC = gcc
# set compile flags for gcc
CFLAGS = -I$(IDIR)
# set linker flags
LDFLAGS = -lm
# define the organization of the project
IDIR = ../include
ODIR = obj
LDIR = ../lib
# list the dependency file names and convert to their relative path
SRCS = tetris.c
# DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
# compute the object file names and convert to their relative path
_OBJS = $(SRCS:.c=.o)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
# name of the executable to generate
EXEC = text-tetris

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)
	chmod +x $(EXEC)

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(EXEC)
