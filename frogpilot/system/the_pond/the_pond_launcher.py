#!/usr/bin/env python3
"""Lightweight launcher for the_pond that defers heavy imports until a client connects.

Listens on the_pond's port with a raw socket. On first connection, closes the
listener, imports the full Flask app, and starts serving. This avoids loading
Flask, panda, CANParser, etc. at boot when nobody is using the web UI.
"""
import socket
import time

from openpilot.system.hardware import PC


def wait_for_connection(port: int, timeout: float = 0.5) -> None:
  """Block until a TCP connection arrives on *port*."""
  with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as srv:
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("0.0.0.0", port))
    srv.listen(1)
    srv.settimeout(timeout)
    print(f"the_pond_launcher: waiting for connection on port {port}...")
    while True:
      try:
        conn, addr = srv.accept()
        conn.close()
        print(f"the_pond_launcher: connection from {addr}, starting the_pond")
        return
      except socket.timeout:
        continue


def main():
  debug = PC or __package__ == "the_pond"
  port = 8083 if debug else 8082

  wait_for_connection(port)

  # Brief pause to let the client-side retry its connection
  time.sleep(0.5)

  # Now import and run the full the_pond
  from openpilot.frogpilot.system.the_pond.the_pond import main as pond_main
  pond_main()


if __name__ == "__main__":
  main()
