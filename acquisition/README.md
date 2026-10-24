# Acquisition

`data_logger.py` runs one recording session against the robot firmware ([firmware/sbara](../firmware/sbara/)):

1. asks for the participant's name, age, and condition (1–8),
2. opens the serial port (`COM14`, 250000 baud),
3. on each "Run a test", sends three randomly chosen M/L disturbances (command `2` or `3`) and logs the
   streamed CoP samples for 5 s after each, then sends `0` to return the platform home,
4. on exit, writes `<condition>, <name>.txt` to the working directory
   (format: see [data/README.md](../data/README.md)).

```bash
pip install pyserial
python data_logger.py
```

Requires Python ≥ 3.10 (`match` statement). Change the COM port in the script to match your machine.
