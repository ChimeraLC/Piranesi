all:
	g++ -I include -Wall -Wextra -Werror -o main foggy.cpp glad.c -lglfw3 -lopengl32 -lgdi32
