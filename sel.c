#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <err.h>
#include <ctype.h>
#include "arg.h"
#define MAX_LINES 1024
#define MAX_LINE_LEN 1024
#define NORM_ASCII(c) (c - 65) // normalize an ascii character to zero

typedef struct row {
    char *strings[MAX_LINES];
    int len;
    int selected;
    int start;
} row_t;

char *argv0 = "sel";

// flags
int fast_quit = 1;

row_t *rows_new();
void rows_add(row_t *rows, char *str);
void rows_destroy(row_t *rows);
void input(row_t *rows, int argc, char *command, char **binds);
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
    rows->start = 0;
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
    for(int i = 0; i < LINES; i++)
    {
        if (i == rows->selected) {
            attron(COLOR_PAIR(1));
        } else {
            attroff(COLOR_PAIR(1));
        }
        mvaddstr(i, 0, (const char *) rows->strings[rows->start + i]);
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

void input(row_t *rows, int argc, char *command, char **binds) {
    char *exec;
    char *s = rows->strings[rows->selected]; // current line
    char c;
    switch ((c = getch())) {
        case 'j':
            if(rows->start + rows->selected < rows->len - 1)
            {
                if(rows->selected >= LINES - 3 && rows->start + rows->selected + 3 < rows->len)
                    rows->start++;
                else
                    rows->selected++;
            }
            break;
        case 'k':
            if(rows->start + rows->selected > 0)
            {
                if(rows->selected < 3 && rows->start + rows->selected - 2 > 0)
                    rows->start--;
                else
                    rows->selected--;
            }
            break;
        case 'q':
            quit(rows);
            break;
        case '\n':
            exec = malloc(MAX_LINE_LEN);
            snprintf(exec, MAX_LINE_LEN, "%s %s", command, s);
            endwin();
            system(exec);
            free(exec);
            if(fast_quit) quitw(rows);
        default:
            if(binds[NORM_ASCII(toupper(c))]) {
                exec = malloc(MAX_LINE_LEN);
                snprintf(exec, MAX_LINE_LEN, "%s %s", binds[NORM_ASCII(toupper(c))], s);
                endwin();
                system(exec);
                free(exec);
                if(fast_quit) quitw(rows);
            }
    }
}

void usage() {
    puts("usage: sel [BINDINGS] COMMAND");
}

int main(int argc, char **argv) {
    WINDOW *w;
    char *binds[26] = {NULL};
    ARGBEGIN {
        case 'q':
            fast_quit = 0;
            break;
        case 'h':
            usage();
            exit(0);
        default:
            if(isupper(ARGC())) {
                if(argc > 0 && argv[1][0] != '-') {
                    binds[NORM_ASCII(ARGC())] = ARGOPT();
                } else
                    errx(2, "flag '%c' requires a command", ARGC());
            }
    } ARGEND

    if(argc < 1) errx(1, "no default command passed");

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
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    clear();
    refresh();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    while(1) {
        display(rows);
        input(rows, argc, argv[0], binds);
    }
}
