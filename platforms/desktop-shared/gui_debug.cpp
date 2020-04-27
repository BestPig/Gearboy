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

#include "imgui/imgui.h"
#include "imgui/imgui_memory_editor.h"
#include "FileBrowser/ImGuiFileBrowser.h"
#include "config.h"
#include "emu.h"
#include "../../src/gearboy.h"
#include "gui.h"

#define GUI_DEBUG_IMPORT
#include "gui_debug.h"

static MemoryEditor mem_edit;
static ImVec4 cyan = ImVec4(0.0f,1.0f,1.0f,1.0f);
static ImVec4 magenta = ImVec4(1.0f,0.502f,0.957f,1.0f);
static ImVec4 yellow = ImVec4(1.0f,1.0f,0.0f,1.0f);
static ImVec4 red = ImVec4(1.0f,0.149f,0.447f,1.0f);
static ImVec4 green = ImVec4(0.0f,1.0f,0.0f,1.0f);
static ImVec4 white = ImVec4(1.0f,1.0f,1.0f,1.0f);
static ImVec4 gray = ImVec4(0.5f,0.5f,0.5f,1.0f);

static void debug_window_processor(void);
static void debug_window_io(void);
static void debug_window_audio(void);
static void debug_window_memory(void);
static void debug_window_disassembler(void);

void gui_debug_windows(void)
{
    if (config_debug.debug)
    {
        if (config_debug.show_processor)
            debug_window_processor();
        if (config_debug.show_memory)
            debug_window_memory();
        if (config_debug.show_disassembler)
            debug_window_disassembler();
        if (config_debug.show_iomap)
            debug_window_io();
        if (config_debug.show_audio)
            debug_window_audio();

        //ImGui::ShowDemoWindow(&config_debug.debug);
    }
}

static void debug_window_memory(void)
{
    ImGui::SetNextWindowSize(ImVec2(483, 368), ImGuiCond_FirstUseEver);

    ImGui::Begin("Memory Editor", &config_debug.show_memory);

    GearboyCore* core = emu_get_core();
    Memory* memory = core->GetMemory();

    ImGui::PushFont(gui_default_font);

    ImGui::TextColored(cyan, "  BANKS: ");ImGui::SameLine();

    ImGui::TextColored(magenta, "ROM1");ImGui::SameLine();
    ImGui::Text("$%02X", memory->GetCurrentRule()->GetCurrentRomBank1Index()); ImGui::SameLine();
    ImGui::TextColored(magenta, "  RAM");ImGui::SameLine();
    ImGui::Text("$%02X", memory->GetCurrentRule()->GetCurrentRamBankIndex()); ImGui::SameLine();
    ImGui::TextColored(magenta, "  WRAM1");ImGui::SameLine();
    ImGui::Text("$%02X", memory->GetCurrentCGBRAMBank()); ImGui::SameLine();
    ImGui::TextColored(magenta, "  VRAM");ImGui::SameLine();
    ImGui::Text("$%02X", memory->GetCurrentLCDRAMBank());

    ImGui::PopFont();

    if (ImGui::BeginTabBar("##memory_tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("ROM0"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetROM0(), 0x4000, 0);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("ROM1"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetROM1(), 0x4000, 0x4000);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("VRAM"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetVRAM(), 0x2000, 0x8000);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("RAM"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetRAM(), 0x2000, 0xA000);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }

            if (emu_is_cgb())
            {
                if (ImGui::BeginTabItem("WRAM0"))
                {
                    ImGui::PushFont(gui_default_font);
                    mem_edit.DrawContents(memory->GetWRAM0(), 0x1000, 0xC000);
                    ImGui::PopFont();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("WRAM1"))
                {
                    ImGui::PushFont(gui_default_font);
                    mem_edit.DrawContents(memory->GetWRAM1(), 0x1000, 0xD000);
                    ImGui::PopFont();
                    ImGui::EndTabItem();
                }
            }
            else
            {
                if (ImGui::BeginTabItem("WRAM"))
                {
                    ImGui::PushFont(gui_default_font);
                    mem_edit.DrawContents(memory->GetWRAM0(), 0x2000, 0xC000);
                    ImGui::PopFont();
                    ImGui::EndTabItem();
                }
            }
            
            if (ImGui::BeginTabItem("OAM"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetMemoryMap() + 0xFE00, 0x00A0, 0xFE00);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("IO"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetMemoryMap() + 0xFF00, 0x0080, 0xFF00);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("HIRAM"))
            {
                ImGui::PushFont(gui_default_font);
                mem_edit.DrawContents(memory->GetMemoryMap() + 0xFF80, 0x007F, 0xFF80);
                ImGui::PopFont();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

    ImGui::End();
}

static void debug_window_disassembler(void)
{
    ImGui::SetNextWindowSize(ImVec2(0, 325), ImGuiCond_FirstUseEver);

    ImGui::Begin("Disassembler", &config_debug.show_disassembler);
    
    ImGui::PushFont(gui_default_font);

    GearboyCore* core = emu_get_core();
    Processor* processor = core->GetProcessor();
    Processor::ProcessorState* proc_state = processor->GetState();
    Memory* memory = core->GetMemory();
    Memory::stDisassembleRecord* map = memory->GetDisassembledMemoryMap();

    int pc = proc_state->PC->GetValue();

    if (ImGui::Button("Step"))
        emu_debug_step();
    ImGui::SameLine();
    if (ImGui::Button("Continue"))
        emu_debug_continue(); 
    ImGui::SameLine();
    if (ImGui::Button("Next Frame"))
        emu_debug_next_frame();
    ImGui::SameLine(0.0f, 20.0f);

    static bool enable_track = true;
    ImGui::Checkbox("Track PC", &enable_track);


    bool window_visible = ImGui::BeginChild("##dis", ImVec2(ImGui::GetWindowContentRegionWidth(), 0), true, 0);
    
    if (window_visible)
    {
        int dis_size = 0;
        int pc_pos = 0;
        
        std::vector<Memory::stDisassembleRecord*> vec(0x10000);
        
        for (int i = 0; i < 0x10000; i++)
        {
            if (map[i].name[0] != 0)
            {
                vec[dis_size] = &map[i];

                if (vec[dis_size]->address == pc)
                    pc_pos = dis_size;

                dis_size++;
            }
        }

        ImGuiListClipper clipper(dis_size, ImGui::GetTextLineHeightWithSpacing());

        while (clipper.Step())
        {
            for (int item = clipper.DisplayStart; item < clipper.DisplayEnd; item++)
            {
                if (vec[item]->address == pc)
                {
                    ImGui::TextColored(yellow, "%04X: %s  > %s", vec[item]->address, vec[item]->bytes, vec[item]->name);
                }
                else
                {
                    ImGui::TextColored(cyan, "%04X:", vec[item]->address);
                    ImGui::SameLine();
                    ImGui::TextColored(gray, "%s   ", vec[item]->bytes);
                    ImGui::SameLine();
                    ImGui::TextColored(white, "%s", vec[item]->name);
                }
            }
        }

        if (enable_track)
        {
            float window_offset = ImGui::GetWindowHeight() / 2.0f;
            float offset = window_offset - (ImGui::GetTextLineHeightWithSpacing() - 4.0f);
            ImGui::SetScrollY((pc_pos * ImGui::GetTextLineHeightWithSpacing()) - offset);
        }
    }

    ImGui::EndChild();
    
    ImGui::PopFont();

    ImGui::End();
}

static void debug_window_processor(void)
{
    ImGui::Begin("Processor Status", &config_debug.show_processor, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

    ImGui::PushFont(gui_default_font);

    GearboyCore* core = emu_get_core();
    Processor* processor = core->GetProcessor();
    Processor::ProcessorState* proc_state = processor->GetState();

    ImGui::Separator();

    u8 flags = proc_state->AF->GetLow();

    ImGui::TextColored(magenta, "   Z"); ImGui::SameLine();
    ImGui::Text("= %d", (bool)(flags & FLAG_ZERO)); ImGui::SameLine();

    ImGui::TextColored(magenta, "  N"); ImGui::SameLine();
    ImGui::Text("= %d", (bool)(flags & FLAG_SUB));

    ImGui::TextColored(magenta, "   H"); ImGui::SameLine();
    ImGui::Text("= %d", (bool)(flags & FLAG_HALF)); ImGui::SameLine();

    ImGui::TextColored(magenta, "  C"); ImGui::SameLine();
    ImGui::Text("= %d", (bool)(flags & FLAG_CARRY));

    ImGui::Columns(2, "registers");
    ImGui::Separator();
    ImGui::TextColored(cyan, " A"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->AF->GetHigh());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->AF->GetHigh()));

    ImGui::NextColumn();
    ImGui::TextColored(cyan, " F"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->AF->GetLow());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->AF->GetLow()));

    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::TextColored(cyan, " B"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->BC->GetHigh());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->BC->GetHigh()));

    ImGui::NextColumn();
    ImGui::TextColored(cyan, " C"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->BC->GetLow());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->BC->GetLow()));

    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::TextColored(cyan, " D"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->DE->GetHigh());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->DE->GetHigh()));

    ImGui::NextColumn();
    ImGui::TextColored(cyan, " E"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->DE->GetLow());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->DE->GetLow()));

    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::TextColored(cyan, " H"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->HL->GetHigh());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->HL->GetHigh()));

    ImGui::NextColumn();
    ImGui::TextColored(cyan, " L"); ImGui::SameLine();
    ImGui::Text("= $%02X", proc_state->HL->GetLow());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->HL->GetLow()));

    ImGui::NextColumn();
    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::TextColored(yellow, "    SP"); ImGui::SameLine();
    ImGui::Text("= $%04X", proc_state->SP->GetValue());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED " " BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->SP->GetHigh()), BYTE_TO_BINARY(proc_state->SP->GetLow()));

    ImGui::Separator();
    ImGui::TextColored(yellow, "    PC"); ImGui::SameLine();
    ImGui::Text("= $%04X", proc_state->PC->GetValue());
    ImGui::Text(BYTE_TO_BINARY_PATTERN_SPACED " " BYTE_TO_BINARY_PATTERN_SPACED, BYTE_TO_BINARY(proc_state->PC->GetHigh()), BYTE_TO_BINARY(proc_state->PC->GetLow()));


    ImGui::Columns(2);
    ImGui::Separator();

    ImGui::TextColored(magenta, " IME"); ImGui::SameLine();
    ImGui::Text("= %d", *proc_state->IME);

    ImGui::NextColumn();

    ImGui::TextColored(magenta, "HALT"); ImGui::SameLine();
    ImGui::Text("= %d", *proc_state->Halt);

    ImGui::NextColumn();

    ImGui::Columns(1);
    
    ImGui::Separator();

    ImGui::TextColored(cyan, " DOUBLE SPEED "); ImGui::SameLine();
    processor->CGBSpeed() ? ImGui::TextColored(green, "ON") : ImGui::TextColored(gray, "OFF");

    ImGui::PopFont();

    ImGui::End();
}

static void debug_window_audio(void)
{
    ImGui::SetNextWindowSize(ImVec2(494, 0), ImGuiCond_FirstUseEver);

    ImGui::Begin("Sound Registers", &config_debug.show_audio);

    ImGui::PushFont(gui_default_font);

    GearboyCore* core = emu_get_core();
    Audio* audio = core->GetAudio();

    gb_apu_state_t apu_state;
    audio->GetApu()->save_state(&apu_state);

    ImGui::Columns(2, "audio");

    ImGui::TextColored(yellow, "CHANNEL 1 - TONE & SWEEP:");

    u8 value = apu_state.regs[0xFF10 - 0xFF10];
    ImGui::TextColored(cyan, " $FF10"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR10"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF11 - 0xFF10];
    ImGui::TextColored(cyan, " $FF11"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR11"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF12 - 0xFF10];
    ImGui::TextColored(cyan, " $FF12"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR12"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF13 - 0xFF10];
    ImGui::TextColored(cyan, " $FF13"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR13"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF14 - 0xFF10];
    ImGui::TextColored(cyan, " $FF14"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR14"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    ImGui::NextColumn();

    ImGui::TextColored(yellow, "CHANNEL 3 - WAVE:");

    value = apu_state.regs[0xFF1A - 0xFF10];
    ImGui::TextColored(cyan, " $FF1A"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR30"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF1B - 0xFF10];
    ImGui::TextColored(cyan, " $FF1B"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR31"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF1C - 0xFF10];
    ImGui::TextColored(cyan, " $FF1C"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR32"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF1D - 0xFF10];
    ImGui::TextColored(cyan, " $FF1D"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR33"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF1E - 0xFF10];
    ImGui::TextColored(cyan, " $FF1E"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR34"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    ImGui::NextColumn();
    ImGui::Separator();

    ImGui::TextColored(yellow, "CHANNEL 2 - TONE:");

    value = apu_state.regs[0xFF16 - 0xFF10];
    ImGui::TextColored(cyan, " $FF16"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR21"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF17 - 0xFF10];
    ImGui::TextColored(cyan, " $FF17"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR22"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF18 - 0xFF10];
    ImGui::TextColored(cyan, " $FF18"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR23"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF19 - 0xFF10];
    ImGui::TextColored(cyan, " $FF19"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR24"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    ImGui::NextColumn();

    ImGui::TextColored(yellow, "CHANNEL 4 - NOISE:");

    value = apu_state.regs[0xFF20 - 0xFF10];
    ImGui::TextColored(cyan, " $FF20"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR41"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF21 - 0xFF10];
    ImGui::TextColored(cyan, " $FF21"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR42"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF22 - 0xFF10];
    ImGui::TextColored(cyan, " $FF22"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR43"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF23 - 0xFF10];
    ImGui::TextColored(cyan, " $FF23"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR44"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    ImGui::NextColumn();
    ImGui::Separator();

    ImGui::TextColored(yellow, "CONTROL:");

    value = apu_state.regs[0xFF24 - 0xFF10];
    ImGui::TextColored(cyan, " $FF24"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR50"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF25 - 0xFF10];
    ImGui::TextColored(cyan, " $FF25"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR51"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    value = apu_state.regs[0xFF26 - 0xFF10];
    ImGui::TextColored(cyan, " $FF26"); ImGui::SameLine();
    ImGui::TextColored(magenta, "NR52"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", value, BYTE_TO_BINARY(value));

    ImGui::NextColumn();

    ImGui::TextColored(yellow, "WAVE ($FF30 - $FF37):" );

    ImGui::Text(" %02X%02X %02X%02X %02X%02X %02X%02X", apu_state.regs[0x20], apu_state.regs[0x21], apu_state.regs[0x22], apu_state.regs[0x23], apu_state.regs[0x24], apu_state.regs[0x25], apu_state.regs[0x26], apu_state.regs[0x27]);

    ImGui::TextColored(yellow, "WAVE ($FF38 - $FF3F):" );

    ImGui::Text(" %02X%02X %02X%02X %02X%02X %02X%02X", apu_state.regs[0x28], apu_state.regs[0x29], apu_state.regs[0x2A], apu_state.regs[0x2B], apu_state.regs[0x2C], apu_state.regs[0x2D], apu_state.regs[0x2E], apu_state.regs[0x2F]);

    ImGui::NextColumn();

    ImGui::Columns(1);

    ImGui::PopFont();

    ImGui::End();
}

static void debug_window_io(void)
{
    ImGui::SetNextWindowSize(ImVec2(494, 0), ImGuiCond_FirstUseEver);

    ImGui::Begin("IO Map", &config_debug.show_iomap);

    ImGui::PushFont(gui_default_font);

    GearboyCore* core = emu_get_core();
    Memory* memory = core->GetMemory();

    ImGui::Columns(2, "iomap");

    ImGui::TextColored(yellow, "INTERRUPTS:");

    ImGui::TextColored(cyan, " $FFFF"); ImGui::SameLine();
    ImGui::TextColored(magenta, "IE  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFFFF), BYTE_TO_BINARY(memory->Retrieve(0xFFFF)));

    ImGui::TextColored(cyan, " $FF0F"); ImGui::SameLine();
    ImGui::TextColored(magenta, "IF  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF0F), BYTE_TO_BINARY(memory->Retrieve(0xFF0F)));

    ImGui::TextColored(cyan, " VBLNK  "); ImGui::SameLine();
    IsSetBit(memory->Retrieve(0xFF0F), 0) && IsSetBit(memory->Retrieve(0xFFFF), 0) ? ImGui::TextColored(green, "ON  ") : ImGui::TextColored(gray, "OFF "); ImGui::SameLine();
    ImGui::TextColored(magenta, "IF:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFF0F), 0)); ImGui::SameLine();
    ImGui::TextColored(magenta, "  IE:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFFFF), 0));

    ImGui::TextColored(cyan, " STAT   "); ImGui::SameLine();
    IsSetBit(memory->Retrieve(0xFF0F), 1) && IsSetBit(memory->Retrieve(0xFFFF), 1) ? ImGui::TextColored(green, "ON  ") : ImGui::TextColored(gray, "OFF "); ImGui::SameLine();
    ImGui::TextColored(magenta, "IF:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFF0F), 1)); ImGui::SameLine();
    ImGui::TextColored(magenta, "  IE:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFFFF), 1));

    ImGui::TextColored(cyan, " TIMER  "); ImGui::SameLine();
    IsSetBit(memory->Retrieve(0xFF0F), 2) && IsSetBit(memory->Retrieve(0xFFFF), 2) ? ImGui::TextColored(green, "ON  ") : ImGui::TextColored(gray, "OFF "); ImGui::SameLine();
    ImGui::TextColored(magenta, "IF:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFF0F), 2)); ImGui::SameLine();
    ImGui::TextColored(magenta, "  IE:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFFFF), 2));

    ImGui::TextColored(cyan, " SERIAL "); ImGui::SameLine();
    IsSetBit(memory->Retrieve(0xFF0F), 3) && IsSetBit(memory->Retrieve(0xFFFF), 3) ? ImGui::TextColored(green, "ON  ") : ImGui::TextColored(gray, "OFF "); ImGui::SameLine();
    ImGui::TextColored(magenta, "IF:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFF0F), 3)); ImGui::SameLine();
    ImGui::TextColored(magenta, "  IE:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFFFF), 3));

    ImGui::TextColored(cyan, " JOYPAD "); ImGui::SameLine();
    IsSetBit(memory->Retrieve(0xFF0F), 4) && IsSetBit(memory->Retrieve(0xFFFF), 4) ? ImGui::TextColored(green, "ON  ") : ImGui::TextColored(gray, "OFF "); ImGui::SameLine();
    ImGui::TextColored(magenta, "IF:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFF0F), 4)); ImGui::SameLine();
    ImGui::TextColored(magenta, "  IE:"); ImGui::SameLine();
    ImGui::Text("%d", IsSetBit(memory->Retrieve(0xFFFF), 4));

    ImGui::TextColored(yellow, "GBC:");

    ImGui::TextColored(cyan, " $FF4D"); ImGui::SameLine();
    ImGui::TextColored(magenta, "KEY1"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF4D), BYTE_TO_BINARY(memory->Retrieve(0xFF4D)));

    ImGui::TextColored(cyan, " $FF70"); ImGui::SameLine();
    ImGui::TextColored(magenta, "SVBK"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF70), BYTE_TO_BINARY(memory->Retrieve(0xFF70)));

    ImGui::TextColored(yellow, "GBC LCD:");

    ImGui::TextColored(cyan, " $FF68"); ImGui::SameLine();
    ImGui::TextColored(magenta, "BCPS"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF68), BYTE_TO_BINARY(memory->Retrieve(0xFF68)));

    ImGui::TextColored(cyan, " $FF69"); ImGui::SameLine();
    ImGui::TextColored(magenta, "BCPD"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF69), BYTE_TO_BINARY(memory->Retrieve(0xFF69)));

    ImGui::TextColored(cyan, " $FF6A"); ImGui::SameLine();
    ImGui::TextColored(magenta, "OCPS"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF6A), BYTE_TO_BINARY(memory->Retrieve(0xFF6A)));

    ImGui::TextColored(cyan, " $FF6B"); ImGui::SameLine();
    ImGui::TextColored(magenta, "OCPD"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF6B), BYTE_TO_BINARY(memory->Retrieve(0xFF6B)));

    ImGui::TextColored(cyan, " $FF4F"); ImGui::SameLine();
    ImGui::TextColored(magenta, "VBK "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF4F), BYTE_TO_BINARY(memory->Retrieve(0xFF4F)));

    ImGui::TextColored(yellow, "GBC HDMA:");

    ImGui::TextColored(cyan, " $FF51:$FF52"); ImGui::SameLine();
    ImGui::TextColored(magenta, "SOURCE "); ImGui::SameLine();
    ImGui::Text("$%04X", (memory->Retrieve(0xFF51) << 8) | memory->Retrieve(0xFF52));

    ImGui::TextColored(cyan, " $FF53:$FF54"); ImGui::SameLine();
    ImGui::TextColored(magenta, "DEST   "); ImGui::SameLine();
    ImGui::Text("$%04X", (memory->Retrieve(0xFF53) << 8) | memory->Retrieve(0xFF54));

    ImGui::TextColored(cyan, " $FF55"); ImGui::SameLine();
    ImGui::TextColored(magenta, "LEN "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF55), BYTE_TO_BINARY(memory->Retrieve(0xFF55)));

    ImGui::TextColored(yellow, "GBC INFRARED:");

    ImGui::TextColored(cyan, " $FF56"); ImGui::SameLine();
    ImGui::TextColored(magenta, "RP  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF56), BYTE_TO_BINARY(memory->Retrieve(0xFF56)));

    ImGui::NextColumn();

    ImGui::TextColored(yellow, "LCD:");

    ImGui::TextColored(cyan, " $FF40"); ImGui::SameLine();
    ImGui::TextColored(magenta, "LCDC"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF40), BYTE_TO_BINARY(memory->Retrieve(0xFF40)));

    ImGui::TextColored(cyan, " $FF41"); ImGui::SameLine();
    ImGui::TextColored(magenta, "STAT"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF41), BYTE_TO_BINARY(memory->Retrieve(0xFF41)));

    ImGui::TextColored(cyan, " $FF42"); ImGui::SameLine();
    ImGui::TextColored(magenta, "SCY "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF42), BYTE_TO_BINARY(memory->Retrieve(0xFF42)));

    ImGui::TextColored(cyan, " $FF43"); ImGui::SameLine();
    ImGui::TextColored(magenta, "SCX "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF43), BYTE_TO_BINARY(memory->Retrieve(0xFF43)));

    ImGui::TextColored(cyan, " $FF44"); ImGui::SameLine();
    ImGui::TextColored(magenta, "LY  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF44), BYTE_TO_BINARY(memory->Retrieve(0xFF44)));

    ImGui::TextColored(cyan, " $FF45"); ImGui::SameLine();
    ImGui::TextColored(magenta, "LYC "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF45), BYTE_TO_BINARY(memory->Retrieve(0xFF45)));

    ImGui::TextColored(cyan, " $FF46"); ImGui::SameLine();
    ImGui::TextColored(magenta, "DMA "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF46), BYTE_TO_BINARY(memory->Retrieve(0xFF46)));

    ImGui::TextColored(cyan, " $FF47"); ImGui::SameLine();
    ImGui::TextColored(magenta, "BGP "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF47), BYTE_TO_BINARY(memory->Retrieve(0xFF47)));

    ImGui::TextColored(cyan, " $FF48"); ImGui::SameLine();
    ImGui::TextColored(magenta, "OBP0"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF48), BYTE_TO_BINARY(memory->Retrieve(0xFF48)));

    ImGui::TextColored(cyan, " $FF49"); ImGui::SameLine();
    ImGui::TextColored(magenta, "OBP1"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF49), BYTE_TO_BINARY(memory->Retrieve(0xFF49)));

    ImGui::TextColored(cyan, " $FF4A"); ImGui::SameLine();
    ImGui::TextColored(magenta, "WY  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF4A), BYTE_TO_BINARY(memory->Retrieve(0xFF4A)));

    ImGui::TextColored(cyan, " $FF4B"); ImGui::SameLine();
    ImGui::TextColored(magenta, "WX  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF4B), BYTE_TO_BINARY(memory->Retrieve(0xFF4B)));

    ImGui::TextColored(yellow, "TIMER:");

    ImGui::TextColored(cyan, " $FF04"); ImGui::SameLine();
    ImGui::TextColored(magenta, "DIV "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF04), BYTE_TO_BINARY(memory->Retrieve(0xFF04)));

    ImGui::TextColored(cyan, " $FF05"); ImGui::SameLine();
    ImGui::TextColored(magenta, "TIMA"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF05), BYTE_TO_BINARY(memory->Retrieve(0xFF05)));

    ImGui::TextColored(cyan, " $FF06"); ImGui::SameLine();
    ImGui::TextColored(magenta, "TMA "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF06), BYTE_TO_BINARY(memory->Retrieve(0xFF06)));

    ImGui::TextColored(cyan, " $FF07"); ImGui::SameLine();
    ImGui::TextColored(magenta, "TAC "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF07), BYTE_TO_BINARY(memory->Retrieve(0xFF07)));

    ImGui::TextColored(yellow, "INPUT:");

    ImGui::TextColored(cyan, " $FF00"); ImGui::SameLine();
    ImGui::TextColored(magenta, "JOYP"); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF00), BYTE_TO_BINARY(memory->Retrieve(0xFF00)));

    ImGui::TextColored(yellow, "SERIAL:");

    ImGui::TextColored(cyan, " $FF01"); ImGui::SameLine();
    ImGui::TextColored(magenta, "SB  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF01), BYTE_TO_BINARY(memory->Retrieve(0xFF01)));

    ImGui::TextColored(cyan, " $FF02"); ImGui::SameLine();
    ImGui::TextColored(magenta, "SC  "); ImGui::SameLine();
    ImGui::Text("$%02X (" BYTE_TO_BINARY_PATTERN_SPACED ")", memory->Retrieve(0xFF02), BYTE_TO_BINARY(memory->Retrieve(0xFF02)));

    ImGui::Columns(1);

    ImGui::PopFont();

    ImGui::End();
}
