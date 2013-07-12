#include "types.h"

class Asm {
public:
    static void cli() {
        ASMV("cli");
    }

    static void sti() {
        ASMV("sti");
    }

    static void hlt() {
        ASMV("hlt");
    }
};
