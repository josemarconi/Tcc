# Final Course Project: A Metaheuristic Approach and Benchmarking for the Mine Planning Problem.

Code and data for the research project (Iniciacao Cientifica / TCC) on the dynamic allocation of trucks and loaders in open-pit mines. The computational goal is to build and compare trip plans and front operation plans for 290 synthetic instances, according to an evaluation function based on penalties for production, quality and the waste/ore ratio (REM, from the Portuguese "relacao esteril/minerio"). The value of the evaluation function is not, by itself, a measure of financial cost, nor a certification of operational feasibility.

This README describes the project as a whole. The details of each part are in:

- [`dados/README.md`](dados/README.md): input files, preparation script, format of the generated `.DAT` files and compatibility check with the C++ code.
- [`src/README.md`](src/README.md): C++ code, implemented methods, build, run, outputs, calibration with irace and open issues.

---

## 1. Problem

Each mine is an instance defined in `dados/minas.txt`. It specifies:

- the number of trucks (30, 40 or 50) and loaders (3, 6 or 9);
- the number of ore and waste fronts (for example `3/1`, `12/5` or `32/10`);
- the number of quality parameters controlled (4 or 10);
- the discharge type (`1/1`: one ore destination; `1/3`: three ore crushers);
- the fleet type (homogeneous, heterogeneous, or homogeneous with 120/130 t trucks);
- the cycle time (short, medium or long).

A solution defines how many trips each truck makes to each front and which loaders operate each front. It is evaluated by an objective function (FO) that sums penalties and must be minimized. The penalties cover ore production, quality grades, REM, truck and loader utilization, and compatibility between equipment.

---

## 2. Folder structure

```
I_C_Mais_Atualizado/
|-- README.md                    this file
|-- arquivos/                    reference material from a previous version of the data
|                                (old format, .dat files and Instance.doc). No current code reads this folder.
|-- dados/                       inputs and preparation script
|   |-- README.md
|   |-- IC_arquivos.ipynb        script that generates Minas/
|   |-- minas.txt                configuration of the 290 mines
|   |-- instancias.txt           instance parameters
|   |-- Descrição_Instancias.txt format specification (in Portuguese)
|   |-- teores_csv/              grades and masses of the fronts (3 files)
|   |-- tempo_Ciclo_csv/         cycle times
|   |-- producao/                production targets and REM
|   |-- Minas/                   notebook output: Mina_1 ... Mina_290 (12 .DAT files each)
|   `-- Minas.zip, *.xlsx        source copies and spreadsheets (not read by the code)
`-- src/                         C++ code, build and analysis
    |-- README.md
    |-- main.cpp                 main program (full experiment)
    |-- Makefile, exec.sh        build and run
    |-- H*.h, U*.cpp             modules: utilities, construction, FO, moves, VND, VNS, ILS, GA
    |-- Calibracao_Ag/           GA calibration with irace
    |-- Calibracao_VNS_ILS/      ILS and VNS calibration with irace
    |-- Tratamento_Resultados/   notebook that consolidates the test CSV files
    |-- Relatorios_Finais/       output of the current program (per method, per test)
    |-- Relatorios/              results of earlier runs (legacy names)
    `-- Solucoes/                solution reports of an earlier version
```

---

## 3. Workflow

The project has three steps, which must be run in this order:

```
1. Data preparation
   dados/IC_arquivos.ipynb  (Jupyter, run inside dados/)
   reads minas.txt, teores_csv/, tempo_Ciclo_csv/, producao/
   writes dados/Minas/Mina_N/*.DAT

2. Experiment
   make, then ./exec  (run inside src/)
   reads ../dados/minas.txt and ../dados/Minas/Mina_N/
   writes src/Relatorios_Finais/<method>/Resumo_Geral_*.csv

3. Results processing
   src/Tratamento_Resultados/tratamentos.ipynb
   consolidates the test CSV files into one table per method
```

The methods compared in the current experiment:

| Method | Initial construction | Improvement |
|---|---|---|
| 1 (VNS-Referencia) | GRASP | VNS with fixed-order VND |
| 2 (VNS-RLP) | GRASP | VNS with random-order VND |
| 3 (AG-RLP) | GRASP (initial population) | Genetic algorithm, with random-order VND as local search |

### 3.1 Nomenclature

- **RLP** stands for "Random Lavra Planning", the name used in the thesis for the random-order variant (in Portuguese, Planejamento de Lavra Aleatorio).
- In the old names, "NORMAL" (or "Normal") means the fixed-order version and "ALEATORIO" (or "Aleatorio") means the random-order version. The suffix "-Referencia" marks the fixed-order version.

The thesis names are used in this documentation. The folders, file names and code paths that the program creates or reads still use the old names, shown below. Until they are renamed, the commands in these READMEs use the names the code currently uses.

| Name used in these READMEs | Old name (folders, files and code) |
|---|---|
| VND-Referencia | `VND_NORMAL`, `VND` (fixed-order VND) |
| VND-RLP | `VND_ALEATORIO`, `VND_Aleatorio` (random-order VND) |
| VNS-Referencia | folder `GRASP_VND_NORMAL`, file prefix `VNS_VND(NORMAL)` |
| VNS-RLP | folder `GRASP_VND_ALEATORIO`, file prefix `VNS_VND(ALEATORIO)` |
| AG-RLP | folder `AG_VND_ALEATORIO` |
| ILS-Referencia | `ILS_Normal`, Smart ILS, `ILS_Grasp_Normal` |
| ILS-RLP | `ILS_Aleatorio` |

The consolidated CSV files follow the folder names (one per method folder: `VNS-Referencia.csv`, `VNS-RLP.csv` and `AG-RLP.csv` once the folders have the new names). The historical folder `Relatorios/` keeps its old names.

The averages below were calculated from these CSV files, first per instance and then over the 290 instances, and match the summary in the thesis:

| Method | Average evaluation function | Average time (s) |
|---|---:|---:|
| VNS-Referencia | 52,206.53 | 114.69 |
| VNS-RLP | 51,963.30 | 93.53 |
| AG-RLP | 450,525.13 | 25.40 |

VNS-RLP had the lowest overall average of the evaluation function among the VNS variants. AG-RLP had the lowest average time, but a much higher and more heterogeneous average of penalties. The comparison is descriptive: it does not show statistical significance, nor the physical feasibility of all plans.

The configured experiment runs 20 tests per method on the 290 instances (17,400 runs). However, the result set currently present in `src/Relatorios_Finais/` contains tests 1 to 8 for each method: 6,960 recorded runs. These eight runs per method and instance are the ones behind the results presented in the thesis; tests 9 to 20 are not yet in that folder. The ILS and simulated annealing exist partially in the code, but are not part of the main experiment (see `src/README.md`).

---

## 4. Prerequisites

| Tool | Use | Note |
|---|---|---|
| Python 3 with `pandas`, `numpy` and Jupyter | Notebooks in `dados/` and `src/Tratamento_Resultados/` | Both must be run from the correct folder (see section 5). |
| `g++` with C++11 | Build of `src/` | Tested with g++ 15.2 (MinGW-w64). |
| `make` (GNU make) and a POSIX shell | `Makefile` and `exec.sh` | On Windows, use Git Bash or MSYS2. |
| R with the `irace` package | Calibration (optional) | Only for the `Calibracao_*` folders. |

---

## 5. How to run

The project paths are relative. Each step must be run from the indicated folder, otherwise the files will not be found.

### 5.1 Prepare the data

```bash
cd D:/Faculdade_8/I_C_Mais_Atualizado/dados
jupyter notebook IC_arquivos.ipynb
```

In Jupyter, run the cells in order (`Restart & Run All`). At the end, `dados/Minas/` must have 290 folders, each with 12 `.DAT` files. Details in [`dados/README.md`](dados/README.md), section 6.

### 5.2 Build and run the experiment

```bash
cd D:/Faculdade_8/I_C_Mais_Atualizado/src
make clean        # removes the objects and executable that came with the repository (they are Linux binaries)
make              # builds the program
./exec.sh         # creates the output folders and runs the full experiment
```

The configured experiment goes through 17,400 combinations and may take many hours. The results currently available are from tests 1 to 8 (6,960 runs); do not confuse this set with the full run configured in the code. Details, prerequisites and how to test a subset are in [`src/README.md`](src/README.md), section 4.

### 5.3 Process the results

Open `src/Tratamento_Resultados/tratamentos.ipynb` from that folder and run the cells in order. The notebook reads the test files in `src/Relatorios_Finais/` and writes, in each method folder, a consolidated CSV with one row per mine. In the current copy, each consolidation covers eight tests. Details in [`src/README.md`](src/README.md), section 7.

---

## 6. Verification performed

Two checks were performed, described in detail in the READMEs of each folder:

1. **Compatibility between `dados/` and `src/`**. The notebook and the C++ code agree on the file names, the data format, the number of fronts, trucks and loaders, and the capacities. There are divergences in the production values (`prod_N.DAT`), which affect the value used as the production target. They are described in [`dados/README.md`](dados/README.md), section 5, and summarized in section 7 below.

2. **English comments in the code**. English comments were added to all code files in `src/` (C++, headers, Makefile, `exec.sh`, calibration and the processing notebook) and to the code cells of the `dados/` notebook. The original Portuguese comments were kept. The change did not alter the program: the compiled objects before and after are byte-identical, and the code cells of the notebooks have the same abstract syntax tree (AST) before and after.

---

## 7. Divergences between data and code (summary)

The divergences below were verified in the code. They were not corrected, because changing any of them alters the results of the experiment. The decision belongs to the author of the project. The full description of each item is in [`dados/README.md`](dados/README.md), section 5.2, and in [`src/README.md`](src/README.md), section 8.

1. **Order of the production values in `prod_N.DAT`**. For discharge 1/1, the notebook writes the target before the minimum, but the C++ code reads the first value as the minimum. The report `src/Solucoes/Solucao_Mina_1.txt` shows `[1600 - 2600 - 3100]`, that is, the target 1600 appears as the minimum.
2. **Target value for mines above 24**. The notebook writes 2600 for mines with a number greater than 24, which contradicts the comment saying the target was changed to 1600.
3. **Discharge 1/3**. The notebook writes one line per crusher, but the C++ code reads only the first. The target used is that of `BRIT_0` (900/1100/1300 t/h), not the total of the three (2700/3300/3900 t/h).
4. **Fixed REM**. The notebook uses 0.25/0.31/0.4 for all mines, without reading `teores_producao.csv`.
5. **Run script**. `src/exec.sh` tests for the existence of `./executo`, but the executable is called `./exec`. Therefore `make` runs on every call.
6. **Calibration**. The `target-runner` in `src/Calibracao_Ag/` refers to the old folder `Codigo_novo`, and `exec_irace` must be built manually. The `--pop` flag in the parameter file is not recognized by the driver.
