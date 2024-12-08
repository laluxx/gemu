#include <common.h>
#include <lume.h>
#include <renderer.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

const int sw = 1920;
const int sh = 1080;

// CPU Registers
typedef struct {
    uint8_t a;    // Accumulator
    uint8_t f;    // Flags
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;  // Stack pointer
    uint16_t pc;  // Program counter
} Registers;

// Memory Management Unit
typedef struct {
    uint8_t memory[0x10000];  // 64KB of memory
    uint8_t *rom;             // Dynamic ROM buffer
    size_t rom_size;          // Size of loaded ROM
} MMU;

typedef struct {
    uint8_t vram[0x2000];           // 8KB of video RAM
    uint8_t screen_data[160 * 144]; // Screen resolution is 160x144
} GPU;

typedef struct {
    Registers cpu;
    MMU mmu;
    GPU gpu;
    uint64_t cycles;
} GameBoy;


void init_registers(Registers *cpu) {
    memset(cpu, 0, sizeof(Registers));
}

void init_mmu(MMU *mmu) {
    memset(mmu->memory, 0, sizeof(mmu->memory));
    mmu->rom = NULL;
    mmu->rom_size = 0;
}

void init_gpu(GPU *gpu) {
    memset(gpu->vram, 0, sizeof(gpu->vram));
    memset(gpu->screen_data, 0, sizeof(gpu->screen_data));
}

void init_gameboy(GameBoy *gb) {
    init_registers(&gb->cpu);
    init_mmu(&gb->mmu);
    init_gpu(&gb->gpu);
    gb->cycles = 0;
}

uint8_t read_byte(MMU *mmu, uint16_t address) {
    return mmu->memory[address];
}

void write_byte(MMU *mmu, uint16_t address, uint8_t value) {
    mmu->memory[address] = value;
}

bool load_rom(MMU *mmu, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open ROM file: %s\n", filename);
        return false;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for ROM
    mmu->rom = (uint8_t*)malloc(size);
    if (!mmu->rom) {
        fclose(file);
        return false;
    }

    // Read ROM data
    if (fread(mmu->rom, 1, size, file) != size) {
        free(mmu->rom);
        fclose(file);
        return false;
    }

    mmu->rom_size = size;

    // Copy ROM to memory (up to 0x8000)
    size_t copy_size = (size < 0x8000) ? size : 0x8000;
    memcpy(mmu->memory, mmu->rom, copy_size);

    fclose(file);
    return true;
}

void update_gpu(GPU *gpu) {
    // GPU rendering logic will go here
}

void gameboy_step(GameBoy *gb) {
    // Fetch
    uint8_t opcode = read_byte(&gb->mmu, gb->cpu.pc);
    gb->cpu.pc++;  // Increment program counter

    // Decode and Execute
    switch (opcode) {
    case 0x00:  // NOP
        break;
            
    case 0x3E:  // LD A, n
        gb->cpu.a = read_byte(&gb->mmu, gb->cpu.pc);
        gb->cpu.pc++;
        break;
            
    default:
        printf("Unknown opcode: 0x%02X\n", opcode);
        break;
    }

    update_gpu(&gb->gpu);
}

void cleanup_gameboy(GameBoy *gb) {
    if (gb->mmu.rom) {
        free(gb->mmu.rom);
        gb->mmu.rom = NULL;
    }
}

int main() {
    initWindow(sw, sh, "GameBoy - Emulator");

    GLuint image = loadTexture("./skin.jpg");

    while (!windowShouldClose()) {
        beginDrawing();
        clearBackground(BLACK);

        useShader("texture");
        /* drawTexture((Vec2f){400.0f, 400.0f,}, (Vec2f){800.0f, 900.0f}, image); */
        /* drawTextureOriginal((Vec2f){0.0f, 0.0f}, image); */
        drawTextureScaled((Vec2f){0.0f, 0.0f}, image, 0.5f);
        flush();

        endDrawing();
    }

    closeWindow();
    return 0;
}
