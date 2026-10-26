# Ben 10: Alien Force — Native PC Port (early work) | PlayStation 2 Static Recompilation

> [!WARNING]
> This repository does **not** include the game, an ISO/ELF, game assets, generated game code or a precompiled executable.
> You need your own PlayStation 2 USA disc. See [the full notice](#notice) at the end.

Early work on a port of the PlayStation 2 game *Ben 10: Alien Force* (USA), built through static recompilation of the Emotion Engine code with [PS2Recomp](https://github.com/ran-j/PS2Recomp). **This public repository is a work-in-progress source snapshot, not a ready-to-play game download or a self-contained build kit.** The game's MIPS code is translated locally from your own disc; no game executable or generated game code is distributed here.

## Status

- **Not playable yet.** The native executable builds and boots: SIF RPC initializes, the IOP modules load and the game's own code runs until it reaches functions the first function map missed.
- EE (R5900) code: 8,760 functions from Ghidra plus 1,174 recovered "gap" functions; 125 SDK functions (SIF, CD, pad, memory card, sound, MPEG) are routed to the runtime's native handlers.
- Next step: build the recovered gap functions and continue the boot. No menu, graphics or audio output has been validated.
- IOP and VU still run through interpreters in the runtime; this is not a fully native port.

## What is in this repository

- `scripts/` — config preparation, gap-function and SDK-handler generators, dispatch bucketing, chunking, guarded generation, native build and smoke test.
- `config/` — recompiler configuration (`recomp.toml`, gap functions, kernel handlers, analyzer heuristics). Ghidra CSV exports are not included.
- `project/` — CMake project and `main.cpp` that installs the handlers; generated sources are not included.
- [NOTES.md](NOTES.md) and [CONTINUACAO.md](CONTINUACAO.md) — development log (in Portuguese). [CONTROLES.md](CONTROLES.md) — planned controls.

The scripts expect the author's local workspace (`../third_party/PS2Recomp` and the shared runtime build).

## Your game files

```text
SLUS_218.15  92004adcf85e877199be5b5ca017c640e95795017c0ca3c8d4ecbde52567cb61
```

Do not upload the ISO, extracted files, Ghidra databases, generated code or the built executable.

## Credits and licenses

- Port integration and tooling: [batatinhafcbatatinha-pixel](https://github.com/batatinhafcbatatinha-pixel).
- [PS2Recomp](https://github.com/ran-j/PS2Recomp) by ran-j and contributors: recompiler, analyzer and runtime base.
- [Ghidra](https://github.com/NationalSecurityAgency/ghidra) with the [ghidra-emotionengine-reloaded](https://github.com/chaoticgd/ghidra-emotionengine-reloaded) extension, used locally for function discovery.
- Ben 10: Alien Force, its code, trademarks and assets belong to Cartoon Network, D3Publisher, Monkey Bar Games and the respective rights holders. This project is not affiliated with or endorsed by them or by Sony.

---

<a id="notice"></a>

**⚠️ THIS PROJECT DOES NOT INCLUDE THE GAME, AN ISO/ELF, GAME ASSETS, GENERATED GAME CODE OR A PRECOMPILED GAME EXECUTABLE. YOU MUST PROVIDE YOUR OWN LAWFULLY OBTAINED GAME FILES AND HAVE THE RIGHTS REQUIRED FOR YOUR USE. THIS PROJECT DOES NOT AUTHORIZE COPYRIGHT INFRINGEMENT. ALL GAME RIGHTS BELONG TO THEIR RESPECTIVE OWNERS. ⚠️**
