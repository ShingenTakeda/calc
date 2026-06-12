shitass calculator expresion compiler

example: \
./calc "with a: a*3" | llc -filetype=obj -relocation-model=pic -o=expr.o \
clang -o expr expr.o rtcalc.c \
./expr
