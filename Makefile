compiler       = clang
#compiler       = gcc
compiler_flags = -O2 -std=c99 -pedantic -W -Wall -Wno-variadic-macros -Waggregate-return -Wbad-function-cast -Wcast-align -Wcast-qual -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-compare -Wstrict-prototypes -Wundef -Wwrite-strings

sources   = $(wildcard *.c)
headers   = $(wildcard *.h)
libraries = -lwiiuse -lpthread

target = quiz

all: $(target)

$(target): $(sources) $(headers)
	$(compiler) $(compiler_flags) -o $@ $(sources) $(libraries)

.PHONY: clean
clean:
	-rm -rf $(target)
