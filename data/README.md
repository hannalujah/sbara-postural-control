# Data

`processed/` holds the recordings analysed in the thesis: 15 participants × 8 conditions = 120 files, named
`<condition>, <subject>.txt` (e.g. `5. Foam Only, S07.txt`). These recordings were trimmed by hand from the
raw session logs.

## File format

```
Name: S07, Age: 26 years old, Time: 2025-04-22 09:28:07

-3.2,-1.1,48975
-2.4,-1.2,48986
...
```

* Line 1: header (subject ID, age, session start time). `1. Pure Disturbance, S01.txt` has no header.
* Then one sample per line: `CoP_x (M/L), CoP_y (A/P), timestamp [ms]` (Arduino `millis()`), as streamed
  by the firmware.
* A gap of more than 500 ms in the timestamp marks the boundary between perturbation trials.

## Participants

[`participants.csv`](participants.csv) lists each subject ID with age and session date. The numbering
S01–S15 matches the order in `analysis/matlab/Subjects.m`.
