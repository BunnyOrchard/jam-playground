# Jam Playground

Generative music for the M-VAVE FM-1. Unofficial firmware.

Power on and it is already playing, and three quiet notes rise over the song. A new boot is a new song. Four synth keys can sound at once. A fifth turns the oldest off. Eleven drums sit beside them. Seventy sounds are in the machine. Knob 1 walks every one of them, including the sampled FM piano, vibes, kalimba, harp, upright bass, flute, tenor sax, muted trumpet, harmonica, and the Dusty kit.

Hold a key and knob 2 walks that key's steps, heard as you turn. It does not switch the key on. With nothing held, knob 2 is still the wah. On the sequence screen, knob 2 writes that track's steps again, any number of them.

ARP is a DJ. On, it plays one effect at a time, on the phrase, and leaves the first bar of each four dry. OCT− is back. In GLO, ARP is still the arpeggiator.

The corner of the tips says JAM. The USB name is Jam Playground FM-1. How to play it is in [docs/IAM-1.md](docs/IAM-1.md). What it is, and how to install it, is in [docs/IAMM-1.md](docs/IAMM-1.md).

The release package is `build/amb-1.1.0.fwsc`, identity `FM-1_7010100`. Build it with `./build.sh --release 1.1.0`. The download page, with a browser installer, is:

```
python3 tools/iamm_pages.py build/amb-1.1.0.fwsc 1.1.0 download
```

Open `download/index.html`. Chrome or Edge can write the package over USB. The same file installs from the command line with `python3 tools/fm1_install.py`.

Installing firmware is at your own risk. M-VAVE's own firmware can be put back. The steps are in [docs/IAMM-1.md](docs/IAMM-1.md).

Jam Playground is a fork of [HYP-1](https://github.com/RustyPawz/fm1-HYP1) by RedtailWorks, a fork of [AMB-1](https://github.com/charlesvestal/fm1-amb) by Charles Vestal, on [Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita and Hügelton Instruments. GPL-3.0-only. See [LICENSE](LICENSE) and [LICENSING.md](LICENSING.md).

M-VAVE and FM-1 are trademarks of their owners. Jam Playground is not affiliated with M-VAVE. 1.1.0 is this release. The 1.0.0 package still says IAMM-1.
