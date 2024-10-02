CC = g++
CFLAGS  = -I include -Wall -Wextra -Werror 
EFLAGS = -lglfw3 -lopengl32 -lgdi32

PROG = main
OBJS = main.cpp glad.c

build: ${PROG}

${PROG}: ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${EFLAGS}