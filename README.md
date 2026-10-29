# Jam Playground

Generative music groovebox for the M-VAVE FM-1. Unofficial firmware fork. 

Power on and it is already playing. A new boot is a new song. 

Jam Playground automatically creates and plays tunes, changes the sounds, mutates the pattern and allows continually changing soundscapes.

We can change every element and jam along at anytime. <Play> stops/starts automatic song mode.

Four synth keys can sound at once. A fifth turns the oldest off. Eleven drums sit beside them. 

Sounds include sampled FM piano, vibes, kalimba, harp, upright bass, flute, tenor sax, muted trumpet, harmonica, and many more.

Changes generally land at the start of the next bar.

Controls in default mode:

Select: Tempo

Presets: Change Vibe

Algorithm: Change Scale

Knob1: Ring FX

Knob2: Wah FX

Knob3: Phaser FX

Knob4: 4 Filters FX

Oct+: Pump

Oct-: Back

FX: Mutate Song

Select: Tips

Env: New Song

LFO: Lets Find Out (makes a couple of changes such as mutate or change instrument/drum)

Edit: Excite

Glo: Once you got a good beat going, go here to play keys / rec or do FX. 

      Black Keys: FX
      White Keys: Keyboard
      Rec: Records Keyboard play for the last 16 bars - Rec again plays it back on Loop
      FX: FX page where white and black keys can be used.
      Arp: Arpeggiator
      On the GLO effects page the four knobs are Repeat, Move, Tone, and Crowd. Each one starts dry.
      
Home: Return to default mode.

Save: Break

ARP: Cycles through different FX automatically.

Seq: Shows all the tracks and Steps. In this mode:

      SELECT knob: cycles between the current tracks.
      ALGORITHM mutates that key's track.
      knob 1 changes that keys sound
      knob 2 changes the individual notes' scale.
      knob 3: Change Volume of selected Key/track.
      knob 4: FX/Cutoff for the selected track.
      White Keys show the selected tracks steps - Edit the steps by pressing the white keys.
      
In default mode, hold a key and the above 6 instructions will do the same thing. 

How to play it is in [docs/IAM-1.md](docs/IAM-1.md). What it is, and how to install it, is in [docs/IAMM-1.md](docs/IAMM-1.md).

The release package is `build/amb-1.2.0.fwsc`, identity `FM-1_7666200`. Build it with `./build.sh --release 1.2.0`. The download page, with a browser installer, is:

```
python3 tools/iamm_pages.py build/amb-1.2.0.fwsc 1.2.0 download
```


Open `download/index.html`. Chrome or Edge can write the package over USB. The same file installs from the command line with `python3 tools/fm1_install.py`.

Installing firmware is at your own risk. M-VAVE's own firmware can be put back. The steps are in [docs/IAMM-1.md](docs/IAMM-1.md).

Jam Playground is a fork of [HYP-1](https://github.com/RustyPawz/fm1-HYP1) by RedtailWorks, a fork of [AMB-1](https://github.com/charlesvestal/fm1-amb) by Charles Vestal, on [Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita and Hügelton Instruments. GPL-3.0-only. See [LICENSE](LICENSE) and [LICENSING.md](LICENSING.md).

M-VAVE and FM-1 are trademarks of their owners. Jam Playground is not affiliated with M-VAVE. 1.2.0 is this release. 1.1.0 stays Jam Playground. The 1.0.0 package still says IAMM-1.
