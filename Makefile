all: terminal

sdl:
	emcc -g src/mpe/*.cpp src/frontend/sdl2/main.cpp -Isrc -std=c++1z \
		 -Wall -Wextra `pkg-config --cflags --libs sdl2`

sdl2:
	clang++ -g src/mpe/*.cpp src/frontend/sdl2/main.cpp -Isrc -std=c++1z \
		 -Wall -Wextra `pkg-config --cflags --libs sdl2`

emscripten:
	emcc -g src/mpe/*.cpp src/frontend/sdl2/main.cpp -Isrc -std=c++1z \
		 -Wall -Wextra -s USE_SDL=2 -o sdl2.html -Wl,--emrun

terminal:
	clang++ -g src/mpe/*.cpp src/term/*.cpp -Isrc -std=c++1z \
		`pkg-config --cflags --libs ncursesw` -lX11

term:
	clang++ -g src/mpe/*.cpp src/frontend/curses/term.cpp -Isrc -std=c++1z \
		 -Wall -Wextra -lncursesw -ltinfo

sfml:
	clang++ -g src/mpe/*.cpp src/sfml/*.cpp -Isrc -std=c++1z \
		-lsfml-graphics -lsfml-window -lsfml-system -Wall -Wextra -g \
		#-fno-omit-frame-pointer -fsanitize=address,undefined
