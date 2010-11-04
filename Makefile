compiler       = clang
#compiler       = gcc
compiler_flags = -O2 -std=c99 -pedantic -W -Wall -Wno-variadic-macros -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-compare -Wstrict-prototypes -Wundef -Wwrite-strings

target  = wiimotes-quiz
sources = $(wildcard *.c)
headers = $(wildcard *.h)
objects = $(patsubst %.c,%.o,$(sources))

libraries = -lwiiuse -lpthread -lrt

all: $(target)

$(target): $(objects) $(headers)
	$(compiler) -o $@ $(objects) $(libraries)

%.o: %.c
	$(compiler) $(compiler_flags) -c $^

.PHONY: clean
clean:
	-rm -rf $(target) $(objects)
