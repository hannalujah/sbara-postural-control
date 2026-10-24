# Postural Responses to Mediolateral Perturbations: Surface, Vision, and Cognitive Load

Code, data, and documents for my BSc project in Mechanical Engineering at Sharif University of Technology
(supervisor: Dr. Saeed Behzadipour, summer 2025).

**Thesis title (fa):** تحلیل و دسته‌بندی پاسخ مرکز فشار در افراد سالم تحت اغتشاش مکانیکی و شرایط حسی-شناختی گوناگون  
*(Analysis and classification of centre-of-pressure responses in healthy adults under mechanical perturbation and different sensory–cognitive conditions)*

**Paper:** H. Budaqi et al., *Postural Responses to Mediolateral Rotations – Contributions of Surface, Vision, and Cognitive Load* ([IEEE Xplore](https://ieeexplore.ieee.org/abstract/document/11392127) · [PDF](docs/paper/))

<p align="center">
  <img src="media/SBARA.png" alt="SBARA robot" width="420"><br>
  <em>The SBARA balance robot</em>
</p>

## Summary

Fifteen healthy adults stood on the **SBARA** balance robot (Movafaghian Rehabilitation Center), which applied
mediolateral (M/L) rotational perturbations while four load cells recorded the centre of pressure (CoP).
Each participant completed eight conditions, a 2 × 2 × 2 design:

| # | Condition | Surface | Vision | Cognitive task |
|---|-----------|---------|--------|----------------|
| 1 | Pure Disturbance | rigid | eyes open | – |
| 2 | Cognitive Task Only | rigid | eyes open | ✔ |
| 3 | Eyes Closed Only | rigid | eyes closed | – |
| 4 | Eyes Closed + Cognitive Task | rigid | eyes closed | ✔ |
| 5 | Foam Only | foam | eyes open | – |
| 6 | Foam + Cognitive Task | foam | eyes open | ✔ |
| 7 | Foam + Eyes Closed | foam | eyes closed | – |
| 8 | Foam + Eyes Closed + Cognitive Task | foam | eyes closed | ✔ |

CoP signals were low-pass filtered (3rd-order Butterworth, 10 Hz), 29 positional, velocity, and frequency-domain
CoP measures were extracted, and each measure was tested with a three-way repeated-measures ANOVA.
Surface, vision, and cognitive load each had an independent (non-interacting) effect on the response; vision
affected all measure groups, while surface and cognitive load affected only some of them.

## Workflow

```
firmware/sbara/sbara.ino          robot: applies perturbations, streams CoP over serial
        │
acquisition/data_logger.py        PC: runs a session, saves one .txt per participant × condition
        │
data/processed/*.txt              trimmed recordings (15 participants × 8 conditions)
        │
analysis/matlab/                  CoP measures per trial → per participant/condition
        │                         → results/tables/DatabaseV5.xlsx, Means.xlsx
analysis/python/anova.py          3-way repeated-measures ANOVA per measure
                                  → results/tables/anova_results_statsmodels.csv
```

## Repository layout

```
├── firmware/sbara/     Arduino Due firmware for the SBARA robot
├── acquisition/        session data logger (Python)
├── analysis/
│   ├── matlab/         CoP measure extraction
│   └── python/         repeated-measures ANOVA
├── data/
│   ├── processed/      analysis input recordings
│   └── participants.csv
├── results/
│   ├── tables/         measure database, means, ANOVA results
│   └── figures/        plots grouped by factor (cognitive / surface / vision)
├── docs/
│   ├── thesis/         final thesis (Persian, PDF)
│   └── paper/          paper (PDF)
└── media/              photo of the SBARA robot (Git LFS)
```

Each code folder has its own README.

## Getting started

| Part | Requirements | Details |
|------|--------------|---------|
| Firmware | Arduino IDE, Arduino Due, `Encoder` and `HX711` libraries | [firmware/README.md](firmware/README.md) |
| Acquisition | Python ≥ 3.10, `pyserial` | [acquisition/README.md](acquisition/README.md) |
| Analysis | MATLAB (Signal Processing Toolbox); Python with `pandas`, `openpyxl`, `statsmodels` | [analysis/README.md](analysis/README.md) |

## Privacy

Participants are identified only by pseudonymous IDs (S01–S15). Names have been removed from data file names
and headers. The original code is kept as used in the study, so `analysis/matlab/Subjects.m` and
`calculateCOPMeasurements.m` still contain participants' first names.
