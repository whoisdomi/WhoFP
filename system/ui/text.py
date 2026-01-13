#!/usr/bin/env python3
import re
import subprocess
import sys
import pyray as rl
from openpilot.system.hardware import HARDWARE, PC
from openpilot.system.ui.lib.application import gui_app
from openpilot.system.ui.lib.scroll_panel import GuiScrollPanel
from openpilot.system.ui.lib.text_measure import measure_text_cached
from openpilot.system.ui.widgets import Widget
from openpilot.system.ui.widgets.button import Button, ButtonStyle


def get_network_info() -> str:
  """Get current network SSID and IP address for SSH debugging."""
  if PC:
    return ""

  info_parts = []
  try:
    # Get connected WiFi SSID
    ssid = subprocess.check_output(["iwgetid", "-r"], encoding='utf-8', stderr=subprocess.DEVNULL).strip()
    if ssid:
      info_parts.append(f"WiFi: {ssid}")
  except Exception:
    pass

  try:
    # Get IP address (first non-localhost IP)
    ip_output = subprocess.check_output(["hostname", "-I"], encoding='utf-8', stderr=subprocess.DEVNULL).strip()
    if ip_output:
      ip = ip_output.split()[0]  # Take first IP
      info_parts.append(f"IP: {ip}")
  except Exception:
    pass

  return "  |  ".join(info_parts) if info_parts else ""

if gui_app.big_ui():
  MARGIN = 50
  SPACING = 40
  FONT_SIZE = 72
  LINE_HEIGHT = 80
  BUTTON_SIZE = rl.Vector2(310, 160)
else:
  MARGIN = 20
  SPACING = 30
  FONT_SIZE = 25
  LINE_HEIGHT = 25
  BUTTON_SIZE = rl.Vector2(150, 80)

DEMO_TEXT = """This is a sample text that will be wrapped and scrolled if necessary.
            The text is long enough to demonstrate scrolling and word wrapping.""" * 30


def wrap_text(text, font_size, max_width):
  lines = []
  font = gui_app.font()

  for paragraph in text.split("\n"):
    if not paragraph.strip():
      # Don't add empty lines first, ensuring wrap_text("") returns []
      if lines:
        lines.append("")
      continue
    indent = re.match(r"^\s*", paragraph).group()
    current_line = indent
    words = re.split(r"(\s+|-)", paragraph[len(indent):])
    while len(words):
      word = words.pop(0)
      test_line = current_line + word + (words.pop(0) if words else "")
      if measure_text_cached(font, test_line, font_size).x <= max_width:
        current_line = test_line
      else:
        lines.append(current_line)
        current_line = word + " "
    current_line = current_line.rstrip()
    if current_line:
      lines.append(current_line)

  return lines


class TextWindow(Widget):
  def __init__(self, text: str):
    super().__init__()
    self._textarea_rect = rl.Rectangle(MARGIN, MARGIN, gui_app.width - MARGIN * 2, gui_app.height - MARGIN * 2)
    self._wrapped_lines = wrap_text(text, FONT_SIZE, self._textarea_rect.width - 20)
    self._content_rect = rl.Rectangle(0, 0, self._textarea_rect.width - 20, len(self._wrapped_lines) * LINE_HEIGHT)
    self._scroll_panel = GuiScrollPanel()
    self._scroll_panel._offset_filter_y.x = -max(self._content_rect.height - self._textarea_rect.height, 0)

    button_text = "Exit" if PC else "Reboot"
    self._button = Button(button_text, click_callback=self._on_button_clicked, button_style=ButtonStyle.TRANSPARENT_WHITE_BORDER, font_size=FONT_SIZE)

    # Get network info for SSH debugging
    self._network_info = get_network_info()

  @staticmethod
  def _on_button_clicked():
    gui_app.request_close()
    if not PC:
      HARDWARE.reboot()

  def _render(self, rect: rl.Rectangle):
    scroll = self._scroll_panel.update(self._textarea_rect, self._content_rect)
    rl.begin_scissor_mode(int(self._textarea_rect.x), int(self._textarea_rect.y), int(self._textarea_rect.width), int(self._textarea_rect.height))
    for i, line in enumerate(self._wrapped_lines):
      position = rl.Vector2(self._textarea_rect.x, self._textarea_rect.y + scroll + i * LINE_HEIGHT)
      if position.y + LINE_HEIGHT < self._textarea_rect.y or position.y > self._textarea_rect.y + self._textarea_rect.height:
        continue
      rl.draw_text_ex(gui_app.font(), line, position, FONT_SIZE, 0, rl.WHITE)
    rl.end_scissor_mode()

    # Draw network info at bottom left (for SSH debugging)
    if self._network_info:
      network_font_size = FONT_SIZE * 0.6
      network_y = rect.height - MARGIN - BUTTON_SIZE.y / 2 - network_font_size / 2
      rl.draw_text_ex(gui_app.font(), self._network_info, rl.Vector2(MARGIN, network_y), network_font_size, 0, rl.Color(180, 180, 180, 255))

    button_bounds = rl.Rectangle(rect.width - MARGIN - BUTTON_SIZE.x - SPACING, rect.height - MARGIN - BUTTON_SIZE.y, BUTTON_SIZE.x, BUTTON_SIZE.y)
    self._button.render(button_bounds)


if __name__ == "__main__":
  text = sys.argv[1] if len(sys.argv) > 1 else DEMO_TEXT
  gui_app.init_window("Text Viewer")
  text_window = TextWindow(text)
  for _ in gui_app.render():
    text_window.render(rl.Rectangle(0, 0, gui_app.width, gui_app.height))
