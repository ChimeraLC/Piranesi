all:
	g++ -I include -Wall -Wextra -Werror -o main house.cpp glad.c -lglfw3 -lopengl32 -lgdi32
