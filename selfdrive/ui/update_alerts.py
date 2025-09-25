#!/usr/bin/env python3
import ast
from pathlib import Path
import re
import sys

# WIP! USE at you risk and don't forget to backup your alert_tr.h
EVENTS_PY = Path(__file__).parent.parent.parent / "controls/lib/events.py"
ALERT_TR_H = Path(__file__).parent.parent.parent / "ui/qt/onroad/alert_tr.h"
FROGPILOT_VARS = Path(__file__).parent.parent.parent.parent / "frogpilot/common/frogpilot_variables.py"

# --- Extract StartupMessageTop/Bottom as two separate text1/text2 ---
startup_messages = []
with open(FROGPILOT_VARS, "r", encoding="utf-8") as f:
    content = f.read()

for match in re.finditer(
    r'\("(?P<name>StartupMessageTop|StartupMessageBottom)",\s*"(?P<line1>[^"]*)",\s*[^,]*,\s*"(?P<line2>[^"]*)"\)',
    content
):
    text1 = match.group("line1")
    text2 = match.group("line2")
    startup_messages.append((text1, text2))

class AlertExtractor(ast.NodeVisitor):
    def __init__(self):
        self.alerts = []

    def visit_Call(self, node):
        if isinstance(node.func, ast.Name) and node.func.id.endswith("Alert"):
            text1, text2 = None, None
            if len(node.args) >= 1:
                text1 = self.get_text(node.args[0])
            if len(node.args) >= 2:
                text2 = self.get_text(node.args[1])
            self.alerts.append({"text1": text1, "text2": text2})
        self.generic_visit(node)

    def get_text(self, node):
        if isinstance(node, ast.Constant) and isinstance(node.value, str):
            return node.value.replace("\n", "<br>")
        elif isinstance(node, ast.JoinedStr):  # f-string
            parts = []
            counter = 1
            for v in node.values:
                if isinstance(v, ast.Str):
                    parts.append(v.s.replace("\n", "<br>"))
                elif isinstance(v, ast.FormattedValue):
                    parts.append(f"%{counter}")
                    counter += 1
            return "".join(parts)
        return None

# Parsing events.py
with open(EVENTS_PY, "r", encoding="utf-8") as f:
    tree = ast.parse(f.read())

extractor = AlertExtractor()
extractor.visit(tree)

# Generating a C++ array
lines = ["inline std::vector<AlertTranslation> alertTranslations = {"]


# --- add startup messages at the beginning ---
# Combine StartupMessageTop and StartupMessageBottom by transposing their lines
if len(startup_messages) == 2:
    # Alert 1: text1 from Top, text2 from Bottom
    lines.append(f'  {{"{startup_messages[1][0]}", "{startup_messages[0][0]}", QT_TRANSLATE_NOOP("Alerts", "{startup_messages[1][0]}"), QT_TRANSLATE_NOOP("Alerts", "{startup_messages[0][0]}")}},')
    # Alert 2: text1 from Bottom, text2 from Top
    lines.append(f'  {{"{startup_messages[1][1]}", "{startup_messages[0][1]}", QT_TRANSLATE_NOOP("Alerts", "{startup_messages[1][1]}"), QT_TRANSLATE_NOOP("Alerts", "{startup_messages[0][1]}")}},')
else:
    # fallback: original behavior
    for text1, text2 in startup_messages:
        lines.append(f'  {{"{text1}", "{text2}", {{QT_TRANSLATE_NOOP("Alerts", "{text1}"), QT_TRANSLATE_NOOP("Alerts", "{text2}")}}}},')

# --- add all alerts from events.py ---
for a in extractor.alerts:
    t1 = a['text1'] if a['text1'] else ""
    t2 = a['text2'] if a['text2'] else ""
    if t1 or t2: lines.append(f'  {{"{t1}", "{t2}", QT_TRANSLATE_NOOP("Alerts", "{t1}"), QT_TRANSLATE_NOOP("Alerts", "{t2}")}},')

lines.append("};\n")

# Let's read the old alert_tr.h
with open(ALERT_TR_H, "r", encoding="utf-8") as f:
    content = f.read()

# Replacing the old array
new_content = re.sub(
    r'inline QMap<QString, AlertTranslation> alertTranslations = \{.*?\};',
    "\n".join(lines),
    content,
    flags=re.DOTALL
)

with open(ALERT_TR_H, "w", encoding="utf-8") as f:
    f.write(new_content)

print(f"Updated {ALERT_TR_H} with {len(extractor.alerts) + len(startup_messages)} alerts (including startup messages)")