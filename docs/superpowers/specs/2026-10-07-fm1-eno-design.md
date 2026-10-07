# fm1-eno: design

A generative ambient machine for the M-VAVE FM-1, in the spirit of *Music for Airports*: continuous,
slowly modulating pads, lush echo and reverb, generative bells on tape loops of unequal length, and
soft Euclidean percussion in polyrhythms. It plays itself; you tend it like a garden. Dead simple.

Private repo (`charlesvestal/fm1-eno`, never pushed public). Built from the fm1-fomni template
(see the parent CLAUDE.md): keep `hal/`, `loader/`, `usb.c`, `storage.c`, `ota.c`, `main_fm1.c`,
`panel.c`, the host simulator and the web emulator; replace `firmware/src/dsp` and `firmware/src/app/ui.c`.

## Identity

- Package identity digit **7**: `FM-1_7XXYYZZ` releases, `FM-1_700` dev builds.
- USB strings (`usb.c` STR1 / STR2): "FM-1 Eno".
- Build output `build/eno.fwsc`; the DSP lives in `firmware/src/dsp/eno.c` / `eno.h` (prefix `en_`).
- UI: FoMni's light palette (cream paper, brown ink, coral / teal / pink), one colour per layer.
- The name is fine while private; if it's ever published, rename the on-screen product (the user's rule:
  names of real people / trademarks only descriptively).

## Interaction: the garden

It starts growing by itself at power-on and runs continuously; PLAY rests it and starts it again. The player shapes it, not performs it.

**The keyboard is the scale.** All 27 keys are a piano (pitch class = key mod 12). Pressing a key toggles
its pitch class in the **pool**, the set of notes every generator draws from. A pool of fewer than 3
notes is refused (the key press is ignored, the screen says why). Key lights show the pool.

| Control | Role |
|---|---|
| PLAY | start (fade in over ~4 s) / stop (fade out over ~4 s; the tails ring on) |
| ARP | next chord now |
| REC | **Freeze**: the chord holds, the reverb tail becomes infinite (decay to ~1, input muted); press again to release |
| OCT− / OCT+ | the bells' register (−2 … +2 octaves) |
| SELECT | tempo (30–120 BPM, default 72) |
| ALGORITHM | mode: Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Pent maj, Pent min (loads the pool; editing with keys shows "Custom") |
| PRESETS | root (C … B, default D) |
| EDIT / HOME SEQ FX SEL GLO | the pages |
| KNOB 1–4 | the page's four values |
| SAVE | saves (also autosaves a few seconds after a change, when quiet, as FoMni does) |

### Pages

| Page | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| **Garden** (HOME) | Pad (level) | Bells (level) | Bass (level) | Pulse (level) |
| **Motion** (SEQ) | Density (bell note probability) | Evolve (mutation chance per pass) | Drift (chord change period, 20 s … 5 min) | Hits (Euclidean density) |
| **Space** (FX) | Echo (send) | Echo time (synced divisions: 1/8 … 1/2 dotted, capped by buffer) | Reverb (send) | Size |
| **Tone** (SEL) | Brightness (pad filter centre) | Swell (pad attack / release) | Bell tone (FM index / ratio) | Motion (depth of the slow modulators) |
| **Setup** (GLO) | Tune | MIDI out | Key lights | — |

## Sound engine (`dsp/eno.c`)

All float, single precision, no libm (`fastmath.h`). Runs in the audio interrupt as FoMni's does: the
main loop posts commands, the render drains them at block start.

**Harmony.** The pool + root give a scale. A chord is built by stacking pool notes in thirds from a
degree (4 notes; 3 if the pool is pentatonic-sized). Every Drift period (randomised ±30 %), the chord
moves to another degree, weighted towards near moves (IV, V, vi, ii from I…), never the same degree
twice. ARP forces a move now. Freeze holds it. All layers follow the current chord.

**PAD** — 4 voices, one per chord note, voiced around C3–C5 with smooth voice leading (each voice moves
to the nearest new chord note). Each voice: 3 oscillators (polyBLEP saw, detuned ±~7 cents, slowly
wandering), through a 2-pole state-variable low-pass whose cutoff is Brightness plus a slow LFO per
voice. Chord changes glide (~2–4 s portamento) and swell. Equal-power pan wander per voice.

**BELLS** — 6-voice 2-operator FM (carrier + modulator, exponential decays, ratio and index from Bell
tone; inspired by the user's Hank). Four **tape loops**, lengths 13, 17, 23, 29 steps (one step = a quarter
note at the tempo, so the loops last about 10 to 24 s at 72 BPM). Each loop holds a phrase: per step, either silence or a chord / pool note (degree
index, so it follows chord changes). New phrases fill each slot with probability Density. At the end of
each pass, with probability Evolve, one slot is regenerated. Loops pan to fixed places, L to R.

**BASS** — sine plus a sub-octave sine, slow attack, long release. Plays the chord root (an octave
below the pad, C1–C2 range) at each chord change, and sometimes the fifth halfway through; glides.

**PULSE** — three small synthesized drums, each with its own Euclidean pattern E(k, n):
thump (sine with pitch drop, n = 16), shaker (filtered noise burst, n = 12), wood (short FM / resonant
click, n = 7). Hits sets each k (scaled per drum, k ≥ 0, 0 = silent). The different n make the
polyrhythms. Soft velocities with slight random variation. The Pulse level defaults lower than the
others: it's an accent, not a beat.

**Modulation** — a handful of slow random (smoothed sample-and-hold) modulators, periods 7–40 s,
scaled by Motion: pad cutoff, detune, pan, bell brightness, echo time wobble (tiny, for tape feel).

**Echo** — stereo ping-pong delay at half rate (22.05 kHz), 16-bit buffer, up to ~1.4 s, with low-pass + high-pass in the
feedback (feedback fixed ~0.55, softened by Size), sized so the pool keeps ≥ 8 KiB spare.

**Reverb** — FoMni's Dattorro plate (reused verbatim), fed by the reverb sends and by the echo's
output. Freeze sets decay ~0.999 and mutes its input.

**Mix** — sends per layer: pad (echo low, reverb high), bells (echo high, reverb high), bass (dry),
pulse (echo some, reverb low). A gentle soft-clip on the master, then the MASTER pot gain (Q12) as in
FoMni.

**Budget.** CPU well under X0X's (no profiling target set; measure with `fm1_perf` and keep ≤ 60 %
of the block). Memory: plate ~132 KB + echo ≤ ~176 KB from the 336 KB pool, ≥ 8 KiB headroom (the
build checks it). Audio ISR stack ≤ ~1 KB.

## MIDI

Kept from FoMni: USB + TRS in, USB out.
- **Out** (on by default): pad chord notes ch 1, bells ch 2, bass ch 3, pulse ch 10; clock + start / stop.
- **In ch 1**: a note toggles that pitch class in the pool (like a key).
- **In clock**: drives the step clock (6 ticks a step), as in FoMni.

## Screen

One screen, three bands like FoMni: the top shows the page name and the chord (e.g. "D Dorian · Em7"),
the middle a 12-note ring with the pool lit and the chord notes bright, with the four bell loops as
concentric arcs whose playheads turn at their own speeds (lighting when they ring) and the pulse as
three small Euclidean dots; the bottom the four knob values. Freeze shows a frost outline.

## Persistence

Project: pool, root, mode, all parameters, bell octave. Saved like FoMni's project: SAVE, and an autosave that only runs while resting (a flash write
interrupts the audio for a moment).
Loop phrases are not saved: they regrow.

## Safety (kept from Felucca)

Boot guard / safe mode, crash screen, watchdog, OCT− + OCT+ held 5 s for update mode, web installer
can restore M-VAVE's firmware.

## Testing

- Host unit tests (`tests/host/eno_test.c`): Euclidean patterns (E(3,8) = x..x..x., etc.), mode pools,
  chord stacking in thirds, voice leading picks nearest notes, pool < 3 refused, loop lengths and
  phrase mutation bounded, render produces no NaN / clipping over 10 minutes of simulated time,
  deterministic with a fixed seed.
- Kept tests: fastmath, encoder, trs, uac, storage, ldr, ota, install, the web emulator.
- Host simulator scenarios (`tests/scenarios/*.eno`): start, change mode, toggle keys, freeze;
  WAV + PNG out for listening and screenshots.
- Build (no doubles, no libm, pool headroom), then flash the attached FM-1.

## Out of scope

Sample playback, live keyboard performance, song / scene sequencing, a public site or release (private).
