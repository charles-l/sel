#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <err.h>

#define MAX_LINES 1024
#define MAX_LINE_LEN 1024

typedef struct row {
    char *strings[MAX_LINES];
    int len;
    int selected;
} row_t;

// prototypes
row_t *rows_new();
void rows_add(row_t *rows, char *str);
void rows_destroy(row_t *rows);
void input(row_t *rows, int argc, char **argv);
void display(row_t *rows);
void usage();
void quitw(row_t *rows);
void quit(row_t *rows);
int main(int argc, char **argv);

//

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

void display(row_t *rows) {
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

void quitw(row_t *rows) {
    rows_destroy(rows);
    exit(0);
}

void quit(row_t *rows) {
    endwin();
    quitw(rows);
}

void input(row_t *rows, int argc, char **argv) {
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
            endwin();
            char *s = rows->strings[rows->selected];
            char *ns = malloc(MAX_LINE_LEN);
            snprintf(ns, MAX_LINE_LEN, "%s %s", argv[1], s);
            fputs(ns, stderr);
            system(ns);
            free(ns);
            quitw(rows);
            break;
    }
}

void usage() {
    puts("usage: sel COMMAND [BINDINGS]");
}

int main(int argc, char **argv) {
    WINDOW *w;
    if(argc < 2) {
        errx(1, "no commands to run on selection");
    }

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
        input(rows, argc, argv);
    }

    // erm... just in case I guess?
    quit(rows);
}
