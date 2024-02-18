all:
	g++ -I include -Wall -Wextra -Werror -o main foggy_cont.cpp glad.c -lglfw3 -lopengl32 -lgdi32
