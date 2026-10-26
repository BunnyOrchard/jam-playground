# IAMM-1

Infinite Automatic Music Machine. Unofficial firmware for the M-VAVE FM-1.

Power on and it is already playing. A new boot is a new song. Four synth keys can sound at once. A fifth turns the oldest off. Eleven drums sit beside them.

The screen and the USB name say IAMM-1. How to play it is in [docs/IAM-1.md](docs/IAM-1.md). What it is, and how to install it, is in [docs/IAMM-1.md](docs/IAMM-1.md).

A release package is `build/amb-1.0.0.fwsc`, identity `FM-1_7010000`. Build it with `./build.sh --release 1.0.0`. The download page, with a browser installer, is:

```
python3 tools/iamm_pages.py build/amb-1.0.0.fwsc 1.0.0 download
```

Open `download/index.html`. Chrome or Edge can write the package over USB. The same file installs from the command line with `python3 tools/fm1_install.py`.

Installing firmware is at your own risk. M-VAVE's own firmware can be put back. The steps are in [docs/IAMM-1.md](docs/IAMM-1.md).

IAMM-1 is a fork of [HYP-1](https://github.com/RustyPawz/fm1-HYP1) by RedtailWorks, a fork of [AMB-1](https://github.com/charlesvestal/fm1-amb) by Charles Vestal, on [Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita and Hügelton Instruments. GPL-3.0-only. See [LICENSE](LICENSE) and [LICENSING.md](LICENSING.md).

M-VAVE and FM-1 are trademarks of their owners. IAMM-1 is not affiliated with M-VAVE.
