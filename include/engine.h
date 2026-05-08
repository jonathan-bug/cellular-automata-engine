#ifndef ENGINE_H
#define ENGINE_H
#define BUFFER_WIDTH 128
#define BUFFER_HEIGHT 64
#define CELL_SIZE 20

#include <stdint.h>

typedef struct {
    uint8_t f_buffer[BUFFER_WIDTH * BUFFER_HEIGHT];
    uint8_t b_buffer[BUFFER_WIDTH * BUFFER_HEIGHT];

    uint8_t *read_buffer;
    uint8_t *write_buffer;
} Engine;

void init_engine(Engine *);
void swap_engine(Engine *);

void update_engine(Engine *);
void render_engine(Engine *);

void export_buffer(Engine *, const char *);
void import_buffer(Engine *, const char *);

#endif