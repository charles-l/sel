#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#define MAX_LINES 1024
#define MAX_LINE_LEN 1024

typedef struct row {
    char *strings[MAX_LINES];
    int len;
    int selected;
} row_t;

row_t *rows_new() {
    row_t *rows = malloc(sizeof(row_t));
    rows->len = 0;
    rows->selected = 0;
    return rows;
}

void rows_add(row_t *rows, char *str) {
    rows->strings[rows->len++] = str;
}

void rows_destroy(row_t *rows) {
    for(int i = 0; i < rows->len; i++) {
        free(rows->strings[i]);
    }
    free(rows);
    rows = NULL;
}

void display(row_t *rows)
{
    clear();
    for(int i = 0; i < rows->len; i++)
    {
        if (i == rows->selected) {
            mvaddch(i, 0, '>');
        }
        mvaddstr(i, 1, (const char *) rows->strings[i]);
    }
    refresh();
}

void quit(row_t *rows) {
    rows_destroy(rows);
    endwin();
    exit(0);
}

void input(row_t *rows)
{
    switch (getch()) {
        case 'j':
            rows->selected++;
            break;
        case 'k':
            rows->selected--;
            break;
        case 'q':
            quit(rows);
            break;
        case '\n':
            puts(rows->strings[rows->selected]);
            quit(rows);
            break;
    }
}

int main(int argc, char **argv)
{
    WINDOW *w;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    row_t *rows = rows_new();

    while ((read = getline(&line, &len, stdin)) != -1) {
        if(rows->len == MAX_LINES) break; // don't overflow
        rows_add(rows, strdup(line));
    }

    // return control to user so pipe doesn't own stdin
    freopen("/dev/tty", "r", stdin);

    // ncurses boilerplate
    w = initscr();
    cbreak();
    noecho();
    curs_set(0);
    clear();
    refresh();

    while(1) {
        display(rows);
        input(rows);
    }

    // erm... just in case I guess?
    quit(rows);
}