#include "EasyChip8.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <iomanip> // Para configurar o formato hexadecimal
#include <chrono>

EasyChip8::EasyChip8()
{

    initializeMemory();

    pc = ROM_START_ADDR;

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];

    // Set up function pointer table
}

void EasyChip8::loadROM(const std::string &filename)
{

    std::ifstream romFile(filename, std::ios::binary);
    if (!romFile.is_open())
    {
        throw std::runtime_error("Error: Cannot open ROM file.");
    }

    // verify file size
    romFile.seekg(0, std::ios::end);
    size_t romSize = romFile.tellg();
    romFile.seekg(0, std::ios::beg);

    if (romSize > (MEM_SIZE - ROM_START_ADDR))
    {
        throw std::runtime_error("Error: Invalid ROM size.");
    }

    // put instructions into memory
    char *buffer = new char[romSize];
    romFile.read(buffer, romSize);
    romFile.close();

    // Load the ROM contents into the Chip8's memory, starting at 0x200
    for (size_t i = 0; i < romSize; ++i)
    {
        memory[ROM_START_ADDR + i] = buffer[i];
    }

    // Free the buffer
    delete[] buffer;

    // Verifica se ocorreu erro na leitura
    if (romFile.fail() || romFile.bad())
    {
        throw std::runtime_error("Error: Bad ROM file.");
    }
}

void EasyChip8::initializeMemory()
{
    for (int i = 0; i < MEM_SIZE; ++i)
    {
        memory[i] = 0;
    }
}

// fetch, decode, execute
void EasyChip8::cycle()
{

    // Fetch
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    // Increment the PC before we execute anything
    pc += 2;

    // Decode and Execute
    switch (opcode & 0xF000u)
    {
    case 0x0000:
        OP_00E0();
        break;

    case 0x1000:
        OP_1nnn();
        break;

    case 0x6000:
        OP_6xnn();
        break;

    case 0x7000:
        OP_7xnn();
        break;

    case 0xA000:
        OP_Annn();
        break;

    case 0xD000:
        OP_Dxyn();
        break;

    default:
        throw std::runtime_error("Opcode desconhecido (0x" + opcode);
    }

    // Decrement the delay timer if it's been set
    if (delayTimer > 0)
    {
        --delayTimer;
    }

    // Decrement the sound timer if it's been set
    if (soundTimer > 0)
    {
        --soundTimer;
    }
}

void EasyChip8::run()
{

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;
    int videoPitch = sizeof(video[0]) * VIDEO_LENGTH;

    Screen screen("EasyChip8", VIDEO_LENGTH * 5, VIDEO_WIDTH * 5, VIDEO_LENGTH, VIDEO_WIDTH);

    while (!quit)
    {
        // quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > 16)
        {
            lastCycleTime = currentTime;

            cycle();

            screen.update(video, videoPitch);
        }
    }
    std::cout << "Saiu\n";
}

// Função para imprimir a memória em formato hexadecimal
void EasyChip8::printMemory()
{
    std::cout << "Instruções a partir de 0x200:\n";

    // Percorre a memória do CHIP-8, começando pelo endereço 0x200
    for (size_t i = 0x200; i < 0x212; i += 2)
    {
        // Lê os dois bytes para formar o opcode de 16 bits
        uint16_t opcode = (memory[i] << 8) | memory[i + 1];

        // Imprime o endereço da instrução seguido do opcode em hexadecimal
        std::cout << "Endereço 0x" << std::hex << std::setw(3) << std::setfill('0') << i << " : ";
        std::cout << std::setw(4) << std::setfill('0') << std::hex << opcode << std::endl;
    }
    std::cout << std::dec << std::endl; // Volta ao formato decimal para outros prints
}

void EasyChip8::debugVideo()
{
    for (int y = 0; y < VIDEO_WIDTH; y++)
    {
        for (int x = 0; x < VIDEO_LENGTH; x++)
        {
            // Calcula o índice do array unidimensional
            int index = y * 64 + x;

            // Imprime o valor do pixel (1 ou 0)
            std::cout << (video[index] ? "#" : "-"); // # para 1 e espaço para 0
        }
        std::cout << std::endl; // Nova linha após cada linha da tela
    }
}

// Instructions

// clear screen
void EasyChip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

// jump addr
void EasyChip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

// set x with nn
void EasyChip8::OP_6xnn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

// add to x nn
void EasyChip8::OP_7xnn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

// load nnn to I
void EasyChip8::OP_Annn()
{
    uint16_t address = opcode & 0x0FFFu;

    index = address;
}

// draw
void EasyChip8::OP_Dxyn()
{

    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // Wrap if going beyond screen boundaries
    uint8_t xPos = registers[Vx] % VIDEO_LENGTH;
    uint8_t yPos = registers[Vy] % VIDEO_WIDTH;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *screenPixel = &video[(yPos + row) * VIDEO_LENGTH + (xPos + col)];

            // std::cout << (spritePixel ? 1 : 0);

            // Sprite pixel is on
            if (spritePixel)
            {
                // Screen pixel also on - collision
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registers[0xF] = 1;
                }

                // Effectively XOR with the sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}