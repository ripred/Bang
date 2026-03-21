#!/usr/bin/env python3
"""
Host-side helper for the Bang BeamNGpy example.

This script is called by Bang's Python agent over the serial command channel.
It expects a local BeamNG instance to already be running with the BeamNGpy
communication server enabled (`-tcom`).
"""

from __future__ import annotations

import argparse
import json
import math
import sys
from typing import Any

IMPORT_ERROR: Exception | None = None

try:
    from beamngpy import BeamNGpy
    from beamngpy.sensors import Electrics
except Exception as exc:  # pragma: no cover - runtime dependency only
    BeamNGpy = None  # type: ignore[assignment]
    Electrics = None  # type: ignore[assignment]
    IMPORT_ERROR = exc


def emit(payload: dict[str, Any]) -> int:
    print(json.dumps(payload, separators=(",", ":")))
    return 0 if payload.get("ok") else 1


def fail(message: str) -> int:
    return emit({"ok": False, "error": message})


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Bang BeamNGpy bridge")
    subparsers = parser.add_subparsers(dest="command", required=True)

    def add_common_arguments(subparser: argparse.ArgumentParser) -> None:
        subparser.add_argument("--host", default="127.0.0.1",
                               help="BeamNG host running the BeamNGpy server.")
        subparser.add_argument("--port", type=int, default=25252,
                               help="BeamNGpy TCP port.")
        subparser.add_argument("--vehicle", default="",
                               help="Optional vehicle id; defaults to player vehicle.")

    telemetry = subparsers.add_parser("telemetry", help="Read a telemetry snapshot.")
    add_common_arguments(telemetry)

    ai = subparsers.add_parser("ai", help="Change the target vehicle AI mode.")
    add_common_arguments(ai)
    ai.add_argument(
        "--mode",
        required=True,
        choices=("disabled", "random", "traffic", "span", "manual",
                 "chase", "flee", "stopping"),
        help="BeamNG AI mode to set.",
    )

    message = subparsers.add_parser("message", help="Display a UI toast message.")
    add_common_arguments(message)
    message.add_argument("--text", required=True,
                         help="Message text to display inside BeamNG.")

    return parser


def connect_beamng(host: str, port: int):
    if IMPORT_ERROR is not None:
        raise RuntimeError(
            "beamngpy is not installed on the host. Run `pip install beamngpy`."
        ) from IMPORT_ERROR

    bng = BeamNGpy(host, port)
    bng.open(launch=False)
    return bng


def resolve_vehicle(bng, vehicle_name: str):
    vehicles = bng.vehicles.get_current()
    if not vehicles:
        raise RuntimeError("No active vehicles are available in the simulator.")

    if vehicle_name:
        vehicle = vehicles.get(vehicle_name)
        if vehicle is None:
            raise RuntimeError(f"Vehicle '{vehicle_name}' was not found.")
        return vehicle

    player_info = bng.vehicles.get_player_vehicle_id()
    player_vid = player_info.get("vid") if isinstance(player_info, dict) else None
    if player_vid and player_vid in vehicles:
        return vehicles[player_vid]

    return next(iter(vehicles.values()))


def command_telemetry(args: argparse.Namespace) -> int:
    bng = connect_beamng(args.host, args.port)
    vehicle = None

    try:
        vehicle = resolve_vehicle(bng, args.vehicle)
        vehicle.connect(bng)

        if "electrics" not in vehicle.sensors.data:
            vehicle.sensors.attach("electrics", Electrics())

        vehicle.sensors.poll()

        state = vehicle.state or {}
        electrics = vehicle.sensors["electrics"]
        pos = state.get("pos", (0.0, 0.0, 0.0))
        vel = state.get("vel", (0.0, 0.0, 0.0))
        fallback_speed = math.sqrt(sum(float(axis) ** 2 for axis in vel))
        speed_mps = float(electrics.get("wheelspeed", fallback_speed))

        return emit({
            "ok": True,
            "vehicle": vehicle.vid,
            "speed_mps": round(speed_mps, 3),
            "speed_kph": round(speed_mps * 3.6, 3),
            "speed_mph": round(speed_mps * 2.2369362921, 3),
            "rpm": round(float(electrics.get("rpm", 0.0)), 1),
            "gear": str(electrics.get("gear", "")),
            "throttle": float(electrics.get("throttle_input",
                                            electrics.get("throttle", 0.0))),
            "brake": float(electrics.get("brake_input",
                                         electrics.get("brake", 0.0))),
            "steering": float(electrics.get("steering_input",
                                            electrics.get("steering", 0.0))),
            "running": bool(electrics.get("running", False)),
            "pos": [round(float(axis), 3) for axis in pos],
        })
    finally:
        if vehicle is not None and vehicle.is_connected():
            vehicle.disconnect()
        bng.disconnect()


def command_ai(args: argparse.Namespace) -> int:
    bng = connect_beamng(args.host, args.port)
    vehicle = None

    try:
        vehicle = resolve_vehicle(bng, args.vehicle)
        vehicle.connect(bng)
        vehicle.ai.set_mode(args.mode)
        return emit({
            "ok": True,
            "vehicle": vehicle.vid,
            "ai_mode": args.mode,
        })
    finally:
        if vehicle is not None and vehicle.is_connected():
            vehicle.disconnect()
        bng.disconnect()


def command_message(args: argparse.Namespace) -> int:
    bng = connect_beamng(args.host, args.port)
    try:
        bng.ui.display_message(args.text)
        return emit({
            "ok": True,
            "message": args.text,
        })
    finally:
        bng.disconnect()


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    try:
        if args.command == "telemetry":
            return command_telemetry(args)
        if args.command == "ai":
            return command_ai(args)
        if args.command == "message":
            return command_message(args)
        return fail(f"Unsupported command '{args.command}'.")
    except Exception as exc:  # pragma: no cover - runtime dependency only
        return fail(str(exc))


if __name__ == "__main__":
    sys.exit(main())
