# `src` folder

This folder contains the C++ program that solves the dynamic allocation of trucks and loaders for each of the 290 mines, together with the calibration drivers, the build script and the notebook that processes the results.

The program inputs come from [`../dados`](../dados/README.md). This document describes: the implemented methods, the modules and functions, correct execution, the output formats and the known open issues.

---

## 1. Overview of the problem and the methods

For each mine the program searches for a solution that defines:

- how many trips each truck makes to each ore or waste front (pile);
- which loader operates each front, and with what production;
- whether the front is operating (flag `op`).

The solution is evaluated by an **objective function (FO) that is minimized**. The FO is a sum of weighted penalties, computed in `Ufavaliacao.cpp` (function `favaliacao`). The terms are:

| Term | Function | What it penalizes |
|---|---|---|
| Ore production | `fa_producao` | Total ore production (sum of the ore fronts) outside the minimum/target/maximum range. The range has four regions: below the minimum (weight `inv`), between minimum and target (`qualn`), between target and maximum (`qualp`), and above the maximum (`inv`). |
| Grades | `fa_teor` | Grade of each quality variable outside the `cmin`/`cmax` limits, with the target at `crec`. |
| REM | `fa_rem` | Waste/ore ratio outside the range and the target. |
| Truck utilization | `fa_produtcam` | Production of each used truck outside the utilization range. Unused trucks are not penalized. |
| Loader production | `fa_prodcarga` | Production of each front outside the `cargamin` and `cargamax` limits of the loader that serves it. |
| Number of trucks used | `fa_numcam` | Number of trucks with at least one trip, multiplied by `wfnumcamutilizados`. |
| Static truck term | `fa_estatico` | Multiplied by `estdin`. |
| Excess trucks per front | `fa_numcamfrente` | Multiplied by `estdin`. |

Note: `estdin` is defined as `0` in `Hdefine.h`. Therefore `fa_estatico` and `fa_numcamfrente` are zero in the current configuration, and the weights `wfestatico` and `wfnumcam` have no effect on the FO. To enable these two terms, `estdin` would have to be `1`.

The weights (`wf*`) are defined in `main.cpp`. The main ones are: production outside the range (`wfproducaoinv = 1000000`), grade (`wfteor = 500000`), REM outside the range (`wfrem = 100000`), and the others range from 1 to 1000 (except the two terms zeroed by `estdin`, which have 10000).

Implemented methods:

| Method | Where it is | Use in the experiment |
|---|---|---|
| GRASP (initial solution) | `constroisolucaoGRASP` and `constroisolucaoGRASP_Parcial` in `Uutilcarga.cpp` | Used by all methods. Builds the solution with a restricted candidate list (parameter `alpha`). |
| VNS-Referencia: VNS with fixed-order VND | `VNS` in `Uvns.cpp`, with `forma = 1` | Method 1 |
| VNS-RLP: VNS with random-order VND | `VNS` in `Uvns.cpp`, with `forma = 2` | Method 2 |
| AG-RLP: genetic algorithm with random-order VND | `AlgoritmoGenetico` in `Uag.cpp` | Method 3 |
| ILS (several variants, named ILS-Referencia and ILS-RLP) | `Uils.cpp`, `Hils.h` | Not called by `main.cpp`. Used only by the calibration drivers. |
| Simulated annealing | `Hsa.h` | Header only. There is no implementation (`Usa.cpp` does not exist) and nothing calls these functions. |

Two kinds of local search:

- **VND** (Variable Neighbourhood Descent) in `Uvnd.cpp`: goes through eight neighbourhoods in fixed order (VND-Referencia, code `VND`) or in random order (VND-RLP, code `VND_Aleatorio`), and accepts only improvements.
- **VNS** in `Uvns.cpp`: at each iteration applies a perturbation (*shaking*) of strength `k` (from 1 to 4), then the local search chosen by `forma`. It keeps the best solution. The number of outer iterations is fixed at 1800.

The eight neighbourhoods are implemented in `Umovimento.cpp`. The acronyms below are an interpretation of the function names:

| Acronym | Function | Move |
|---|---|---|
| NT | `movimentoNumberTrips` | Changes the number of trips of a truck to a front. |
| L | `movimentoLoad` | Adjusts the load (trips) between trucks and fronts. |
| TT | `movimentoRelocateTripTruck` | Moves a trip to another truck. |
| TP | `movimentoRelocateTripPit` | Moves a trip to another front. |
| PO | `movimentoPitOperation` | Turns the operation of a front on or off. |
| TO | `movimentoTruckOperation` | Turns the operation of a truck on a front on or off. |
| ST | `movimentoSwapTrips` | Swaps trips between two trucks. |
| SS | `movimentoSwapShovels` | Swaps the loaders between two fronts. |

The genetic algorithm (`Uag.cpp`) uses a population of 32 individuals (`tam_pop`) and 50 generations (`max_geracoes`), both fixed in the code. The operators are: tournament selection (`torneio`), crossover of load and trips with truck-time repair (`crossover_carga_viagens`), fast production repair (`reparar_producao_rapido`), directed mutation (`mutacao_dirigida`) and GRASP mutation on ore (`mutacao_GRASP_minerio`). The local search inside the GA is VND (`forma_vnd = 1`) or random-order VND (any other value). In method 3 of `main.cpp`, random-order VND (VND-RLP) is used.

---

## 2. File map

### 2.1 Source code and headers

| File | Role |
|---|---|
| `main.cpp` | Main program (experiment driver). See section 3. |
| `Hdefine.h` | Comparison macros with tolerance (`EQ`, `LE`, `GE`, `LT`, `GT`), `MAX`, `MIN`, `SWAP`, and the constants `disponivel = -1`, `compativel = 1`, `estdin = 0`. |
| `hstruct.h` | `struct alocarga { int equip; bool op; }`: loader assigned to each front and operation flag. |
| `Hutil.h`, `Uutil.cpp` | Allocation and freeing of vectors and matrices, readers of the `.DAT` files, writing of solutions and auxiliary reports. |
| `Hutilcarga.h`, `Uutilcarga.cpp` | Construction of initial solutions (random, greedy, GRASP), production and time helpers, and writing of the solution report (`imprimerelatorio`). |
| `Hfavaliacao.h`, `Ufavaliacao.cpp` | Objective function and its terms. |
| `Hmovimento.h`, `Umovimento.cpp` | The eight neighbourhood moves. |
| `Hvnd.h`, `Uvnd.cpp` | VND, random-order VND (VND-RLP) and the neighbourhoods that compose them. |
| `Hvns.h`, `Uvns.cpp` | VNS and its parameterized version. |
| `Hils.h`, `Uils.cpp` | ILS variants and GRASP-ILS. |
| `Hag.h`, `Uag.cpp` | Genetic algorithm, `Individuo` struct and operators. |
| `Hsa.h` | Simulated annealing header (no implementation). |

Each `U*.cpp` module has an `H*.h` header with the corresponding prototypes. The headers `Hvnd.h` and `Hsa.h` have no include guards (`#ifndef`), so they must not be included more than once in the same file.

### 2.2 Build, execution and output folders

| Item | Role |
|---|---|
| `Makefile` | Compiles all `*.cpp` files in this folder into `exec`. Targets: `all` (default), `dirs`, `run`, `clean`. |
| `exec.sh` | Run script: creates the output folders, compiles if needed and runs `./exec`. |
| `exec` | Executable that came with the repository. It is an ELF binary (Linux), which does not run on Windows. Rebuild with `make` in your environment before using it. |
| `*.o` | Object files. They are also ELF (Linux) and must be removed with `make clean` before rebuilding. |
| `Relatorios_Finais/` | Output of the current program: one folder per method, with one CSV per test. See section 5. The folders still have the old names (see section 3). |
| `Solucoes/` | 290 `Solucao_Mina_N.txt` files with the report of each solution. Generated by an earlier version of the program, in non-UTF-8 encoding. The current program does not write this folder. |
| `Relatorios/` | Legacy results of earlier tests, kept with their old names and organized by combination of methods (`GRASP_VND_NORMAL`, `GRASP_ILS_*`, `GRASP_SILS_*`, `AG_VND_*`). Read by the processing notebook. |
| `Calibracao_Ag/` | GA calibration with irace. See section 6. |
| `Calibracao_VNS_ILS/` | ILS and VNS calibration with irace. See section 6. |
| `Tratamento_Resultados/tratamentos.ipynb` | Notebook that consolidates the test CSV files. See section 7. |

---

## 3. Main program flow (`main.cpp`)

The program limits are set for a full experiment:

- 20 tests (`j` from 1 to 20);
- 3 methods per test (`metodo` from 1 to 3);
- 290 mines per method (`id_mina` from 1 to 290).

Configured total: 20 x 3 x 290 = **17,400 runs**. In the result set currently present in `Relatorios_Finais/`, there are eight complete test files per method (tests 1 to 8), totaling **6,960 recorded runs**. This is the result subset used in the thesis; tests 9 to 20 are not available in that folder.

For each mine, in this order:

1. **Dimensions**: counts the lines of `quant_N.DAT` (fronts), `capcam_est_N.DAT` (trucks), `cargamax_N.DAT` (loaders) and `cmax_N.DAT` (quality parameters). Reads the number of ore fronts from `../dados/minas.txt` (function `descobrir_npminerio`).
2. **Reading**: loads all `.DAT` files of the mine (formats in [`../dados/README.md`](../dados/README.md), section 4). The production (`prod_N.DAT`) is read by the function `ler_arquivo_producao`.
3. **Travel time**: computes the maximum number of trips of each truck to each front as `floor(360 / cycle_time)`. The value `360` is fixed in the code (`tcontrole`).
4. **Penalties**: for each quality parameter, the range `cmax - cmin` sets the penalty weight (`100 / (range + 0.001)`). If the range is almost zero, 5% of the recommended value is used.
5. **Seed**: `srand(run_number * 997 + mine_id * 31 + method * 101)`. Each combination of test, mine and method has a different, reproducible seed.
6. **Initial solution**: `constroisolucaoGRASP` with `alpha = 0.20` for the GA and `alpha = 0.3741` for the VNS methods.
7. **Improvement**: `VNS` (methods 1 and 2) or `AlgoritmoGenetico` (method 3).
8. **Final evaluation**: `favaliacao` on the final solution. The value is the recorded FO.
9. **Record**: appends a line `Mina;Fo;Tempo(s);Iter` to the method's CSV.

Methods and their output files:

| Method | Description | Output folder | File |
|---|---|---|---|
| 1 | GRASP + VNS with VND (fixed order) (VNS-Referencia) | `Relatorios_Finais/VNS-Referencia/` | `Resumo_Geral_Minas_VNS-Referencia_Teste_<j>.csv` |
| 2 | GRASP + VNS with random-order VND (VNS-RLP) | `Relatorios_Finais/VNS-RLP/` | `Resumo_Geral_Minas_VNS-RLP_Teste_<j>.csv` |
| 3 | GRASP + GA with random-order VND (AG-RLP) | `Relatorios_Finais/AG-RLP/` | `Resumo_Geral_Minas_AG-RLP_Teste_<j>.csv` |

Note on names: the code, the output folders and the output files now use the thesis names above. Earlier results on disk were renamed to match (see the output section).

The program does not write the `Solucoes/` folder. The `Iter` field of each line is the number of outer iterations of the VNS (1800) or of the GA (50), and `Tempo(s)` is the wall-clock time of the search, without the data reading.

---

## 4. How to build and run correctly

### 4.1 Prerequisites

- `g++` with C++11 support. Tested with g++ 15.2 (MinGW-w64, UCRT).
- `make` (GNU make).
- A POSIX shell for `exec.sh` and for the `Makefile` rules that use `mkdir -p` and `rm -f`. On Windows, use Git Bash or MSYS2.
- The folder `../dados/Minas/` must be complete. Generate it first with the notebook (see [`../dados/README.md`](../dados/README.md)).

### 4.2 Essential rule: run from `src/`

The program uses relative paths, such as `../dados/minas.txt` and `Relatorios_Finais/...`. It **must** be run with `src/` as the current directory. If it is run from another folder, it fails with `Erro ao abrir ...` (cannot open ...) or cannot find the files.

```bash
cd D:/Faculdade_8/I_C_Mais_Atualizado/src
```

### 4.3 Build

```bash
make            # compiles the .cpp files in this folder and generates ./exec
```

The `Makefile` compiles only the `*.cpp` files of this folder (the wildcard does not enter the `Calibracao_*` subfolders). The flags are `-O3 -Wall -std=c++11`, with some warnings suppressed.

Before the first build on Windows, rebuild from scratch. The objects and `exec` that are in the repository are Linux binaries (ELF) and cannot be reused:

```bash
make clean
make
```

Note: the `Makefile` declares no dependencies on the `.h` headers. After changing a `.h`, run `make clean` before `make`, so that outdated objects are not used.

### 4.4 Run

Option A, with the script (creates the output folders and runs):

```bash
./exec.sh
```

Option B, with the `Makefile` target (creates the output folders, builds and runs):

```bash
make run
```

Option C, running directly after building (the folder names are the ones the code currently uses, see the note in section 3):

```bash
mkdir -p Relatorios_Finais/VNS-Referencia Relatorios_Finais/VNS-RLP Relatorios_Finais/AG-RLP Solucoes
./exec
```

The `Relatorios_Finais/` folders must exist before running. If they do not, the program prints `Erro: Certifique-se de que a estrutura de pastas 'Relatorios_Finais/...' existe.` (Error: make sure the folder structure 'Relatorios_Finais/...' exists; printed in Portuguese) and exits with code 1.

Run time: the configured experiment has 17,400 runs and may take many hours. The current copy has results for tests 1 to 8, with 290 mines per test and method. For example, in the test 1 file of the fixed-order method (`GRASP_VND_NORMAL` on disk), mine 1 recorded 5.4211 s; this is an individual observation, not the average time of the thesis. To test a subset, temporarily change the loop limits in `main.cpp` (`j <= 20`, `metodo <= 3` and `id_mina <= 290`).

### 4.5 Clean up

```bash
make clean      # removes *.o and exec
```

---

## 5. Outputs and how to read them

### 5.1 Summary CSV (`Relatorios_Finais/`)

Each file has the header `Mina;Fo;Tempo(s);Iter` and one line per processed mine:

```
Mina;Fo;Tempo(s);Iter
1;57907;5.4211;1800
2;29957;2.2587;1800
```

- `Fo`: final value of the objective function (integer). The lower, the better.
- `Tempo(s)`: time of the search in seconds.
- `Iter`: outer iterations of the VNS (1800) or of the GA (50).

Current state: each method folder has 8 complete tests (290 mines each). The plan is 20 tests, so tests 9 to 20 still need to be run.

After running the processing notebook (section 7), each folder also contains a consolidated CSV, `<method>.csv` (`VNS-Referencia.csv`, `VNS-RLP.csv` and `AG-RLP.csv` once the folders have the new names), with one row per mine and the columns `fo_<n>` and `tempo_<n>` of each test.

### 5.2 Solution report (`Solucoes/`)

Text file, with the method used, the time, the FO and the ore production in the format `[prodmin - prodrec - prodmax]`. Example for mine 1:

```
Producao de Minerio  1952.00  [  1600.0 -  2600.0 -  3100.0]
```

(The line label "Producao de Minerio" is printed in Portuguese, meaning ore production.)

This format shows the order of the production values, and it is the evidence for divergence 1 in section 8.

### 5.3 Legacy `Relatorios/` folders

Legacy results of earlier runs, kept with their old names (for example `GRASP_VND_NORMAL`, `GRASP_ILS_*`, `GRASP_SILS_*`, `AG_VND_*`). They are organized by combination of methods (GRASP, ILS, SILS, VND, AG). Each folder contains:

- ten individual CSV files, one per run, in the format `Mina;Fo;Tempo(s);Iter` (decimal point). In these old runs the `Iter` column is 100;
- a consolidated CSV named after the folder (for example `GRASP_VND_NORMAL.csv`), generated by the processing notebook, in the format `mina;fo_1;tempo_1;...;iter` (decimal comma).

The current program does not write to these folders.

---

## 6. Calibration with irace

There are two calibration folders. Both use the `irace` package of R to tune parameters, running the C++ program on one instance at a time.

| Folder | Calibrated algorithm | Parameters (`parameters*.txt`) | Instances (`instances*.txt`) |
|---|---|---|---|
| `Calibracao_Ag/` | GA | `tam_pop` (values 12, 20, 32, passed as `--pop`) and `alpha` (0.05; 0.10; 0.15; 0.20) | 1, 72, 116, 290 |
| `Calibracao_VNS_ILS/` | ILS or VNS | `algoritmo` (ILS or VNS), `iteracoes` (800 to 1800), `nivel` (ILS), `k` (VNS) and `alpha` (0.1 to 0.5) | 1, 290, 72, 116 |

Each folder has:

- `main_irace.cpp` or `main_irace_2.cpp`: driver that runs one instance with the parameters received on the command line and prints the FO. It accepts `--instance`/`-i` and `--seed`/`-s`, and the parameters of the respective algorithm. In `main_irace.cpp`, the flag `--metodo` selects the local search of the GA: 11 means VND-Referencia (fixed-order VND, `forma_vnd = 1`) and 12 means VND-RLP (random-order VND, `forma_vnd = 2`).
- `run_irace.R`: irace configuration and execution (`maxExperiments` of 1000 for the GA and of 200 for VNS/ILS).
- `target-runner`: bash script that irace calls for each configuration. It converts the irace arguments into a call `./exec_irace --instance ... --seed ... <parameters>` and returns the FO value.
- Result files from earlier runs (`configuracoes.csv`, `irace.Rdata`, `all_configurations.csv`, `experiments_matrix.csv`, `irace-output.Rdata`).

To run the calibration as planned:

1. Build the `exec_irace` executable **in `src/`**, linking the chosen driver to the program modules, except `main.cpp`. Example for the GA (tested, no errors):

   ```bash
   cd D:/Faculdade_8/I_C_Mais_Atualizado/src
   g++ -O3 -std=c++11 -I. -o exec_irace Calibracao_Ag/main_irace.cpp \
       Ufavaliacao.cpp Uils.cpp Umovimento.cpp Uutil.cpp Uutilcarga.cpp Uvnd.cpp Uvns.cpp Uag.cpp
   ```

   For ILS/VNS, replace `Calibracao_Ag/main_irace.cpp` with `Calibracao_VNS_ILS/main_irace_2.cpp`.

   On Windows, `g++` generates `exec_irace.exe`. The `target-runner` is a bash script, and Git Bash resolves `./exec_irace` to `exec_irace.exe` automatically.

2. Go into the calibration folder and run irace:

   ```bash
   cd Calibracao_Ag
   Rscript run_irace.R
   ```

   The `target-runner` does `cd` into the parent folder (`src/`), where `exec_irace` is located.

Known limitations (see section 8):

- The `target-runner` of `Calibracao_Ag/` still mentions `Codigo_novo`, the name of an earlier organization. This affects only the comment; the `cd` uses the parent folder, which is `src/`.
- `exec_irace` is not generated by the `Makefile`. It must be built manually, as described above.
- The `parameters.txt` of `Calibracao_Ag` passes `--pop`, but the driver `main_irace.cpp` does not recognize this flag. Therefore `tam_pop` has no effect: the GA always uses 32 individuals (fixed value in `Uag.cpp`).

---

## 7. Processing notebook (`Tratamento_Resultados/tratamentos.ipynb`)

The notebook consolidates the results in `Relatorios_Finais/`. It finds the method folders automatically (every subfolder of `Relatorios_Finais/`, in alphabetical order). In each folder it reads all the test files named `..._Teste_<n>.csv`, takes the test number from the file name, and writes `<folder name>.csv` inside that folder, with one row per mine:

```
Mina;fo_1;tempo_1;...;fo_N;tempo_N;iter
```

Results from the first runs were stored in folders with the old names (`GRASP_VND_NORMAL`, `GRASP_VND_ALEATORIO`, `AG_VND_ALEATORIO`); they were renamed to `VNS-Referencia`, `VNS-RLP` and `AG-RLP`, with their files, to match the code.

- `N` is the number of test files found in each folder (currently 8).
- Times use a comma as decimal separator, and the column separator is `;`.
- A mine that does not appear in some test gets an empty cell. For each method, the notebook reports how many Fo values are missing.
- The `iter` column comes from the first run in which the mine appears.

How to run:

1. Open the notebook inside `D:/Faculdade_8/I_C_Mais_Atualizado/src/Tratamento_Resultados/`. The `Relatorios_Finais/` folder is searched from that directory first as `../Relatorios_Finais`, then as `Relatorios_Finais` (when run from `src/`), and then as `src/Relatorios_Finais` (when run from the repository root).
2. Run the cells in order (`Restart & Run All`).
3. A consolidated CSV is written to each `Relatorios_Finais/<folder>/` (one per method folder; three at present). If the notebook is run again, these files are overwritten.

Requirements: `pandas` (which depends on `python-dateutil`).

Verification performed when the notebook was updated: the Fo and time values of each test were compared with the raw files, with 6,960 cells checked (8 tests x 290 mines x 3 methods), with no divergences.

To process another folder, such as `Relatorios/` (earlier runs, with names `..._<n>.csv`, without `_Teste_`), the `CANDIDATAS` list and the `PADRAO_TESTE` pattern in the first code cell must be changed.

---
