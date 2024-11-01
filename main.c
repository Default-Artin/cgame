#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define max_x COLS
#define max_y LINES

void init(void *w, void *px, void *py, void *fx, void *fy, void *ex, void *ey) {
	char (*world)[max_x] = w;
	for (int i = 0; i < max_y; i++) {
		for (int j = 0; j < max_x; j++) {
			world[i][j] = (rand() % 100)+1 > 2 ? ' ' : '.';
		}
	}
	int *player_x = px;
	*player_x = rand() % max_x;
	int *player_y = py;
	*player_y = rand() % max_y;

	int (*foods_x)[10] = fx;
	int (*foods_y)[10] = fy;
	for (int f = 0; f < 10; f++) {
		*foods_x[f] = rand() % max_x;
		*foods_y[f] = rand() % max_y;
	}

	int (*enemy_x)[3] = ex;
	int (*enemy_y)[3] = ey;
	for (int e = 0; e < 3; e++) {
		*enemy_x[e] = rand() % max_x;
		*enemy_y[e] = rand() % max_y;
	}
}

void draw(void *w, int score, int player_x, int player_y, void *fx, void *fy, void *ex, void *ey) {
	char (*world)[max_x] = w;
	for (int i = 0; i < max_y; i++) {
		for (int j = 0; j < max_x; j++) {
			mvprintw(i, j, "%c", world[i][j]);
		}
	}

	attron(COLOR_PAIR(64));
	mvprintw(1, 1, "Score: %d", score);
	attroff(COLOR_PAIR(64));

	attron(COLOR_PAIR(26));
	mvprintw(player_y, player_x, "@");
	attroff(COLOR_PAIR(26));

	int (*foods_x)[10] = fx;
	int (*foods_y)[10] = fy;
	attron(COLOR_PAIR(7));
	for (int f = 0; f < 10; f++)
		mvprintw(*foods_y[f], *foods_x[f], "$");
	attroff(COLOR_PAIR(7));

	int (*enemy_x)[3] = ex;
	int (*enemy_y)[3] = ey;
	attron(COLOR_PAIR(2));
	for (int e = 0; e < 3; e++)
		mvprintw(*enemy_y[e], *enemy_x[e], "&");
	attroff(COLOR_PAIR(2));
}

unsigned in_range(int a, int min, int max) {
	if (a > max)
		return max;
	if (a < min)
		return min;
	return a;
}

void check_food(int *score, int player_x, int player_y, void *fx, void *fy) {
	int (*foods_x)[10] = fx;
	int (*foods_y)[10] = fy;

	for (int i = 0; i < 10; i++) {
		if (*foods_x[i] == player_x && *foods_y[i] == player_y) {
			*score += 10;
			*foods_x[i] = rand() % max_x;
			*foods_y[i] = rand() % max_y;
		}
	}
}

void move_enemy(_Bool *playing, int player_x, int player_y, void *ex, void *ey) {
	int (*enemy_x)[3] = ex;
	int (*enemy_y)[3] = ey;
	for (int i = 0; i < 3; i++) {
		if (rand() % 100 > 83) {
			if (*enemy_x[i] > player_x) *enemy_x[i] -= 1;
		}
		if (rand() % 100 > 83) {
			if (*enemy_x[i] < player_x) *enemy_x[i] += 1;
		}
		if (rand() % 100 > 83) {
			if (*enemy_y[i] > player_y) *enemy_y[i] -= 1;
		}
		if (rand() % 100 > 83) {
			if (*enemy_y[i] < player_y) *enemy_y[i] += 1;
		}
		*enemy_x[i] = in_range(*enemy_x[i], 0, max_x-1);
		*enemy_y[i] = in_range(*enemy_y[i], 0, max_y-1);

		if (*enemy_x[i] == player_x && *enemy_y[i] == player_y) {
			clear();
			attron(COLOR_PAIR(10));
			mvprintw(max_y/2, max_x/2-4, "YOU DIED!");
			refresh();
			attroff(COLOR_PAIR(10));
			usleep(2000000);
			*playing = false;
		}
	}
}

int main(void) {
	initscr();
	start_color();
	use_default_colors();
	for (int c = 0; c < COLORS; c++) init_pair(c+1, c, -1);
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(FALSE);
	srand(time(0));
	clear();


	char world[max_y][max_x];

	int player_x;
	int player_y;

	int score = 0;

	int foods_x[10];
	int foods_y[10];

	int enemy_x[3];
	int enemy_y[3];

	init(world, &player_x, &player_y, foods_x, foods_y, enemy_x, enemy_y);
	_Bool playing = true;

	while (playing) {
		char c = getch();
		if (c == 'w' && world[player_y-1][player_x] != '.')
			player_y--;
		else if (c == 's' && world[player_y+1][player_x] != '.')
			player_y++;
		else if (c == 'a' && world[player_y][player_x-1] != '.')
			player_x--;
		else if (c == 'd' && world[player_y][player_x+1] != '.')
			player_x++;
		else if (c == 'q')
			playing = false;
		player_y = in_range(player_y, 0, max_y-1);
		player_x = in_range(player_x, 0, max_x-1);

		check_food(&score, player_x, player_y, foods_x, foods_y);
		move_enemy(&playing, player_x, player_y, enemy_x, enemy_y);
		usleep(10000);
		draw(world, score, player_x, player_y, foods_x, foods_y, enemy_x, enemy_y);
	}
	refresh();


	clear();
	attron(COLOR_PAIR(11));
	mvprintw(max_y/2, max_x/2-10, "Thanks for Playing!");
	refresh();
	usleep(2000000);
	endwin();
	return 0;
}