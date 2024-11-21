#include <iostream>
#include <EasyChip8.h>
#include <fstream>
#include <vector>
#include <SDL.h>

void dummyFile(char *name)
{
    // Define os opcodes do programa CHIP-8
    std::vector<uint8_t> program = {
        0x00, 0xE0, // 00E0: Limpar a tela

        0x60, 0x05, // 6000: LD V0, 0x00
        0x61, 0x10, // 6105: LD V1, 0x05
        0xA0, 0x82, // A050: LD I, 0x050
        0xD0, 0x15, // D015: DRW V0, V1, 5

        0x60, 0x0A, // 6000: LD V0, 0x00
        0x61, 0x10, // 6105: LD V1, 0x05
        0xA0, 0xA5, // A050: LD I, 0x050
        0xD0, 0x15, // D015: DRW V0, V1, 5

        0x60, 0x14, // 6000: LD V0, 0x00
        0x61, 0x10, // 6105: LD V1, 0x05
        0xA0, 0x87, // A050: LD I, 0x050
        0xD0, 0x15, // D015: DRW V0, V1, 5

        0x60, 0x19, // 6000: LD V0, 0x00
        0x61, 0x10, // 6105: LD V1, 0x05
        0xA0, 0x96, // A050: LD I, 0x050
        0xD0, 0x15, // D015: DRW V0, V1, 5

        0x60, 0x1E, // 6000: LD V0, 0x00
        0x61, 0x10, // 6105: LD V1, 0x05
        0xA0, 0xA0, // A050: LD I, 0x050
        0xD0, 0x15, // D015: DRW V0, V1, 5

        0x60, 0x24, // 6000: LD V0, 0x00
        0x61, 0x10, // 6105: LD V1, 0x05
        0xA0, 0x82, // A050: LD I, 0x050
        0xD0, 0x15, // D015: DRW V0, V1, 5

        0x12, 0x32 // 1200: JP 0x202 (loop infinito)
    };

    // Abre o arquivo binário para escrita
    std::ofstream outfile(name, std::ios::binary);

    if (!outfile)
    {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return;
    }

    // Escreve os bytes do programa no arquivo
    outfile.write(reinterpret_cast<char *>(program.data()), program.size());

    std::cout << "Arquivo binário CHIP-8 criado com sucesso!" << std::endl;
}

int main(int argv, char **argc)
{

    if (argv < 2)
    {
        std::cout << "Usage: easychip8.exe <code>" << std::endl;
    }

    EasyChip8 emulator;

    dummyFile(argc[1]);

    emulator.loadROM(argc[1]);

    emulator.run();

    return 0;
}