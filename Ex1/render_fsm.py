"""Render the Lab 3 Ex1 draw.io FSM to a report-ready PNG."""

from html import unescape
from pathlib import Path
import re
import xml.etree.ElementTree as ET

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "Lab3_Ex1_FSM.drawio"
OUTPUT = ROOT / "Lab3_Ex1_FSM.png"
SCALE = 2
WIDTH, HEIGHT = 1800, 1000

tree = ET.parse(SOURCE)
cells = {cell.get("id"): cell for cell in tree.findall(".//mxCell")}
canvas = Image.new("RGB", (WIDTH * SCALE, HEIGHT * SCALE), "white")
draw = ImageDraw.Draw(canvas)

font_path = Path(r"C:\Windows\Fonts\arial.ttf")
bold_path = Path(r"C:\Windows\Fonts\arialbd.ttf")
font = ImageFont.truetype(str(font_path), 17 * SCALE)
bold = ImageFont.truetype(str(bold_path), 17 * SCALE)
header = ImageFont.truetype(str(bold_path), 30 * SCALE)
section = ImageFont.truetype(str(bold_path), 22 * SCALE)
label_font = ImageFont.truetype(str(font_path), 15 * SCALE)


def p(x, y):
    return (round(x * SCALE), round(y * SCALE))


def geom(cell_id):
    g = cells[cell_id].find("mxGeometry")
    return tuple(float(g.get(k)) for k in ("x", "y", "width", "height"))


def lines(cell_id):
    value = unescape(cells[cell_id].get("value", ""))
    value = re.sub(r"<br\s*/?>", "\n", value, flags=re.I)
    return value.split("\n")


def centered_multiline(cell_id, box, color="#26394D"):
    x, y, w, h = box
    text_lines = lines(cell_id)
    if cell_id in {"modeNote", "trafficNote"}:
        words = text_lines[0].split()
        wrapped = []
        current = ""
        for word in words:
            candidate = f"{current} {word}".strip()
            if draw.textlength(candidate, font=font) > (w - 50) * SCALE and current:
                wrapped.append(current)
                current = word
            else:
                current = candidate
        if current:
            wrapped.append(current)
        text_lines = wrapped
    line_height = 25
    total_height = len(text_lines) * line_height
    yy = y + (h - total_height) / 2
    for i, line in enumerate(text_lines):
        this_font = font if cell_id in {"modeNote", "trafficNote"} else (bold if i == 0 else font)
        if line:
            draw.text(p(x + w / 2, yy), line, font=this_font, fill=color,
                      anchor="mt")
        yy += line_height


def rounded_cell(cell_id, fill, outline):
    box = geom(cell_id)
    x, y, w, h = box
    draw.rounded_rectangle((*p(x, y), *p(x + w, y + h)),
                           radius=14 * SCALE, fill=fill,
                           outline=outline, width=3 * SCALE)
    centered_multiline(cell_id, box)


def arrow(points, label=None, label_at=None):
    scaled = [p(x, y) for x, y in points]
    draw.line(scaled, fill="#17365D", width=4 * SCALE, joint="curve")
    x1, y1 = scaled[-2]
    x2, y2 = scaled[-1]
    if abs(x2 - x1) >= abs(y2 - y1):
        direction = 1 if x2 > x1 else -1
        head = [(x2, y2), (x2 - direction * 12 * SCALE, y2 - 7 * SCALE),
                (x2 - direction * 12 * SCALE, y2 + 7 * SCALE)]
    else:
        direction = 1 if y2 > y1 else -1
        head = [(x2, y2), (x2 - 7 * SCALE, y2 - direction * 12 * SCALE),
                (x2 + 7 * SCALE, y2 - direction * 12 * SCALE)]
    draw.polygon(head, fill="#17365D")
    if label and label_at:
        tx, ty = label_at
        bbox = draw.textbbox((0, 0), label, font=label_font)
        tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
        cx, cy = p(tx, ty)
        draw.rounded_rectangle((cx - tw / 2 - 9, cy - th / 2 - 5,
                                cx + tw / 2 + 9, cy + th / 2 + 5),
                               radius=5 * SCALE, fill="white")
        draw.text((cx, cy), label, font=label_font, fill="#17365D",
                  anchor="mm")


draw.text(p(900, 28), lines("title")[0], font=header, fill="#17365D",
          anchor="mt")
draw.text(p(70, 100), lines("modeTitle")[0], font=section,
          fill="#0B5D91", anchor="lt")
draw.text(p(70, 505), lines("trafficTitle")[0], font=section,
          fill="#0B5D91", anchor="lt")

for cell_id, fill, outline in [
    ("m1", "#E8F1FB", "#2672A9"),
    ("m2", "#FDECEB", "#C23B3B"),
    ("m3", "#FFF5D9", "#C68B14"),
    ("m4", "#E8F5EA", "#2A8A4B"),
    ("t1", "#E8F5EA", "#2A8A4B"),
    ("t2", "#FFF5D9", "#C68B14"),
    ("t3", "#E8F5EA", "#2A8A4B"),
    ("t4", "#FFF5D9", "#C68B14"),
    ("modeNote", "#F4F6F9", "#8192A6"),
    ("trafficNote", "#F4F6F9", "#8192A6"),
]:
    rounded_cell(cell_id, fill, outline)

for cx, cy in [(52, 217), (52, 637)]:
    draw.ellipse((*p(cx - 12, cy - 12), *p(cx + 12, cy + 12)), fill="#17365D")

arrow([(64, 217), (95, 217)], "Start", (80, 192))
arrow([(435, 227), (505, 227)], "MODE press", (470, 190))
arrow([(845, 227), (915, 227)], "MODE press", (880, 190))
arrow([(1255, 227), (1325, 227)], "MODE press", (1290, 190))
arrow([(1495, 300), (1495, 335), (265, 335), (265, 300)],
      "MODE press", (890, 335))

arrow([(64, 637), (95, 637)], "Start", (80, 610))
arrow([(435, 647), (505, 647)], "Timer = 0", (470, 612))
arrow([(845, 647), (915, 647)], "Timer = 0", (880, 612))
arrow([(1255, 647), (1325, 647)], "Timer = 0", (1290, 612))
arrow([(1495, 720), (1495, 795), (265, 795), (265, 720)],
      "Timer = 0", (890, 795))

canvas.save(OUTPUT, optimize=True)
print(OUTPUT)
