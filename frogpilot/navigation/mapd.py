#!/usr/bin/env python3
# PFEIFER - MAPD - Modified by FrogAi for FrogPilot
import json
import os
import shutil
import stat
import subprocess
import tempfile
import time
import urllib.request

from pathlib import Path

from openpilot.common.params import Params

from openpilot.frogpilot.common.frogpilot_variables import MAPD_PATH, RESOURCES_REPO

VERSION = "v2"

GITHUB_VERSION_URL = f"https://github.com/{RESOURCES_REPO}/raw/Versions/mapd_version_{VERSION}.json"
GITLAB_VERSION_URL = f"https://gitlab.com/{RESOURCES_REPO}/-/raw/Versions/mapd_version_{VERSION}.json"

VERSION_PATH = Path("/data/media/0/osm/mapd_version")

def cleanup_temp_files():
  parent = MAPD_PATH.parent
  try:
    if not parent.exists() or not parent.is_dir():
      return
  except OSError as e:
    print(f"Skipping cleanup; cannot access {parent}: {e}")
    return
  try:
    for file in parent.glob("mapd*"):
      if file == MAPD_PATH or file == VERSION_PATH:
        continue
      if file.is_file():
        try:
          file.unlink()
        except Exception as exception:
          print(f"Failed to delete leftover file {file}: {exception}")
  except OSError as exception:
    print(f"Skipping cleanup in {parent} due to I/O error: {exception}")

def get_latest_version():
  for url in [GITHUB_VERSION_URL, GITLAB_VERSION_URL]:
    try:
      with urllib.request.urlopen(url, timeout=10) as response:
        return json.loads(response.read().decode("utf-8"))["version"]
    except Exception as exception:
      print(f"Error fetching mapd version from {url}: {exception}")
  return None

def download():
  Path(MAPD_PATH).parent.mkdir(parents=True, exist_ok=True)

  latest_version = get_latest_version()
  if latest_version is None:
    print("Could not fetch mapd version, will retry later")
    return False

  urls = [
    f"https://github.com/pfeiferj/openpilot-mapd/releases/download/{latest_version}/mapd",
    f"https://gitlab.com/{RESOURCES_REPO}/-/raw/Mapd/{latest_version}"
  ]
  for url in urls:
    try:
      with urllib.request.urlopen(url, timeout=30) as response:
        with tempfile.NamedTemporaryFile("wb", delete=False, dir=MAPD_PATH.parent) as temp_file:
          shutil.copyfileobj(response, temp_file)
          temp_file_path = Path(temp_file.name)
          os.fsync(temp_file.fileno())

      os.chmod(temp_file_path, os.stat(temp_file_path).st_mode | stat.S_IEXEC)
      os.rename(temp_file_path, MAPD_PATH)

      with open(VERSION_PATH, "w") as version_file:
        version_file.write(latest_version)
        os.fsync(version_file.fileno())

      return True
    except Exception as exception:
      print(f"Failed to download mapd from {url}: {exception}")
      if "temp_file_path" in locals() and temp_file_path.exists():
        temp_file_path.unlink(missing_ok=True)
  return False

def update_mapd():
  if not MAPD_PATH.exists() or not VERSION_PATH.exists():
    missing = "mapd binary" if not MAPD_PATH.exists() else "version file"
    print(f"{missing} not found. Downloading...")
    return download()

  if not os.access(MAPD_PATH, os.X_OK):
    print(f"{MAPD_PATH} is not executable. Fixing permissions...")
    try:
      os.chmod(MAPD_PATH, os.stat(MAPD_PATH).st_mode | stat.S_IEXEC)
    except Exception as exception:
      print(f"Failed to set executable permissions on {MAPD_PATH}: {exception}")
    return False

  # Check for updates — if network is unavailable, just use what we have
  try:
    with open(VERSION_PATH) as version_file:
      local_version = version_file.read().strip()
    latest_version = get_latest_version()
    if latest_version is not None and local_version != latest_version:
      print("New mapd version available. Updating...")
      return download()
  except Exception as exception:
    print(f"Error checking version: {exception}")

  return True

def mapd_thread():
  params_memory = Params(memory=True)

  params_memory.put("MapdLogLevel", "disabled")

  while True:
    try:
      cleanup_temp_files()
    except OSError as exception:
      print(f"Cleanup errored: {exception}")
      time.sleep(5)
      continue

    while not update_mapd():
      time.sleep(10)
      continue

    try:
      process = subprocess.Popen(str(MAPD_PATH))
      process.wait()
    except FileNotFoundError as error:
      print(f"Subprocess failed: {error}")
      download()

def main():
  mapd_thread()

if __name__ == "__main__":
  main()
