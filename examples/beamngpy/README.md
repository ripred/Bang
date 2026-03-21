BeamNGpy Bang Example
=====================

This example shows how to use Bang's host-command mechanism to call the
official BeamNGpy Python API from an Arduino sketch.

What it does
------------

- `beamngpy.ino` runs on the Arduino and uses `bang.exec(...)` to call a
  host-side helper script.
- `beamngpy_bridge.py` runs on the host and talks to BeamNG through BeamNGpy.
- The helper returns compact JSON so the Arduino can parse telemetry and
  status information.

Host setup
----------

1. Install the Python dependency on the host that runs BeamNG and
   `arduino_exec.py`:

   ```bash
   pip install beamngpy
   ```

2. Start BeamNG with BeamNGpy communication enabled.

   BeamNG's documentation states that `-tcom` starts the BeamNGpy server and,
   by default, listens on `127.0.0.1:25252`.

   Windows:

   ```bat
   BeamNG.tech.exe -tcom -console
   ```

   Linux:

   ```bash
   BinLinux/BeamNG.tech.x64 -tcom
   ```

3. Start Bang's Python agent on the same host:

   ```bash
   python3 arduino_exec.py -p <YOUR_SERIAL_PORT>
   ```

4. Edit `beamngpy.ino` and set `BEAMNGPY_BRIDGE` to the absolute path of
   `beamngpy_bridge.py` on that host.

5. Upload `beamngpy.ino`, open the Serial Monitor at `115200`, and use:

   - `t` for a telemetry snapshot
   - `r` to set AI mode to `traffic`
   - `d` to disable AI
   - `m` to show a BeamNG UI toast

Notes
-----

- This example is compile-safe for CI, but runtime use is local-only. It
  requires a local BeamNG installation plus the `beamngpy` Python package.
- The helper uses `BeamNGpy.open(launch=False)`, so it expects BeamNG to
  already be running and reachable.
- If you want to talk to another vehicle instead of the player vehicle, extend
  the helper invocation with `--vehicle <vehicle_id>`.
