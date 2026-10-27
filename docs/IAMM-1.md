# Jam Playground

Generative music for the M-VAVE FM-1.
Switch it on. It plays, and it says JAM.

Five engines. Four synth keys at once. Eleven drums. Seventy sounds. One hundred and four variations.

Jam Playground is unofficial firmware for the M-VAVE FM-1. It is not affiliated with, endorsed by, or
supported by M-VAVE. FM-1 is their instrument. Digitakt, Octatrack, and Digitone are Elektron
instruments. Cyma Forma RND is its own instrument. Those names are a way to describe a flavor.
They are not in this box, and their makers have nothing to do with this firmware.

Installing firmware is at your own risk. You can put M-VAVE's own firmware back. The steps are
below.

## Description

Power on and it is already playing. Ask for another and it arrives on the next bar, in time,
finished. One hundred and four are in the machine. It writes the one after that.

It hits like a Digitakt, moves like an Octatrack, speaks like a Digitone, and sits in the room
of a Cyma Forma RND. Those instruments are not in this box. This one makes the record.

How to play it, key by key, is in [IAM-1.md](IAM-1.md). A development build's USB name is Jam Playground FM-1. The tips say JAM. The 1.0.0 package still says IAMM-1.

## Specs

| | |
|---|---|
| Engines | 5. Pad, bass, acid, air, drums |
| Tracks | Sixteen synth slots and eleven drums on the screen. Four synth keys sound at once. A fifth turns the oldest off. The drums are not part of that count |
| Sounds | 70. Twenty pad, seventeen bass, twenty acid, three air, ten kits. FM piano, vibes, kalimba and harp, the upright bass, flute, tenor sax, muted trumpet, harmonica, and the Dusty kit are sampled. A new song draws those with the others. Knob 1 walks the whole list, one step a turn. SEL with ALGORITHM wraps it. Sleep stays on its own oscillators |
| Variations | 104. Eight inside each of thirteen. Turn again and the next one comes in |

Hold a key and knob 2 wobbles that sound, on the eighth, from dry up to a full sweep. On a drum it
sweeps the pitch, and the next hit shows it. A click of FX while the key is held mutates that key.
A click with nothing held mutates the song. On the sequence screen, ALGORITHM mutates the selected
track. Knob 1 is that track's sound or kit. Knobs 2 and 4 change that track's wobble, sweep, filter,
or tune.

## Install

Use a USB data cable. The FM-1 has to show up as a MIDI port. A charge-only cable will not do it.

From this tree, with `mido` and `python-rtmidi` installed for the `python3` you run:

```
python3 tools/fm1_install.py --info
python3 tools/fm1_install.py build/amb.fwsc
```

`--info` prints the identity of the connected unit and writes nothing. The installer accepts the
package when the name before the underscore matches, so an FM-1 accepts an FM-1 package. Add
`--yes` to skip the question.

A development build is `build/amb.fwsc`, identity `FM-1_700`. A numbered release is a different
file. Build it with:

```
./build.sh --release 1.0.0
python3 tools/fm1_install.py build/amb-1.0.0.fwsc
```

That package's identity is `FM-1_7010000`. The FM-1 reports that identity after the install. The
same command installs a later version over an earlier one.

If the write stops part way, the unit stays in update mode. Plug it in again and run the same
command. It finishes the write. Exit 0 means it is done. Exit 3 means the unit was not found.
Exit 6 means the package is for a different model.

## Back up

The installer does not copy the firmware off the unit before it writes. Keep the package you
might want to return to:

- M-VAVE's own `FM-1.fwsc` (their V15), if you want stock back without downloading it again
- the `.fwsc` of any Felucca firmware you were running, including an older IAMM-1 package

Songs you have stored on the unit live in its flash. Going back to stock can lose them. Copy out
anything you need before you overwrite the firmware.

The rescue tool below does one extra thing: before it writes stock, it reads the whole flash and
saves `fm1-backup-YYYYMMDD-HHMMSS.bin` next to the rescue script. That file is a raw image of the
chip. It is not a package, and `fm1_install.py` will not install it. It is the copy you keep if a
rescue was the only way out.

## Go back to stock

Stock is M-VAVE's FM-1 V15, file `FM-1.fwsc`. The rescue tool accepts only the file whose
SHA-256 is `db1642b2b6fa5c2cccb11ffd13878068bb28601678d3644049f99dc40e7edb8a`.

Two ways, while the unit still starts or is sitting in safe mode with USB on:

- M-VAVE's own updater, M-UPGRADE, from their downloads (PC Software, M-UPGRADE)
- The Felucca web installer's "Back to the stock firmware": download `FM-1.fwsc`, choose that
  file, connect the FM-1, and press the button. The page checks the file before it writes

When that finishes, the FM-1 restarts on M-VAVE's firmware.

## Rescue a bad flash

Try these in order.

1. The install stopped, or the screen says it is in update mode. Plug the cable in again and run
   the same `python3 tools/fm1_install.py` command. The unit stays in update mode until a write
   finishes.
2. The screen says safe mode. It got there because the firmware crashed twice while starting.
   USB is on, and there is no sound. PLAY tries the firmware again. To reinstall, run the
   installer while it is sitting on that screen. You can also hold OCT− and OCT+ together for
   5 seconds. The screen counts down, then the chip enters update mode, and the same installer
   reaches it.
3. It no longer starts. After enough failed boots the chip opens its own update mode and USB
   shows `WL80UBOOT1.00`. On a Mac, from this tree:

```
bash tools/fm1_rescue.sh
```

It sets itself up once, checks that the stock file is really V15, and waits. When it says it is
waiting, plug the FM-1 in and switch it on. It will crash and go blank. That is when the script
catches it. It saves the flash backup first, then writes V15, and only the sectors that differ.
It does not touch the bootloader below `0x4000` or the device data above `0x93000`. It asks for
your Mac password because macOS otherwise takes that USB device and reboots it. Ctrl-C stops the
wait.

If that script cannot see the unit, recovery is
[FM-1-transporter](https://github.com/kurogedelic/FM-1-transporter).

## Versions

A release is `./build.sh --release X.Y.Z`. The package is `build/amb-X.Y.Z.fwsc`. The FM-1
reports it as `FM-1_7MMmmpp`, two digits each for the major, the minor, and the patch. So 1.0.0
is `FM-1_7010000`, and 1.2.3 is `FM-1_7010203`. A build without `--release` stays `FM-1_700`.
That identity is a development build. It is not a version you can tell apart from the last one
by the number.

| Version | Identity | What it is |
|---|---|---|
| 1.0.0 | `FM-1_7010000` | The first release. `build/amb-1.0.0.fwsc` is 609657 bytes. The image is 271900 bytes. The screen and the USB name say IAMM-1 |
| development | `FM-1_700` | What a plain `./build.sh` writes. The USB name is Jam Playground FM-1. The tips say JAM. The identity number does not change between development builds |

1.0.0 is that first release: five engines, four synth keys at once, eleven drums,
sixty sounds, one hundred and four variations. The seventy sounds in the spec row include
the sampled library added in the source after that package. Later versions get a row in this table when they exist.

## Credits

Jam Playground is a fork of [HYP-1](https://github.com/RustyPawz/fm1-HYP1) by
[RedtailWorks](https://www.redtail.works), a fork of
[AMB-1](https://github.com/charlesvestal/fm1-amb) by Charles Vestal, on
[Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita and Hügelton Instruments. The
synth, the drums, and the room are theirs. The reverb is Jon Dattorro's plate ("Effect Design,
Part 1", JAES 1997). The platform came by way of
[X0X](https://github.com/charlesvestal/fm1-x0x). GPL-3.0-only. See [LICENSE](../LICENSE) and
[LICENSING.md](../LICENSING.md).
