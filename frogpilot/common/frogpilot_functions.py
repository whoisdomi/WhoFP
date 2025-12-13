#!/usr/bin/env python3
import datetime
import random
import shutil
import string
import tarfile
import threading
import time
import zstandard as zstd

from pathlib import Path

from openpilot.common.basedir import BASEDIR
from openpilot.common.params import Params
from openpilot.common.time_helpers import system_time_valid
from openpilot.system.athena.registration import register
from openpilot.system.hardware import HARDWARE

from openpilot.frogpilot.assets.theme_manager import ThemeManager
from openpilot.frogpilot.common.frogpilot_utilities import delete_file, is_FrogsGoMoo, run_cmd, use_konik_server
from openpilot.frogpilot.common.frogpilot_variables import (
  ERROR_LOGS_PATH, EXCLUDED_KEYS, FROGPILOT_BACKUPS, FROGS_GO_MOO_PATH, HD_LOGS_PATH, KONIK_LOGS_PATH, THEME_SAVE_PATH, TOGGLE_BACKUPS,
  FrogPilotVariables, get_frogpilot_toggles
)


def cleanup_backups(directory, limit):
  directory.mkdir(parents=True, exist_ok=True)

  for backup in directory.glob("*_in_progress*"):
    delete_file(backup, report=False)

  backups = sorted(directory.glob("*_auto*"), key=lambda f: f.stat().st_mtime, reverse=True)
  for oldest_backup in backups[limit:]:
    delete_file(oldest_backup, report=False)


def create_backup(backup, destination, success_message, fail_message, params, minimum_backup_size=0, compressed=False):
  final_destination = destination.parent / f"{destination.name}.tar.zst" if compressed else destination

  if final_destination.exists():
    print("Backup already exists. Aborting...")
    return

  if compressed:
    compressed_temp = destination.parent / f"{destination.name}_in_progress.tar.zst"

    with open(compressed_temp, "wb") as f_out:
      cctx = zstd.ZstdCompressor()
      with cctx.stream_writer(f_out) as compressor:
        with tarfile.open(fileobj=compressor, mode="w") as tar:
          try:
            tar.add(backup, arcname=destination.name)
          except OSError:
            pass

    compressed_temp.rename(final_destination)

    compressed_backup_size = final_destination.stat().st_size
    if minimum_backup_size == 0 or compressed_backup_size < minimum_backup_size:
      params.put("MinimumBackupSize", int(compressed_backup_size))
  else:
    in_progress_destination = destination.parent / f"{destination.name}_in_progress"

    shutil.copytree(backup, in_progress_destination, symlinks=True)

    in_progress_destination.rename(destination)

  print(success_message)


def backup_frogpilot(build_metadata, params):
  maximum_backups = 3
  cleanup_backups(FROGPILOT_BACKUPS, maximum_backups)

  today = datetime.datetime.now().date()
  for backup in FROGPILOT_BACKUPS.glob("*_auto.tar.zst"):
    if backup.name.endswith(f"_{build_metadata.channel}_auto.tar.zst"):
      if datetime.datetime.fromtimestamp(backup.stat().st_mtime).date() == today:
        if not backup.name.startswith(f"{build_metadata.openpilot.git_commit[:6]}_"):
          delete_file(backup, report=False)

  _, _, free = shutil.disk_usage(FROGPILOT_BACKUPS)
  minimum_backup_size = params.get("MinimumBackupSize")
  if free > minimum_backup_size * maximum_backups:
    destination = FROGPILOT_BACKUPS / f"{build_metadata.openpilot.git_commit}_{build_metadata.channel}_auto"
    create_backup(Path(BASEDIR), destination, "Successfully backed up FrogPilot!", "Failed to backup FrogPilot...", params, minimum_backup_size, compressed=True)


def backup_toggles(params, boot_run=False):
  params_backup = Params("/dev/shm/params_backup", return_defaults=True)

  changes_found = False
  for key in params.all_keys():
    current_value = params.get(key)
    if current_value is None:
      continue

    if boot_run:
      params_backup.put(key, current_value)
      changes_found = True
    elif current_value != params_backup.get(key):
      params_backup.put(key, current_value)
      changes_found |= key not in EXCLUDED_KEYS

  maximum_backups = 5
  cleanup_backups(TOGGLE_BACKUPS, maximum_backups)

  if not changes_found:
    print("Toggles are identical to the previous backup. Aborting...")
    return

  destination = TOGGLE_BACKUPS / f"{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}_auto"
  create_backup(Path(params_backup.get_param_path()), destination, "Successfully backed up toggles!", "Failed to backup toggles...", params)


def frogpilot_boot_functions(build_metadata, params):
  params_memory = Params(memory=True)

  FrogPilotVariables()
  ThemeManager(params, params_memory, boot_run=True).update_active_theme(time_validated=system_time_valid(), frogpilot_toggles=get_frogpilot_toggles(), boot_run=True)

  if use_konik_server():
    if params.get("KonikDongleId") is not None:
      params.put("DongleId", params.get("KonikDongleId"))
    else:
      params.put("KonikDongleId", register(show_spinner=True, register_konik=True))
      params.put("DongleId", params.get("KonikDongleId"))
  elif params.get("DongleId") == params.get("KonikDongleId"):
    params.put("DongleId", params.get("StockDongleId"))

  def boot_thread():
    while not system_time_valid():
      print("Waiting for system time to become valid...")
      time.sleep(1)

    backup_frogpilot(build_metadata, params)

  threading.Thread(target=boot_thread, daemon=True).start()


def install_frogpilot(build_metadata, params):
  paths = [
    ERROR_LOGS_PATH,
    HD_LOGS_PATH,
    KONIK_LOGS_PATH,
    THEME_SAVE_PATH
  ]
  for path in paths:
    path.mkdir(parents=True, exist_ok=True)

  if params.get("FrogPilotDongleId") is None:
    params.put("FrogPilotDongleId", "".join(random.choices(string.ascii_lowercase + string.digits, k=16)))

  update_boot_logo(frogpilot=True)

  if build_metadata.channel == "FrogPilot-Development" and is_FrogsGoMoo():
    mount_options = run_cmd(["findmnt", "-n", "-o", "OPTIONS", "/persist"], "Successfully retrieved mount options", "Failed to retrieve mount options")
    run_cmd(["sudo", "mount", "-o", "remount,rw", "/persist"], "Successfully remounted /persist as read-write", "Failed to remount /persist")
    run_cmd(["sudo", "python3", FROGS_GO_MOO_PATH], "Successfully ran frogsgomoo.py", "Failed to run frogsgomoo.py")
    run_cmd(["sudo", "mount", "-o", f"remount,{mount_options}", "/persist"], "Successfully restored /persist mount options", "Failed to restore /persist mount options")


def uninstall_frogpilot():
  update_boot_logo(stock=True)

  HARDWARE.uninstall()


def update_boot_logo(frogpilot=False, stock=False):
  boot_logo = Path("/usr/comma/bg.jpg")

  if frogpilot:
    target_logo = Path(__file__).resolve().parents[1] / "assets/other_images/frogpilot_boot_logo.jpg"
  elif stock:
    target_logo = Path(__file__).resolve().parents[1] / "assets/other_images/stock_bg.jpg"
  else:
    print("Error: Must specify either stock=True or frogpilot=True")
    return

  if not target_logo.is_file():
    print(f"Error: Target logo file not found at {target_logo}")
    return

  if target_logo.read_bytes() != boot_logo.read_bytes():
    mount_options = run_cmd(["findmnt", "-n", "-o", "OPTIONS", "/"], "Successfully retrieved mount options", "Failed to retrieve mount options")
    run_cmd(["sudo", "mount", "-o", "remount,rw", "/"], "Successfully remounted / as read-write", "Failed to remount /")
    run_cmd(["sudo", "cp", target_logo, boot_logo], "Successfully replaced boot logo", "Failed to replace boot logo")
    run_cmd(["sudo", "mount", "-o", f"remount,{mount_options}", "/"], "Successfully restored / mount options", "Failed to restore / mount options")
