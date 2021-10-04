#include <stdio.h>
#include <windows.h>
#include <time.h>
#define scount 80
#define screen_x 80
#define screen_y 25

int x, y;
int HP = 10;
int colors = 7;
HANDLE wHnd;
HANDLE rHnd;
DWORD fdwMode;
COORD star[scount];
COORD characterPos = { 0,0 };
COORD bufferSize = { screen_x,screen_y };
CHAR_INFO consoleBuffer[screen_x * screen_y];
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };

int setConsole(int x, int y) {
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

int setMode() {
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void draw_ship(const char* strChar, COORD coord, unsigned short colors = 7) {
	int x = coord.X;
	int y = coord.Y;
	for (size_t i = 0; i < strlen(strChar); i++) {
		if (strChar[i] == '\n') {
			x = coord.X;
			y++;
		}
		if (0 <= x && x < screen_x && 0 <= y && y < screen_y) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = strChar[i];
			consoleBuffer[x + screen_x * y].Attributes = colors;
		}
		x++;
	}
}

void init_star() {
	for (int i = 0; i < 80; i++) {
		star[i].X = rand() % 85;
		star[i].Y = rand() % 25;
	}
}
void star_fall() {
	for (int i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { short(rand() % screen_x),1 };
		}
		else {
			star[i].Y++;
			if (star[i].X >= x && star[i].X <= x + 4 && star[i].Y == y) {
				HP--;
				star[i] = { short(rand() % screen_x),1 };
			}
		}
	}
}

void fill_data_to_buffer() {
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 1;
		}
	}
}

void fill_star_to_buffer() {
	for (int i = 0; i < 80; i++) {
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = '*';
		consoleBuffer[star[i].X + screen_x * star[i].Y].Attributes = 7;
	}
}

void fill_buffer_to_console() {
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}

int main() {
	srand(time(NULL));
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	init_star();
	while (play && HP > 0) {
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						colors = rand() % 15 + 1;
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						colors = rand() % 15 + 1;
					}
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						x = posx - 2;
						y = posy;
					}
				}
			}
			delete[] eventBuffer;
		}
		star_fall();
		fill_data_to_buffer();
		fill_star_to_buffer();
		draw_ship("<-0->", { short(x),short(y) }, colors);
		fill_buffer_to_console();
		Sleep(100);
	}
	return 0;
}