#include "engine.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

void init_engine(Engine *engine) {
    memset(engine->f_buffer, 0, sizeof(engine->f_buffer));
    memset(engine->b_buffer, 0, sizeof(engine->b_buffer));
    memset(engine->quick_buffer, 0, sizeof(engine->quick_buffer));

    engine->read_buffer = engine->f_buffer;
    engine->write_buffer = engine->b_buffer;
}

void render_engine(Engine *engine) {
    for (int y = 0; y < BUFFER_HEIGHT; y++) {
        for (int x = 0; x < BUFFER_WIDTH; x++) {
            int i = x + y * BUFFER_WIDTH;

            printf("%s", engine->read_buffer[i] ? "██" : "  ");
        }

        printf("\n");
    }
}

void update_engine(Engine *engine) {
    for(int x = 0; x < BUFFER_WIDTH; x++) {
        for(int y = 0; y < BUFFER_HEIGHT; y++) {
            int n = 0;

            int i = x + y * BUFFER_WIDTH;

            n += engine->read_buffer[((x + BUFFER_WIDTH - 1) % BUFFER_WIDTH) +  ((y + BUFFER_HEIGHT - 1) % BUFFER_HEIGHT) * BUFFER_WIDTH];
            n += engine->read_buffer[x +  ((y + BUFFER_HEIGHT - 1) % BUFFER_HEIGHT) * BUFFER_WIDTH];
            n += engine->read_buffer[((x + BUFFER_WIDTH + 1) % BUFFER_WIDTH) +  ((y + BUFFER_HEIGHT - 1) % BUFFER_HEIGHT) * BUFFER_WIDTH];

            n += engine->read_buffer[((x + BUFFER_WIDTH - 1) % BUFFER_WIDTH) +  y * BUFFER_WIDTH];
            n += engine->read_buffer[((x + BUFFER_WIDTH + 1) % BUFFER_WIDTH) +  y * BUFFER_WIDTH];

            n += engine->read_buffer[((x + BUFFER_WIDTH - 1) % BUFFER_WIDTH) +  ((y + BUFFER_HEIGHT + 1) % BUFFER_HEIGHT) * BUFFER_WIDTH];
            n += engine->read_buffer[x + ((y + BUFFER_HEIGHT + 1) % BUFFER_HEIGHT) * BUFFER_WIDTH];
            n += engine->read_buffer[((x + BUFFER_WIDTH + 1) % BUFFER_WIDTH) +  ((y + BUFFER_HEIGHT + 1) % BUFFER_HEIGHT) * BUFFER_WIDTH];

            if(engine->read_buffer[i]) {
                engine->write_buffer[i] = (n == 2 || n == 3);
            }else {
                engine->write_buffer[i] = (n == 3);
            }
        }
    }
}

void swap_engine(Engine *engine) {
    uint8_t *temp = engine->read_buffer;
    
    engine->read_buffer = engine->write_buffer;
    engine->write_buffer = temp;
}

void export_buffer(Engine *engine, const char *file_name) {
    FILE *file = fopen(file_name, "w");

    if(!file) {
        return;
    }

    fprintf(file, "x = %d, y = %d, rule = B3/S23\n", BUFFER_WIDTH, BUFFER_HEIGHT);

    for(int y = 0; y < BUFFER_HEIGHT; y++) {
        int count = 0;
        int current = -1;

        for(int x = 0; x < BUFFER_WIDTH; x++) {
            int cell = engine->read_buffer[x + y * BUFFER_WIDTH];

            if(cell == current) {
                count++;
            }else {
                if(current != -1) {
                    if(count > 1) {
                        fprintf(file, "%d", count);
                    }

                    fprintf(file, "%c", current? 'o': 'b');
                }

                current = cell;
                count = 1;
            }
        }

        if (count > 1) {
            fprintf(file, "%d", count);
        }

        fprintf(file, "%c", current ? 'o' : 'b');

        if(y < BUFFER_HEIGHT - 1 ) {
            fprintf(file, "$");
        }else {
            fprintf(file, "!");
        }

        fprintf(file, "\n");
    }

    fclose(file);
}

void import_buffer(Engine *engine, const char *file_name) {
    FILE *file = fopen(file_name, "r");

    if(!file) {
        return;
    }

    char line[512];
    int x = 0;
    int y = 0;
    int count = 0;

    while(fgets(line, sizeof(line), file)) {
        if(line[0] == 'x' || line[0] == '#') {
            continue;
        }

        for(int i = 0; line[i] != '\0'; i++) {
            char c = line[i];

            if(isdigit(c)) {
                count = count * 10 + (c - '0');
            }else if(c == 'o' || c == 'b') {
                if(count == 0) {
                    count = 1;
                }

                int current = (c == 'o')? 1: 0;

                for(int j = 0; j < count; j++) {
                    if(x < BUFFER_WIDTH && y < BUFFER_HEIGHT) {
                        engine->read_buffer[x + y * BUFFER_WIDTH] = current;
                    }

                    x++;
                }

                count = 0;
            }else if(c == '$') {
                if(count == 0) {
                    count = 1;
                }

                y += count;
                x = 0;
                count = 0;
            }else if(c == '!') {
                fclose(file);
                return;
            }
        }
    }

    fclose(file);
}