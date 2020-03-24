/*
 * Gearboy - Nintendo Game Boy Emulator
 * Copyright (C) 2012  Ignacio Sanchez

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 *
 */

#include <SDL.h>
#include <SDL_opengl.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/ImGuiFileBrowser.h"
#include "emu_sdl.h"
#include "Emulator.h"

#define EMU_IMGUI_IMPORT
#include "emu_imgui.h"

static void gui_main_menu(void);
static void gui_main_window(void);
static void gui_about_window(void);

struct EmulatorOptions
{
    int save_slot = 0;
    bool start_paused;
    bool force_dmg;
    bool save_in_rom_folder;
};

struct VideoOptions
{
    bool fps;
    bool bilinear;
    bool mix_frames;
    bool vsync;
    bool matrix;
    ImVec4 color[4];
};

struct AudioOptions
{
    bool enable = true;
    bool sync;
    int freq = 44100;
};

struct InputOptions
{
    bool gamepad;
};

imgui_addons::ImGuiFileBrowser file_dialog;

Emulator* emu;
GB_Color* emu_frame_buffer;

int gui_main_menu_height;
bool gui_show_about_window = false;
bool gui_show_debug = false;

EmulatorOptions gui_emulator_options;
VideoOptions gui_video_options;
AudioOptions gui_audio_options;
InputOptions gui_input_options;

void emu_imgui_init(void)
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(emu_sdl_window, emu_sdl_gl_context);
    ImGui_ImplOpenGL2_Init();

    emu_frame_buffer = new GB_Color[GAMEBOY_WIDTH * GAMEBOY_HEIGHT];
    emu = new Emulator();
    emu->Init();
}

void emu_imgui_destroy(void)
{
    SafeDelete(emu);
    SafeDeleteArray(emu_frame_buffer);
    
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    ImGui::DestroyContext();
}

void emu_imgui_update(void)
{
    emu->RunToVBlank(emu_frame_buffer);

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame(emu_sdl_window);

    ImGui::NewFrame();

    gui_main_menu();
    gui_main_window();
    if (gui_show_about_window)
        gui_about_window();

    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Render();
    
    ImVec4 clear_color = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void emu_imgui_event(const SDL_Event* event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

static void gui_main_menu(void)
{
    bool open = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Game Boy"))
        {
            if (ImGui::MenuItem("Open ROM...", "Ctrl+O"))
            {
                open = true;
            }

            if (ImGui::MenuItem("Pause", "Ctrl+P")) {}
            if (ImGui::MenuItem("Reset", "Ctrl+R")) {}

            ImGui::Separator();

            if (ImGui::MenuItem("Save State As...")) {}
            if (ImGui::MenuItem("Open State From...")) {}

            ImGui::Separator();
           
            if (ImGui::BeginMenu("Select State Slot")) // <-- Append!
            {
                ImGui::Combo("", &gui_emulator_options.save_slot, "Slot 1\0Slot 2\0Slot 3\0Slot 4\0Slot 5\0\0");
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save State", "Ctrl+S")) {}
            if (ImGui::MenuItem("Load State", "Ctrl+L")) {}

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "Alt+F4")) {}

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::BeginMenu("Emulator"))
            {
                ImGui::MenuItem("Force DMG", "", &gui_emulator_options.force_dmg);
                ImGui::MenuItem("Start Paused", "", &gui_emulator_options.start_paused);
                ImGui::MenuItem("Save RAM in ROM folder", "", &gui_emulator_options.save_in_rom_folder);                
                
                if (ImGui::BeginMenu("Cheats"))
                {
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Video"))
            {
                ImGui::MenuItem("Show FPS", "", &gui_video_options.fps);
                ImGui::MenuItem("Bilinear Filtering", "", &gui_video_options.bilinear);
                ImGui::MenuItem("Screen Ghosting", "", &gui_video_options.mix_frames);
                ImGui::MenuItem("Dot Matrix", "", &gui_video_options.matrix);
                ImGui::MenuItem("Vertical Sync", "", &gui_video_options.vsync);
                
                ImGui::Separator();

                if (ImGui::BeginMenu("Palette"))
                {
                    ImGui::Combo("", &gui_emulator_options.save_slot, "Original\0Sharp\0Black & White\0Autumn\0Soft\0Slime\0Custom\0\0");
                    ImGui::EndMenu();
                }

                ImGui::ColorEdit4("Color #1", (float*)&gui_video_options.color[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
                ImGui::ColorEdit4("Color #2", (float*)&gui_video_options.color[1], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
                ImGui::ColorEdit4("Color #3", (float*)&gui_video_options.color[2], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
                ImGui::ColorEdit4("Color #4", (float*)&gui_video_options.color[3], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
                
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Input"))
            {
                if (ImGui::BeginMenu("Keyboard Configuration"))
                {
                    ImGui::Text("Up:");
                    ImGui::SameLine();
                     if (ImGui::Button("UP"))
                        ImGui::OpenPopup("keyboard_definition");
                                        
                    ImGui::Text("Down:");
                    ImGui::SameLine();
                    if (ImGui::Button("DOWN"))
                        ImGui::OpenPopup("keyboard_definition");

                    if (ImGui::BeginPopupModal("keyboard_definition", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                    {
                        ImGui::Text("Press any key...\n\n");
                        ImGui::Separator();
                        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                        ImGui::SetItemDefaultFocus();
                        ImGui::SameLine();
                        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                        ImGui::EndPopup();
                    }                   
                   
                    ImGui::EndMenu();
                }
                ImGui::MenuItem("Enable Gamepad", "", &gui_input_options.gamepad);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Audio"))
            {
                ImGui::MenuItem("Enable", "", &gui_audio_options.enable);
                ImGui::MenuItem("Sync", "", &gui_audio_options.sync);
                
                if (ImGui::BeginMenu("Frequency"))
                {
                    ImGui::Combo("", &gui_audio_options.freq, " 48000\0 44100\0 22050\0\0");
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))
        {
            ImGui::MenuItem("Enabled", "", &gui_show_debug, false);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            ImGui::MenuItem("About " GEARBOY_TITLE " " GEARBOY_VERSION " ...", "", &gui_show_about_window);
            ImGui::EndMenu();
        }

        gui_main_menu_height = ImGui::GetWindowSize().y;

        ImGui::EndMainMenuBar();       
    }

    if (open)
        ImGui::OpenPopup("Open ROM");

    if(file_dialog.showFileDialog("Open ROM", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), "*.*,.gb,.gbc,.cgb,.sgb,.dmg,.rom,.zip"))
    {
        emu->LoadRom(file_dialog.selected_path.c_str(), gui_emulator_options.force_dmg, gui_emulator_options.save_in_rom_folder);

        if (gui_emulator_options.start_paused)
            emu->Pause();
    }
}

static void gui_main_window(void)
{
    int w = ImGui::GetIO().DisplaySize.x;
    int h = ImGui::GetIO().DisplaySize.y - gui_main_menu_height;

    int factor_w = w / GAMEBOY_WIDTH;
    int factor_h = h / GAMEBOY_HEIGHT;

    int factor = (factor_w < factor_h) ? factor_w : factor_h;

    int window_w = GAMEBOY_WIDTH * factor;
    int window_h = GAMEBOY_HEIGHT * factor;

    int window_x = (w - (GAMEBOY_WIDTH * factor)) / 2;
    int window_y = ((h - (GAMEBOY_HEIGHT * factor)) / 2) + gui_main_menu_height;
    
    ImGui::SetNextWindowPos(ImVec2(window_x, window_y));
    ImGui::SetNextWindowSize(ImVec2(window_w, window_h));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin(GEARBOY_TITLE, 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoNav);
    
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::PopStyleVar();
}

static void gui_about_window(void)
{
    ImGui::SetNextWindowSize(ImVec2(400,230));
    ImGui::SetNextWindowPos(ImVec2((ImGui::GetIO().DisplaySize.x / 2) - 200, (ImGui::GetIO().DisplaySize.y / 2) - 115));

    ImGui::Begin("About " GEARBOY_TITLE, &gui_show_about_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
    
    ImGui::Text("%s %s", GEARBOY_TITLE, GEARBOY_VERSION);
    ImGui::Separator();
    ImGui::Text("By Ignacio Sánchez (twitter.com/drhelius)");
    ImGui::Text("%s is licensed under the GPL-3.0 License,\nsee LICENSE for more information.", GEARBOY_TITLE);
    ImGui::Separator();        
    
#ifdef _WIN32
    ImGui::Text("Windows 32 bit detected.");
#endif
#ifdef _WIN64
    ImGui::Text("Windows 32 bit detected.");
#endif
#ifdef __linux__
    ImGui::Text("Linux detected.");
#endif
#ifdef __APPLE__
    ImGui::Text("macOS detected.");
#endif
#ifdef _MSC_VER
    ImGui::Text("Built with Microsoft C++ %d.", _MSC_VER);
#endif
#ifdef __MINGW32__
    ImGui::Text("Built with MinGW 32 bit.");
#endif
#ifdef __MINGW64__
    ImGui::Text("Built with MinGW 64 bit.");
#endif
#ifdef __GNUC__
    ImGui::Text("Built with GCC %d.", (int)__GNUC__);
#endif
#ifdef __clang_version__
    ImGui::Text("Built with Clang %s.", __clang_version__);
#endif
#ifdef DEBUG
    ImGui::Text("define: DEBUG");
#endif
#ifdef DEBUG_GEARBOY
    ImGui::Text("define: DEBUG_GEARBOY");
#endif
    ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
    ImGui::Text("Dear ImGui %s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
    ImGui::End();
}
