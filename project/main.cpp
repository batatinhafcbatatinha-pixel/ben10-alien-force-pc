// Keep the shared frontend; install this ELF's recovered entries after static initialization.
#define main Ben10SharedMain
#include "../../third_party/PS2Recomp/ps2xRuntime/src/main.cpp"
#undef main
#include "kernel-handlers/Ben10KernelCopy_0x2d40f0.cpp"
#include "kernel-handlers/Ben10KernelFind_0x2d4128.cpp"

struct Ben10TableEntry { uint32_t address; PS2Runtime::RecompiledFunction function; };
#include "sdk-handlers.inc"
#include "gap-functions.inc"

extern const uint32_t g_ps2RecompiledFunctionTableBase;
extern const uint32_t g_ps2RecompiledFunctionTableSlotCount;
extern PS2Runtime::RecompiledFunction g_ps2RecompiledFunctionTable[];

int main(int argc, char** argv) {
    struct Entry { uint32_t address; PS2Runtime::RecompiledFunction function; };
    const Entry entries[] = {
        {0x2d40f0u, Ben10KernelCopy_0x2d40f0},
        {0x2d4100u, Ben10KernelCopy_0x2d40f0},
        {0x2d4128u, Ben10KernelFind_0x2d4128},
        {0x2d4140u, Ben10KernelFind_0x2d4128},
    };
    for (const auto& entry : entries) {
        const uint32_t slot = (entry.address - g_ps2RecompiledFunctionTableBase) / 4;
        if (entry.address < g_ps2RecompiledFunctionTableBase ||
            slot >= g_ps2RecompiledFunctionTableSlotCount || g_ps2RecompiledFunctionTable[slot]) {
            std::cerr << "Ben10 supplemental function conflicts with AOT table\n";
            return 2;
        }
        g_ps2RecompiledFunctionTable[slot] = entry.function;
    }
    for (const auto& entry : kBen10GapFunctions) {
        const uint32_t slot = (entry.address - g_ps2RecompiledFunctionTableBase) / 4;
        if (entry.address >= g_ps2RecompiledFunctionTableBase && slot < g_ps2RecompiledFunctionTableSlotCount &&
            !g_ps2RecompiledFunctionTable[slot]) {
            g_ps2RecompiledFunctionTable[slot] = entry.function;
        }
    }
    // SDK library matches replace their translated guest code with the runtime's native IOP-side services
    // (SIF RPC, CD, pad, memory card, ...): there is no IOP processor answering these requests.
    if (!std::getenv("BEN10_GUEST_SDK")) {
        for (const auto& entry : kBen10SdkHandlers) {
            const uint32_t slot = (entry.address - g_ps2RecompiledFunctionTableBase) / 4;
            if (entry.address < g_ps2RecompiledFunctionTableBase || slot >= g_ps2RecompiledFunctionTableSlotCount) {
                std::cerr << "Ben10 SDK handler outside AOT table\n";
                return 2;
            }
            g_ps2RecompiledFunctionTable[slot] = entry.function;
        }
    }
    return Ben10SharedMain(argc, argv);
}
