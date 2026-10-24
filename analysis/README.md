# Analysis

Two stages: MATLAB computes the CoP measures, Python runs the statistics.

## 1. CoP measures (MATLAB)

Requires the Signal Processing Toolbox (`butter`, `filtfilt`, `pwelch`).

| Script | Role |
|--------|------|
| `dataAnalysisProgram.m` | Entry point: runs `calculateCOPMeasurements` then `cleanVars` |
| `calculateCOPMeasurements.m` | Reads all recordings, splits them into trials (timestamp gap > 500 ms), keeps trials longer than 347 samples, computes measures per trial, averages per participant and condition, exports `DatabaseV5.xlsx` and `Means.xlsx` |
| `calculateVars.m` | Low-pass filter (3rd-order Butterworth, 10 Hz) and the 29 CoP measures for one trial |
| `internalDBCalc.m` | Stores one trial's measures |
| `addingVarsV2.m` | Stores a participant/condition mean in `Database` |
| `meanVars.m` | Group mean over participants |
| `CoPVars.m`, `States.m`, `Subjects.m` | Index constants for the measure, condition, and participant dimensions |
| `varLabels.m` | Measure names (Excel sheet names) |
| `cleanVars.m` | Clears the workspace except `data`, `Database`, `MeanComplete` |

**Output:** `DatabaseV5.xlsx` has one sheet per measure, with 15 participants (rows) × 8 conditions (columns).
`Means.xlsx` has the group means.

### Running on this repository

Two settings in `calculateCOPMeasurements.m` must be adapted first:

1. `baseFolder`: set it to the absolute path of `data/processed/`.
2. `Subjects`: the data files are named by ID, so the list should be
   `{'S14','S12','S08','S15','S09','S02','S03','S05','S07','S01','S06','S04','S13','S11','S10'}`
   (same order as the original names).

## 2. Repeated-measures ANOVA (Python)

`python/anova.py` reads every sheet of `DatabaseV5.xlsx` and runs a three-way repeated-measures ANOVA
(`statsmodels` `AnovaRM`; within-subject factors Surface, Vision, Cognitive). F, degrees of freedom, and p-values
for all main effects and interactions of every measure go to `anova_results_statsmodels.csv`.

```bash
pip install pandas openpyxl statsmodels
cd results/tables
python ../../analysis/python/anova.py
```

The script reads and writes in the working directory. `results/tables/Anova Test Results.xlsx` is the
formatted p-value table built from its output.
