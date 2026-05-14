#include "engine.h"
#include <unistd.h>
#include <time.h>
#include <raylib.h>
#include <stdio.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#define PADDING 8
#define BUTTON_WIDTH 150
#define BUTTON_HEIGHT 36

int main() {
    // GUI
    const int screen_width = 64 * 20;
    const int screen_height = 32 * 20;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screen_width, screen_height, "Cellular automata engine");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.target = (Vector2) { (BUFFER_WIDTH * CELL_SIZE) / 2.0f, (BUFFER_HEIGHT * CELL_SIZE) / 2.0f };
    camera.offset = (Vector2) { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    // Engine
    Engine engine;
    init_engine(&engine);
    import_buffer(&engine, "simulation.rle");

    float time0 = 0.0f;
    float time1 = 0.0f;

    // Flags
    int paused = 1;

    while(!WindowShouldClose()) {
        int on_gui = 
            CheckCollisionPointRec(GetMousePosition(), (Rectangle) { PADDING, PADDING, BUTTON_WIDTH * 2 + PADDING, BUTTON_HEIGHT }) ||
            CheckCollisionPointRec(GetMousePosition(), (Rectangle) { GetScreenWidth() - (BUTTON_WIDTH + PADDING) * 4, PADDING, BUFFER_WIDTH * 4, BUTTON_HEIGHT });

        // Timers update
        time1 = GetFrameTime();
        time0 += time1;

        BeginDrawing();
            ClearBackground(BLACK);

            //Cell
            BeginMode2D(camera);

            for (int y = 0; y < BUFFER_HEIGHT; y++) {
                for (int x = 0; x < BUFFER_WIDTH; x++) {
                    if (engine.read_buffer[x + y * BUFFER_WIDTH]) {
                        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
                    }
                }
            }

            if(paused) {
                // Grid
                for(int y = 0; y < BUFFER_HEIGHT + 1; y++) {
                    DrawLine(0, CELL_SIZE * y, BUFFER_WIDTH * CELL_SIZE, CELL_SIZE * y, DARKGRAY);
                }

                for(int x = 0; x < BUFFER_WIDTH + 1; x++) {
                    DrawLine(CELL_SIZE * x, 0, CELL_SIZE * x, BUFFER_HEIGHT * CELL_SIZE, DARKGRAY);
                }

                Vector2 world = GetScreenToWorld2D(GetMousePosition(), camera);

                int x = world.x / CELL_SIZE;
                int y = world.y / CELL_SIZE;

                if((x >= 0 && x < BUFFER_WIDTH) && (y >= 0 && y < BUFFER_HEIGHT)) {
                    DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GRAY);
                }
            }else {
                DrawLine(0, 0, CELL_SIZE * BUFFER_WIDTH, 0, DARKGRAY);
                DrawLine(0, CELL_SIZE * BUFFER_HEIGHT + 1, CELL_SIZE * BUFFER_WIDTH, CELL_SIZE * BUFFER_HEIGHT + 1, DARKGRAY);
                DrawLine(0, 0, 0, CELL_SIZE * BUFFER_HEIGHT, DARKGRAY);
                DrawLine(CELL_SIZE * BUFFER_WIDTH + 1, 0, CELL_SIZE * BUFFER_WIDTH + 1, CELL_SIZE * BUFFER_HEIGHT, DARKGRAY);
            }

            EndMode2D();

            // UI
            if(paused) {
                //DrawText("PAUSED", 20, 20, 20, GRAY);
                int anchor_top_right_x = 0;
                int anchor_top_right_y = 0;

                Vector2 anchor_top_left = (Vector2) { 0, 0 };
                Vector2 anchor_top_right = (Vector2) { GetScreenWidth() - BUTTON_WIDTH, 0 };

                if(GuiButton((Rectangle){ anchor_top_left.x + PADDING, anchor_top_left.y + PADDING, BUTTON_WIDTH, BUTTON_HEIGHT }, "#02#Save")) {
                    export_buffer(&engine, "simulation.rle");
                }
                anchor_top_left.x += BUTTON_WIDTH + PADDING;
                if(GuiButton((Rectangle){ anchor_top_left.x + PADDING, anchor_top_left.y + PADDING, BUTTON_WIDTH, BUTTON_HEIGHT }, "#01#Load")) {
                    import_buffer(&engine, "simulation.rle");
                }
                anchor_top_left.x += BUTTON_WIDTH + PADDING;


                if(GuiButton((Rectangle){ anchor_top_right.x - PADDING, anchor_top_right.y + PADDING, BUTTON_WIDTH, BUTTON_HEIGHT }, "#56#Quick load (Q)")) {
                    for(int i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++) {
                        engine.read_buffer[i] = engine.quick_buffer[i];
                    }
                }
                anchor_top_right.x -= BUTTON_WIDTH + PADDING;

                if(GuiButton((Rectangle){ anchor_top_right.x - PADDING, anchor_top_right.y + PADDING, BUTTON_WIDTH, BUTTON_HEIGHT }, "#16#Quick save (E)")) {
                    for(int i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++) {
                        engine.quick_buffer[i] = engine.read_buffer[i];
                    }
                }

                anchor_top_right.x -= BUTTON_WIDTH + PADDING;
                
                if(GuiButton((Rectangle){ anchor_top_right.x - PADDING, anchor_top_right.y + PADDING, BUTTON_WIDTH, BUTTON_HEIGHT }, "#28#Clear (R)")) {
                    init_engine(&engine);
                }

                anchor_top_right.x -= BUTTON_WIDTH + PADDING;
                
                if(GuiButton((Rectangle){ anchor_top_right.x - PADDING, anchor_top_right.y + PADDING, BUTTON_WIDTH, BUTTON_HEIGHT }, "#48#Center (Left shift)")) {
                    float min_scale_x =
                        (float)GetScreenWidth() / (BUFFER_WIDTH * CELL_SIZE);
                    float min_scale_y =
                        (float)GetScreenHeight() / (BUFFER_HEIGHT * CELL_SIZE);
                    float min_scale =
                        (min_scale_x > min_scale_y) ? min_scale_x : min_scale_y;

                    camera.zoom = min_scale;
                    camera.target =
                        (Vector2){(BUFFER_WIDTH * CELL_SIZE) / 2.0f,
                                  (BUFFER_HEIGHT * CELL_SIZE) / 2.0f};
                    camera.offset = (Vector2){GetScreenWidth() / 2.0f,
                                              GetScreenHeight() / 2.0f};
                }

                anchor_top_right.x -= BUTTON_WIDTH + PADDING;
            }
        EndDrawing();

        // Update
        if(time0 >= 0.1f) {
            if(!paused) {
                update_engine(&engine);
                swap_engine(&engine);
            }

            time0 = 0.0f;
        }

        // Add cell
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !on_gui) {
            Vector2 world = GetScreenToWorld2D(GetMousePosition(), camera);

            int x = world.x / CELL_SIZE;
            int y = world.y / CELL_SIZE;
            
            if(x >= 0 && x < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT) {
                int i = x + y * BUFFER_WIDTH;

                engine.read_buffer[i] = 1;
            }
        }
        
        // Delete cell
        if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON) && !on_gui) {
            Vector2 world = GetScreenToWorld2D(GetMousePosition(), camera);

            int x = world.x / CELL_SIZE;
            int y = world.y / CELL_SIZE;
            
            if(x >= 0 && x < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT) {
                int i = x + y * BUFFER_WIDTH;

                engine.read_buffer[i] = 0;
            }
        }

        if(IsKeyPressed(KEY_SPACE)) {
            paused = !paused;
        }

        if(IsKeyPressed(KEY_E)) {            
            //export_buffer(&engine, "simulation.rle");
            for(int i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++) {
                engine.quick_buffer[i] = engine.read_buffer[i];
            }
        }

        if(IsKeyPressed(KEY_Q)) {
            //import_buffer(&engine, "simulation.rle");
            for(int i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++) {
                engine.read_buffer[i] = engine.quick_buffer[i];
            }
        }

        if(IsKeyDown(KEY_A) && !IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.target.x -= 20.0f / camera.zoom;
        }

        if(IsKeyDown(KEY_D) && !IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.target.x += 20.0f / camera.zoom;
        }

        if(IsKeyDown(KEY_W) && !IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.target.y -= 20.0f / camera.zoom;
        }

        if(IsKeyDown(KEY_S) && !IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.target.y += 20.0f / camera.zoom;
        }

        if(IsKeyPressed(KEY_R)) {
            init_engine(&engine);
        }

        float scroll = GetMouseWheelMove();

        if(scroll != 0) {
            Vector2 world = GetScreenToWorld2D(GetMousePosition(), camera);

            camera.target = world;
            camera.offset = GetMousePosition();
            camera.zoom *= 1.0f + (0.1f * scroll);
        }

        if (IsKeyPressed(KEY_LEFT_SHIFT)) {
            float min_scale_x =
                (float)GetScreenWidth() / (BUFFER_WIDTH * CELL_SIZE);
            float min_scale_y =
                (float)GetScreenHeight() / (BUFFER_HEIGHT * CELL_SIZE);
            float min_scale =
                (min_scale_x > min_scale_y) ? min_scale_x : min_scale_y;

            camera.zoom = min_scale;
            camera.target = (Vector2) { (BUFFER_WIDTH * CELL_SIZE) / 2.0f, (BUFFER_HEIGHT * CELL_SIZE) / 2.0f };
            camera.offset = (Vector2) { GetScreenWidth() / 2.0f , GetScreenHeight() / 2.0f };
        }
    }

    return 0;
}