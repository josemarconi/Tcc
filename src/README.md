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
| Ore production | `fa_producao` | Average ore production in t/h across all ore fronts. Uses weight `inv` outside the limits, `qualn` between the minimum and the target, and `qualp` between the target and the maximum. |
| Grades | `fa_teor` | Grade of each quality variable outside the `cmin`/`cmax` limits, with the target at `crec`. |
| REM | `fa_rem` | Waste/ore ratio outside the range and the target. |
| Mechanical availability | `tempoUtilizadoCaminhao` | Feasibility constraint. A solution that exceeds any truck's available time is rejected. |
| Loader production | `fa_prodcarga` | Average production of each front in t/h, evaluated against the hourly limits of the assigned loader. |
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
| VNS-Random: VNS with random-order VND | `VNS` in `Uvns.cpp`, with `forma = 2` | Method 2 |
| AG-Random: genetic algorithm with random-order VND | `AlgoritmoGenetico` in `Uag.cpp` | Method 3 |
| ILS (several variants, named ILS-Referencia and ILS-Random) | `Uils.cpp`, `Hils.h` | Not called by `main.cpp`. Used only by the calibration drivers. |
| Simulated annealing | `Hsa.h` | Header only. There is no implementation (`Usa.cpp` does not exist) and nothing calls these functions. |

Two kinds of local search:

- **VND** (Variable Neighbourhood Descent) in `Uvnd.cpp`: goes through eight neighbourhoods in fixed order (VND-Referencia, `VND_Referencia`) or in random order (VND-Random, `VND_RLP`), and accepts only improvements.
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

The genetic algorithm (`Uag.cpp`) runs for 50 generations. It builds `tam_pop` parents by GRASP and produces `tam_pop` offspring in each generation. Tournament selection, crossover, repair and mutation produce offspring, and the best `tam_pop` individuals become the next population. The parameter `alpha` sets the GRASP candidate list size for both the initial population and the GRASP mutation. Method 3 in `main.cpp` uses random-order VND (VND-Random) and the GA defaults in `Hag.h`.

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
| `Hvnd.h`, `Uvnd.cpp` | VND, random-order VND (VND-Random) and the neighbourhoods that compose them. |
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
3. **Travel time**: computes the maximum number of trips of truck `c` to a single front as `floor(360 * dispmec[c] / cycle_time)`. The total cycle time of that truck across all active, compatible fronts must also fit within `360 * dispmec[c]`.
4. **Penalties**: for each quality parameter, the range `cmax - cmin` sets the penalty weight (`100 / (range + 0.001)`). If the range is almost zero, 5% of the recommended value is used.
5. **Seed**: `srand(run_number * 997 + mine_id * 31 + method * 101)`. Each combination of test, mine and method has a different, reproducible seed.
6. **Initial solution**: the VNS methods use one GRASP solution with `alpha = 0.3741`. The GA constructs its GRASP population internally.
7. **Improvement**: `VNS` (methods 1 and 2) or `AlgoritmoGenetico` (method 3).
8. **Final evaluation**: `favaliacao` on the final solution. The value is the recorded FO.
9. **Record**: appends a line `Mina;Fo;Tempo(s);Iter` to the method's CSV.

Mechanical availability is the fraction of the horizon during which a truck can operate. The instances have no mechanical downtime input, so the experiment and both irace drivers use 100% availability by default (`disponibilidade_mecanica_padrao` in `Hdefine.h`). At 85% availability, a truck has 306 time units in a horizon of 360. Any combination of trips that fits within those 306 units respects the availability constraint.

Before local search, plans exceeding the budget are repaired by removing the longest trips first. The genetic algorithm applies the same repair at crossover, and its production repair adds trips only while there is time available. The shared objective rejects moves that exceed the budget. The previous utilization penalty and its weights have no effect in the corrected objective. Existing result CSVs describe the earlier implementation.

The ore targets and loader limits in the instance files are hourly rates (t/h). Truck payloads and front inventories are in tonnes per trip and tonnes, respectively. The objective compares hourly limits with the tonnes transported divided by the horizon in hours (6 by default). Construction, mutation and production repair convert the hourly ore target to tonnes over the same horizon before assigning trips.

Methods and their output files:

| Method | Description | Output folder | File |
|---|---|---|---|
| 1 | GRASP + VNS with VND (fixed order) (VNS-Referencia) | `Relatorios_Finais/VNS-Referencia/` | `Resumo_Geral_Minas_VNS-Referencia_Teste_<j>.csv` |
| 2 | GRASP + VNS with random-order VND (VNS-Random) | `Relatorios_Finais/VNS-RLP/` | `Resumo_Geral_Minas_VNS-RLP_Teste_<j>.csv` |
| 3 | GRASP + GA with random-order VND (AG-Random) | `Relatorios_Finais/AG-RLP/` | `Resumo_Geral_Minas_AG-RLP_Teste_<j>.csv` |

The description column uses the method names in this documentation. The output columns give the paths and filenames used by the program.

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

After running the processing notebook (section 7), each folder contains a consolidated CSV named after that folder (`VNS-Referencia.csv`, `VNS-RLP.csv` or `AG-RLP.csv`), with one row per mine and the columns `fo_<n>` and `tempo_<n>` for each test.

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
| `Calibracao_Ag/` | AG-Random | `tam_pop` (12, 20, 32, passed as `--pop`) and `alpha` (0.05; 0.10; 0.15; 0.20, passed as `--alpha`) | 1, 72, 116, 290 |
| `Calibracao_VNS_ILS/` | ILS or VNS | `algoritmo` (ILS or VNS), `iteracoes` (800 to 1800), `nivel` (ILS), `k` (VNS) and `alpha` (0.1 to 0.5) | 1, 290, 72, 116 |

Each folder has:

- `main_irace.cpp` or `main_irace_2.cpp`: driver that runs one instance with the parameters received on the command line and prints the FO. It accepts `--instance`/`-i` and `--seed`/`-s`, and the parameters of the respective algorithm. In `main_irace.cpp`, the flag `--metodo` selects the local search of the GA: 11 means VND-Referencia (fixed-order VND, `forma_vnd = 1`) and 12 means VND-Random (random-order VND, `forma_vnd = 2`).
- `run_irace.R`: irace configuration and execution. The AG script compares all 12 pairs on the four training mines using a common seed per mine across the candidates. The VNS/ILS script has a budget of 200 experiments.
- `target-runner`: Bash wrapper for running a calibration case. The AG script calls its executable directly from R, including on Windows.
- Result files from earlier runs (`configuracoes.csv`, `irace.Rdata`, `all_configurations.csv`, `experiments_matrix.csv`, `irace-output.Rdata`).

To run the calibration as planned:

1. Build the calibration executable **in `src/`**, linking the chosen driver to the program modules, except `main.cpp`. For AG-Random, run the following command.

   ```bash
   cd D:/Faculdade_8/I_C_Mais_Atualizado/src
   g++ -O3 -std=c++11 -I. -o exec_irace_ag Calibracao_Ag/main_irace.cpp \
       Ufavaliacao.cpp Uils.cpp Umovimento.cpp Uutil.cpp Uutilcarga.cpp Uvnd.cpp Uvns.cpp Uag.cpp
   ```

   For ILS/VNS, use `Calibracao_VNS_ILS/main_irace_2.cpp` and name its executable `exec_irace`.

   On Windows, the AG executable is `exec_irace_ag.exe`.

2. Go into the calibration folder and run irace:

   ```bash
   cd Calibracao_Ag
   Rscript run_irace.R
   ```

   The AG script stores the irace log, elite configurations and per-run costs in `Calibracao_Ag/recalibracao/`.

The AG-Random calibration ran 48 evaluations. All 12 configurations obtained the same objective value on each training mine and a mean of 164,780. The experiment uses `tam_pop = 12`, the smallest tested population, with `alpha = 0.05` from the irace ranking.

---

## 7. Processing notebook (`Tratamento_Resultados/tratamentos.ipynb`)

The notebook consolidates the results in `Relatorios_Finais/`. It finds the method folders automatically (every subfolder of `Relatorios_Finais/`, in alphabetical order). In each folder it reads all the test files named `..._Teste_<n>.csv`, takes the test number from the file name, and writes `<folder name>.csv` inside that folder, with one row per mine:

```
Mina;fo_1;tempo_1;...;fo_N;tempo_N;iter
```

The notebook reads the report folders listed in section 3. The historical folders `GRASP_VND_NORMAL`, `GRASP_VND_ALEATORIO` and `AG_VND_ALEATORIO` are under `Relatorios/`.

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

## 8. Open issues and known divergences

The points below were observed while reading the code and were not corrected, because changing the logic alters the results. They must be decided by the author of the project.

### 8.1 Data (see also [`../dados/README.md`](../dados/README.md), section 5)

1. **Order of the fields in `prod_N.DAT`**: the notebook writes `Meta,Minimo,Maximo` (target, minimum, maximum) for discharge 1/1, but the C++ code reads `prodmin,prodrec,prodmax`. The report `Solucoes/Solucao_Mina_1.txt` shows `[1600 - 2600 - 3100]`.
2. **Value 2600 for mines above 24** in the same field, contrary to the notebook comment about the target of 1600.
3. **Discharge 1/3**: the notebook writes three lines (one per crusher), but the C++ code reads only the second line, which corresponds to `BRIT_0` (900/1100/1300 t/h), and not the total (2700/3300/3900 t/h).
4. **Fixed REM** at `0.25; 0.31; 0.4` in the notebook, without reading `teores_producao.csv`.
5. **`capcam_min_N.DAT`** is generated, but not read.

### 8.2 Build and scripts

6. `exec.sh` tests for the existence of `./executo`, but the binary generated by the `Makefile` is called `./exec`. Therefore `make` always runs, even when the executable already exists.
7. A comment in `main.cpp` says "Tests from 1 to 5", but the loop goes from 1 to 20.
8. The `Makefile` has no dependencies on the headers (see section 4.3).

### 8.3 Calibration

9. The calibration executables are built separately as described in section 6.
10. `main_irace_2.cpp` uses the default `wf*` weights.

### 8.4 C++ code

The items below are observations from reading the code, not tests. None was reproduced by running the program.

12. **Index `-1` for a loader**: in `Umovimento.cpp`, some functions use `compatibilidade[c][carga[f].equip]` without checking whether `equip` is `disponivel` (-1). `movimentoRelocateTripPit` has no check of `op`.
13. **`movimentoLoad`**: the comment says it validates compatibility with the new loader, but the function only limits the trips to `nviagem`.
14. **GA time repair** (`Uag.cpp`): the repair counts only fronts with a loader, but the removal chooses any front. If no trips remain and the time stays above the limit, the loop may never end.
15. **Directed mutation of the GA**: move 5 is never chosen.
16. **Random-order VND (`VND_RLP`, VND-Random)** (`Uvnd.cpp`): the comment says the order is weighted by weights, but the weights do not affect the order, which is uniform.
17. **Neighbourhood numbering**: `VND_Referencia` (VND-Referencia) and `VND_RLP` (VND-Random) use different numberings for the same moves.
18. **Simulated annealing**: `Hsa.h` has prototypes without implementation and without include guards. It does not include `hstruct.h`, but it uses `struct alocarga`.
19. **`learqmatriz`, `fopen` without checks**: some readers and writers in `Uutil.cpp` call `fprintf` or `feof` even when `fopen` returns `NULL`. The `while (!feof(...))` readers do not check the file content.
20. **Functions without callers**: several functions in `Uutilcarga.cpp` (for example `constroisolucaoinicialvisitas02`, `constroisolucaogulosa` and `imprimerelatorio`) are not called by `main.cpp`. They are code from earlier versions.

---

## 9. Code conventions

- Variable and function names are in Portuguese (for example `constroisolucaoGRASP`, `prodtotal`). The comments added in this review are in English.
- The original Portuguese comments were kept, without changes.
- The new comments describe what the code does. Open issues are not in the comments; they are in this document.
- Check performed when the comments were added: the 9 objects of `src/*.cpp` and the 2 calibration drivers were rebuilt with the same flags and are byte-identical to the objects of the previous version. The abstract syntax tree (AST) of the code cells of the notebooks was compared before and after, and is identical. In other words, the change was only in comments.
