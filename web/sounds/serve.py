#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""The sound check, on this machine. The page plays amb.c, the same engine as the FM-1.

    python3 web/sounds/serve.py

Then open the address it prints. It listens on this computer only.
"""
import os
import subprocess
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlparse

ROOT = Path(__file__).resolve().parents[2]
HEAR_C = ROOT / "web" / "sounds" / "hear.c"
AMB_C = ROOT / "firmware" / "src" / "dsp" / "amb.c"
HEAR = ROOT / "build" / "sounds" / "hear"
PAGE = ROOT / "web" / "sounds" / "index.html"
SDK = os.environ.get("SDKROOT", "/Library/Developer/CommandLineTools/SDKs/MacOSX26.sdk")
KINDS = {"pad", "bass", "acid", "air", "dust", "bed", "lane", "xdrum", "key"}
PORT = 8765


def build():
    HEAR.parent.mkdir(parents=True, exist_ok=True)
    newest = max(HEAR_C.stat().st_mtime, AMB_C.stat().st_mtime)
    if HEAR.is_file() and HEAR.stat().st_mtime >= newest:
        return
    cmd = [
        "cc", "-O2", "-ffp-contract=off", "-std=c99", "-Wall", "-Wextra", "-Wno-unused-function",
        "-DEN_HOST", "-Ifirmware/src/dsp", "-o", str(HEAR), str(HEAR_C), str(AMB_C), "-lm",
    ]
    env = os.environ.copy()
    env["SDKROOT"] = SDK
    print("compiling the engine...", flush=True)
    r = subprocess.run(cmd, cwd=ROOT, env=env)
    if r.returncode != 0:
        sys.exit("the sound engine did not compile")


def run_hear(args):
    r = subprocess.run([str(HEAR), *args], cwd=ROOT, capture_output=True, timeout=20)
    if r.returncode != 0:
        err = r.stderr.decode("utf-8", "replace")[-400:]
        raise RuntimeError(err or f"hear exited {r.returncode}")
    return r.stdout, r.stderr.decode("utf-8", "replace").strip()


class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        sys.stderr.write("%s\n" % (fmt % args))

    def do_GET(self):
        u = urlparse(self.path)
        if u.path in ("/", "/index.html"):
            body = PAGE.read_bytes()
            self._send(200, "text/html; charset=utf-8", body)
            return
        if u.path == "/catalog.json":
            try:
                body, _ = run_hear(["--list"])
            except Exception as e:
                self._send(500, "text/plain; charset=utf-8", str(e).encode())
                return
            self._send(200, "application/json", body)
            return
        if u.path == "/hear":
            q = parse_qs(u.query)
            kind = (q.get("kind") or [""])[0]
            if kind not in KINDS:
                self._send(400, "text/plain; charset=utf-8", b"unknown sound")
                return
            a = (q.get("a") or ["0"])[0]
            b = (q.get("b") or ["0"])[0]
            if not a.lstrip("-").isdigit() or not b.lstrip("-").isdigit():
                self._send(400, "text/plain; charset=utf-8", b"bad index")
                return
            try:
                body, peak = run_hear([kind, a, b])
            except Exception as e:
                self._send(500, "text/plain; charset=utf-8", str(e).encode())
                return
            self.send_response(200)
            self.send_header("Content-Type", "audio/wav")
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Cache-Control", "no-store")
            self.send_header("X-Peak", peak)
            self.end_headers()
            self.wfile.write(body)
            return
        self._send(404, "text/plain; charset=utf-8", b"not found")

    def _send(self, code, ctype, body):
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)


def main():
    build()
    port = PORT
    for _ in range(20):
        try:
            httpd = ThreadingHTTPServer(("127.0.0.1", port), Handler)
            break
        except OSError:
            port += 1
    else:
        sys.exit("no free port")
    print(f"Sound check: http://127.0.0.1:{port}/", flush=True)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print()


if __name__ == "__main__":
    main()
