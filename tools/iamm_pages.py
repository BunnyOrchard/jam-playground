#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""The IAMM-1 download. Not the HYP-1 site.

  index.html               what IAMM-1 is, the package, and the way in
  install/index.html       the web installer (Chrome or Edge, Web MIDI)
  firmware/iamm-X.Y.Z.fwsc the package that page writes

  python3 tools/iamm_pages.py build/amb-1.0.0.fwsc 1.0.0 download

The package must be a release build (./build.sh --release X.Y.Z). Its identity,
FM-1_7XXYYZZ, is what the installer checks and what the FM-1 reports afterwards.
"""
import html
import json
import re
import shutil
import sys
from pathlib import Path

SRC = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(SRC / "web"))
from make_site import product_of, strip_module  # noqa: E402


def page(version, pkg_name, product):
    v = html.escape(version)
    p = html.escape(pkg_name)
    ident = html.escape(product)
    return f"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>IAMM-1 {v}</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Chakra+Petch:wght@600;700&family=Barlow+Semi+Condensed:wght@400;600&display=swap">
<style>
  :root {{ --ground: #141318; --card: #1f1c27; --ink: #f1ecf4; --muted: #aaa1b8; --rule: #3e3a4c;
           --orange: #ff8020; color-scheme: dark; }}
  * {{ box-sizing: border-box; }}
  body {{ margin: 0; background: var(--ground); color: var(--ink);
          font: 400 18px/1.55 "Barlow Semi Condensed", "Helvetica Neue", Arial, sans-serif; }}
  main {{ max-width: 40rem; margin: 0 auto; padding: 2rem 1rem 4rem; }}
  h1, h2, a.btn {{ font-family: "Chakra Petch", "Helvetica Neue", Arial, sans-serif; }}
  h1 {{ font-size: 2rem; letter-spacing: .06em; text-transform: uppercase; margin: 0 0 .4rem; }}
  h2 {{ font-size: 1.05rem; letter-spacing: .08em; text-transform: uppercase; margin: 2rem 0 .6rem; }}
  .ver {{ color: var(--orange); font-weight: 600; letter-spacing: .04em; }}
  a {{ color: var(--ink); }}
  a.btn {{ display: inline-block; margin: .3rem .6rem .3rem 0; padding: .55rem 1rem; background: var(--orange);
           color: #1a0c03; text-decoration: none; border-radius: 8px; font-weight: 700; letter-spacing: .06em; }}
  .card {{ background: var(--card); border: 1px solid var(--rule); border-radius: 10px; padding: 1rem 1.1rem; }}
  .small {{ color: var(--muted); font-size: 15px; }}
  code {{ font-size: .95rem; }}
</style>
</head>
<body>
<main>
  <h1>IAMM-1</h1>
  <p class="ver">version {v} · {ident}</p>
  <p>Power on and it is already playing. A new boot is a new song. Four synth keys can sound at once. A fifth turns the oldest off. Eleven drums sit beside them.</p>
  <p>
    <a class="btn" href="install/">Install in the browser</a>
    <a class="btn" href="firmware/{p}">Download the package</a>
  </p>
  <div class="card">
    <p>Chrome or Edge, with the FM-1 on a USB data cable. The installer page writes this package. Do not unplug the cable while it writes.</p>
    <p class="small">From a terminal, with <code>mido</code> and <code>python-rtmidi</code>: <code>python3 tools/fm1_install.py firmware/{p}</code></p>
  </div>
  <h2>If the write stalls</h2>
  <p>The package can finish writing and the unit can come back while the page is still saying it is restarting. Unplug nothing. If the screen is playing, the install finished. If the screen says it is in update mode, plug the cable in again and press Install once more. It finishes the write.</p>
  <h2>Back to stock</h2>
  <p>The install page can write M-VAVE's FM-1 V15 back. Songs stored on the unit can be lost. Installing firmware is at your own risk. IAMM-1 is not affiliated with M-VAVE.</p>
  <p class="small">IAMM-1 is a fork of HYP-1 by RedtailWorks, a fork of AMB-1 by Charles Vestal, on Felucca by Leo Kuroshita and Hügelton Instruments. GPL-3.0-only. The synth, the drums, and the room are theirs.</p>
</main>
</body>
</html>
"""


def installer(version, pkg_name, product):
    raw = (SRC / "web" / "amb_installer.html").read_text(encoding="utf-8")
    lib = strip_module((SRC / "web" / "fm1pkg.js").read_text(encoding="utf-8")) + "\n" + \
        strip_module((SRC / "web" / "fm1ota.js").read_text(encoding="utf-8"))
    meta = json.dumps({"version": version, "product": product, "pkg": "../firmware/" + pkg_name})
    for mark in ("/*LIB*/", "/*META*/"):
        if raw.count(mark) != 1:
            raise SystemExit(f"amb_installer.html must contain {mark} once")
    page = raw.replace("/*LIB*/", lib).replace("/*META*/", meta)
    page = page.replace("<title>HYP-1 installer</title>", "<title>IAMM-1 installer</title>")
    page = page.replace("<h1>Install HYP-1</h1>", "<h1>Install IAMM-1</h1>")
    page = page.replace('<img src="../img/panda.png" width="48" height="38" alt="">HYP-1', "IAMM-1")
    page = page.replace("Back to HYP-1", "Back to IAMM-1")
    page = page.replace("HYP-1 のページへ", "IAMM-1 のページへ")
    page = page.replace("from HYP-1 (or its safe mode, X0X or Felucca)", "from IAMM-1 (or its safe mode)")
    page = page.replace("HYP-1（またはそのセーフモード）から同じ USB ケーブルで書き込みます。HYP-1 で保存した曲の設定は消えることがあります。",
                        "IAMM-1（またはそのセーフモード）から同じ USB ケーブルで書き込みます。本体に保存した曲は消えることがあります。")
    page = page.replace("The pieces saved in HYP-1 may be lost.", "Songs saved on the unit may be lost.")
    page = page.replace("HYP-1 ${meta.version}", "IAMM-1 ${meta.version}")
    page = page.replace('href="../manual/#recovering-an-fm-1-that-won-t-start"', 'href="../"')
    # the HYP nav points at pages this download does not ship
    page = re.sub(r'<nav>.*?</nav>', '<nav><a href="../">IAMM-1</a><a href="../" aria-current="page">Install</a></nav>', page, count=1)
    old = ("HYP-1 is a <a href=\"https://www.redtail.works\" style=\"color:inherit\">RedtailWorks</a> project "
           "(<a href=\"https://www.youtube.com/@RedtailWorks\" style=\"color:inherit\">YouTube</a>), a fork of AMB-1 by Charles Vestal, "
           "built on Felucca by Leo Kuroshita (@kurogedelic), <a href=\"https://hugelton.com\" target=\"_blank\" rel=\"noopener\" style=\"color:inherit\">Hügelton Instruments</a>,\n"
           "    by way of X0X and FoMni.<br>")
    new = ("IAMM-1 is a fork of HYP-1 by RedtailWorks, a fork of AMB-1 by Charles Vestal, "
           "on Felucca by Leo Kuroshita (@kurogedelic), <a href=\"https://hugelton.com\" target=\"_blank\" rel=\"noopener\" style=\"color:inherit\">Hügelton Instruments</a>.<br>")
    if old not in page:
        raise SystemExit("installer footer did not match; the HYP-1 page changed")
    return page.replace(old, new)


def main(argv):
    if len(argv) != 4:
        raise SystemExit("usage: tools/iamm_pages.py PACKAGE.fwsc X.Y.Z OUT_DIR")
    pkg, version, out = Path(argv[1]), argv[2], Path(argv[3])
    if not re.fullmatch(r"\d{1,2}\.\d{1,2}\.\d{1,2}", version):
        raise SystemExit(f"{version}: use X.Y.Z")
    raw = pkg.read_bytes()
    product = product_of(raw)
    if not re.fullmatch(r"FM-1_7\d{6}", product):
        raise SystemExit(f"{pkg}: identity {product!r}: make a release build (./build.sh --release X.Y.Z)")
    if b"FELUCCA-LOADER-1" not in raw:
        raise SystemExit(f"{pkg}: no update loader in it")
    name = f"iamm-{version}.fwsc"
    if out.exists():
        shutil.rmtree(out)
    (out / "install").mkdir(parents=True)
    (out / "firmware").mkdir()
    shutil.copy(pkg, out / "firmware" / name)
    (out / "index.html").write_text(page(version, name, product), encoding="utf-8")
    (out / "install" / "index.html").write_text(installer(version, name, product), encoding="utf-8")
    (out / ".nojekyll").write_text("")
    print(f"download: {out}  {name}  {len(raw)} B  {product}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
