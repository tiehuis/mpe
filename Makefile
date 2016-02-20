all: terminal

terminal:
	clang++ -g src/mpe/*.cpp src/ui/terminal/*.cpp -Isrc -std=c++1z \
		`pkg-config --cflags --libs ncursesw` -DNO_X11 -lX11

term:
	clang++ -g src/mpe/*.cpp src/frontend/curses/term.cpp -Isrc -std=c++1z \
		 -Wall -Wextra -lncursesw -ltinfo

sfml:
	clang++ -g src/mpe/*.cpp src/ui/sfml/*.cpp -Isrc -std=c++1z \
		-lsfml-graphics -lsfml-window -lsfml-system -Wall -Wextra -g \
		#-fno-omit-frame-pointer -fsanitize=address,undefined
