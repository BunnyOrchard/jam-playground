# Eno for the M-VAVE FM-1

Eno turns the M-VAVE FM-1 into a generative ambient machine, in the spirit of *Music for Airports*.
It plays itself from the moment it's switched on: a slowly drifting pad, soft bells on tape loops of
unequal length, a gliding bass and quiet Euclidean percussion, all through a long echo and a plate
reverb. You don't perform it, you tend it: pick a scale, take notes out, put them back, and let it
grow.

![Eno's screen: the garden](docs/img/screen-garden.png) ![Frozen](docs/img/screen-frozen.png)

Private for now. Install it with `python3 tools/fm1_install.py build/eno.fwsc` (see below).

## The garden

- **Pad**: four voices, three detuned saws each through a slowly moving low-pass. Every so often
  (**Drift**, 20 s to 5 min) the chord moves to another one in the scale, on the bar, with each voice
  gliding the shortest way there.
- **Bells**: soft FM bells played by four tape loops 13, 17, 23 and 29 beats long. Each holds a few
  notes, so the loops slide against each other and the melody never quite repeats. **Density** sets
  how many notes a loop holds; **Evolve** how often a note changes or moves.
- **Bass**: the chord's root, low and soft, sometimes leaning on the fifth.
- **Pulse**: a soft thump, a shaker and a wooden knock, each on its own Euclidean pattern over 16, 12
  and 7 sixteenths: polyrhythms. **Hits** sets how many.

Everything follows one scale and one chord; the thump is tuned to the chord's root and the wood
plays the chord's notes.

## Playing

| | |
|---|---|
| Keys | The scale. Each key adds its note to the scale, or takes it out (the keys light up for the notes in it). The root stays, and the scale keeps at least three notes. |
| PLAY | Grow / rest. Eno starts growing by itself at power-on; at rest, the sounds fade out slowly. |
| ARP | The next chord (on the next bar). |
| REC | **Freeze**: the chord holds and the reverb rings on forever, while the bells and pulse go on dry above it. Press again to thaw. |
| OCT− / OCT+ | The bells' octave. |
| SELECT | Tempo (30–120). |
| ALGORITHM | Mode: Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, pentatonic major and minor. Editing the scale with the keys makes it Custom. |
| PRESETS | The root. |
| KNOB 1–4 | The four values on the screen. |
| SAVE | Saves. Eno also saves by itself a few seconds after a change, but only while resting (writing the flash interrupts the sound for a moment). |

### The pages

Each page puts four values on KNOB 1–4. EDIT steps through the pages.

- **HOME: Garden.** Pad, Bells, Bass, Pulse: the four layers' levels.
- **SEQ: Motion.** Density, Evolve, Drift, Hits.
- **FX: Space.** Echo (send), Echo time (1/8 to 1/2 dotted, shortened when the tape is too short at
  slow tempos), Reverb (send), Size (the plate's decay and the echo's feedback).
- **SEL: Tone.** Bright (the pad's filter), Swell (the pad's attack and release), Bell tone (soft to
  metallic), Motion (how far the slow random movements wander: filter, detune, pan, echo wow).
- **GLO: Setup.** Tune, MIDI out, Key lights, CPU.

The screen: the twelve notes as a ring (the scale lit, the chord bright, the root in coral) with the
chord's name in it; the four bell loops around it, their notes on them and their playheads going
round; the three pulse patterns on the right; the four layers' levels on the left; the way to the next
chord at the top right.

### MIDI

In over USB and the TRS jack, out over USB.

- **In, channel 1:** a note adds or removes its note from the scale, like a key.
- **In, clock:** the grid follows MIDI clock and start / stop.
- **Out** (on by default): the pad's chord on channel 1, the bells on 2, the bass on 3, the pulse on
  10, and clock with start / stop when Eno runs on its own tempo.

## Building and testing

The toolchain is the same as X0X's (see [BUILDING.md](BUILDING.md)):

```
tests/run_tests.sh      # the instrument, the platform pieces, the app in a simulator
./build.sh              # build/eno.fwsc
python3 tools/fm1_install.py build/eno.fwsc     # install it over USB
```

`build/host/eno_host SCRIPT OUTDIR` runs the whole app on a computer from a script
(`tests/scenarios/*.eno`), with audio and screenshots coming out. `web/emu/build.sh` builds the
same code for the browser.

## Credits

The idea comes from Brian Eno's generative music: *Music for Airports*' tape loops of unequal length.
The reverb is Jon Dattorro's plate ("Effect Design, Part 1", JAES 1997), as in
[FoMni](https://github.com/charlesvestal/fm1-fomni); the Euclidean rhythms are Godfried Toussaint's
reading of Bjorklund's algorithm. The platform (hardware layer, USB, update loader, storage,
installer) is [Felucca](https://github.com/hugelton/Felucca)'s by Leo Kuroshita (Hügelton
Instruments), by way of [X0X](https://github.com/charlesvestal/fm1-x0x) and FoMni. The idea of
running FM-1 firmware in the browser comes from [Groove OS](https://www.groove-os.com/).
GPL-3.0-only; see [LICENSING.md](LICENSING.md).
