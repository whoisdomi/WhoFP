#!/usr/bin/env python3
import requests
import tempfile

from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Optional

from openpilot.frogpilot.common.frogpilot_utilities import delete_file, is_url_pingable
from openpilot.frogpilot.common.frogpilot_variables import RESOURCES_REPO

GITHUB_URL = f"https://raw.githubusercontent.com/{RESOURCES_REPO}"
GITLAB_URL = f"https://gitlab.com/{RESOURCES_REPO}/-/raw"

def download_file(cancel_param: str, destination: Path, progress_param: str, url: str, download_param: str, session: requests.Session, params_memory: Any, offset_bytes: int = 0, total_bytes: int = 0) -> None:
  try:
    destination.parent.mkdir(parents=True, exist_ok=True)

    with session.get(url, stream=True, timeout=10) as response:
      if response.status_code == 404 and url.endswith(".gif"):
        print(f"GIF download failed (404). Attempting fallback to PNG for {destination.name}")
        return download_file(cancel_param, destination.with_suffix(".png"), progress_param, url.replace(".gif", ".png"), download_param, session, params_memory, offset_bytes, total_bytes)

      response.raise_for_status()

      total_size = int(response.headers.get("Content-Length", 0))
      if total_size == 0:
        handle_error(None, "Download invalid...", "Download invalid...", download_param, progress_param, params_memory)
        return

      with tempfile.NamedTemporaryFile(delete=False, dir=destination.parent, suffix=".tmp") as temp_file:
        downloaded_size = 0

        temp_file_path = Path(temp_file.name)

        try:
          for chunk in response.iter_content(chunk_size=16384):
            if params_memory.get_bool(cancel_param):
              raise InterruptedError

            if chunk:
              temp_file.write(chunk)
              downloaded_size += len(chunk)

              if total_bytes:
                overall_progress = (offset_bytes + downloaded_size) / total_bytes * 100
              elif total_size > 0:
                overall_progress = downloaded_size / total_size * 100
              else:
                overall_progress = 0

              if overall_progress < 100:
                params_memory.put(progress_param, f"{overall_progress:.0f}%")
              else:
                params_memory.put(progress_param, "Verifying authenticity...")

        except InterruptedError:
          temp_file_path.unlink(missing_ok=True)
          handle_error(None, "Download cancelled...", "Download cancelled...", download_param, progress_param, params_memory)
          return
        except Exception:
          temp_file_path.unlink(missing_ok=True)
          raise

        temp_file_path.replace(destination)

  except Exception as exception:
    handle_request_error(exception, destination, download_param, progress_param, params_memory)


def get_remote_file_size(url: str, session: requests.Session, params_memory: Any) -> int:
  try:
    response = session.head(url, headers={"Accept-Encoding": "identity"}, timeout=10)
    response.raise_for_status()
    return int(response.headers.get("Content-Length", 0))
  except Exception as exception:
    handle_request_error(exception, None, None, None, params_memory)
    return 0


def get_repository_url(session: requests.Session) -> Optional[str]:
  if is_url_pingable("https://github.com") and not github_rate_limited(session):
    return GITHUB_URL
  if is_url_pingable("https://gitlab.com"):
    return GITLAB_URL
  return None


def github_rate_limited(session: requests.Session) -> bool:
  try:
    response = session.get("https://api.github.com/rate_limit", timeout=10)
    response.raise_for_status()
    rate_limit_info = response.json()
    remaining = rate_limit_info["rate"]["remaining"]
    print(f"GitHub API Requests Remaining: {remaining}")

    if remaining <= 0:
      reset_timestamp = rate_limit_info["rate"]["reset"]
      reset_time = datetime.fromtimestamp(reset_timestamp, tz=timezone.utc).strftime("%Y-%m-%d %H:%M:%S")
      print("GitHub rate limit reached")
      print(f"GitHub Rate Limit Resets At (UTC): {reset_time}")
      return True
    return False

  except requests.exceptions.RequestException as exception:
    print(f"Error checking GitHub rate limit: {exception}")
    return True


def handle_error(destination: Optional[Path], error_message: str, error: Any, download_param: Optional[str], progress_param: Optional[str], params_memory: Any) -> None:
  if destination:
    delete_file(destination)

  if progress_param and "404" not in error_message:
    print(f"Error occurred: {error}")
    params_memory.put(progress_param, error_message)
    params_memory.remove(download_param)


def handle_request_error(error: Exception, destination: Optional[Path], download_param: Optional[str], progress_param: Optional[str], params_memory: Any) -> None:
  if isinstance(error, requests.exceptions.HTTPError) and error.response is not None:
    error_message = f"Server error ({error.response.status_code})"
  else:
    error_map = {
      requests.exceptions.ChunkedEncodingError: "Connection dropped",
      requests.exceptions.ConnectionError: "Connection dropped",
      requests.exceptions.ReadTimeout: "Read timed out",
      requests.exceptions.Timeout: "Download timed out",
    }
    error_message = error_map.get(type(error))
    if not error_message:
      if isinstance(error, requests.exceptions.RequestException):
        error_message = "Network request error. Check connection"
      else:
        error_message = "Unexpected error"

  handle_error(destination, f"Failed: {error_message}", error, download_param, progress_param, params_memory)


def verify_download(file_path: Path, url: str, session: requests.Session, params_memory: Any) -> bool:
  remote_file_size = get_remote_file_size(url, session, params_memory)

  if remote_file_size == 0:
    print(f"Error fetching remote size for {file_path}")
    return False

  if not file_path.is_file():
    print(f"File not found: {file_path}")
    return False

  local_size = file_path.stat().st_size
  if remote_file_size != local_size:
    print(f"File size mismatch for {file_path}: Remote {remote_file_size} vs Local {local_size}")
    return False

  return True
