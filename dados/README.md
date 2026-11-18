# `dados` folder

This folder contains the input data of the project and the script that turns these inputs into the `.DAT` files read by the C++ program in the [`src`](../src/README.md) folder.

The full flow is:

```
minas.txt, instancias.txt (parameters)
teores_csv/*.csv  (grades and masses of the fronts)
tempo_Ciclo_csv/tempoCiclo_tratado.csv  (cycle times)
producao/teores_producao.csv  (production targets and REM)
            |
            v
IC_arquivos.ipynb  (preparation script, run in Jupyter)
            |
            v
Minas/Mina_1 ... Minas/Mina_290  (12 .DAT files per mine)
            |
            v
src/exec  (C++ program, reads ../dados/minas.txt and ../dados/Minas/)
```

This document describes: the contents of the folder, the format of each file, how the notebook works, how to run it correctly, and the result of the compatibility check between the notebook and the C++ code.

---

## 1. Folder contents

| Item | Type | Role |
|---|---|---|
| `IC_arquivos.ipynb` | Jupyter notebook | Preparation script. Generates the `Minas/` folder. See section 3. |
| `minas.txt` | Text | One line per mine (290 mines). Defines the configuration of each instance. See section 2.1. |
| `instancias.txt` | Text | General instance parameters (number of trucks, capacities, loaders, fronts, control parameters, cycle times). Read by the notebook only for display. See section 2.2. |
| `Descrição_Instancias.txt` | Text | Specification document of the formats of the `.DAT` files (in Portuguese). Reference for section 4. |
| `teores_csv/` | Folder with 3 CSV files | Quality grades and mass of each front, for three front configurations. See section 2.3. |
| `tempo_Ciclo_csv/tempoCiclo_tratado.csv` | CSV | Cycle times per truck capacity. See section 2.4. |
| `producao/teores_producao.csv` | CSV | Production targets (minimum, target, maximum) per crusher and REM, for the two discharge types. See section 2.5. |
| `Minas/` | Generated folder | 290 subfolders `Mina_N`, each with 12 `.DAT` files. It is the output of the notebook and the input of the C++ code. See section 4. |
| `Minas.zip` | Compressed file | Compressed copy of an earlier version of the `Minas/` folder. No code reads this file. |
| `teores.xlsx`, `teores_producao.xlsx`, `tempoCiclo.xlsx` | Spreadsheets | Source spreadsheets. The notebook reads the CSV versions exported from these spreadsheets, not the `.xlsx` files. |

---

## 2. Input files

### 2.1 `minas.txt`

Each line defines one mine (instance). The C++ program reads this file to find the number of ore fronts of each mine.

Format:

```
Mina <id>: <n_caminhoes> cam, <n_carregadeiras> carga, <m>/<e> frentes, <p> par, <d> descarga, <tipo_frota>, <tempo_ciclo>
```

| Field | Meaning |
|---|---|
| `<id>` | Mine number, from 1 to 290. |
| `<n_caminhoes>` | Number of trucks (30, 40 or 50). |
| `<n_carregadeiras>` | Number of loaders (3, 6 or 9). |
| `<m>/<e>` | Number of ore fronts (`m`) and waste fronts (`e`). Examples: `3/1`, `12/5`, `32/10`. |
| `<p>` | Number of quality parameters controlled (4 or 10). |
| `<d>` | Discharge type. `1/1`: one ore destination and one waste destination. `1/3`: three ore destinations (`BRIT_0`, `BRIT_1`, `BRIT_2`) and one waste destination. |
| `<tipo_frota>` | `homogênea` (homogeneous), `heterogênea` (heterogeneous: half of the trucks with low capacity, half with high capacity) or `homogênea120` (all trucks with high capacity). These are the literal values written in the file. |
| `<tempo_ciclo>` | `curto` (short), `médio` (medium) or `longo` (long). Selects the column of the cycle time table. These are the literal values written in the file. |

Example:

```
Mina 1: 30 cam, 3 carga, 3/1 frentes, 4 par, 1/1 descarga, homogênea, curto
```

This mine has 30 homogeneous trucks, 3 loaders, 3 ore fronts and 1 waste front, 4 quality parameters, discharge 1/1, and a short cycle time.

The numbers of trucks and loaders vary by range of mines: mines 1 to 20 use 30 trucks and 3 loaders, and the final mines use up to 50 trucks and 9 loaders.

### 2.2 `instancias.txt`

Reference document with the ranges of values used in the project:

| Parameter | Values |
|---|---|
| Number of trucks | 30; 40; 50 |
| Truck capacity (waste/ore, t) | 61/64; 120/130 |
| Distribution of the heterogeneous fleet | 0.5/0.5 |
| Loaders | 3; 6; 9 |
| Maximum loader capacity (t/h) | 800; 900; 1100 |
| Minimum loader capacity (t/h) | 100; 250; 400 |
| Fronts (ore/waste) | 3/1; 12/5; 32/10 |
| Control parameters | 4; 10 |
| Discharge points | 1/1; 1/3 |
| Cycle times | short, medium, long |

The notebook reads this file in cell 1, but only prints the values. The values used to generate the `.DAT` files are fixed in the code of cell 9, and were checked manually against this file (see section 5).

### 2.3 `teores_csv/`

Three files, exported from the spreadsheet `teores.xlsx`, one for each front configuration:

| File | Configuration |
|---|---|
| `teores.xlsx - 3-1.csv` | 3 ore fronts and 1 waste front |
| `teores.xlsx - 12-5.csv` | 12 ore fronts and 5 waste fronts |
| `teores.xlsx - 32-10.csv` | 32 ore fronts and 10 waste fronts |

Each file has one line per front, with identifier `Frente_min_<k>` (ore) or `Frente_est_<k>` (waste), and columns:

- `FEGL`, `SIGL`, `ALGL`, `PGL`, `MNGL`, `PFGL`, `G1`, `FE1`, `SI1`, `AL1`: grades of the 10 quality variables. For mines with 4 parameters, only the first 4 columns (`FEGL`, `SIGL`, `ALGL`, `PGL`) are used.
- `Massa (t)`: mass of the front, in tonnes.

Besides the fronts, each file has three lines of grade limits: `teor_min`, `teor_rec` (recommended) and `teor_max`.

Decimal numbers use a comma and the values are enclosed in quotes, for example `"43,516404"`.

### 2.4 `tempo_Ciclo_csv/tempoCiclo_tratado.csv`

Table of cycle times per truck capacity:

```
Esteril,Minerio,Tempo_ciclo_curto,Tempo_ciclo_medio,Tempo_ciclo_longo
30,35,5.8,7.8,11.8
61,64,7.3,9.8,14.8
90,96,9.8,13.13333333,19.8
120,130,14.8,19.8,29.8
```

Each line associates a waste capacity and an ore capacity with three cycle times. The notebook uses the capacities 61/64 and 120/130, which are the capacities of the project fleet. The 30/35 and 90/96 lines are not used by the current mines.

### 2.5 `producao/teores_producao.csv`

Production targets per crusher (ton/hour) and REM. There are two tables:

- **Discharge 1/3**: three crushers (`BRIT_0`, `BRIT_1`, `BRIT_2`), each with minimum, target and maximum. Totals: 2700, 3300 and 3900 ton/hour. REM 0.25.
- **Discharge 1/1**: one crusher (`BRIT_0`) with minimum 2600, target 2600 and maximum 3100 ton/hour. REM 0.31.

The notebook reads this file in cells 7 and 9. See the divergences in section 5.

---

## 3. The `IC_arquivos.ipynb` notebook

The notebook is the preparation script. It must be run **from the `dados/` folder**, because all read and write paths are relative to it.

| Cell | Type | What it does |
|---|---|---|
| 0 | Markdown | Title "Coletando Variáveis" ("Collecting variables"). |
| 1 | Code | Reads `instancias.txt` up to the `/*` marker and extracts with regular expressions the parameters (fleets, capacities, loaders, fronts, control parameters, discharges, cycle times). Only prints the result. |
| 2 | Markdown | Title "Teores - Variáveis" ("Grades - Variables"). |
| 3 | Code | Reads the three CSV files in `teores_csv/`. For each front it creates the vector `Frente_<tipo>_<k>_<sufixo>`, and for each grade line it creates `teor_<min\|rec\|max>_<sufixo>`. The suffix (`3-1`, `12-5`, `32-10`) comes from the file name. |
| 4 | Markdown | Title "Tempo de ciclo - Variáveis" ("Cycle time - Variables"). |
| 5 | Code | Reads `tempoCiclo_tratado.csv` and creates the lists of capacities (waste and ore) and of cycle times (short, medium, long). |
| 6 | Markdown | Title "Produção" ("Production"). |
| 7 | Code | Reads `teores_producao.csv` and builds the dictionary `dados_mineracao` with the targets of each crusher, for discharges 1/3 and 1/1. |
| 8 | Markdown | Title "Minas" ("Mines"). |
| 9 | Code | Function `gerar_arquivos_dat` (and its call). For each line of `minas.txt`, creates `Minas/Mina_N/` and writes the 12 `.DAT` files. See section 4. |

Important points about cell 9:

- The regular expression applied to `minas.txt` separates the fields of each line. The fleet type is normalized (accents removed), and the cycle time (`médio` becomes `medio`).
- Fixed capacities: homogeneous fleet 61 (waste) and 64 (ore) t; homogeneous120 120 and 130 t; heterogeneous fleet half of each. Loaders with maximum production 800, 900 and 1100 t/h and minimum 100, 250 and 400 t/h, repeated in blocks of three.
- Compatibility for the heterogeneous fleet: trucks of the first half can only use the first half of the loaders, and those of the second half only the second half. In a homogeneous fleet everything is compatible.
- Cycle time: the value comes from the time table, according to the truck capacity, the type of front (ore or waste) and the cycle time of the mine. Invalid combinations receive the value `99999`.
- If the mine does not have the grade keys (`teor_min_<sufixo>`, etc.) in the dictionary, the cell skips the mine after having already written the truck and loader files. That mine is left with an incomplete folder, which the C++ code cannot load. Check the file count after the run.
- The cell prints `AVISO` (warning) for lines of `minas.txt` that are not recognized and `ERRO` (error) for missing files. These printed messages are in Portuguese.

---

## 4. Output: `Minas/Mina_N/`

Each mine generates a `Mina_N` folder with 12 files. The names follow the pattern `<name>_<N>.DAT`. All files are text, comma-separated, without a header.

| File | Format of each line | Content | Read by C++ |
|---|---|---|---|
| `capcam_est_N.DAT` | `truck,capacity` | Waste capacity of each truck (t). Also defines the number of trucks. | Yes |
| `capcam_min_N.DAT` | `truck,capacity` | Ore capacity of each truck (t). | No (generated but not used) |
| `cargamax_N.DAT` | `loader,production` | Maximum production of each loader (t/h). Defines the number of loaders. | Yes |
| `cargamin_N.DAT` | `loader,production` | Minimum production of each loader (t/h). | Yes |
| `comp_N.DAT` | `truck,loader,value` | Compatibility: 1 if the truck can operate with the loader, 0 otherwise. | Yes |
| `quant_N.DAT` | `front,mass` | Mass of each front (t). Defines the total number of fronts. | Yes |
| `content_N.DAT` | `front,parameter,grade` | Grade of each quality parameter in each front. | Yes |
| `cmin_N.DAT` | `parameter,grade` | Minimum allowed grade of each parameter. | Yes |
| `crec_N.DAT` | `parameter,grade` | Recommended grade of each parameter. | Yes |
| `cmax_N.DAT` | `parameter,grade` | Maximum allowed grade of each parameter. Defines the number of parameters. | Yes |
| `tciclo_N.DAT` | `truck,front,destination,time` | Cycle time of each truck between each front and each destination. `99999` indicates an invalid combination. | Yes |
| `prod_N.DAT` | See below | REM and production targets. | Yes (only the first two lines) |

Format of `prod_N.DAT`:

- Line 1: `REM_min,REM_meta,REM_max` (REM minimum, target, maximum). The notebook always writes `0.25, 0.31, 0.4`.
- Line 2: `prodmin,prodrec,prodmax`. For discharge 1/1 the notebook writes `Meta,Minimo,Maximo` (target, minimum, maximum). For discharge 1/3 it writes one line per crusher (`Minima,Meta,Maxima`), three lines in total.

The C++ code reads only line 1 and line 2 (see section 5).

Number of files: 290 mines x 12 files = 3,480 `.DAT` files.

---

## 5. Compatibility check with the C++ code

The check compared what the notebook writes with what the `src/` folder reads. The results are divided into points of agreement and divergences.

### 5.1 Points of agreement

| Item | Status |
|---|---|
| Names of the 12 files per mine | Consistent. The C++ code reads the same names (`capcam_est`, `cargamax`, `cargamin`, `comp`, `quant`, `content`, `cmin`, `crec`, `cmax`, `tciclo`, `prod`). |
| Folder path | Consistent. The notebook writes to `dados/Minas/Mina_N`, and the C++ code reads `../dados/Minas/Mina_N/` from `src/`. |
| File format | Consistent. Each C++ reader uses the writing format of the notebook (`%d,%f`, `%d,%d,%f`, `%d,%d,%d,%f`). |
| Number of fronts and parameters | Consistent. The C++ code counts the lines of `quant` and `cmax` to size the matrices. |
| Fronts and grades | Consistent. The identifiers `Frente_min_k` and `Frente_est_k` and the keys `teor_min/rec/max` match what the C++ code looks for. |
| Number of trucks and loaders | Consistent with `instancias.txt` (30/40/50 and 3/6/9). |
| Fixed capacities (61/64, 120/130, 800/900/1100, 100/250/400) | Consistent with `instancias.txt`. |
| Reading of `minas.txt` | Consistent. The C++ code extracts the number of ore fronts (`m` in `m/e`) from the line `Mina <id>:`. |
| Number of mines | Consistent. 290 mines in `minas.txt`, 290 folders in `Minas/`, and the C++ code goes from 1 to 290. |

### 5.2 Divergences

**Divergence 1 (important): order of the fields in `prod_N.DAT`.**

The C++ code reads the second line as `prodmin, prodrec, prodmax`, that is, minimum, target and maximum. For discharge 1/1 the notebook writes `Meta, Minimo, Maximo` (target, minimum, maximum), so the minimum and the target swap places. The file `src/Solucoes/Solucao_Mina_1.txt` confirms the effect: it shows `[1600.0 - 2600.0 - 3100.0]`, that is, the target 1600 was read as the minimum.

In addition, the notebook writes `2600` as the first value for mines above 24 (for example mine 33), while the code comment says the target was changed to 1600 as requested. This means the behavior differs for mines 1 to 24 and for mines above 24. It is not possible to tell from the code which value is intended.

**Divergence 2: discharge 1/3 and ignored lines.**

For discharge 1/3 the notebook writes three production lines (one per crusher). The C++ code reads only the second line, which corresponds to `BRIT_0`. Thus the target used by the program is 900/1100/1300 t/h, while the sum of the three crushers is 2700/3300/3900 t/h. If the intent was to use the total, the C++ code must read the total; if the intent was to use one crusher at a time, the notebook should write only one line.

**Divergence 3: fixed REM.**

The notebook writes `0.25, 0.31, 0.4` for all mines, without reading the REM from `teores_producao.csv`. That file has REM 0.25 for discharge 1/3 and 0.31 for discharge 1/1. The notebook comment says the REM was redefined, so the divergence may be intentional, but it should be confirmed.

**Divergence 4: `capcam_min_N.DAT` is not used.**

The file is generated, but the C++ code reads only `capcam_est_N.DAT`.

---

## 6. How to run correctly

### 6.1 Prerequisites

- Python 3 with `pandas`, `numpy` and Jupyter (`jupyter notebook` or `jupyter lab`).
- No other file is needed: all inputs are already in this folder.

### 6.2 Step by step

1. Open a terminal **inside the `dados/` folder**:

   ```bash
   cd D:/Faculdade_8/I_C_Mais_Atualizado/dados
   jupyter notebook IC_arquivos.ipynb
   ```

   If the notebook is opened from another folder, the relative paths fail (`FileNotFoundError` for `instancias.txt`, `teores_csv/...`, `minas.txt`).

2. Run the cells **in order**, from 1 to 9 (for example with `Kernel > Restart & Run All`). Cell 9 creates the `Minas/` folder (if it does not exist) and overwrites the existing `.DAT` files.

3. Check the output of cell 9. It must end with the message `GERAÇÃO DE TODOS OS ARQUIVOS .DAT CONCLUÍDA COM SUCESSO` (all .DAT files generated successfully; the message is printed in Portuguese). Any `AVISO` or `ERRO` line must be investigated.

4. Check the folder count:

   ```bash
   ls Minas | wc -l      # must return 290
   ls Minas/Mina_1       # must list the 12 files
   ```

5. Only after that, run the C++ program (see [`src/README.md`](../src/README.md)).

Notes:

- If you change `minas.txt`, `teores_csv/` or `producao/`, run the whole notebook again. The C++ code does not read these inputs directly.
- If you change the capacities or the loads in cell 9, remember that `instancias.txt` is not updated automatically.
