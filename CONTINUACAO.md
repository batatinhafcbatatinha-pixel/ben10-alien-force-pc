# Checkpoint — 26/10/2026 noite (Claude): Sif resolvido; boot entra no jogo; falta compilar funções de "buraco"

- `scripts/gen_sdk_handlers.py` → `project/sdk-handlers.inc`: 125 matches SDK do analisador (sceSif*/sceCd*/scePad*/sceMc*/
  sceSd*/sceMpeg*...) instalados sobre a tabela de funções em `main.cpp` (sem regenerar o AOT; `BEN10_GUEST_SDK=1` desliga).
  Pulados de propósito: `_sceSifSendCmd` e `_sceSifCmdIntrHdlr` (ABI diferente de sceSifSendCmd).
- Runtime compartilhado: `SetupThread` com stack=-1 devolvia sp = RAM-size (pilha fora da região reservada); agora sp = fim
  da RAM. `reserveGuestStackRegion` move as pilhas de callback (128 KiB) para baixo da pilha do jogo e encolhe o arena do
  runtime (`shrinkRuntimeArena`). CoD passa pilha explícita → não afetado.
- Resultado (build com os 2 itens acima, testado): SifInitRpc ok, 8 IRX carregados, RPCs respondidos, código do jogo roda
  até chamar `0x2DCE60` — função que o Ghidra só tinha como rótulo (`entry_002dce60`).
- `scripts/gen_gap_functions.py`: 1.174 inícios do analisador fora das funções do Ghidra → `project/gap-functions/`
  (gerado, 21 MB) + `project/gap-functions.inc`; `main.cpp` instala só em slots vazios; CMake compila com /Od.
  **Build dessa etapa foi interrompido** (Claude Code matou por pouca memória livre). Próximo: `cmake -S project -B
  D:\CodexBuilds
ative-portsen10-af` + build com menos jobs (2), depois `python scripts/smoke.py --seconds 30`.

# Checkpoint — 26/10/2026 ~17h15 (Claude): bloqueio atual = sceSifInitRpc

Estado vindo do Codex (12h, não registrado antes): handlers de syscall traduzidos, pilha nativa aumentada; o boot
agora fica girando em `0x2D18D8` (ra `0x2AC320`): é o `sceSifInitRpc` da libkernel (`0x2D1780`), que manda o comando
SIF `0x80000002` (INIT_CMD) via `sceSifSendCmd` `0x2D1508` e espera `sceSifGetSreg(0)` (`0x2D0ED8`) ≠ 0 — resposta que
o IOP daria. Próximo passo (caminho nativo, sem emular IOP): aplicar os handlers HLE do runtime PS2Recomp às funções do
SDK identificadas em `config/analyzer-heuristic.toml` (sceSif*/sceCd*/scePad*/sceMc* etc., como o codwaw-ff faz em
`config/recomp.toml` — 326 stubs), regenerar e recompilar. Codex hoje 12h também viu Boku2 entrar no código do jogo e
pedir a função ausente `0x0020A920`; Black ganhou 31 continuações (380→349 destinos ausentes).

# Ben 10 Alien Force — continuação de 25/10/2026

**Último checkpoint:** EE AOT + runtime isolado + ps2EntryRunner.exe compilados; jogo não executado. Sem suplementos codwaw. Rasterizador OpenGL/interlock + scanout compute GPU obrigatórios compilados; IOP/VU ainda precisam migração nativa. Sem execução/compilação GLSL pelo driver. Ver analysis/native-integration.json.

## Estado confirmado

- ELF `SLUS_218.15`, SHA-256 `92004adcf85e877199be5b5ca017c640e95795017c0ca3c8d4ecbde52567cb61`; entry `0x00100008`. Metadados e seções em `analysis/elf-layout.json` (registro local `ben10-elf-layout-20261025`, não Evidence REA).
- Importação/análise Ghidra R5900 concluída e banco salvo em `ghidra/Ben10AF`; exportou **8.760 funções e 67.600 labels executáveis**. Os 76.360 registros do CSV não são uma contagem de funções confirmadas nem percentual de conclusão.
- `analysis/ghidra-export.log` registra sucesso de análise/importação/save e também erro `VarnodeContext: out of address spaces` em `0x0029c894` (`vf9`). Blocos DVP não byte-mapped foram ignorados. VU/renderização continuam sem tradução/validação estabelecida.
- Analisador PS2Recomp existente encontrou 10.189 funções heurísticas e 411 matches SDK (127 com handler, 284 entry points adicionais). São candidatos; não foram usados como substituições automáticas.
- `config/recomp.toml` é preparado a partir de `ghidra-functions-only.csv`: **8.760 funções do FunctionManager**, sem 67.600 labels sintéticos do mapa completo. `ExportBen10FunctionsOnly.java` exporta isso do banco salvo sem reanálise. Os labels continuam em `ghidra-functions.csv` como evidência.
- 48 wrappers foram classificados pelo exportador. O stub inicial sem evidência `SetGsCrt@0x00100000` foi removido; o exportador identifica wrapper em `0x002CE320`. Ver `analysis/config-evidence.json` (registro local `ben10-ghidra-map-20261025`, não Evidence REA).

## Pipeline e espaço

`generate.cmd` prepara config e usa o `ps2_recomp.exe` já existente em `../codwaw-ff/build/ps2recomp/ps2xRecomp/`; `PS2_RECOMP_EXE` permite escolher outro executável existente. Não baixa nem recompila ferramentas.

Geração com um worker/low-memory, limite observado de 700 MiB de fontes e reserva de 20 GiB livres. Se for interrompida, o diretório resultante é parcial. Builds verificam fontes/estimativa de espaço antes de configurar dependências e usam dois jobs por padrão. A estimativa prévia não garante o pico real de um build futuro.

ISO continua em `D:\ISO\Ben 10 - Alien Force (USA) (En,Fr,De,Es,It)\Ben 10 - Alien Force (USA) (En,Fr,De,Es,It).iso`, sem nova extração integral ou cópia.

## Validação e próximo passo

Ghidra export e analyzer executados. Sintaxe dos scripts Python verificada. Falhas de fontes ausentes em `make_chunks.py` e `check_build_space.py` confirmadas, evitando configure/build prematuros.

Geração AOT **concluída com exit 0**: 8.760 funções processadas, **8.712 recompiladas + 48 stubs**, zero skips/decode failures, 6.122 warnings e zero errors. Saída tem 8.763 arquivos (~265 MiB), agrupados em **275 chunks**. `analysis/codegen-summary.json` registra os totais; `analysis/recompile.log` contém os warnings.

O compilador gerou 493.331 entradas de retomada em 7.888 funções proprietárias por fallback de JR/JALR indiretos. São saídas conservadoras da tradução; não são chamadas observadas nem funções independentes confirmadas. A tentativa inicial com todos os labels foi interrompida e ficou em `project/runner-labels-partial` (~108 MiB), fora do build. A remoção desse diretório gerado, com caminho absoluto verificado, foi rejeitada pela aprovação automática (comando recursivo bloqueado); nenhum arquivo original foi removido. O projeto inteiro soma cerca de **436 MiB**, abaixo de 1 GiB de novos artefatos.

Marcador de geração completa/config SHA criado após verificar exit 0 e tabela de funções. Preflight de build passou sem configurar CMake nem baixar dependências. **Nenhum build nativo ou boot foi executado.** Nenhuma afirmação de imagem, áudio, controles ou saves funcionando foi estabelecida.

Antes do build: isolar o runtime compartilhado, cujo `ps2xRuntime/CMakeLists.txt` inclui `codwaw_*_chunk_*.cpp` incondicionalmente; esses suplementos pertencem a outro jogo e não devem entrar por acidente no Ben10. Revisar warnings/cobertura e planejar build que preserve a reserva de disco; então executar diagnóstico de boot e resolver bloqueadores reais sem stubs genéricos adivinhados.

## Isolamento e compilação nativa — continuação sem execução

O CMake compartilhado agora possui `PS2X_ENABLE_CODWAW_SUPPLEMENTS` (padrão ON preserva os projetos existentes); Ben10 força OFF. Includes privados, registros de funções e suplementos de overlays CoD foram isolados, além dos arquivos CMake. `compile_commands.json` confirma 275 chunks próprios e nenhuma unidade `codwaw_*`. `PS2X_FFMPEG_ROOT` permite usar o decoder FFmpeg já existente, sem novo download; raylib também reutiliza checkout local com rede desativada.

`project/CMakeLists.txt` e `build-native.cmd` fornecem integração própria. O alvo estático `ps2_runtime.lib` já compilou (~10,4 MiB). A biblioteca AOT e o executável foram compilados com código de saída 0 (`ps2_game_code.lib` e `ps2EntryRunner.exe`). Para viabilizar a integração dos grandes switches de retomada com custo previsível, a configuração inicial usa `/Od /Ob0 /fp:precise` só nos fontes gerados; `BEN10_OPTIMIZE_AOT=ON` permite `/O1 /Ob1` depois. O runtime permanece Release. A ausência de otimização nesse bootstrap afeta desempenho, não significa interpretação do EE.

**Ainda existem intérpretes de IOP/VU e frontend GS no runtime.** A tradução EE/arquivo nativo não permite anunciar 100% nativo. Não foi executado jogo nesta rodada. Guarda ativa: reserva de 20 GiB, teto observado de 2 GiB em `build/native/` e dois compiladores.

Resultado final desta continuação: `ps2EntryRunner.exe` ligado (325.321.216 bytes), `ps2_game_code.lib` e runtime próprios compilados, sem execução do jogo. O grande dispatch de `FUN_0028b290` foi dividido em 88 grupos, mantendo todos os 22.215 PCs/labels, o corpo traduzido e o caminho default. `scripts/bucket_dispatch.py` aplica/verifica a transformação antes de futuros builds; evidência em `analysis/bucketed-dispatch.json`. O diretório `build/native/` terminou em ~1,70 GiB, abaixo do teto observado de 2 GiB. Ainda há intérpretes IOP/VU e implementação GS de compatibilidade, portanto isto não é conclusão de port 100% nativo.

## GPU nativa — continuação sem execução

O código compartilhado já possuía rasterizador `GSGpuBackend` OpenGL4.6/interlock; o rótulo histórico “software GS” não descrevia toda a seleção de backend. Agora este projeto força `PS2X_REQUIRE_GPU_RASTERIZER=ON`: `PS2X_GS=cpu`, falha de criação do backend e renderizadores GL de software conhecidos impedem a inicialização; não há fallback silencioso. O padrão compartilhado continua OFF, preservando os demais projetos.

`PS2X_GPU_COMPUTE_SCANOUT=ON` adiciona scanout compute: detile VRAM, conversão CT16/CT16S/CT24/CT32, blend PMODE de dois CRTs, seleção de campo interlaçado e sondagem de presença para o workaround FBP0. Neste perfil, `Present()` não lê os4MiB da VRAM nem chama `GSCpuBackend::Present`; somente RGBA final e bits de presença voltam à CPU. A apresentação final ainda usa a interface raylib com cópia de pixels, e leituras explícitas local-to-host/debug continuam por helpers de shadow sincronizados.

Runtime e executável recompilados com exit0; nenhuma unidade EE AOT recompilada, nenhum jogo/contexto OpenGL foi executado. C++ compila, mas compilação GLSL pelo driver, saída gráfica e desempenho permanecem não observados. Não existe glslangValidator/glslc disponível nos toolchains locais consultados, e nenhuma ferramenta foi baixada. Ver `analysis/native-gpu-integration.json` com hashes de fontes/limites. IOP/VU ainda usam intérpretes; não declarar100% nativo.

## Reservas de espaço removidas — 26/10/2026

Por solicitação do usuário, os pipelines não exigem mais reservas mínimas de 20 GiB nem de 1 GiB para fontes/logs. As referências anteriores a essas reservas são históricas. Permanecem os limites de tamanho de artefatos e logs, as verificações de integridade e, nas extrações, a verificação de espaço para os próprios arquivos. A estimativa de build do Ben10 é apenas informativa.
