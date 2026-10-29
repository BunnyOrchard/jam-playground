/* SPDX-License-Identifier: GPL-3.0-only */
/* The instrument: HYP-1, hypnotic techno and acid for the FM-1 (a fork of AMB-1), built like a small
 * modular: generative voices you shape over time, over drums you sequence. The voices draw on one pool
 * of notes (a scale) and one chord that drifts slowly through it:
 *
 *   PAD     four breathing voices, each breath its own envelope (attack, hold, release,
 *           rest) and the chord forming and dissolving through them; an LFO on its filter, level, pan or pitch.
 *   NOISE   a bed of noise in eight colours (white to black), filtered, pumping with the kick.
 *   DRONE   three detuned saws and a sub-octave through a 24 dB low-pass, on the chord's root; an envelope
 *           on its filter (on each chord, or every 1 .. 8 bars) and an LFO (16 bars, a sine, by default).
 *   ACID    a generative 303-style line: a loop of 1 .. 16 sixteenths of chord tones with accents and
 *           slides, a resonant low-pass with an envelope; MUTATE re-rolls its steps as it goes.
 *   DRUMS   a sixteen-step sequencer, six lanes (kick, snare, clap, closed hat, open hat, perc), six
 *           patterns; a new pattern waits for the end of the bar. The kick ducks everything else.
 *
 * The sections are the player's: a breakdown, a build, the drop (en_section). Over it all the ARC, a slow
 * tide between sparse and full (MOTION). Then a tempo-synced tape echo and an Echoplex, Dattorro's plate,
 * and the held performance effects (en_fx).
 *
 * Threads: en_render() runs in the audio interrupt. Everything else posts commands, which the render
 * drains at the start of a block; the state below marked (UI) is written by the render and only read
 * elsewhere (the drum patterns, en_pats, are the exception: the UI edits them, the render reads them).
 * Float only, no libm: fastmath.h. */
#pragma once
#include <stdint.h>

#define EN_SR 44100.0f
#define EN_NDRUM 6                     /* the drum lanes: kick, snare, clap, closed hat, open hat, perc */
#define EN_NLANE EN_NDRUM
#define EN_XDRUM 5                     /* rim, tom, cowbell, shaker, crash. Not parameters: the lane
                                        * block would shift every par above the drums */
#define EN_NPAT 6                      /* drum patterns */
#define EN_NSTEP 16
#define EN_NPAD 4                      /* pad voices */
#define EN_NTHEME 5                    /* the screen's colour themes (the UI's; kept with the settings) */
#define EN_NSTYLE 5                    /* the old styles (before 1.0.27): a stored piece's is turned into sounds
                                        * and a kit (en_style_sounds); they were kits 0..4 */
#define EN_NSOUND 16                   /* oscillator sounds. A new song draws from these */
#define EN_NPAD_SND 20                 /* then FM piano, vibes, kalimba, harp */
#define EN_NBASS_SND 17                /* then an upright bass */
#define EN_NACID_SND 20                /* then flute, tenor sax, muted trumpet, harmonica */
#define EN_NKIT_OSC 9                  /* synth kits. A new song draws from these */
#define EN_NKIT 10                     /* then Dusty, the sampled kit. Kick, snare, hat and perc keep a
                                        * parameter. Clap, the open hat, and the five extra drums pick a kit
                                        * beside those */
#define EN_KIT_DUSTY 9
enum { EN_V_PAD, EN_V_DRONE, EN_V_ACID };   /* the voices the keys can play (en_note) */
static inline int en_bank_n(int voice)     /* how far knob 1 walks on this voice */
{
    if (voice == EN_V_DRONE)
        return EN_NBASS_SND;
    if (voice == EN_V_ACID)
        return EN_NACID_SND;
    return EN_NPAD_SND;
}

/* modes: pools relative to the root (bit i: root + i semitones) */
enum { MODE_IONIAN, MODE_DORIAN, MODE_PHRYGIAN, MODE_LYDIAN, MODE_MIXOLYDIAN, MODE_AEOLIAN, MODE_PENT_MAJ,
       MODE_PENT_MIN, EN_NMODES };
extern const char *const EN_MODE_NAME[EN_NMODES];
extern const uint16_t EN_MODE_POOL[EN_NMODES];
extern const char *const EN_NOTE_NAME[12];
extern const char *const EN_LANE_NAME[EN_NLANE];
extern const char *const EN_PAD_SOUND[EN_NPAD_SND], *const EN_DRONE_SOUND[EN_NBASS_SND], *const EN_ACID_SOUND[EN_NACID_SND];
extern const char *const EN_KIT_NAME[EN_NKIT];
void en_style_sounds(int16_t *par);             /* a piece stored with a style (before 1.0.27): its sounds and
                                                 * kit set as the style had them, the style cleared */

/* parameters: value ranges in en_param_info() */
enum {
    P_PAD, P_ACID, P_BASS, P_PULSE, P_NOISE,              /* the layers' levels */
    P_TEMPO, P_EVOLVE, P_MOTION, P_DRIFT,                 /* tempo */
    P_BRIGHT, P_PADTONE, P_DETUNE,                        /* pad */
    P_PE_ATK, P_PE_HOLD, P_PE_REL, P_PE_REST,             /* pad envelope (the breath) */
    P_PL_RATE, P_PL_DEPTH, P_PL_SHAPE, P_PL_DEST,         /* pad LFO */
    P_NZ_COLOR, P_NZ_TONE, P_NZ_PUMP,                     /* noise */
    P_BASSOCT, P_DR_CUT, P_DR_RES,                        /* drone */
    P_DE_TRIG, P_DE_ATK, P_DE_DEC, P_DE_AMT,              /* drone envelope */
    P_DL_RATE, P_DL_DEPTH, P_DL_SHAPE, P_DL_DEST,         /* drone LFO */
    P_AC_CUT, P_AC_RES, P_AC_ENV,                         /* acid */
    P_AC_DEC, P_AC_ACC, P_AC_SLIDE, P_AC_WAVE,            /* acid envelope */
    P_AC_DENS, P_AC_LEN, P_AC_RANGE, P_AC_MUT,            /* acid line */
    P_AC_OCT,                                             /* OCT- / OCT+ */
    P_DRIVE, P_SWING, P_PATTERN,                          /* drums */
    P_LANE,                                               /* each lane's level, tune, decay: P_LANE + 3 * lane + 0..2 */
    P_ECHO = P_LANE + 3 * EN_NLANE, P_ETIME, P_REVERB, P_SIZE,   /* space */
    P_ROOT, P_MODE, P_TUNE, P_MIDI, P_THEME,              /* global (the mode on ALGORITHM) */
    P_STYLE,                                              /* kept for the bank's layout: a piece stored before
                                                           * 1.0.27 has its style here (read once, then 0) */
    P_LINKS, P_LK_KICK, P_LK_SNARE, P_LK_HAT, P_LK_PERC,  /* the drums into the voices: on / off (SEL + SEQ);
                                                           * kick > drone, snare and clap > pad, hats > acid
                                                           * accents, perc > the acid's clock */
    P_OCT, P_PAD_OCT,                                     /* the whole song's octave; the pad's (OCT- / OCT+;
                                                           * the drone's is P_BASSOCT, the acid's P_AC_OCT) */
    P_DR_INST,                                            /* the drone's sound (the pad's: P_PADTONE, the acid's:
                                                           * P_AC_WAVE) */
    P_KIT_KICK, P_KIT_SNARE, P_KIT_HAT, P_KIT_PERC,       /* the kit for kick, snare, closed hat, perc.
                                                           * Clap and the open hat pick their own (en_lane_kit) */
    P_PAD_MAN, P_DR_MAN, P_AC_MAN,                        /* a voice off the algorithm (SEL + ENV on its page):
                                                           * silent but for the keys (en_note) */
    P_CHORD, P_VOICING,                                   /* the pad's chords: their kind (EN_CH_*), close or
                                                           * open (drop 2) */
    P_DD_CHANGE, P_DD_EVERY, P_DD_POOL, P_DD_MUTATE,      /* drum drift: the patterns change by themselves (Off,
                                                           * Next, Random), when (with the chord, or every 4-64
                                                           * bars), from patterns 1..pool, a fill the bar before;
                                                           * and mutate (Off, Shift, Morph): hits move, come, go */
    P_NPARAMS
};
#define P_LANE_LVL(l) (P_LANE + 3 * (l))
#define P_LANE_TUNE(l) (P_LANE + 3 * (l) + 1)
#define P_LANE_DEC(l) (P_LANE + 3 * (l) + 2)
typedef struct {
    const char *name;
    int16_t lo, hi, def;
} en_param_t;
const en_param_t *en_param_info(int i);
void en_param_text(int i, int v, char *buf);   /* the value as the screen shows it (10 bytes) */

/* commands (main loop -> render) */
void en_init(void);
void en_set(int param, int value);
void en_pool(uint32_t pool);                   /* the scale, relative to the root (bit 0 always set) */
void en_play(int on);                          /* play / stop (the tails ring on) */
void en_next(void);                            /* the next chord: on the next bar (now while resting) */
void en_next_now(void);                        /* the next chord on this bar, for a place that just landed */
void en_freeze(int on);
void en_seed(uint32_t s);                      /* stir some entropy into the generators */
void en_note(int voice, int note, int on);      /* a key played on the page of a voice off the algorithm (P_*_MAN):
                                                 * EN_V_*, MIDI note, on / off */
void en_hands_off(void);                       /* the keys are up: the line goes back to the song */
void en_mute(int on);                          /* remote: silent, no MIDI out (1) or back (0), in turn */
void en_hush(int on);                          /* a store while playing: fade out (1), back in (0) */
extern volatile uint8_t en_hushed;             /* faded right out (the flash can be written) */
void en_shed(int out);                         /* a new piece: fade pad, bass, acid, and air out (1), then back (0) */
extern volatile uint8_t en_shed_done;          /* the four are down: the new piece can come in */
void en_panic(void);                           /* everything quiet, now */
void en_regrow(void);                          /* a new piece: fresh loops, the home chord */

/* the sections: a breakdown, a build, the drop, each taken on the next bar. A breakdown lasts until
 * a build (four bars, then the drop) or a drop is asked for; a build cut short by a drop drops. */
enum { EN_SEC_GROOVE, EN_SEC_BREAK, EN_SEC_BUILD };
enum { EN_CMD_BREAK, EN_CMD_BUILD, EN_CMD_DROP };
void en_section(int cmd);
void en_haze(int shape);                       /* the noise's part: 0 the bed, 1 dust, 2 chop, 3 air, 4 hush, 5 wind */
void en_haze_oct(int oct);                     /* that part, in octaves. 0 is the part as written */
void en_vstep(int part, unsigned mask);        /* pad, bass, acid, fourth: 16 steps. Until the fourth is named, dust and chop keep their own rhythm */
void en_melody(int which, int depth);          /* 0..3 engines, 4..14 drums. 0 is the written note. A phrase in the scale */
void en_sleep(int on);                         /* 1: throat, flute, a singing bowl, chimes, and birds in the wind */
void en_sleep_cue(int which);                  /* bit 0 a chime, bit 1 a bell, on the next downbeat */
void en_pad_lock(int on);                      /* 1: the pad's four voices breathe one phrase together */
void en_cloud(int delay, int helix, int loops); /* ping-pong, helix, and micro-loops. 0 is dry */
void en_echo(int amt);                         /* tape and Echoplex, over the song's own. 0 leaves them as baked */
void en_verb(int amt);                         /* more of the plate, over the room the song baked. 0 leaves it */
void en_shimmer(int amt);                      /* an octave up in that plate. 0 is dry */
void en_ring(int amt);                         /* knob 1: the mix times a tone. 0 is dry */
void en_wah(int amt);                          /* knob 2: a resonant band through the mix. 0 is dry */
void en_phase(int amt);                        /* a phaser on the master. 0 is dry */
void en_wobble(int part, int amt);             /* pad, bass, acid, air: an eighth-note sweep. 0 is the song */
void en_snap(int drum, int amt);               /* a drum's pitch sweep. 0 is the hit the kit already has */
void en_keys(int amt);                         /* white keys in GLO, 0 silent .. 100 the voice. 90 is a tenth down */
void en_layers(unsigned mask);                 /* twelve named keys, one bit each. They join the engines */
void en_layer_house(int on);                   /* House: the pluck layer sits forty lower */
void en_layer_level(int part, int lvl);        /* pad, bass, acid: 0..100, the stored level. A dark part stays */
extern volatile uint16_t en_layer_hot;         /* which of those twelve are in the air (UI) */
void en_hop(int kind);                         /* a crossing: 0 off, 1 cue, 2 throw, 3 stutter, 4 reverse */
void en_find(int mode);                        /* Lets Find Out: 0 lets go, 1 loops one beat, 2 a distant blur */
extern volatile uint8_t en_sec;                /* the section now: EN_SEC_* (UI) */
extern volatile uint8_t en_sec_req;            /* a command waiting for the bar: EN_CMD_* + 1, or 0 (UI) */

/* performance effects, held: one bit each, all at once as a mask (the UI sends it every frame). The
 * tracks, for mute and solo: kick; hats (closed and open); snare (snare, clap, perc); drone; acid;
 * pad (with the noise). */
enum {
    FX_RPT2, FX_RPT4, FX_RPT6, FX_RPT8, FX_RPT16, FX_RPT32,   /* beat repeat: the last 1/2 .. 1/32 note, looped */
    FX_REV, FX_STOP,                                          /* reverse; tape stop */
    FX_MUTE0, FX_MUTE1, FX_MUTE2, FX_MUTE3, FX_MUTE4, FX_MUTE5,
    FX_LP, FX_HP,                                             /* the filters closing in */
    FX_GATE16, FX_GATE8,                                      /* a trance gate on the synths */
    FX_THROW, FX_CRUSH, FX_FILL,                              /* an echo throw; a bit crusher; a drum fill */
    FX_SOLO0, FX_SOLO1, FX_SOLO2, FX_SOLO3, FX_SOLO4, FX_SOLO5,
    FX_FLANGE,                                                /* a jet: a short delay sweeping across the master */
    FX_FLANGER,                                               /* a slow wide flanger: the two sides sweep against each other */
    EN_NFX
};
#define EN_NTRACK 6
extern const char *const EN_FX_NAME[EN_NFX];   /* short names, for the screen */
void en_fx(uint32_t mask);
void en_dj(unsigned mask);                     /* GLO crowd keys: 14 bits, held. Zero is the mix alone */
/* MIDI clock in (as FoMni): ticks drive the 1/16 grid (6 a step) while they keep coming */
enum { EN_CLK_TICK, EN_CLK_START, EN_CLK_CONTINUE, EN_CLK_STOP };
void en_clock(int msg);

/* the render (audio ISR): n stereo frames, 24-bit in int32; gain Q12 (the MASTER pot) */
void en_render(int32_t *out_lr, uint32_t n, uint32_t gain_q12);
void en_jam(void);                             /* three notes at boot, half the earlier level. Host stays silent */

/* MIDI out, from the render: the app sends it (USB); status, data 1, data 2 */
void en_midi_out(uint32_t st, uint32_t d1, uint32_t d2);

/* state for the screen (UI) */
extern volatile uint8_t en_playing, en_frozen, en_ext;
extern volatile uint8_t en_chord[EN_NPAD];     /* the chord's notes (MIDI), pad voices' targets */
extern volatile uint8_t en_chord_root;         /* the chord's root, MIDI pitch class 0..11 */
extern volatile uint8_t en_chord_n;            /* its distinct tones: 3 or 4 */
extern volatile float en_drift;                /* 0..1: how far to the next chord */
extern volatile float en_pad_level, en_bass_level, en_bass_env;
extern volatile float en_noise_level, en_acid_level;
extern volatile uint8_t en_acid_len, en_acid_pos;    /* the acid loop's length and step */
extern volatile uint16_t en_acid_gate, en_acid_acc;  /* its steps that play; those accented */
extern volatile float en_acid_glow;
extern volatile uint8_t en_held;              /* keys held playing the voices (en_note) */
extern volatile uint8_t en_drum_pos;           /* the step playing (0..15) */
extern volatile float en_drum_glow[EN_NDRUM];
extern volatile float en_xdrum_glow[EN_XDRUM]; /* rim, tom, cowbell, shaker, crash */
void en_xdrum(int i, int on, int kit, unsigned steps, int level, int tune, int decay);
void en_lane_kit(int lane, int kit);           /* lane 2 clap, lane 4 open hat. 0 kit: follow the parent */
extern volatile uint8_t en_pat_now, en_pat_next;
extern volatile int8_t en_dd_left;             /* drum drift: bars to the change (1: the fill bar), -1 none */
extern volatile uint32_t en_dd_changes, en_dd_mutations;   /* changes made and mutations since start */      /* the pattern playing; the one waiting for the bar */
extern volatile uint16_t en_pats[EN_NPAT][EN_NLANE];   /* the patterns: bit s, a hit on step s (the UI edits) */
extern volatile uint32_t en_bars;              /* sixteenths played since start (for tests) */
extern volatile float en_arc;                  /* the arc, -1 sparse .. +1 full (times Motion) */
extern volatile uint8_t en_pulse_rest;         /* the pulse is out (a breakdown) */
extern volatile uint8_t en_pad_note[EN_NPAD];  /* the note each voice sings now (MIDI) */
extern volatile uint8_t en_hand_note;          /* the last note a hand played, after that voice folded it */
extern volatile float en_pad_amp[EN_NPAD];     /* and how loud */

/* the pure parts, for the tests and the screen */
uint32_t en_euclid(int k, int n, int rot);     /* Bjorklund's pattern, as a bitmask (bit 0 = step 0) */
int en_par_global(int p);                      /* the player's, not a piece's: Tune, MIDI, Theme */
int en_scale(uint32_t pool, int *steps);       /* the pool's pitch classes (relative), ascending; count */
/* the chord on scale degree deg: its tones (MIDI pitch classes, absolute), 3 or 4 of them; count */
int en_chord_tones(uint32_t pool, int root, int deg, int *pcs);
/* the kinds of chord (P_CHORD): as the scale has it (sevenths on a 7-note scale, else triads); triads;
 * sevenths; ninths (1 3 7 9); sus (1 4 5 b7); dub (a minor 9 shape on each degree, the scale or not);
 * fourths (stacked fourths: an m11 sound) */
enum { EN_CH_AUTO, EN_CH_TRIAD, EN_CH_7TH, EN_CH_9TH, EN_CH_SUS, EN_CH_DUB, EN_CH_4THS, EN_NCHORD };
extern const char *const EN_CHORD_NAME[EN_NCHORD];
int en_chord_kind(uint32_t pool, int root, int deg, int kind, int *pcs);   /* en_chord_tones, of a kind */
/* voice leading: new pad notes for the tones, each voice moving as little as possible, all in lo..hi */
void en_voice_lead(const int *from, const int *pcs, int ntones, int *to, int lo, int hi);
const char *en_chord_name(const int *pcs, int n, char *buf);   /* "Em7", "Dsus4", "C" ... */

/* the pieces: recipes for everything but Tune, MIDI out and Theme (the player's), and six drum patterns
 * each. Named by number, 01 .. 10. */
#define EN_NPIECES 10
const char *en_piece_name(int i);
void en_piece(int i, int16_t *par, uint16_t *pool);
void en_piece_pats(int i, uint16_t pats[EN_NPAT][EN_NLANE]);   /* its factory drum patterns */
