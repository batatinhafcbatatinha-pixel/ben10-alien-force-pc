# Ben 10: Alien Force — Port Nativo PC (PlayStation 2 Recomp)

## Informações do Jogo e Mídia Original

- **Origem:** `D:\ISO\Ben 10 - Alien Force (USA) (En,Fr,De,Es,It)\Ben 10 - Alien Force (USA) (En,Fr,De,Es,It).iso`
- **Tamanho da ISO:** 2.351.071.232 bytes (2,243 MiB)
- **Plataforma:** PlayStation 2 (Emotion Engine MIPS R5900 + Vector Units VU0/VU1)
- **ID do Título:** `SLUS_218.15`
- **Versão:** 1.01 (NTSC)
- **Publicadora / Desenvolvedora:** D3Publisher / Monkey Bar Games

---

## Estrutura do Executável Principal (`SLUS_218.15`)

- **Caminho:** `disc-root/SLUS_218.15` e `analysis/disc/SLUS_218.15`
- **Tamanho:** 3.890.444 bytes
- **SHA-256:** `92004adcf85e877199be5b5ca017c640e95795017c0ca3c8d4ecbde52567cb61`
- **Ponto de Entrada (Entry Point):** `0x00100008`
- **Seções Principais:**
  - `.text`: `0x00100000` — `0x0045F4A0` (3.536.024 bytes de código MIPS)
  - `.vutext`: `0x0045F4A0` (81.968 bytes de código VU)
  - `.DVP.ovlytab` / `.DVP.overlay.*`: microprogramas de renderização e animação VU1
  - `.data`: `0x00473500` (136.704 bytes)
  - `.rodata`: `0x00494B00` (39.232 bytes)
  - `.bss`: `0x0049E580` (625.972 bytes)

---

## Módulos IOP Extraídos (`disc-root/MODULES`)

- `CDVDSTM.IRX` (33.521 bytes) — Driver de streaming de CD/DVD
- `IOMGR.IRX` (27.832 bytes) — Gerenciador de I/O
- `IOPRP310.IMG` (278.353 bytes) — Imagem do Sistema IOP (Sony SDK 3.1.0)
- `LIBSD.IRX` (30.085 bytes) — Biblioteca de áudio Sound Driver
- `MCMAN.IRX` (96.181 bytes) — Gerenciador de Memory Card
- `MCSERV.IRX` (7.385 bytes) — Servidor RPC de Memory Card
- `PADMAN.IRX` (45.925 bytes) — Gerenciador de Controles (DualShock 2)
- `SDRDRV.IRX` (9.161 bytes) — Driver de reprodução de som
- `SIO2MAN.IRX` (6.641 bytes) — Gerenciador de barramento serial SIO2

---

## Cadeia de Ferramentas e Recompilação

O projeto utiliza o pipeline baseado em `PS2Recomp` (`third_party/PS2Recomp`):

1. **Exportação de Funções com Ghidra Headless:**
   Execute `ghidra-export.cmd` para importar o binário `SLUS_218.15` no Ghidra com a extensão R5900 e gerar a lista de símbolos em `config/ghidra-functions.csv`.
2. **Tradução Estática (AOT):**
   Execute `generate.cmd`. O script prepara a configuração com o mapa deste jogo e usa o recompilador existente em `codwaw-ff/build/ps2recomp/ps2xRecomp/ps2_recomp.exe`, sem recompilar ferramentas. Para usar outro executável existente, defina `PS2_RECOMP_EXE`.
   A geração exige 20 GiB livres e interrompe ao detectar 700 MiB de fontes ou menos de 20 GiB livres. Resultados parciais não devem ser compilados. Matches heurísticos do SDK ficam separados em `analysis/config-evidence.json` para revisão; não são substituições confirmadas.
3. **Agrupamento de Fontes em Chunks:**
   Execute `python scripts/make_chunks.py` para agrupar as funções geradas em lotes compactos dentro de `project/runner-chunks/`.
4. **Compilação do Executável Nativo:**
   Execute `build-game.cmd full` para compilar o executável `ps2EntryRunner.exe`.
   O build verifica fontes e uma reserva conservadora de espaço antes de configurar dependências, e usa dois jobs por padrão (`JOBS` permite ajuste). Esta continuação não executa build nativo grande.
5. **Execução:**
   Execute `run-game.cmd` ou qualquer um dos atalhos de janela (`jogar-janela.cmd`, `jogar-tela-cheia.cmd`, `jogar-sem-bordas.cmd`).

---

## Arquivos e Diretórios Base do Port

- `disc-root/` — Arquivos extraídos da mídia original (módulos, executável e metadados).
- `analysis/` — Registros estáticos de análise de binário e inventário da mídia.
- `config/` — Configurações de recompilação (`recomp.toml`), stubs e tabelas de símbolos.
- `project/` — Diretórios de destino para o código retransladado (`runner` e `runner-chunks`).
- `scripts/` — Utilitários de agrupamento e suporte ao build.
- `CONTROLES.md` — Mapeamento completo de botões para gamepad e teclado.

O estado validado mais recente e as limitações ficam em [CONTINUACAO.md](CONTINUACAO.md). Geração AOT concluída: 8.712 funções recompiladas, 48 stubs, 275 chunks; sem build/boot. Antes de compilar, é necessário isolar os suplementos `codwaw_*` do runtime compartilhado. Os comandos de execução e controles ainda dependem de build e validação do jogo.

## Isolamento e compilação nativa — continuação sem execução

O CMake compartilhado agora possui `PS2X_ENABLE_CODWAW_SUPPLEMENTS` (padrão ON preserva os projetos existentes); Ben10 força OFF. Includes privados, registros de funções e suplementos de overlays CoD foram isolados, além dos arquivos CMake. `compile_commands.json` confirma 275 chunks próprios e nenhuma unidade `codwaw_*`. `PS2X_FFMPEG_ROOT` permite usar o decoder FFmpeg já existente, sem novo download; raylib também reutiliza checkout local com rede desativada.

`project/CMakeLists.txt` e `build-native.cmd` fornecem integração própria. O alvo estático `ps2_runtime.lib` já compilou (~10,4 MiB). A biblioteca AOT e o executável foram compilados com código de saída 0 (`ps2_game_code.lib` e `ps2EntryRunner.exe`). Para viabilizar a integração dos grandes switches de retomada com custo previsível, a configuração inicial usa `/Od /Ob0 /fp:precise` só nos fontes gerados; `BEN10_OPTIMIZE_AOT=ON` permite `/O1 /Ob1` depois. O runtime permanece Release. A ausência de otimização nesse bootstrap afeta desempenho, não significa interpretação do EE.

**Ainda existem intérpretes de IOP/VU e frontend GS no runtime.** A tradução EE/arquivo nativo não permite anunciar 100% nativo. Não foi executado jogo nesta rodada. Guarda ativa: reserva de 20 GiB, teto observado de 2 GiB em `build/native/` e dois compiladores.

Resultado final desta continuação: `ps2EntryRunner.exe` ligado (325.321.216 bytes), `ps2_game_code.lib` e runtime próprios compilados, sem execução do jogo. O grande dispatch de `FUN_0028b290` foi dividido em 88 grupos, mantendo todos os 22.215 PCs/labels, o corpo traduzido e o caminho default. `scripts/bucket_dispatch.py` aplica/verifica a transformação antes de futuros builds; evidência em `analysis/bucketed-dispatch.json`. O diretório `build/native/` terminou em ~1,70 GiB, abaixo do teto observado de 2 GiB. Ainda há intérpretes IOP/VU e implementação GS de compatibilidade, portanto isto não é conclusão de port 100% nativo.

## GPU nativa — continuação sem execução

O código compartilhado já possuía rasterizador `GSGpuBackend` OpenGL4.6/interlock; o rótulo histórico “software GS” não descrevia toda a seleção de backend. Agora este projeto força `PS2X_REQUIRE_GPU_RASTERIZER=ON`: `PS2X_GS=cpu`, falha de criação do backend e renderizadores GL de software conhecidos impedem a inicialização; não há fallback silencioso. O padrão compartilhado continua OFF, preservando os demais projetos.

`PS2X_GPU_COMPUTE_SCANOUT=ON` adiciona scanout compute: detile VRAM, conversão CT16/CT16S/CT24/CT32, blend PMODE de dois CRTs, seleção de campo interlaçado e sondagem de presença para o workaround FBP0. Neste perfil, `Present()` não lê os4MiB da VRAM nem chama `GSCpuBackend::Present`; somente RGBA final e bits de presença voltam à CPU. A apresentação final ainda usa a interface raylib com cópia de pixels, e leituras explícitas local-to-host/debug continuam por helpers de shadow sincronizados.

Runtime e executável recompilados com exit0; nenhuma unidade EE AOT recompilada, nenhum jogo/contexto OpenGL foi executado. C++ compila, mas compilação GLSL pelo driver, saída gráfica e desempenho permanecem não observados. Não existe glslangValidator/glslc disponível nos toolchains locais consultados, e nenhuma ferramenta foi baixada. Ver `analysis/native-gpu-integration.json` com hashes de fontes/limites. IOP/VU ainda usam intérpretes; não declarar100% nativo.
