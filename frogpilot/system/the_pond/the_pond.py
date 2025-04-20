#!/usr/bin/env python3
from datetime import datetime
from flask import Flask, Response, jsonify, render_template, request, send_from_directory

import json
import os
import requests
import secrets
import time

from openpilot.common.realtime import DT_HW
from openpilot.system.hardware import PC
from openpilot.system.hardware.hw import Paths
from openpilot.system.version import get_build_metadata

from openpilot.frogpilot.common.frogpilot_variables import ERROR_LOGS_PATH, params, params_cache
from openpilot.frogpilot.system.the_pond import helpers, utilities

FOOTAGE_PATHS = [
  Paths.log_root(HD=True, raw=True),
  Paths.log_root(konik=True, raw=True),
  Paths.log_root(raw=True),
]

KEYS = {
  "amap1": ("amap1", "", "AMapKey1", "Amap key #1", 39),
  "amap2": ("amap2", "", "AMapKey2", "Amap key #2", 39),
  "public": ("public", "pk.", "MapboxPublicKey", "Public key", 80),
  "secret": ("secret", "sk.", "MapboxSecretKey", "Secret key", 80),
}

def setup(app):
  @app.errorhandler(404)
  def not_found(_):
    return render_template("index.html")

  @app.route("/")
  def index():
    return render_template("index.html")

  @app.route("/api/navigation/favorite", methods=["POST"])
  def add_favorite_destination():
    existing = json.loads(params.get("FavoriteDestinations", encoding="utf8") or "[]")

    favorite_to_add = request.json
    if favorite_to_add not in existing:
      existing.append(favorite_to_add)

    params.put("FavoriteDestinations", json.dumps(existing))
    return {"message": "Destination added to favorites!"}

  @app.route("/api/navigation/favorite", methods=["GET"])
  def list_favorite_destinations():
    favorites = json.loads(params.get("FavoriteDestinations", encoding="utf8") or "[]")
    return jsonify(favorites=favorites)

  @app.route("/api/navigation/favorite", methods=["DELETE"])
  def remove_favorite_destination():
    existing = json.loads(params.get("FavoriteDestinations", encoding="utf8") or "[]")
    to_remove = request.json
    existing = [
      f for f in existing
      if not (
        f.get("name") == to_remove.get("name") and
        f.get("latitude") == to_remove.get("latitude") and
        f.get("longitude") == to_remove.get("longitude") and
        f.get("routeId") == to_remove.get("routeId")
      )
    ]
    params.put("FavoriteDestinations", json.dumps(existing))
    return jsonify(message="Destination removed from favorites!")

  @app.route("/api/navigation", methods=["DELETE"])
  def clear_navigation():
    params.remove("NavDestination")
    return {"message": "Destination cleared"}

  @app.route("/api/navigation", methods=["GET"])
  def navigation():
    last_position = json.loads(
      params.get("LastGPSPosition", encoding="utf8") or
      "{\"latitude\": 51.276824158421331, \"longitude\": 30.221928335547232, \"altitude\": 111.000000000000000}"
    )

    latitude = str(last_position["latitude"])
    longitude = str(last_position["longitude"])

    return {
      "amap1Key": params_cache.get("AMapKey1", encoding="utf8") or "",
      "amap2Key": params_cache.get("AMapKey2", encoding="utf8") or "",
      "destination": params.get("NavDestination", encoding="utf8") or "",
      "isMetric": params.get_bool("IsMetric"),
      "lastPosition": {"latitude": latitude, "longitude": longitude},
      "mapboxPublic": params_cache.get("MapboxPublicKey", encoding="utf8") or "",
      "mapboxSecret": params_cache.get("MapboxSecretKey", encoding="utf8") or "",
      "previousDestinations": params.get("ApiCache_NavDestinations", encoding="utf8") or "",
    }

  @app.route("/api/navigation_key", methods=["DELETE"])
  def delete_navigation_key():
    meta = KEYS.get(request.args.get("type"))
    params_cache.remove(meta[2])
    return jsonify(message=f"{meta[3]} deleted successfully!")

  @app.route("/api/navigation_key", methods=["POST"])
  def set_navigation_keys():
    data = request.get_json() or {}

    saved = []
    for meta in KEYS.values():
      raw = (data.get(meta[0]) or "").strip()
      if not raw:
        continue

      full = raw if raw.startswith(meta[1]) else meta[1] + raw
      if len(full) < meta[4]:
        return jsonify(error=f"{meta[3]} is invalid or too short..."), 400

      params_cache.put(meta[2], full)

      saved.append(meta[3])

    if not saved:
      return jsonify(error="Nothing to update"), 400

    return jsonify(message=f"{', '.join(saved)} saved successfully!")

  @app.route("/api/navigation", methods=["POST"])
  def set_navigation():
    params.remove("NavDestination")

    time.sleep(1)

    params.put("NavDestination", json.dumps(request.json))
    return {"message": "Destination set"}

  @app.route("/api/routes")
  def list_routes():
    routes = []
    for footage_path in FOOTAGE_PATHS:
      for name in helpers.get_routes_names(footage_path):
        path = f"{footage_path}{name}--0"
        qcamera = f"{path}/qcamera.ts"

        utilities.video_to_gif(qcamera, f"{path}/preview.gif")
        utilities.video_to_png(qcamera, f"{path}/preview.png")

        routes.append({
          "name": name,
          "gif": f"/thumbnails/{name}--0/preview.gif",
          "png": f"/thumbnails/{name}--0/preview.png",
        })
    return routes, 200

  @app.route("/api/error_logs")
  def get_error_logs():
    if request.accept_mimetypes["text/html"]:
      return render_template("v2/error-logs.jinja", active="error_logs")

    if request.accept_mimetypes["application/json"]:
      return helpers.list_file(ERROR_LOGS_PATH), 200

  @app.route("/api/error_logs/<filename>")
  def get_error_log(filename):
    with open(os.path.join(ERROR_LOGS_PATH, filename)) as file:
      return file.read(), 200, {"Content-Type": "text/plain; charset=utf-8"}

  @app.route("/api/params")
  def get_param():
    return params.get(request.args.get("key")) or "", 200

  @app.route("/api/routes/<name>")
  def get_route(name):
    for footage_path in FOOTAGE_PATHS:
      base_path = f"{footage_path}{name}--0"
      if os.path.exists(base_path):
        segments = helpers.get_segments_in_route(name, footage_path)
        segment_urls = [f"/video/{segment}" for segment in segments]

        if not segment_urls:
          break

        last_segment_path = f"{footage_path}{name}--{len(segment_urls)-1}/qcamera.ts"
        last_duration = utilities.get_video_duration(last_segment_path)
        total_duration = round(last_duration + ((len(segment_urls) - 1) * 60))

        available_cameras = utilities.get_available_cameras(base_path)
        route_date = datetime.strptime(name, "%Y-%m-%d--%H-%M-%S")

        return {
          "name": name,
          "segment_urls": segment_urls,
          "total_duration": total_duration,
          "date": route_date,
          "available_cameras": available_cameras,
        }, 200

    return {"error": "Route not found"}, 404

  @app.route("/api/stats")
  def get_stats():
    build_metadata = get_build_metadata()

    short_branch = build_metadata.channel
    if short_branch == "FrogPilot-Development":
      env = "Development"
    elif build_metadata.release_channel:
      env = "Release"
    elif short_branch == "FrogPilot-Testing":
      env = "Testing"
    elif build_metadata.tested_channel:
      env = "Staging"
    else:
      env = short_branch

    return {
      "driveStats": utilities.get_drive_stats(),
      "diskUsage": utilities.get_disk_usage(),
      "firehoseStats": {"segments": requests.get(f"https://api.comma.ai/v1/devices/{params.get('DongleId', encoding='utf8')}/firehose_stats", timeout=10).json().get("firehose", 0)},
      "softwareInfo": {
        "branchName": build_metadata.channel,
        "buildEnvironment": env,
        "commitHash": build_metadata.openpilot.git_commit,
        "forkMaintainer": utilities.get_repo_owner(build_metadata.openpilot.git_normalized_origin),
        "updateAvailable": "Yes" if params.get_bool("UpdaterFetchAvailable") else "No",
        "versionDate": utilities.format_git_date(build_metadata.openpilot.git_commit_date),
      },
    }

  @app.route("/thumbnails/<path:file_path>")
  def get_thumbnail(file_path):
    for footage_path in FOOTAGE_PATHS:
      try:
        return send_from_directory(footage_path, file_path, as_attachment=True)
      except FileNotFoundError:
        continue
    return {"error": "Thumbnail not found"}, 404

  @app.route("/video/<path>")
  def get_video(path):
    camera = request.args.get("camera")
    filename = {"driver": "dcamera.hevc", "wide": "ecamera.hevc"}.get(camera, "qcamera.ts")

    for footage_path in FOOTAGE_PATHS:
      filepath = f"{footage_path}{path}/{filename}"
      if os.path.exists(filepath):
        process = helpers.ffmpeg_mp4_wrap_process_builder(filepath)
        return Response(process.stdout.read(), status=200, mimetype="video/mp4")

    return {"error": "Video not found"}, 404

  @app.route("/playground")
  def playground():
    return render_template("playground.html")

  @app.route('/mapbox-help/<path:filename>')
  def serve_mapbox_help(filename):
    return send_from_directory("/data/openpilot/frogpilot/navigation/navigation_training", filename)

def main():
  app = Flask(__name__, static_folder="assets", static_url_path="/assets")
  setup(app)

  debug = PC or __package__ == "the_pond"
  port = 8084 if debug else 8083

  if debug:
    print("\"The Pond\" is not running on a comma device, enabling debug mode")

  app.secret_key = secrets.token_hex(32)
  app.run(host="0.0.0.0", port=port, debug=debug)

if __name__ == "__main__":
  main()
