all:
	g++ -I include -Wall -Wextra -Werror -o main test.cpp glad.c -lglfw3 -lopengl32 -lgdi32
