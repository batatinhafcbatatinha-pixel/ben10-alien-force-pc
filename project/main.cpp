// Keep the shared frontend; install this ELF's recovered entries after static initialization.
#define main Ben10SharedMain
#include PS2X_SHARED_MAIN_CPP // the PS2Recomp frontend (path set by project/CMakeLists.txt)
#undef main
#include "kernel-handlers/Ben10KernelCopy_0x2d40f0.cpp"
#include "kernel-handlers/Ben10KernelFind_0x2d4128.cpp"

struct Ben10TableEntry { uint32_t address; PS2Runtime::RecompiledFunction function; };
#include "sdk-handlers.inc"
#include "gap-functions.inc"

extern const uint32_t g_ps2RecompiledFunctionTableBase;
extern const uint32_t g_ps2RecompiledFunctionTableSlotCount;
extern PS2Runtime::RecompiledFunction g_ps2RecompiledFunctionTable[];
std::string ps2xDispatchTrail();

// BEN10_PROBE=<hex>: log ra/args/trail of the first calls (then powers of two) to one guest address.
static PS2Runtime::RecompiledFunction g_probeOriginal = nullptr;
static void Ben10Probe(uint8_t* rdram, R5900Context* ctx, PS2Runtime* runtime) {
    static int n = 0;
    static const char* raFilter = std::getenv("BEN10_PROBE_RA");
    if (raFilter && getRegU32(ctx, 31) != std::strtoul(raFilter, nullptr, 16)) { g_probeOriginal(rdram, ctx, runtime); return; }
    ++n;
    if (n <= 6 || (n & (n - 1)) == 0 || (n <= 64 && std::getenv("BEN10_PROBE_RET")))
        std::cerr << "[probe] pc=0x" << std::hex << ctx->pc << " ra=0x" << getRegU32(ctx, 31) << " a0=0x" << getRegU32(ctx, 4)
                  << " a1=0x" << getRegU32(ctx, 5) << " a2=0x" << getRegU32(ctx, 6) << " sp=0x" << getRegU32(ctx, 29) << std::dec
                  << " trail=" << ps2xDispatchTrail() << std::endl;
    g_probeOriginal(rdram, ctx, runtime);
    if (n <= 64 && std::getenv("BEN10_PROBE_RET"))
        std::cerr << "[probe] #" << n << " ret v0=0x" << std::hex << getRegU32(ctx, 2) << " pc=0x" << ctx->pc << std::dec << std::endl;
}

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
        if (entry.address >= g_ps2RecompiledFunctionTableBase && slot < g_ps2RecompiledFunctionTableSlotCount) {
            // The AOT table also binds every Ghidra label address, mapping it to the translated function that
            // *contains* it. A gap start is by construction outside every Ghidra function, so a slot it occupies was
            // bound to a neighbouring function (e.g. 0x2dc558 -> FUN_002dc958) and must be replaced, not skipped.
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
    if (const char* probe = std::getenv("BEN10_PROBE")) {
        const uint32_t slot = (std::strtoul(probe, nullptr, 16) - g_ps2RecompiledFunctionTableBase) / 4;
        if (slot < g_ps2RecompiledFunctionTableSlotCount && g_ps2RecompiledFunctionTable[slot]) {
            g_probeOriginal = g_ps2RecompiledFunctionTable[slot];
            g_ps2RecompiledFunctionTable[slot] = Ben10Probe;
        }
    }
    return Ben10SharedMain(argc, argv);
}
