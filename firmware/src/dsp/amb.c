/* SPDX-License-Identifier: GPL-3.0-only */
/* The instrument (amb.h). One compilation unit, -O2. */
#include "amb.h"
#include "fastmath.h"
#include "iam_samples.h"

#ifdef EN_HOST
#define EN_POOL
#else
#define EN_POOL __attribute__((section(".pool")))
#endif
#define BARRIER() __asm__ volatile("" ::: "memory")

const char *const EN_NOTE_NAME[12] = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab", "A", "Bb", "B"};
const char *const EN_MODE_NAME[EN_NMODES] = {"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolyd.", "Aeolian",
                                             "Pent maj", "Pent min"};
#define B(a, b, c, d, e, f, g) (1u << (a) | 1u << (b) | 1u << (c) | 1u << (d) | 1u << (e) | 1u << (f) | 1u << (g))
const uint16_t EN_MODE_POOL[EN_NMODES] = {
    B(0, 2, 4, 5, 7, 9, 11), B(0, 2, 3, 5, 7, 9, 10), B(0, 1, 3, 5, 7, 8, 10), B(0, 2, 4, 6, 7, 9, 11),
    B(0, 2, 4, 5, 7, 9, 10), B(0, 2, 3, 5, 7, 8, 10), B(0, 2, 4, 7, 9, 9, 9),  B(0, 3, 5, 7, 10, 10, 10)};
#undef B
const char *const EN_LANE_NAME[EN_NLANE] = {"Kick", "Snare", "Clap", "Hat", "Open hat", "Perc"};

static const en_param_t PARAMS[P_NPARAMS] = {
    [P_PAD] = {"Pad", 0, 100, 45},          [P_ACID] = {"Acid", 0, 100, 55},       [P_BASS] = {"Drone", 0, 100, 65},
    [P_PULSE] = {"Drums", 0, 100, 75},      [P_NOISE] = {"Noise", 0, 100, 20},
    [P_TEMPO] = {"Tempo", 48, 190, 128},   [P_EVOLVE] = {"Evolve", 0, 100, 40},   [P_MOTION] = {"Motion", 0, 100, 60}, /* 48: Sleep sits in the 50s. Dance songs stay above 70 */
    [P_DRIFT] = {"Drift", 0, 100, 60},
    [P_BRIGHT] = {"Bright", 0, 100, 35},    [P_PADTONE] = {"Sound", 0, EN_NPAD_SND - 1, 1},   [P_DETUNE] = {"Detune", 0, 100, 30},
    [P_PE_ATK] = {"Attack", 0, 100, 45},    [P_PE_HOLD] = {"Hold", 0, 100, 35},    [P_PE_REL] = {"Release", 0, 100, 55},
    [P_PE_REST] = {"Rest", 0, 100, 20},
    [P_PL_RATE] = {"Rate", 0, 7, 4},        [P_PL_DEPTH] = {"Depth", 0, 100, 25},  [P_PL_SHAPE] = {"Shape", 0, 5, 0},
    [P_PL_DEST] = {"Target", 0, 3, 0},
    [P_NZ_COLOR] = {"Colour", 0, 7, 1},     [P_NZ_TONE] = {"Tone", 0, 100, 50},    [P_NZ_PUMP] = {"Pump", 0, 100, 60},
    [P_BASSOCT] = {"Octave", -3, 3, 0},     [P_DR_CUT] = {"Cutoff", 0, 100, 45},   [P_DR_RES] = {"Reso", 0, 100, 45},
    [P_DE_TRIG] = {"Trigger", 0, 5, 0},     [P_DE_ATK] = {"Attack", 0, 100, 25},   [P_DE_DEC] = {"Decay", 0, 100, 55},
    [P_DE_AMT] = {"Amount", 0, 100, 35},
    [P_DL_RATE] = {"Rate", 0, 7, 6},        [P_DL_DEPTH] = {"Depth", 0, 100, 55},  [P_DL_SHAPE] = {"Shape", 0, 5, 0},
    [P_DL_DEST] = {"Target", 0, 3, 0},
    [P_AC_CUT] = {"Cutoff", 0, 100, 35},    [P_AC_RES] = {"Reso", 0, 100, 70},     [P_AC_ENV] = {"Env mod", 0, 100, 55},
    [P_AC_DEC] = {"Decay", 0, 100, 40},     [P_AC_ACC] = {"Accent", 0, 100, 50},   [P_AC_SLIDE] = {"Slide", 0, 100, 30},
    [P_AC_WAVE] = {"Sound", 0, EN_NACID_SND - 1, 0},
    [P_AC_DENS] = {"Density", 0, 100, 60},  [P_AC_LEN] = {"Length", 1, 16, 16},    [P_AC_RANGE] = {"Range", 1, 3, 1},
    [P_AC_MUT] = {"Mutate", 0, 100, 25},    [P_AC_OCT] = {"Acid oct", -3, 3, 0},
    [P_DRIVE] = {"Drive", 0, 100, 40},      [P_SWING] = {"Swing", 0, 100, 0},      [P_PATTERN] = {"Pattern", 0, EN_NPAT - 1, 0},
    [P_LANE_LVL(0)] = {"Level", 0, 100, 85}, [P_LANE_TUNE(0)] = {"Tune", -50, 50, 0}, [P_LANE_DEC(0)] = {"Decay", 0, 100, 50},
    [P_LANE_LVL(1)] = {"Level", 0, 100, 60}, [P_LANE_TUNE(1)] = {"Tune", -50, 50, 0}, [P_LANE_DEC(1)] = {"Decay", 0, 100, 45},
    [P_LANE_LVL(2)] = {"Level", 0, 100, 65}, [P_LANE_TUNE(2)] = {"Tune", -50, 50, 0}, [P_LANE_DEC(2)] = {"Decay", 0, 100, 45},
    [P_LANE_LVL(3)] = {"Level", 0, 100, 55}, [P_LANE_TUNE(3)] = {"Tune", -50, 50, 0}, [P_LANE_DEC(3)] = {"Decay", 0, 100, 35},
    [P_LANE_LVL(4)] = {"Level", 0, 100, 50}, [P_LANE_TUNE(4)] = {"Tune", -50, 50, 0}, [P_LANE_DEC(4)] = {"Decay", 0, 100, 50},
    [P_LANE_LVL(5)] = {"Level", 0, 100, 50}, [P_LANE_TUNE(5)] = {"Tune", -50, 50, 0}, [P_LANE_DEC(5)] = {"Decay", 0, 100, 40},
    [P_ECHO] = {"Echo", 0, 100, 45},        [P_ETIME] = {"Echo time", 0, 5, 1},    [P_REVERB] = {"Reverb", 0, 100, 40},
    [P_SIZE] = {"Size", 0, 100, 70},
    [P_ROOT] = {"Root", 0, 11, 9},          [P_MODE] = {"Mode", 0, EN_NMODES - 1, 5}, [P_TUNE] = {"Tune", -50, 50, 0},
    [P_MIDI] = {"MIDI out", 0, 1, 1},       [P_THEME] = {"Theme", 0, EN_NTHEME - 1, 0},
    [P_STYLE] = {"Style", 0, EN_NSTYLE - 1, 0},
    [P_LINKS] = {"Links", 0, 1, 0},         [P_LK_KICK] = {"Kick", 0, 100, 50},    [P_LK_SNARE] = {"Snare", 0, 100, 40},
    [P_LK_HAT] = {"Hats", 0, 100, 50},      [P_LK_PERC] = {"Perc", 0, 1, 0},
    [P_OCT] = {"Song oct", -3, 3, 0},       [P_PAD_OCT] = {"Pad oct", -3, 3, 0},
    [P_DR_INST] = {"Sound", 0, EN_NBASS_SND - 1, 0},
    [P_KIT_KICK] = {"Kick", 0, EN_NKIT - 1, 0}, [P_KIT_SNARE] = {"Snare", 0, EN_NKIT - 1, 0},
    [P_KIT_HAT] = {"Hats", 0, EN_NKIT - 1, 0},  [P_KIT_PERC] = {"Perc", 0, EN_NKIT - 1, 0},
    [P_PAD_MAN] = {"Pad hand", 0, 1, 0},     [P_DR_MAN] = {"Drone hand", 0, 1, 0}, [P_AC_MAN] = {"Acid hand", 0, 1, 0},
    [P_CHORD] = {"Chord", 0, EN_NCHORD - 1, 0}, [P_VOICING] = {"Voicing", 0, 1, 0},
    [P_DD_CHANGE] = {"Change", 0, 2, 0}, [P_DD_EVERY] = {"Every", 0, 5, 2}, [P_DD_POOL] = {"Pool", 2, EN_NPAT, 4},
    [P_DD_MUTATE] = {"Mutate", 0, 2, 0},
};
const en_param_t *en_param_info(int i) { return (i >= 0 && i < P_NPARAMS) ? &PARAMS[i] : &PARAMS[0]; }
int en_par_global(int p) { return p == P_TUNE || p == P_MIDI || p == P_THEME; }

/* the pieces' factory drum patterns: six for each piece (tools/gen_pats.py writes them) */
#include "pats.h"
volatile uint16_t en_pats[EN_NPAT][EN_NLANE];

/* the pieces: each the defaults above with a few changed (a parameter, its value; -1 ends), its own
 * kit's sound (the lanes) among them. Numbered on the screen; the names are for reading here. */
#define LV(l) P_LANE_LVL(l)
#define TU(l) P_LANE_TUNE(l)
#define DE(l) P_LANE_DEC(l)
#define KIT(k) P_KIT_KICK, k, P_KIT_SNARE, k, P_KIT_HAT, k, P_KIT_PERC, k   /* all four drums from kit k */
static const int16_t PC_HYPNOS[] = {-1};
static const int16_t PC_UNDERTOW[] = {P_TEMPO, 124, P_ROOT, 2, P_MODE, 1, P_PAD, 30, P_ACID, 45, P_BASS, 80, P_NOISE, 25,
    P_NZ_COLOR, 2, P_BASSOCT, -1, P_DR_RES, 60, P_DL_DEPTH, 70, P_AC_CUT, 25, P_AC_DENS, 45, P_AC_LEN, 13,
    P_PATTERN, 1, P_ECHO, 55, P_BRIGHT, 20, TU(0), -10, DE(0), 65, LV(5), 60, -1};
static const int16_t PC_MONOLITH[] = {P_TEMPO, 132, P_ROOT, 4, P_MODE, 2, P_PAD, 20, P_ACID, 60, P_BASS, 75,
    P_AC_RES, 80, P_AC_ENV, 70, P_AC_DENS, 70, P_DRIVE, 60, P_NOISE, 15, P_REVERB, 25, P_SIZE, 60, P_PATTERN, 1,
    LV(0), 90, LV(2), 70, DE(3), 25, -1};
static const int16_t PC_ASCENSION[] = {P_TEMPO, 138, P_ROOT, 7, P_PAD, 60, P_BRIGHT, 60, P_ACID, 45, P_BASS, 60,
    P_AC_LEN, 12, P_PATTERN, 1, P_REVERB, 55, P_SIZE, 85, P_MOTION, 80, P_NZ_COLOR, 0, P_NOISE, 15, P_PL_DEPTH, 40,
    LV(4), 60, LV(2), 70, -1};
static const int16_t PC_NIGHT_DRIVE[] = {P_TEMPO, 130, P_ROOT, 9, P_MODE, 1, P_PAD, 35, P_ACID, 65, P_AC_CUT, 45,
    P_AC_RES, 75, P_AC_WAVE, 1, P_AC_SLIDE, 50, P_AC_DENS, 55, P_AC_LEN, 14, P_PATTERN, 1, P_NZ_COLOR, 4, P_NOISE, 12,
    P_SWING, 15, TU(5), 20, LV(5), 55, -1};
static const int16_t PC_DUB_CHAMBER[] = {P_TEMPO, 120, P_ROOT, 5, P_MODE, 7, P_PAD, 70, P_CHORD, EN_CH_DUB, P_PADTONE, 8, P_ACID, 40, P_AC_CUT, 20,
    P_AC_DEC, 60, P_ECHO, 80, P_SIZE, 90, P_AC_LEN, 7, P_AC_DENS, 35, P_PATTERN, 1, P_NZ_COLOR, 6, P_NOISE, 35,
    P_DE_TRIG, 1, P_SWING, 20, TU(1), 25, DE(1), 25, DE(0), 70, -1};
static const int16_t PC_GLASS_TUNNEL[] = {P_TEMPO, 134, P_ROOT, 11, P_MODE, 6, P_PAD, 30, P_BRIGHT, 55, P_ACID, 55,
    P_AC_LEN, 10, P_AC_DENS, 65, P_AC_MUT, 40, P_PATTERN, 1, P_NZ_COLOR, 7, P_NOISE, 20, P_PL_RATE, 3, P_PL_DEPTH, 50,
    P_PL_DEST, 2, TU(5), 30, DE(5), 25, DE(3), 25, -1};
static const int16_t PC_TIDAL[] = {P_TEMPO, 126, P_ROOT, 10, P_MODE, 4, P_PAD, 50, P_ACID, 50, P_BASS, 75,
    P_DL_DEPTH, 85, P_DL_RATE, 5, P_MOTION, 95, P_EVOLVE, 75, P_AC_LEN, 15, P_PATTERN, 1, P_NZ_COLOR, 3, P_NOISE, 15,
    P_DE_TRIG, 3, P_DE_AMT, 60, TU(5), -25, DE(5), 60, LV(5), 60, -1};
static const int16_t PC_WAREHOUSE[] = {P_TEMPO, 136, P_ROOT, 0, P_MODE, 2, P_PAD, 15, P_ACID, 55, P_BASS, 80,
    P_DRIVE, 75, P_AC_RES, 85, P_AC_ENV, 75, P_AC_DENS, 75, P_EVOLVE, 55, P_MOTION, 50, P_DL_DEPTH, 65,
    P_NOISE, 20, P_NZ_COLOR, 5, P_REVERB, 30, P_SIZE, 55, P_ECHO, 30, P_PATTERN, 1, KIT(2), P_PADTONE, 7, P_DR_INST, 2,
    LV(0), 90, DE(0), 60, LV(4), 60, DE(4), 60, -1};
static const int16_t PC_DEEP_FIELD[] = {P_TEMPO, 118, P_ROOT, 3, P_PAD, 60, P_BRIGHT, 30, P_PE_ATK, 60, P_PE_REL, 70,
    P_ACID, 35, P_AC_CUT, 25, P_AC_DENS, 40, P_AC_LEN, 11, P_AC_MUT, 40, P_BASS, 60, P_NOISE, 30, P_NZ_COLOR, 1,
    P_REVERB, 60, P_SIZE, 85, P_ECHO, 60, P_ETIME, 2, P_PATTERN, 1, P_SWING, 25, P_MOTION, 85, P_DRIFT, 75,
    TU(1), -10, DE(1), 60, DE(0), 60, -1};
#undef LV
#undef TU
#undef DE
#undef KIT
static const int16_t *const PIECES[EN_NPIECES] = {
    PC_HYPNOS, PC_UNDERTOW, PC_MONOLITH, PC_ASCENSION, PC_NIGHT_DRIVE,
    PC_DUB_CHAMBER, PC_GLASS_TUNNEL, PC_TIDAL, PC_WAREHOUSE, PC_DEEP_FIELD,
};
static const char *const PIECE_NAME[EN_NPIECES] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10"};

const char *en_piece_name(int i) { return PIECE_NAME[(unsigned)i < EN_NPIECES ? i : 0]; }
void en_piece(int i, int16_t *par, uint16_t *pool)
{
    int p;
    const int16_t *c;
    i = (unsigned)i < EN_NPIECES ? i : 0;
    for (p = 0; p < P_NPARAMS; p++)
        par[p] = PARAMS[p].def;
    for (c = PIECES[i]; c[0] >= 0; c += 2)
        par[c[0]] = c[1];
    *pool = EN_MODE_POOL[par[P_MODE]];
}
void en_piece_pats(int i, uint16_t pats[EN_NPAT][EN_NLANE])
{
    int p, l;
    i = (unsigned)i < EN_NPIECES ? i : 0;
    for (p = 0; p < EN_NPAT; p++)
        for (l = 0; l < EN_NLANE; l++)
            pats[p][l] = EN_PIECE_PATS[i][p][l];
}

static const char *const ETIME_NAME[6] = {"1/8", "3/16", "1/4", "1/4.", "1/2", "1 bar"};
static const float ETIME_BEATS[6] = {0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 4.0f};
static const char *const LFO_RATE_NAME[8] = {"1/4", "1/2", "1 bar", "2 bars", "4 bars", "8 bars", "16 bars", "32 bars"};
static const float LFO_RATE_BEATS[8] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f};
static const char *const LFO_SHAPE_NAME[6] = {"Sine", "Triangle", "Saw", "Square", "S&H", "Drift"};
static const char *const PAD_DEST_NAME[4] = {"Filter", "Level", "Pan", "Pitch"};
static const char *const DRONE_DEST_NAME[4] = {"Cutoff", "Pitch", "Level", "Drive"};
static const char *const NOISE_NAME[8] = {"White", "Pink", "Brown", "Blue", "Violet", "Grey", "Black", "Yellow"};
static const char *const TRIG_NAME[6] = {"Chord", "1 bar", "2 bars", "4 bars", "8 bars", "Off"};
static const char *const THEME_NAME[EN_NTHEME] = {"Synth", "Acid", "Ice", "Ember", "Phosphor"};
/* the voice bank: what each voice can play (the sounds themselves: PAD_SOUND, DRONE_SOUND, ACID_SOUND, and
 * the kits: the old styles' drums) */
const char *const EN_PAD_SOUND[EN_NPAD_SND] = {"Bloom", "Voices", "Strings", "Glass", "Organ", "Warm", "Stabs", "Dark",
                                              "Dub", "Trance", "Poly", "Mist", "Pings", "Hits", "Rave", "Sine",
                                              "FM Piano", "Vibes", "Kalimba", "Harp"};
const char *const EN_DRONE_SOUND[EN_NBASS_SND] = {"Saws", "Warm", "Grit", "Dirty", "Reese", "Pulse", "FM", "Organ",
                                                  "Rumble", "Sub", "Square", "Hoover", "Fifths", "Metal", "Sync", "Bright",
                                                  "Upright"};
const char *const EN_ACID_SOUND[EN_NACID_SND] = {"Saw", "Square", "Driven", "Sync", "Pluck", "PWM", "Chip", "Sub",
                                                 "Trance", "Bleep", "Fifth", "Bell", "Organ", "Laser", "Fold", "Soft",
                                                 "Flute", "Tenor Sax", "Mute Tpt", "Harmonica"};
const char *const EN_KIT_NAME[EN_NKIT] = {"Hypno", "House", "Techno", "Acid", "Break", "Minimal", "Hard", "808",
                                          "909", "Dusty"};

/* the envelopes' times, from their knobs (0..100), in seconds */
static float pe_atk_s(int v) { return 0.05f * fm_exp2f((float)v * 0.0764f); }        /* 0.05 .. 10 s */
static float pe_hold_s(int v) { return 8.0f * (float)(v * v) * 0.0001f; }              /* 0 .. 8 s */
static float pe_rel_s(int v) { return 0.1f * fm_exp2f((float)v * 0.0691f); }          /* 0.1 .. 12 s */
static float pe_rest_s(int v) { return 6.0f * (float)(v * v) * 0.0001f; }             /* 0 .. 6 s */
static float de_atk_s(int v) { return 0.01f * fm_exp2f((float)v * 0.0864f); }         /* 0.01 .. 4 s */
static float de_dec_s(int v) { return 0.05f * fm_exp2f((float)v * 0.0832f); }         /* 0.05 .. 16 s */

static char *itoa_s(int v, char *b)
{
    char t[8];
    int n = 0, neg = v < 0;
    if (neg)
        v = -v;
    do {
        t[n++] = (char)('0' + v % 10);
        v /= 10;
    } while (v && n < 6);
    if (neg)
        *b++ = '-';
    while (n)
        *b++ = t[--n];
    *b = 0;
    return b;
}

static void secs_text(float s, char *b)          /* "0.25s", "2.4s", "12s" */
{
    if (s < 1.0f) {
        int ms = (int)(s * 100.0f + 0.5f);
        *b++ = '0';
        *b++ = '.';
        *b++ = (char)('0' + ms / 10 % 10);
        *b++ = (char)('0' + ms % 10);
    } else if (s < 10.0f) {
        int d = (int)(s * 10.0f + 0.5f);
        *b++ = (char)('0' + d / 10);
        *b++ = '.';
        *b++ = (char)('0' + d % 10);
    } else {
        b = itoa_s((int)(s + 0.5f), b);
    }
    *b++ = 's';
    *b = 0;
}

static float drift_secs(int v) { return 20.0f * fm_exp2f((float)v * 0.039069f); }   /* 20 s .. 5 min */

void en_param_text(int i, int v, char *b)
{
    static const char *const ONOFF[2] = {"Off", "On"};
    const char *s = 0;
    switch (i) {
    case P_ETIME: s = ETIME_NAME[(unsigned)v < 6u ? v : 0]; break;
    case P_MIDI: s = ONOFF[v ? 1 : 0]; break;
    case P_PADTONE: s = EN_PAD_SOUND[(unsigned)v < EN_NPAD_SND ? v : 0]; break;
    case P_DR_INST: s = EN_DRONE_SOUND[(unsigned)v < EN_NBASS_SND ? v : 0]; break;
    case P_KIT_KICK: case P_KIT_SNARE: case P_KIT_HAT: case P_KIT_PERC: s = EN_KIT_NAME[(unsigned)v < EN_NKIT ? v : 0]; break;
    case P_MODE: s = EN_MODE_NAME[(unsigned)v < EN_NMODES ? v : 0]; break;
    case P_ROOT: s = EN_NOTE_NAME[(unsigned)v < 12u ? v : 0]; break;
    case P_PL_RATE: case P_DL_RATE: s = LFO_RATE_NAME[v & 7]; break;
    case P_PL_SHAPE: case P_DL_SHAPE: s = LFO_SHAPE_NAME[(unsigned)v < 6u ? v : 0]; break;
    case P_PL_DEST: s = PAD_DEST_NAME[v & 3]; break;
    case P_DL_DEST: s = DRONE_DEST_NAME[v & 3]; break;
    case P_NZ_COLOR: s = NOISE_NAME[v & 7]; break;
    case P_DE_TRIG: s = TRIG_NAME[(unsigned)v < 6u ? v : 0]; break;
    case P_AC_WAVE: s = EN_ACID_SOUND[(unsigned)v < EN_NACID_SND ? v : 0]; break;
    case P_THEME: s = THEME_NAME[(unsigned)v < EN_NTHEME ? v : 0]; break;
    case P_LINKS: s = ONOFF[v ? 1 : 0]; break;
    case P_LK_PERC: s = v ? "Clock" : "Off"; break;
    case P_CHORD: s = EN_CHORD_NAME[(unsigned)v < EN_NCHORD ? v : 0]; break;
    case P_VOICING: s = v ? "Open" : "Close"; break;
    case P_DD_CHANGE: { static const char *const C[3] = {"Off", "Next", "Random"}; s = C[(unsigned)v < 3u ? v : 0]; break; }
    case P_DD_EVERY: { static const char *const E[6] = {"Chord", "4 bars", "8 bars", "16 bars", "32 bars", "64 bars"}; s = E[(unsigned)v < 6u ? v : 0]; break; }
    case P_DD_MUTATE: { static const char *const M[3] = {"Off", "Shift", "Morph"}; s = M[(unsigned)v < 3u ? v : 0]; break; }
    default: break;
    }
    if (s) {
        int k = 0;
        while (s[k] && k < 9) {
            b[k] = s[k];
            k++;
        }
        b[k] = 0;
        return;
    }
    if (i == P_DRIFT) {
        int sec = (int)(drift_secs(v) + 0.5f);
        if (sec < 60) {
            b = itoa_s(sec, b);
            *b++ = 's';
            *b = 0;
        } else {
            b = itoa_s(sec / 60, b);
            *b++ = ':';
            *b++ = (char)('0' + sec % 60 / 10);
            *b++ = (char)('0' + sec % 10);
            *b = 0;
        }
        return;
    }
    switch (i) {
    case P_PE_ATK: secs_text(pe_atk_s(v), b); return;
    case P_PE_HOLD: secs_text(pe_hold_s(v), b); return;
    case P_PE_REL: secs_text(pe_rel_s(v), b); return;
    case P_PE_REST: secs_text(pe_rest_s(v), b); return;
    case P_DE_ATK: secs_text(de_atk_s(v), b); return;
    case P_DE_DEC: secs_text(de_dec_s(v), b); return;
    case P_PATTERN: itoa_s(v + 1, b); return;
    case P_DD_POOL: *b++ = '1'; *b++ = '-'; itoa_s(v, b); return;
    case P_AC_RANGE: b = itoa_s(v, b); *b++ = ' '; *b++ = 'o'; *b++ = 'c'; *b++ = 't'; *b = 0; return;
    case P_SWING: b = itoa_s(50 + v / 4, b); *b++ = '%'; *b = 0; return;
    default: break;
    }
    if ((i == P_TUNE || i == P_AC_OCT || i == P_BASSOCT || i == P_OCT || i == P_PAD_OCT || (i >= P_LANE && i < P_ECHO && (i - P_LANE) % 3 == 1)) && v > 0)
        *b++ = '+';
    itoa_s(v, b);
}

/* ------------------------------------------------------- the pure parts --- */
uint32_t en_euclid(int k, int n, int rot)
{
    /* Bjorklund's algorithm (as Toussaint's "The Euclidean Algorithm Generates Traditional Musical
     * Rhythms"): k groups "x" and n - k groups ".", the remainders folded onto the heads until at most
     * one remains. Groups are bit strings, LSB first: (bits, length). */
    uint32_t gb[32], m = 0;
    uint8_t gl[32];
    int ng, heads, tails, i, pos = 0;
    if (n <= 0 || n > 32 || k <= 0)
        return 0;
    if (k > n)
        k = n;
    for (i = 0; i < n; i++) {
        gb[i] = i < k ? 1u : 0u;
        gl[i] = 1;
    }
    ng = n;
    heads = k;
    tails = n - k;
    while (tails > 1 && heads > 0) {
        int pairs = heads < tails ? heads : tails;
        for (i = 0; i < pairs; i++) {           /* a tail onto each head */
            gb[i] |= gb[ng - pairs + i] << gl[i];
            gl[i] = (uint8_t)(gl[i] + gl[ng - pairs + i]);
        }
        if (heads > tails) {                    /* the unpaired heads become the new tails */
            tails = heads - pairs;
            heads = pairs;
        } else {
            tails -= pairs;
            heads = pairs;
        }
        ng = heads + tails;
    }
    for (i = 0; i < ng; i++) {
        uint32_t j;
        for (j = 0; j < gl[i]; j++)
            if (gb[i] >> j & 1u)
                m |= 1u << (((pos + (int)j + rot) % n + n) % n);
        pos += gl[i];
    }
    return m;
}

int en_scale(uint32_t pool, int *steps)
{
    int i, m = 0;
    pool |= 1u;
    for (i = 0; i < 12; i++)
        if (pool >> i & 1u)
            steps[m++] = i;
    return m;
}

const char *const EN_CHORD_NAME[EN_NCHORD] = {"Auto", "Triads", "7ths", "9ths", "Sus", "Dub", "Fourths"};

int en_chord_kind(uint32_t pool, int root, int deg, int kind, int *pcs)
{
    /* the scale steps above the degree to take, in order (then what's left, should they repeat) */
    static const uint8_t OFF[5][7] = {
        {0, 2, 4, 6, 1, 3, 5},                  /* thirds: 1 3 5 7 */
        {0, 2, 6, 1, 4, 3, 5},                  /* ninths: 1 3 7 9 (the fifth left out) */
        {0, 3, 4, 6, 1, 2, 5},                  /* sus: 1 4 5 b7 */
    };
    static const uint8_t SHAPE[2][4] = {{0, 3, 10, 14}, {0, 5, 10, 15}};   /* dub: m9 (no fifth); fourths */
    int st[12], m = en_scale(pool, st), want, n = 0, i, j;
    const uint8_t *off = OFF[0];
    deg = ((deg % m) + m) % m;
    if (kind == EN_CH_DUB || kind == EN_CH_4THS) {   /* a fixed shape on the degree's note */
        for (i = 0; i < 4; i++)
            pcs[i] = (root + st[deg] + SHAPE[kind == EN_CH_4THS][i]) % 12;
        return 4;
    }
    want = kind == EN_CH_TRIAD ? 3 : kind == EN_CH_AUTO ? (m >= 6 ? 4 : 3) : 4;
    if (kind == EN_CH_9TH)
        off = OFF[1];
    else if (kind == EN_CH_SUS)
        off = OFF[2];
    for (i = 0; i < 7 && n < want; i++) {
        int pc = (root + st[(deg + off[i]) % m]) % 12, dup = 0;
        for (j = 0; j < n; j++)
            dup |= pcs[j] == pc;
        if (!dup)
            pcs[n++] = pc;
    }
    return n;
}
int en_chord_tones(uint32_t pool, int root, int deg, int *pcs) { return en_chord_kind(pool, root, deg, EN_CH_AUTO, pcs); }

static int nearest(int from, int pc, int lo, int hi)    /* the note of class pc nearest from, in lo..hi */
{
    int n = from + ((pc - from) % 12 + 12) % 12;          /* at or above */
    if (n - from > 6)
        n -= 12;
    while (n < lo)
        n += 12;
    while (n > hi)
        n -= 12;
    return n;
}

void en_voice_lead(const int *from, const int *pcs, int nt, int *to, int lo, int hi)
{
    static const uint8_t PERM[24][4] = {
        {0, 1, 2, 3}, {0, 1, 3, 2}, {0, 2, 1, 3}, {0, 2, 3, 1}, {0, 3, 1, 2}, {0, 3, 2, 1},
        {1, 0, 2, 3}, {1, 0, 3, 2}, {1, 2, 0, 3}, {1, 2, 3, 0}, {1, 3, 0, 2}, {1, 3, 2, 0},
        {2, 0, 1, 3}, {2, 0, 3, 1}, {2, 1, 0, 3}, {2, 1, 3, 0}, {2, 3, 0, 1}, {2, 3, 1, 0},
        {3, 0, 1, 2}, {3, 0, 2, 1}, {3, 1, 0, 2}, {3, 1, 2, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}};
    int t[4], p, v, best = 1 << 30;
    for (v = 0; v < 4; v++)
        t[v] = pcs[v < nt ? v : 0];                        /* three tones: the root doubled */
    for (p = 0; p < 24; p++) {
        int c = 0, n[4], a, b;
        for (v = 0; v < 4; v++) {
            n[v] = nearest(from[v], t[PERM[p][v]], lo, hi);
            c += n[v] > from[v] ? n[v] - from[v] : from[v] - n[v];
        }
        for (a = 0; a < 4; a++)                            /* no two voices on one note */
            for (b = a + 1; b < 4; b++)
                if (n[a] == n[b])
                    c += 40;
        if (c < best) {
            best = c;
            for (v = 0; v < 4; v++)
                to[v] = n[v];
        }
    }
}

const char *en_chord_name(const int *pcs, int n, char *buf)
{
    uint32_t iv = 0;
    int i, k = 0, third, fifth;
    const char *q = "", *r = EN_NOTE_NAME[pcs[0] % 12];
    for (i = 1; i < n; i++)
        iv |= 1u << ((pcs[i] - pcs[0] + 12) % 12);
    third = iv >> 4 & 1u ? 4 : iv >> 3 & 1u ? 3 : 0;
    fifth = iv >> 7 & 1u ? 7 : iv >> 6 & 1u ? 6 : iv >> 8 & 1u ? 8 : 0;
    if (third == 4) {
        q = fifth == 8 ? "aug" : iv >> 11 & 1u ? (iv >> 2 & 1u ? "M9" : "M7") : iv >> 10 & 1u ? (iv >> 2 & 1u ? "9" : "7")
            : iv >> 9 & 1u ? "6"
            : iv >> 2 & 1u ? "add9" : "";
    } else if (third == 3) {
        if (fifth == 6)
            q = iv >> 10 & 1u ? "m7b5" : iv >> 9 & 1u ? "dim7" : "dim";
        else if (iv >> 10 & 1u)
            q = iv >> 5 & 1u ? "m11" : iv >> 2 & 1u ? "m9" : "m7";
        else
            q = iv >> 11 & 1u ? "mM7" : iv >> 9 & 1u ? "m6" : iv >> 2 & 1u ? "madd9" : "m";
    } else {
        q = iv >> 5 & 1u ? (iv >> 10 & 1u ? "7sus4" : "sus4") : iv >> 2 & 1u ? "sus2" : "5";
    }
    while (*r)
        buf[k++] = *r++;
    while (*q)
        buf[k++] = *q++;
    buf[k] = 0;
    return buf;
}

/* ------------------------------------------------------------ commands --- */
enum { C_SET, C_POOL, C_PLAY, C_NEXT, C_FREEZE, C_SEED, C_PANIC, C_CLOCK, C_REGROW, C_SECTION, C_FX, C_MUTE, C_NOTE, C_HUSH, C_HOP, C_FIND, C_LAYER, C_DJ, C_SHED };
static uint8_t muted;                            /* remote: no sound, no MIDI (C_MUTE, after what's queued) */
static uint8_t hush;                             /* a store while playing: fade out, hold, fade back in (C_HUSH) */
static float hush_g = 1.0f;                      /* its gain, 0..1 */
volatile uint8_t en_hushed;                      /* faded right out: the flash can be written */
static uint8_t shed;                             /* a piece change: the four engines dip, the drums stay */
static float shed_g = 1.0f;
volatile uint8_t en_shed_done;
#define QN 256u                                  /* more than every parameter at once (project_apply), with room */
static uint32_t q[QN];
static volatile uint32_t q_w, q_r;
static void post(uint32_t c, int a, int b)
{
    uint32_t w = q_w;
    if (w - q_r >= QN)
        return;                                   /* full: dropped (never, at the UI's rate) */
    q[w % QN] = c << 24 | ((uint32_t)a & 0xFFu) << 16 | ((uint32_t)b & 0xFFFFu);
    BARRIER();
    q_w = w + 1;
}
void en_set(int p, int v) { post(C_SET, p, v); }
void en_pool(uint32_t pool) { post(C_POOL, 0, (int)(pool & 0xFFFu)); }
void en_play(int on) { post(C_PLAY, on, 0); }
void en_next(void) { post(C_NEXT, 0, 0); }
void en_next_now(void) { post(C_NEXT, 1, 0); }
void en_freeze(int on) { post(C_FREEZE, on, 0); }
void en_seed(uint32_t s) { post(C_SEED, 0, (int)(s & 0xFFFFu)); }
void en_panic(void) { post(C_PANIC, 0, 0); }
void en_mute(int on) { post(C_MUTE, on, 0); }
void en_hush(int on) { post(C_HUSH, on, 0); }
void en_shed(int out) { post(C_SHED, out, 0); }
void en_note(int voice, int note, int on) { post(C_NOTE, voice, (note & 0x7F) | (on ? 0x100 : 0)); }
void en_hands_off(void) { post(C_NOTE, 0xFF, 0); } /* not a note: every key has let go */
void en_clock(int msg) { post(C_CLOCK, msg, 0); }
void en_regrow(void) { post(C_REGROW, 0, 0); }
void en_section(int cmd) { post(C_SECTION, cmd, 0); }
void en_fx(uint32_t mask)                       /* in two halves: the queue carries 16 bits */
{
    post(C_FX, 0, (int)(mask & 0xFFFFu));
    post(C_FX, 1, (int)(mask >> 16 & 0xFFFFu));
}
void en_dj(unsigned mask) { post(C_DJ, 0, (int)(mask & 0x3FFFu)); }
void en_hop(int kind) { post(C_HOP, kind, 0); } /* the audio thread grabs the ring when it hears this */
void en_find(int mode) { post(C_FIND, mode, 0); } /* Lets Find Out, on the same thread */
void en_layers(unsigned mask) { post(C_LAYER, 0, (int)(mask & 0xFFFu)); }
void en_layer_house(int on) { post(C_LAYER, 1, on ? 1 : 0); }
void en_layer_level(int part, int lvl)
{
    if (part < 0 || part > 2)
        return;
    if (lvl < 0)
        lvl = 0;
    if (lvl > 100)
        lvl = 100;
    post(C_LAYER, part + 2, lvl);
}

/* -------------------------------------------------------------- state --- */
volatile uint8_t en_playing, en_frozen, en_ext;
volatile uint8_t en_chord[EN_NPAD], en_chord_root, en_chord_n;
volatile float en_drift, en_pad_level, en_bass_level, en_bass_env;
volatile float en_noise_level, en_acid_level;
volatile uint8_t en_acid_len = 16, en_acid_pos;
volatile uint16_t en_acid_gate, en_acid_acc;
volatile float en_acid_glow;
volatile uint8_t en_held;
volatile uint8_t en_drum_pos;
volatile float en_drum_glow[EN_NDRUM];
volatile float en_xdrum_glow[EN_XDRUM];
volatile uint8_t en_pat_now, en_pat_next;
volatile uint32_t en_bars;
volatile float en_arc;
volatile uint8_t en_pulse_rest;
volatile uint8_t en_pad_note[EN_NPAD];
volatile uint8_t en_hand_note;                   /* last hand note, after fold_note. The screen does not read it */
static float keys_amt = 1.0f;                    /* GLO white keys. 1 is the voice. IAM boots a tenth under */
volatile float en_pad_amp[EN_NPAD];

static int16_t par[P_NPARAMS];
static int links_on(void) { return par[P_LINKS] != 0; }
static uint32_t pool = 0xAADu;                  /* Ionian until told */
static int nsteps, steps[12];                   /* the scale */
static int deg;                                 /* the chord's scale degree */
static uint32_t sixteenth;                      /* the grid. tick() bumps it after the step plays */
static int chord_pcs[4], chord_nt;
static float tunefac = 1.0f;
static uint32_t rng = 0x9E3779B9u;

static uint32_t rnd(void)
{
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}
static float frand(void) { return (float)(rnd() >> 8) * (1.0f / 16777216.0f); }
static int irand(int n) { return n > 0 ? (int)((rnd() >> 8) % (uint32_t)n) : 0; }

static void midi(uint32_t st, uint32_t d1, uint32_t d2)
{
    if (par[P_MIDI] && !muted)
        en_midi_out(st, d1, d2);
}

static float mtof(float n) { return 440.0f * tunefac * fm_exp2f((n - 69.0f) * (1.0f / 12.0f)); }

/* a sine table: 1024 points a turn, plus one */
#define SN 1024
static float sint[SN + 1];
static inline float sine(float ph)              /* ph in turns, any value >= 0 */
{
    float x = (ph - (float)(int)ph) * (float)SN;
    int i = (int)x;
    float f = x - (float)i;
    return sint[i] + (sint[i + 1] - sint[i]) * f;
}

static inline float sine_any(float ph)          /* ph in turns, any value (FM's phases go below 0) */
{
    float fl = (float)(int)ph;
    if (ph < fl)
        fl -= 1.0f;
    return sine(ph - fl);
}

/* ------------------------------------------------- slow random motion --- */
#define NMOD 10                                 /* 0..5 the pads', 6..8 the echo's, 9 the drone's */
#define NMOD_PAD 6
static struct {
    float v, tgt, rate;
    uint32_t left;                              /* samples until a new target */
} mod[NMOD];

static void mods_step(uint32_t n)
{
    int i;
    for (i = 0; i < NMOD; i++) {
        if (mod[i].left <= n) {
            float secs = 7.0f + 33.0f * frand();
            mod[i].tgt = frand() * 2.0f - 1.0f;
            mod[i].left = (uint32_t)(secs * EN_SR);
            mod[i].rate = (float)n / (secs * EN_SR * 0.5f);
        } else {
            mod[i].left -= n;
        }
        mod[i].v += (mod[i].tgt - mod[i].v) * fm_minf(mod[i].rate, 1.0f);
    }
}
static float motion;                            /* 0..1: Motion */

/* -------------------------------------------------- the voice bank --- */
/* What each voice plays. The drums: a kit for each of the kick, the snare (with the clap), the hats (closed
 * and open) and the perc (KITS: the first five were the old styles'). The pad, drone and acid: sixteen
 * sounds each (PAD_SOUND, DRONE_SOUND, ACID_SOUND below). Before 1.0.27 a piece had one style for all of
 * it; en_style_sounds turns a stored style into these. */
typedef struct {
    float k_start, k_drop, k_dec0, k_dec1, k_click, k_drive, k_rumble;   /* kick: its sweep from k_start times
                                                 * its note, the drop's speed, its decay, click, drive, a rumble */
    float sn_f1, sn_f2, sn_noise, sn_body, sn_dec;   /* snare: its two tones (Hz), noise, body, decay */
    float cl_gap, cl_dec;                       /* clap: its bursts apart (samples), its tail */
    float h_metal, h_noise, h_dec, oh_dec;      /* hats: the metal's pitch, the noise in it, the decays */
    float p_freq, p_ratio, p_idx;               /* perc: its note (Hz), the modulator's ratio, its index */
    float duck, chop, crush;                    /* the kick's: its duck; chops and crush (the drums all) */
} kit_t;
static const kit_t KITS[EN_NKIT_OSC] = {
    /* Hypno: as it always was */
    {5.0f, 0.0016f, 0.15f, 0.75f, 0.25f, 1.0f, 0.0f,
     185.0f, 330.0f, 1.1f, 0.7f, 1.0f, 353.0f, 1.0f,
     1.0f, 0.6f, 1.0f, 1.0f, 380.0f, 1.48f, 1.4f, 1.0f, 0.0f, 0.0f},
    /* House: a round kick, a wide clap, open hats, congas */
    {3.2f, 0.0011f, 0.2f, 0.8f, 0.12f, 0.6f, 0.0f,
     210.0f, 360.0f, 1.3f, 0.5f, 0.9f, 440.0f, 1.4f,
     1.25f, 0.9f, 1.1f, 1.4f, 300.0f, 1.0f, 0.6f, 1.25f, 0.0f, 0.0f},
    /* Techno: a hard kick with a rumble, metal hats, a tight clap, a bell perc */
    {7.0f, 0.0024f, 0.12f, 0.5f, 0.4f, 1.6f, 0.6f,
     170.0f, 290.0f, 1.4f, 0.4f, 0.7f, 260.0f, 0.7f,
     1.55f, 0.35f, 0.7f, 1.3f, 560.0f, 3.5f, 2.2f, 1.1f, 0.0f, 0.0f},
    /* Acid: a punchy 909-ish kit with a cowbell */
    {4.0f, 0.0014f, 0.13f, 0.6f, 0.35f, 1.25f, 0.0f,
     180.0f, 330.0f, 1.5f, 0.6f, 1.2f, 440.0f, 1.1f,
     1.0f, 0.6f, 0.9f, 1.6f, 560.0f, 1.48f, 2.0f, 1.0f, 0.0f, 0.0f},
    /* Break: a distorted kick, a cracking snare, noisy hats, a tom; chopped and crushed */
    {6.0f, 0.0022f, 0.1f, 0.45f, 0.45f, 2.5f, 0.2f,
     240.0f, 420.0f, 1.6f, 0.8f, 0.6f, 300.0f, 0.8f,
     1.3f, 1.0f, 0.6f, 0.8f, 200.0f, 1.0f, 0.9f, 0.8f, 1.0f, 0.6f},
    /* Minimal: a short clicky kick, a rim, tight hats, a woodblock */
    {4.0f, 0.0018f, 0.08f, 0.35f, 0.5f, 1.0f, 0.0f,
     400.0f, 820.0f, 0.5f, 1.0f, 0.5f, 300.0f, 0.6f,
     1.4f, 0.3f, 0.5f, 0.7f, 720.0f, 2.0f, 0.8f, 0.9f, 0.0f, 0.0f},
    /* Hard: an industrial kick, distorted with a long rumble, a noisy snare, a metal clank; a little crushed */
    {8.0f, 0.0028f, 0.14f, 0.6f, 0.6f, 3.2f, 0.9f,
     200.0f, 340.0f, 1.8f, 0.5f, 0.8f, 220.0f, 0.8f,
     1.7f, 0.8f, 0.8f, 1.2f, 140.0f, 2.76f, 2.8f, 1.3f, 0.0f, 0.25f},
    /* 808: a long boom, a snappy snare, a wide clap, a cowbell */
    {2.2f, 0.0007f, 0.3f, 1.6f, 0.08f, 0.5f, 0.0f,
     238.0f, 476.0f, 1.0f, 0.9f, 1.1f, 380.0f, 1.6f,
     1.15f, 0.5f, 0.8f, 1.8f, 540.0f, 1.48f, 1.2f, 1.0f, 0.0f, 0.0f},
    /* 909: a punchy kick, a noisy snare, a tight clap, bright hats, a low tom */
    {4.6f, 0.0021f, 0.10f, 0.42f, 0.55f, 1.2f, 0.0f,
     188.0f, 340.0f, 1.65f, 0.4f, 0.62f, 250.0f, 0.55f,
     1.7f, 0.4f, 0.42f, 1.05f, 160.0f, 1.0f, 0.55f, 1.0f, 0.0f, 0.0f},
};
static const kit_t *KK = &KITS[0], *KS = &KITS[0], *KH = &KITS[0], *KP = &KITS[0];   /* kick, snare,
                                                 * hats, perc */
static const kit_t *KC, *KO;                    /* clap and the open hat. Null: the snare's kit, the hat's */
static uint8_t kit_i[4], kit_c, kit_o, kit_c_set, kit_o_set;   /* the kit index, including Dusty */
static const kit_t *kit_ptr(unsigned v)         /* Dusty borrows the house duck. Its hits are samples */
{
    if (v >= EN_NKIT_OSC)
        v = 1;
    return &KITS[v];
}

/* the pad's sounds: the oscillator, stabs (chords on the off-beats) or breaths, its bus filter and level */
enum { PO_RHODES, PO_VOICES, PO_STRINGS, PO_GLASS, PO_ORGAN, PO_WARM, PO_SUPER, PO_PWM, PO_MIST, PO_SINE,
       PO_BLOOM };
static const struct { uint8_t osc, stabs; float fc, g; } PAD_SOUND[EN_NSOUND] = {
    {PO_BLOOM, 0, 1.2f, 0.95f},                 /* Bloom: two sines, an octave that opens with the breath */
    {PO_VOICES, 0, 1.0f, 1.0f}, {PO_STRINGS, 0, 1.25f, 0.75f}, {PO_GLASS, 0, 1.15f, 1.0f},
    {PO_ORGAN, 0, 1.1f, 0.8f}, {PO_WARM, 0, 0.85f, 1.0f}, {PO_RHODES, 1, 1.3f, 0.9f}, {PO_VOICES, 0, 0.6f, 0.8f},
    {PO_STRINGS, 1, 0.55f, 1.4f},               /* Dub: saw chords stabbed, dark, into the echo */
    {PO_SUPER, 0, 1.3f, 0.75f},                 /* Trance: a supersaw */
    {PO_PWM, 0, 1.1f, 1.1f},                   /* Poly: two detuned saws less each other, a pulse's width drifting */
    {PO_MIST, 0, 1.1f, 0.85f},                  /* Mist: a sine, its detuned pair, and a quiet fifth */
    {PO_GLASS, 1, 1.6f, 1.05f},                 /* Pings: a mallet bar, stabbed */
    {PO_ORGAN, 1, 1.3f, 1.05f},                 /* Hits: a house organ, stabbed */
    {PO_SUPER, 1, 1.5f, 1.1f},                  /* Rave: supersaw stabs */
    {PO_SINE, 0, 1.0f, 0.65f},                   /* Sine: pure, for minimal */
};
/* the drone's: the oscillator; the saws and sub's levels, drive, cutoff (octaves) and level */
enum { DO_SAWS, DO_REESE, DO_PULSE, DO_FM, DO_ORGAN, DO_SINE, DO_SQUARE, DO_HOOVER, DO_FIFTHS, DO_METAL, DO_SYNC,
       DO_DIRTY };
typedef struct { uint8_t osc; float saw, sub, drive, cut, g; } drone_snd_t;
static const drone_snd_t DRONE_SOUND[EN_NSOUND] = {
    {DO_SAWS, 0.4f, 0.55f, 1.0f, 0.0f, 1.0f},   {DO_SAWS, 0.3f, 0.75f, 0.8f, -0.4f, 1.0f},
    {DO_SAWS, 0.45f, 0.5f, 2.2f, -0.3f, 1.0f},  {DO_DIRTY, 0.85f, 0.35f, 1.55f, 0.35f, 0.9f}, /* Dirty: a narrow pulse and its octave */
    {DO_REESE, 0.5f, 0.4f, 1.4f, 0.0f, 1.0f},   {DO_PULSE, 0.45f, 0.45f, 1.1f, 0.0f, 1.0f},
    {DO_FM, 0.55f, 0.35f, 1.2f, 0.4f, 1.0f},    {DO_ORGAN, 0.45f, 0.5f, 1.0f, 0.6f, 0.9f},
    {DO_SAWS, 0.12f, 1.0f, 2.6f, -1.0f, 0.95f}, /* Rumble: the sub driven, the saws far back: a techno rumble */
    {DO_SINE, 0.6f, 0.5f, 1.3f, 0.0f, 1.05f},   /* Sub: a sine and its sub, a little drive */
    {DO_SQUARE, 0.45f, 0.45f, 1.0f, 0.2f, 0.95f},   /* Square: a hollow deep-house bass */
    {DO_HOOVER, 0.4f, 0.5f, 1.6f, 0.5f, 0.85f}, /* Hoover: detuned pulses, far apart, and an octave down */
    {DO_FIFTHS, 0.4f, 0.5f, 1.2f, 0.2f, 0.9f},  /* Fifths: the root and its fifth, a dub techno chord */
    {DO_METAL, 0.5f, 0.45f, 1.3f, 0.5f, 0.95f}, /* Metal: FM at a ratio off the harmonics: clangy */
    {DO_SYNC, 0.45f, 0.5f, 1.2f, 0.3f, 0.85f},  /* Sync: a saw synced, the envelope sweeping it */
    {DO_SAWS, 0.45f, 0.4f, 1.2f, 1.2f, 0.85f},  /* Bright: the saws, the filter open */
};
/* the acid's: the oscillator, its drive and level */
enum { AO_SAW, AO_SQUARE, AO_SYNC, AO_PLUCK, AO_PWM, AO_CHIP, AO_SUB, AO_TRANCE, AO_BLEEP, AO_FIFTH, AO_BELL, AO_ORGAN,
       AO_LASER, AO_FOLD, AO_SOFT, AO_DRIVE };
typedef struct { uint8_t osc; float drive, g; } acid_snd_t;
static const acid_snd_t ACID_SOUND[EN_NSOUND] = {
    {AO_SAW, 1.0f, 1.0f}, {AO_SQUARE, 1.0f, 1.0f}, {AO_DRIVE, 1.15f, 1.05f}, {AO_SYNC, 1.1f, 0.85f},
    {AO_PLUCK, 1.0f, 1.15f}, {AO_PWM, 1.0f, 1.0f}, {AO_CHIP, 1.2f, 0.7f}, {AO_SUB, 1.1f, 0.95f},
    {AO_TRANCE, 1.0f, 0.9f},                   /* Trance: three saws detuned, a trance arp's */
    {AO_BLEEP, 0.9f, 0.85f},                     /* Bleep: a sine, the bleeps of early techno */
    {AO_FIFTH, 1.1f, 0.85f},                    /* Fifth: a square and a sine a fifth up */
    {AO_BELL, 1.0f, 1.0f},                      /* Bell: a tube, brighter as the note starts */
    {AO_ORGAN, 1.0f, 0.95f},                    /* Organ: the twelfth shouts, then an 8' and a 4' */
    {AO_LASER, 1.0f, 1.0f},                     /* Laser: a sine, its pitch falling with the envelope */
    {AO_FOLD, 1.1f, 1.0f},                      /* Fold: a sine folded over, more as the envelope opens */
    {AO_SOFT, 0.9f, 0.95f},                      /* Soft: a triangle, round, for quiet arps */
};

static int pad_tone(void)
{
    unsigned v = (unsigned)par[P_PADTONE];
    return v < EN_NPAD_SND ? (int)v : 0;
}
static int pad_is_smp(void) { return pad_tone() >= EN_NSOUND; }
static float pad_smp_gain(void)               /* dry chord peaks, brought near the oscillators */
{
    static const float G[4] = {1.50f, 0.95f, 2.05f, 1.35f}; /* FM piano, vibes, kalimba, harp */
    int i = pad_tone() - EN_NSOUND;
    if (i < 0)
        i = 0;
    if (i > 3)
        i = 3;
    return G[i];
}
static int pad_snd(void)
{
    int t = pad_tone();
    return t < EN_NSOUND ? t : 0;               /* the oscillator table. A sample does not index it */
}
static int pad_stabs(void) { return PAD_SOUND[pad_snd()].stabs; }
static int pad_osc(void) { return PAD_SOUND[pad_snd()].osc; }
static uint8_t stabs_were;                      /* the pad was stabbing (to start the breaths again after) */

/* ------------------------------------------------- sampled library --- */
/* IMA ADPCM from iam_samples.h (CC0 Versilian). One reader per pad voice, one
 * for the bass, one for the acid, and one per drum. The bytes stay in flash.
 * A sample replaces that engine's oscillator. It does not play beside it. */
static const int16_t ISTEP[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97, 107,
    118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963,
    1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894,
    6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767};
static const int8_t IIDX[8] = {-1, -1, -1, -1, 2, 4, 6, 8};
typedef struct {
    const iam_zone_t *z;
    uint32_t k;
    float frac, step, s0, s1;
    int32_t pred;
    int16_t idx;
    uint8_t done, live, note, set;
    uint16_t tag;
} iam_smp;
static iam_smp pad_smp[EN_NPAD], bass_smp, acid_smp, drum_smp[EN_NDRUM], xdrum_smp[EN_XDRUM];
static uint16_t pad_tag[EN_NPAD], acid_tag;

static const iam_zone_t *iam_zone(int set, int note)
{
    const iam_set_t *st;
    int k;
    if ((unsigned)set >= IAM_SMP_NSETS)
        set = 0;
    st = &IAM_SET[set];
    for (k = 0; k < st->nz; k++) {
        const iam_zone_t *z = &IAM_ZONE[st->z0 + k];
        if (note >= z->lo && note <= z->hi)
            return z;
    }
    return &IAM_ZONE[st->z0];
}
static float iam_smp_next(iam_smp *s)
{
    const iam_zone_t *z = s->z;
    uint32_t k = s->k;
    int c, st, d;
    if (!z || k >= z->n) {
        if (z && z->looped) {
            k = z->ls;
            s->pred = z->pred;
            s->idx = z->idx;
        } else {
            s->done = 1;
            return 0.0f;
        }
    }
    c = (IAM_SMP_DATA[z->off + (k >> 1)] >> ((k & 1u) * 4u)) & 15;
    st = ISTEP[s->idx];
    d = st >> 3;
    if (c & 4)
        d += st;
    if (c & 2)
        d += st >> 1;
    if (c & 1)
        d += st >> 2;
    s->pred += c & 8 ? -d : d;
    if (s->pred > 32767)
        s->pred = 32767;
    else if (s->pred < -32768)
        s->pred = -32768;
    s->idx = (int16_t)(s->idx + IIDX[c & 7]);
    if (s->idx < 0)
        s->idx = 0;
    else if (s->idx > 88)
        s->idx = 88;
    s->k = k + 1;
    return (float)s->pred * (1.0f / 32768.0f);
}
static void iam_smp_go(iam_smp *s, int set, int note, float ratio)
{
    const iam_zone_t *z;
    float step;
    if (note < 0)
        note = 0;
    if (note > 127)
        note = 127;
    if (ratio < 0.25f)
        ratio = 0.25f;
    if (ratio > 4.0f)
        ratio = 4.0f;
    z = iam_zone(set, note);
    step = (float)z->rate * (1.0f / 65536.0f) * fm_exp2f(((float)note - z->root) * (1.0f / 12.0f)) * ratio;
    if (step < 0.05f)
        step = 0.05f;
    if (step > 4.0f)
        step = 4.0f;
    s->z = z;
    s->k = 0;
    s->pred = 0;
    s->idx = 0;
    s->done = 0;
    s->frac = 0.0f;
    s->step = step;
    s->s0 = 0.0f;
    s->s1 = iam_smp_next(s);
    s->live = 1;
    s->note = (uint8_t)note;
    s->set = (uint8_t)set;
}
/* The same sample, a few scale steps off. The phase stays, so a pad does not restart. */
static void iam_smp_tune(iam_smp *s, int semi)
{
    float ratio, step;
    if (!s->live || !s->z)
        return;
    ratio = tunefac;
    if (semi)
        ratio *= fm_exp2f((float)semi * (1.0f / 12.0f));
    if (ratio < 0.25f)
        ratio = 0.25f;
    if (ratio > 4.0f)
        ratio = 4.0f;
    step = (float)s->z->rate * (1.0f / 65536.0f) * fm_exp2f(((float)s->note - s->z->root) * (1.0f / 12.0f)) * ratio;
    if (step < 0.05f)
        step = 0.05f;
    if (step > 4.0f)
        step = 4.0f;
    s->step = step;
}
static void iam_smp_at(iam_smp *s, int set, int note, float ratio, unsigned tag)
{
    if (s->live && s->set == (uint8_t)set && s->note == (uint8_t)note && s->tag == (uint16_t)tag)
        return;
    iam_smp_go(s, set, note, ratio);
    s->tag = (uint16_t)tag;
}
static float iam_smp_read(iam_smp *s)
{
    float x;
    if (!s->live || s->done || !s->z)
        return 0.0f;
    x = s->s0 + (s->s1 - s->s0) * s->frac;
    s->frac += s->step;
    while (s->frac >= 1.0f && !s->done) {
        s->frac -= 1.0f;
        s->s0 = s->s1;
        s->s1 = iam_smp_next(s);
    }
    return s->done ? 0.0f : x;
}
/* Dusty: cajon, rope snare, clap, closed hat, open hat, woodblock.
 * The extras: woodblock, rope tom, woodblock, shaker, open hat. */
static const uint8_t DUSTY_LANE[EN_NDRUM] = {36, 38, 41, 42, 43, 40};
static const uint8_t DUSTY_X[EN_XDRUM] = {40, 39, 40, 44, 43};
static int lane_kit_now(int d)                 /* 0 kick, 1 snare, 2 clap, 3 hat, 4 open hat, 5 perc */
{
    if (d == 0)
        return kit_i[0];
    if (d == 1)
        return kit_i[1];
    if (d == 2)
        return kit_c_set ? kit_c : kit_i[1];
    if (d == 3)
        return kit_i[2];
    if (d == 4)
        return kit_o_set ? kit_o : kit_i[2];
    return kit_i[3];
}
static int lane_dusty(int d) { return lane_kit_now(d) == EN_KIT_DUSTY; }

void en_style_sounds(int16_t *p)                /* a stored style (1..4): its kit and sounds, as it sounded */
{
    static const int8_t PADS[EN_NSTYLE] = {-1, 6, 7, -1, -1}, DRONES[EN_NSTYLE] = {0, 1, 2, 0, 3},
                        ACIDS[EN_NSTYLE] = {-1, -1, -1, 2, -1};
    int st = p[P_STYLE];
    if (st <= 0 || st >= EN_NSTYLE)
        return;
    p[P_KIT_KICK] = p[P_KIT_SNARE] = p[P_KIT_HAT] = p[P_KIT_PERC] = (int16_t)st;
    if (PADS[st] >= 0)
        p[P_PADTONE] = PADS[st];
    p[P_DR_INST] = DRONES[st];
    if (ACIDS[st] >= 0)
        p[P_AC_WAVE] = ACIDS[st];
    p[P_STYLE] = 0;
}
static float stab_env, stab_k = 0.99f;          /* the pad's stabs: the envelope, its fall each 32 samples */

/* ---------------------------------------------------------------- links --- */
/* The drums patched into the voices (P_LINKS on, SEL + SEQ): each kick opens the drone's filter for a
 * moment (a rolling bass from the kick's pattern), each snare or clap flashes the pad's filter open, a
 * hat on an acid note accents it, and the perc lane can be the acid's clock (it steps only on a perc
 * hit; in a breakdown, with the drums out, it runs free again). Each by its amount (0: none). */
static float lk_kick, lk_snare;                 /* their envelopes, 1 at a hit, falling */
static uint8_t lk_hat;                          /* a hat on this sixteenth */
static int links_on(void);

/* ---------------------------------------------------------------- pad --- */
/* Breathing voices, as the sung tape loops of Music for Airports' "2/1": each voice swells in, hangs
 * and fades on its own cycle (the pad's envelope: attack, hold, release, rest; each voice's a little
 * longer than the last, so they drift apart), taking its note of the chord as it starts, so the chord
 * forms and dissolves rather than sitting there. The timbre: two saws a few
 * cents apart through two formant band-passes, a vowel that Bright moves from "oo" through "ah" to
 * "eh", with a little of the first formant's low-pass for body. */
typedef struct {
    float ph[2], inc[2];
    float note, target;                         /* MIDI: the note of this breath, the next one's */
    float t, rate, peak, amp;                   /* t 0..1 through the breath, per 32 samples */
    float fa, fh, fr;                           /* the breath's attack, hold and release, as parts of it */
    float ic[4];                                /* the two filters' states */
    float c1[4], c2[4];                         /* their coefficients: a1, a2, a3, k */
    float vib, pan;
    float hf;                                   /* a handoff's fade: 1 normal */
    float out;                                  /* the shared phrase, when the pad is locked to the bar */
    uint8_t midi_note, hand;                    /* hand: 1 fading out of the old key, 2 into the new */
    uint8_t voices;                             /* this breath's oscillator (PO_*) */
    uint8_t play;                               /* played from the keys: 1 held, 2 let go (ringing out), 0 not */
    uint8_t pnote;                              /* the key that plays it */
    uint32_t pord;                              /* when it was played (the oldest is taken first) */
    float pamp;                                 /* a played note's envelope, 0..1 */
} pad_t;
static pad_t pad[EN_NPAD];
static float pad_gate, pad_gain, pad_vol, bright, detune = 0.00144f;
static const float PAD_PAN[EN_NPAD] = {-0.55f, -0.18f, 0.18f, 0.55f};
static const float BREATH[EN_NPAD] = {1.0f, 1.21f, 1.43f, 1.67f};    /* unequal cycles, as 2/1's 12..20 s loops */
static const float START_T[EN_NPAD] = {0.0f, 0.9f, 0.82f, 0.95f};    /* the first breaths overlap */
static const float VOWEL[4][2] = {{320.0f, 800.0f}, {480.0f, 900.0f}, {730.0f, 1090.0f}, {530.0f, 1840.0f}};
static float arcm;                              /* the slow arc, -1 sparse .. +1 full, times Motion */
static float pad_filt, pad_lvl = 1.0f, pad_pan, pad_pitch;   /* the pad's LFO, where it goes this chunk */
static int pad_lock;                            /* 1: one shared phrase. The private cycle still runs */

void en_pad_lock(int on) { pad_lock = on ? 1 : 0; }

static float breath(const pad_t *v, float t)    /* rise, hang, fall, rest: the envelope */
{
    if (t < v->fa)
        return 0.5f - 0.5f * fm_cosf(FM_PI * t / v->fa);
    t -= v->fa;
    if (t < v->fh)
        return 1.0f;
    t -= v->fh;
    if (t < v->fr)
        return 0.5f + 0.5f * fm_cosf(FM_PI * t / v->fr);
    return 0.0f;
}

/* the LFOs (the pad's, the drone's): synced to the tempo, 1/4 note .. 32 bars a cycle; sine, triangle,
 * saw, square, sample and hold, or a drift (a smooth wander to a new random place each cycle) */
typedef struct {
    float ph, v, a, b;                          /* the phase (turns); the value -1..1; S&H / drift: from, to */
} lfo_t;
static lfo_t pad_lfo, drone_lfo;
static float frand(void);
static void lfo_step(lfo_t *l, int rate, int shape, float n, float step_samples)
{
    float p;
    l->ph += n / (step_samples * 4.0f * LFO_RATE_BEATS[rate & 7]);
    if (l->ph >= 1.0f) {
        l->ph -= (float)(int)l->ph;
        l->a = l->b;
        l->b = frand() * 2.0f - 1.0f;
    }
    p = l->ph;
    switch (shape) {
    case 0: l->v = fm_sinf(FM_TWO_PI * p); break;
    case 1: l->v = p < 0.5f ? 4.0f * p - 1.0f : 3.0f - 4.0f * p; break;
    case 2: l->v = 2.0f * p - 1.0f; break;
    case 3: l->v = p < 0.5f ? 1.0f : -1.0f; break;
    case 4: l->v = l->b; break;
    default: l->v = l->a + (l->b - l->a) * (0.5f - 0.5f * fm_cosf(FM_PI * p)); break;
    }
}

static void svf_coefs(float *c, float f, float k)
{
    float g = fm_tanf(FM_PI * fm_clampf(f, 40.0f, 16000.0f) * (1.0f / EN_SR));
    c[0] = 1.0f / (1.0f + g * (g + k));
    c[1] = g * c[0];
    c[2] = g * c[1];
    c[3] = k;
}

static inline float svf_lp(float x, const float *c, float *s)   /* TPT SVF (Zavalishin): the low-pass */
{
    float v3 = x - s[1], v1 = c[0] * s[0] + c[1] * v3, v2 = s[1] + c[1] * s[0] + c[2] * v3;
    s[0] = 2.0f * v1 - s[0];
    s[1] = 2.0f * v2 - s[1];
    return v2;
}

static void midi(uint32_t st, uint32_t d1, uint32_t d2);
static void pad_breathe(pad_t *v, int i)        /* a new breath: its length, its strength, its note */
{
    float a = pe_atk_s(par[P_PE_ATK]), h = pe_hold_s(par[P_PE_HOLD]), r = pe_rel_s(par[P_PE_REL]),
          secs = a + h + r + pe_rest_s(par[P_PE_REST]);
    secs = fm_maxf(secs, 0.3f);
    v->fa = fm_maxf(a / secs, 0.002f);
    v->fh = h / secs;
    v->fr = fm_maxf(r / secs, 0.002f);
    secs *= BREATH[i] * (0.85f + 0.3f * frand());
    v->rate = 32.0f / (secs * EN_SR);
    v->peak = 0.6f + 0.4f * frand();
    if (en_playing && v->midi_note)
        midi(0x80, v->midi_note, 0);
    v->note = v->target;
    v->voices = (uint8_t)pad_osc();
    v->midi_note = (uint8_t)v->target;
    pad_tag[i]++;
    if (en_playing && !par[P_PAD_MAN])
        midi(0x90, v->midi_note, (uint32_t)(50.0f + 40.0f * v->peak));
}

/* a new key (piece, root, scale): the voices still singing the old one fade out in about a second and
 * breathe again on the new chord; a voice at rest just takes its new note. Chord drift doesn't do
 * this: there the old notes are meant to hang over */
#define HAND_STEP (32.0f / (1.0f * EN_SR))
static void pad_handoff(void)
{
    int i;
    for (i = 0; i < EN_NPAD; i++) {
        pad_t *v = &pad[i];
        if (v->play)                            /* played from the keys: left alone */
            continue;
        if (v->amp > 0.002f && ((float)v->midi_note != v->target || v->voices != pad_osc()))
            v->hand = 1;
        else if (!v->hand)
            v->note = v->target;
    }
}

/* a chord change: a voice singing a note the new chord doesn't have fades out in about a second and
 * breathes in on its new note; a voice on a shared note holds it to the end of its breath */
static void pad_leave(void)
{
    int i, k;
    for (i = 0; i < EN_NPAD; i++) {
        pad_t *v = &pad[i];
        int pc = v->midi_note % 12, shared = 0;
        if (v->play)
            continue;
        for (k = 0; k < chord_nt; k++)
            shared |= chord_pcs[k] == pc;
        if (v->amp > 0.002f && !shared && !v->hand)
            v->hand = 1;
        else if (v->amp <= 0.002f && !v->hand)
            v->note = v->target;
    }
}

static void pad_coefs(pad_t *v, int i)          /* every 32 samples: the breath, pitch, the vowel */
{
    float f, vw, f1, f2;
    int a;
    if (v->hand == 1) {                         /* out of the old key */
        v->hf -= HAND_STEP;
        if (v->hf <= 0.0f) {
            v->hf = 0.0f;
            v->hand = 2;
            v->t = 0.15f;                       /* a breath already rising, on the new chord */
            pad_breathe(v, i);
        }
    } else if (v->hand == 2) {                  /* into the new */
        v->hf += HAND_STEP * 0.6f;
        if (v->hf >= 1.0f) {
            v->hf = 1.0f;
            v->hand = 0;
        }
    }
    if (v->play) {                              /* played: its own envelope (Pad env's attack and release, quicker) */
        float rel = fm_clampf(pe_rel_s(par[P_PE_REL]) * 0.35f, 0.08f, 3.0f),
              atk = fm_clampf(pe_atk_s(par[P_PE_ATK]) * 0.12f, 0.006f, 2.0f);
        if (v->play == 1)
            v->pamp = fm_minf(v->pamp + 32.0f / (atk * EN_SR), 1.0f);
        else if ((v->pamp -= 32.0f / (rel * EN_SR)) <= 0.0f) {
            v->pamp = 0.0f;
            v->out = v->amp;                    /* the phrase resumes from this note, not from a frozen peak */
            v->play = 0;
            v->note = v->target;
            v->hand = 0;
            v->hf = 1.0f;
            v->t = 0.0f;                        /* a breath from the start: rising again (when it's back) */
            if (!par[P_PAD_MAN])
                pad_breathe(v, i);
        }
    } else if (!pad_stabs() && !par[P_PAD_MAN]) {   /* stabs: the tick plays them; off the algorithm: none */
        v->t += v->rate;
        if (v->t >= 1.0f) {
            v->t -= 1.0f;
            pad_breathe(v, i);
        }
    }
    v->vib += (4.6f + 0.4f * (float)i) * 32.0f / EN_SR;
    if (v->vib >= 1.0f)
        v->vib -= 1.0f;
    f = mtof(v->note + 0.08f * motion * mod[i].v + pad_pitch);              /* a slow drift, as tape; the LFO */
    v->inc[0] = f * (1.0f - detune) * (1.0f / EN_SR);   /* Detune: 2.5 .. 25 cents either way */
    v->inc[1] = f * (1.0f + detune) * (1.0f / EN_SR);
    vw = fm_clampf((float)bright * 0.01f + 0.25f * arcm + 0.2f * motion * mod[(i + 2) % NMOD_PAD].v + 0.4f * pad_filt,
                   0.0f, 1.0f) * 2.999f;
    a = (int)vw;
    vw -= (float)a;
    f1 = VOWEL[a][0] + (VOWEL[a + 1][0] - VOWEL[a][0]) * vw;
    f2 = VOWEL[a][1] + (VOWEL[a + 1][1] - VOWEL[a][1]) * vw;
    svf_coefs(v->c1, f1 * (1.0f + 0.03f * (float)i), 0.45f);
    svf_coefs(v->c2, f2 * (1.0f - 0.02f * (float)i), 0.3f);
}

static inline float blep_saw(float t, float dt)
{
    float y = 2.0f * t - 1.0f;
    if (t < dt) {
        float x = t / dt;
        y -= x + x - x * x - 1.0f;
    } else if (t > 1.0f - dt) {
        float x = (t - 1.0f) / dt;
        y -= x * x + x + x + 1.0f;
    }
    return y;
}

/* ----------------------------------------------------------------- drone --- */
/* (bass in the code) Three saws a few cents apart and a sub-octave sine, through two low-passes in a
 * row (24 dB, the first resonant), a little driven. The note hardly moves (the chord's root, gliding
 * when the chord changes); what moves is the filter: CUTOFF and RESO, its envelope (retriggered on
 * each chord or every 1 .. 8 bars), its LFO (to the cutoff, the pitch, the level or the drive), a slow
 * random drift, the arc, and the sections (it closes in a breakdown, opens through the build). */
static struct {
    float ph[3], sph, mph, note, target, amp;   /* mph: Metal's modulator, Sync's slave */
    float s[4], c1[4], c2[4];                   /* the filters' states, their coefficients */
    float env;                                  /* its envelope, 0..1 */
    float gtarget;                              /* the chord's root, while the keys play it */
    uint8_t midi_note, rest, env_up;            /* env_up: attacking */
    uint8_t play;                               /* played from the keys */
} bass;
static float bass_vol, reso_k = 1.0f;
static const float GLIDE_K = 1.0f / (0.6f * EN_SR);
static float brk;                               /* the sections, smoothed: +1 breakdown, -0.5 build, 0 groove */
static float duck_env, duck_dec = 0.9997f;      /* the kick's sidechain: 1 just hit, falling */

/* ------------------------------------------------------------------ acid --- */
/* A 303-ish line, made up as it goes: a loop of LENGTH sixteenths (1 .. 16, so it can turn against the
 * bar), each step a note or a rest (DENSITY), the note a chord tone (now and then another of the
 * scale) within RANGE octaves, some steps accented, some sliding into the next. MUTATE re-rolls a
 * few steps each time round; the notes follow the chord. The voice: a saw or a square through two
 * low-passes in a row, the first resonant, driven between them; an envelope sweeps the cutoff (ENV MOD,
 * DECAY), an accent sweeps it further and plays louder (ACCENT); a slide glides the pitch without a
 * new attack. */
static struct {
    float ph, inc, inc_t, amp, amp_t, env, edec, acc, s[4], c1[4], c2[4];
    int32_t gate_left;                          /* samples until the note's gate closes; -1 held (a slide) */
    uint8_t note, sliding;
    uint8_t play;                               /* played from the keys: the line steps on, silent */
    float ps, p3, lph;                          /* more phases (sync, sub, the trance saws...), the PWM's LFO */
} acid;
static int8_t ac_off[EN_NSTEP], ac_oct[EN_NSTEP];   /* each step: scale steps above the chord's degree, octave */
static uint16_t ac_gate, ac_accent, ac_slide;       /* bit s: step s plays; is accented; slides into the next */
static int ac_pos;
static float acid_vol;

/* ----------------------------------------------------------------- noise --- */
/* A bed of noise, in eight colours: white; pink (-3 dB an octave, Paul Kellet's filter); brown (-6 dB,
 * a leaky sum); blue (+3 dB: pink, differenced); violet (+6 dB: white, differenced); grey (loud at both
 * ends, as the ear hears flat); black (mostly silence: crackle, as dust on vinyl); yellow (pink through
 * a band-pass at ~1.8 kHz: a radio's hiss). TONE closes a low-pass on the bed; PUMP ducks it with the kick,
 * harder than the rest. Two generators, left and right.
 * en_haze picks a part for IAM. 0 is this bed, which HYP keeps. Dust is a shaker on the off-beats:
 * Tone opens its grain and shortens the note. */
typedef struct {
    float b0, b1, b2, br, pw, pp, y1, y2, crack, lp;
    uint32_t rng;
} nz_t;
static nz_t nz[2] = {{.rng = 0x1234567u}, {.rng = 0x89ABCDEu}};
static float noise_vol, nz_lp_k = 0.5f, nz_pump;
static int nz_color;
static int nz_shape;                            /* 0 bed, 1 dust, 2 chop, 3 air, 4 hush, 5 wind */
static int nz_oct;                              /* player octaves on those parts. 0 is the part as written */
static float nz_env, nz_ph, nz_hp[2];
/* Sixteen steps for the four engines. An open row is the voice as it was.
 * The fourth key stays on its own rhythm until IAM names the row. */
static uint16_t vgate[4] = {0xFFFFu, 0xFFFFu, 0xFFFFu, 0xFFFFu};
static float vgate_sm[3] = {1.0f, 1.0f, 1.0f};
static float vgate_nz = 1.0f;
static uint8_t vgate_on3;

void en_vstep(int part, unsigned mask)
{
    if (part < 0 || part > 3)
        return;
    mask &= 0xFFFFu;
    if (part < 3 && (uint16_t)mask == 0xFFFFu)
        vgate_sm[part] = 1.0f;                  /* an open row is the ungated voice */
    if (part == 3) {
        vgate_on3 = 1;
        if ((uint16_t)mask == 0xFFFFu)
            vgate_nz = 1.0f;
    }
    vgate[part] = (uint16_t)mask;
}

/* SEQ knob 2. The hits stay where they are. Each step leans off the written
 * note by a scale degree or two, and another turn opens that phrase. 0 is
 * the song. The downbeat stays put, so the bar still starts where it was. */
static volatile int8_t mel_d[4];                /* pad, bass, acid, fourth */
static volatile int8_t dmel_d[11];              /* the eleven drums, same phrase */
static int8_t mel_hold[EN_NDRUM];               /* the step a swung hit was armed on */
static int8_t xmel_hold[EN_XDRUM];
static const int8_t MEL_PHRASE[16] = {
    0,  1,  0, -1,
    2,  1, -1,  1,
    0, -1,  1,  2,
    1,  0, -1,  1
};
static int mel_deg(int depth, int step)
{
    int o;
    if (!depth)
        return 0;
    o = (int)MEL_PHRASE[step & 15] * depth;
    if (o > 3)
        o = 3;
    if (o < -3)
        o = -3;
    return o;
}
/* Semitones from the root, walking the scale. steps[0] is 0: the root is in it. */
static int scale_interval(int off)
{
    int d, o;
    if (!off || nsteps <= 0)
        return 0;
    d = off;
    o = 0;
    while (d < 0) {
        d += nsteps;
        o--;
    }
    o += d / nsteps;
    return steps[d % nsteps] + 12 * o - steps[0];
}
static int mel_render_semi(int part)           /* the step already playing */
{
    if ((unsigned)part > 3u || !mel_d[part])
        return 0;
    return scale_interval(mel_deg((int)mel_d[part], (int)((sixteenth - 1u) & 15u)));
}
static int mel_hit_semi(int key, int st)       /* a drum, on the step that arms the hit */
{
    if ((unsigned)key >= 11u || !dmel_d[key])
        return 0;
    return scale_interval(mel_deg((int)dmel_d[key], st));
}
void en_melody(int which, int depth)
{
    if (depth > 3)
        depth = 3;
    if (depth < -3)
        depth = -3;
    if (which >= 0 && which < 4)
        mel_d[which] = (int8_t)depth;
    else if (which >= 4 && which < 15)
        dmel_d[which - 4] = (int8_t)depth;
}

/* Dust's offbeats, and a chop on the & of each beat, until a row is named. */
static int nz_step_on(int shape, int st)
{
    if (vgate_on3)
        return (vgate[3] >> (st & 15)) & 1;
    if (shape == 1)
        return (st & 1) != 0;
    if (shape == 2)
        return (st & 3) == 2;
    return 1;
}
/* Sleep. Off unless the song asks. The dance songs never enter these. */
static volatile uint8_t sleep_on, sleep_bell, sleep_chime;
static float sl_l, sl_r, sl_wh, sl_wb, sl_ph, sl_gust, sl_hum, sl_form, sl_vib, sl_fl;
static float sl_bird_ph, sl_bird_f, sl_bird_df;
static int sl_bird_left, sl_bird_wait, sl_bird_n;
static float nz_hum_ph[4];                      /* air, hush, dust: sides, a fifth */
static float nz_rev = 0.15f, nz_echo;

/* ----------------------------------------------------------------- drums --- */
/* six voices, one a lane: a kick (a sine falling fast from five times its note, a click, driven), a
 * snare (two tuned sines and band-passed noise), a clap (three bursts of noise, then its tail), a
 * closed and an open hat (the 808's six squares' metal and noise, high-passed; the closed chokes the
 * open), a perc (an FM blip: a rim or a tom, by its tune). Each lane its level, tune and decay. */
typedef struct {
    float ph, inc, inc_end, amp, adec, idx, x1, x2, att, mph[4], pend, t, hp, ph2, inc2;
    int32_t wait;                               /* swing: samples until a hit that waits (pend: its velocity) */
    uint8_t on, note;
} drum_t;
static drum_t drum[EN_NDRUM];
static drum_t xdrum[EN_XDRUM];                  /* rim, tom, cowbell, shaker, crash */
static float xg[EN_XDRUM];
static volatile uint16_t xpat[EN_XDRUM];
static volatile uint8_t xon[EN_XDRUM], xkit_i[EN_XDRUM], xlvl[EN_XDRUM];
static volatile int8_t xtune[EN_XDRUM];
static volatile uint8_t xdecay[EN_XDRUM] = {50, 50, 50, 50, 50}; /* 50 is the length each hit already had */
static float pulse_vol, swing, drive_k = 1.0f, lane_g[EN_NDRUM], lane_tune[EN_NDRUM], lane_dec[EN_NDRUM];
static float step_len, pulse_left;               /* a sixteenth, in samples; to the next MIDI clock pulse */
static uint32_t noise = 22222u;
static const uint8_t DRUM_GM[EN_NDRUM] = {36, 38, 39, 42, 46, 37};   /* kick, snare, clap, hats, side stick */
/* the hats' metal: four square waves at the 808's inharmonic ratios (Hz / EN_SR); the style's pitch */
static const float HAT_INC[4] = {304.4f / EN_SR, 369.6f / EN_SR, 522.7f / EN_SR, 800.0f / EN_SR};
static float hat_inc[4] = {304.4f / EN_SR, 369.6f / EN_SR, 522.7f / EN_SR, 800.0f / EN_SR};


/* ------------------------------------------------------------------ space --- */
static float echo_send, rev_send, echo_len, echo_len_t, echo_fb = 0.55f, echo_fbx = 0.55f;   /* echo_fbx: with a throw */
static uint8_t echo_amt;                         /* performance echo. 0: this path is the one the song baked */
static float echo_sm;
static uint8_t verb_amt;                         /* performance plate. 0: the room the song baked */
static float verb_sm;
static uint8_t shim_amt;                         /* an octave in that plate's tail. 0 is dry */
static float shim_sm;
static uint8_t phase_amt;                        /* knob 3. 0 leaves the master alone */
static float phase_sm, phase_ph, phase_yl, phase_yr;
static float phase_z[2][4];
static uint8_t kn1_amt;                         /* knob 1. 0 leaves the master alone */
static float kn1_sm, kn1_ph;                     /* not the GLO ring key */
static uint8_t kn2_amt;                          /* knob 2. Not the GLO wah key */
static float kn2_sm, kn2_ph, kn2_c[4], kn2_z[2][2];
static uint8_t kn2_n;
static float rev_in_gain = 1.0f;                /* falls to 0 while frozen */

/* -------------------------------------------------------------- harmony --- */
static void scale_set(void)
{
    nsteps = en_scale(pool, steps);
}

static int acid_note(int s)                     /* acid step s's note: from the chord's degree, in its register */
{
    int bend = mel_d[2] ? mel_deg((int)mel_d[2], (int)(sixteenth & 15u)) : 0;
    int d = deg + ac_off[s] + bend, o, n;
    if (d >= 0)
        o = d / nsteps;
    else {                                      /* a downward phrase. The positive path stays the old divide */
        o = 0;
        while (d < 0 && nsteps > 0) {
            d += nsteps;
            o--;
        }
        if (nsteps > 0)
            o += d / nsteps;
    }
    n = 36 + par[P_ROOT] + steps[d % nsteps] + 12 * (o + ac_oct[s] + par[P_AC_OCT] + par[P_OCT]);   /* the root: C2 .. B2 */
    while (n > 96)
        n -= 12;
    while (n < 28)
        n += 12;
    return n;
}

static void pad_retarget(void)                  /* the chord's tones to the four voices */
{
    int from[4], to[4], i;
    chord_nt = en_chord_kind(pool, par[P_ROOT], deg, par[P_CHORD], chord_pcs);
    for (i = 0; i < EN_NPAD; i++)
        from[i] = (int)(pad[i].target + 0.5f);
    {   /* the pad's register (C3 .. F5), moved by its octaves and the song's, within C1 .. C8 */
        int sh = 12 * (par[P_PAD_OCT] + par[P_OCT]), lo = 48 + sh, hi = 77 + sh;
        while (lo < 24)
            lo += 12, hi += 12;
        while (hi > 108)
            lo -= 12, hi -= 12;
        en_voice_lead(from, chord_pcs, chord_nt, to, lo, hi);
        if (par[P_VOICING]) {                   /* open: drop 2, the second voice from the top an octave down */
            int a = -1, b = -1;
            for (i = 0; i < EN_NPAD; i++)
                if (a < 0 || to[i] > to[a])
                    a = i;
            for (i = 0; i < EN_NPAD; i++)
                if (i != a && (b < 0 || to[i] > to[b]))
                    b = i;
            if (to[b] - 12 >= lo - 7)
                to[b] -= 12;
        }
    }
    for (i = 0; i < EN_NPAD; i++) {             /* each voice takes its note at its next breath */
        pad[i].target = (float)to[i];
        en_chord[i] = (uint8_t)to[i];
    }
    en_chord_root = (uint8_t)chord_pcs[0];
    en_chord_n = (uint8_t)chord_nt;
}

static void bass_to(int pc)
{
    int n = 36 + pc % 12 + 12 * (par[P_BASSOCT] + par[P_OCT]);   /* C2..B2, its octaves and the song's */
    while (n < 24)                              /* kept where it can be heard: C1 .. B5 */
        n += 12;
    while (n > 83)
        n -= 12;
    bass.gtarget = (float)n;
    if (bass.play)                              /* the keys have it: the root waits */
        return;
    bass.target = (float)n;
    if (en_playing && bass.midi_note != n) {
        midi(0x82, bass.midi_note, 0);
        midi(0x92, (uint32_t)n, 80);
    }
    bass.midi_note = (uint8_t)n;
}

static float drift_left, drift_total, evolve;   /* evolve: Evolve, 0..1 */
static uint8_t change_pending, fifth_done;

static void drift_reset(void)
{
    drift_total = drift_secs(par[P_DRIFT]) * (0.7f + 0.6f * frand()) * EN_SR;
    drift_left = drift_total;
    fifth_done = 0;
}

static void chord_change(void)                  /* to another degree: the near, strong moves first */
{
    static const float W7[7] = {4.0f, 2.0f, 1.0f, 3.0f, 3.0f, 3.0f, 0.4f};
    float w[12], sum = 0.0f, r;
    int i;
    for (i = 0; i < nsteps; i++) {
        w[i] = i == deg ? 0.0f : nsteps == 7 ? W7[i] : i == 0 ? 3.0f : 1.5f;
        sum += w[i];
    }
    r = frand() * sum;
    for (i = 0; i < nsteps; i++) {
        r -= w[i];
        if (r <= 0.0f && w[i] > 0.0f)
            break;
    }
    if (i >= nsteps)
        i = deg == 0 ? (nsteps > 1 ? 1 : 0) : 0;
    deg = i;
    pad_retarget();
    pad_leave();
    bass_to(chord_pcs[0]);
    if (par[P_DE_TRIG] == 0)
        bass.env_up = 1;                        /* the drone's envelope: on each chord */
    drift_reset();
}

/* ----------------------------------------------------------- the acid line --- */
static void ac_roll(int st)                     /* a step made anew */
{
    float dens = (float)par[P_AC_DENS] * 0.01f;
    uint16_t m = (uint16_t)(1u << st);
    if (frand() < (st == 0 ? fm_minf(dens + 0.3f, 1.0f) : dens))   /* the first step: more often a note */
        ac_gate |= m;
    else
        ac_gate &= (uint16_t)~m;
    ac_off[st] = (int8_t)(frand() < 0.8f ? 2 * irand(chord_nt) : irand(nsteps));   /* a chord tone, or now and then not */
    ac_oct[st] = (int8_t)(frand() < 0.55f ? 0 : irand(par[P_AC_RANGE]));
    if (frand() < 0.3f)
        ac_accent |= m;
    else
        ac_accent &= (uint16_t)~m;
    if (frand() < (float)par[P_AC_SLIDE] * 0.007f)
        ac_slide |= m;
    else
        ac_slide &= (uint16_t)~m;
}

static void ac_publish(void)
{
    en_acid_gate = ac_gate;
    en_acid_acc = ac_accent;
    en_acid_len = (uint8_t)par[P_AC_LEN];
}

static void ac_regrow(void)
{
    int st;
    for (st = 0; st < EN_NSTEP; st++)
        ac_roll(st);
    ac_publish();
}

static void ac_mutate(void)                     /* the loop's end: Mutate re-rolls a few steps */
{
    float m = (float)par[P_AC_MUT] * 0.0025f + 0.05f * evolve;
    int st;
    for (st = 0; st < par[P_AC_LEN]; st++)
        if (frand() < m)
            ac_roll(st);
    ac_publish();
}

static void acid_step(void)                     /* a sixteenth of the acid line */
{
    int st = ac_pos, len = par[P_AC_LEN], prev = (st + len - 1) % len;
    uint16_t m = (uint16_t)(1u << st);
    en_acid_pos = (uint8_t)st;
    if (acid.play || par[P_AC_MAN]) {           /* the keys have it: the line steps on, unheard */
    } else if (ac_gate & m) {
        int n = acid_note(st), slid = (ac_slide >> prev & 1u) && (ac_gate >> prev & 1u) && acid.amp_t > 0.0f;
        float dec = 0.06f * fm_exp2f((float)par[P_AC_DEC] * 0.05f);   /* 0.06 .. 1.9 s */
        acid.inc_t = mtof((float)n) * (1.0f / EN_SR);
        if (!slid) {                            /* a new note: the envelope again */
            acid.inc = acid.inc_t;
            acid.env = 1.0f;
            acid.acc = (ac_accent & m) ? 1.0f : 0.0f;
            if (lk_hat && links_on())            /* the links: a hat here accents it */
                acid.acc = fm_maxf(acid.acc, (float)par[P_LK_HAT] * 0.012f);
            acid.amp_t = 1.0f;
            acid.edec = fm_expf(-6.9f * 32.0f / (dec * (acid.acc > 0.0f ? 0.6f : 1.0f) * EN_SR));
            if (acid.note)
                midi(0x83, acid.note, 0);
            midi(0x93, (uint32_t)n, acid.acc > 0.0f ? 120u : 90u);
            acid.note = (uint8_t)n;
            acid_tag++;
        }
        acid.sliding = (uint8_t)slid;
        if (sleep_on && !(ac_slide & m))
            acid.gate_left = (int32_t)(10.0f * step_len); /* a flute note, about two beats */
        else
            acid.gate_left = (ac_slide & m) ? -1 : (int32_t)(0.55f * step_len);
        en_acid_glow = 1.0f;
    } else if (acid.gate_left < 0) {            /* a slide into a rest: let go */
        acid.gate_left = 0;
        acid.amp_t = 0.0f;
    }
    if (++ac_pos >= len) {
        ac_pos = 0;
        ac_mutate();
    }
}

/* ---------------------------------------------------------------- drums --- */
/* The sequencer: six lanes of sixteen steps, six patterns (en_pats, the UI's to edit). A new pattern
 * (Pattern) waits for the end of the bar, so turning through several on the way plays none of them.
 * SWING delays the off sixteenths.
 *
 * The sections are the player's (en_section, SAVE on the FM-1): a breakdown on the next bar, the drums
 * gone, the drone's filter closing and the pad rising, for as long as it is left; then a build of four
 * bars, the hats back and a snare roll quickening (quarters, eighths, sixteenths), the filter opening;
 * or the drop at once. The drop often brings a new chord. */
enum { L_KICK, L_SNARE, L_CLAP, L_HAT, L_OHAT, L_PERC };
enum { SEC_GROOVE = EN_SEC_GROOVE, SEC_BREAK = EN_SEC_BREAK, SEC_BUILD = EN_SEC_BUILD };
static int sec, sec_bars, sec_req, pat_now, pat_next;   /* sec_bars: a build's bars left; sec_req: cmd + 1 */
static uint32_t bars;                           /* bars played (the drone's envelope counts them) */
static uint8_t fill_hold;
volatile uint8_t en_sec, en_sec_req;

/* DRUM DRIFT, as GRAV-1's Drift and LOF-1's: at each bar, a count to the next change; the bar before it a
 * fill (the snare and hats rolling in its second half); then the next pattern, or another from 1..Pool.
 * Mutate, at the same moments: Shift moves a hit or two a step (never the kick's: the floor stays), Morph
 * also adds or takes one. What it changes stays in the pattern (the UI saves en_pats). With Every on Chord,
 * a chord change starts the count: the fill next bar, the change the bar after. */
static int8_t dd_in = -1;                        /* bar lines to the change, -1 none */
static uint8_t dd_fill;                          /* this bar ends in a fill */
static uint32_t dd_bars;
volatile int8_t en_dd_left = -1;
volatile uint32_t en_dd_changes, en_dd_mutations;
static int dd_len(void) { static const uint8_t L[6] = {0, 4, 8, 16, 32, 64}; return L[(unsigned)par[P_DD_EVERY] < 6u ? par[P_DD_EVERY] : 0]; }
static int bits16(uint32_t b) { int n = 0; while (b) { n += (int)(b & 1u); b >>= 1; } return n; }

static void dd_mutate(int p)
{
    static const uint8_t LN[4] = {L_SNARE, L_HAT, L_OHAT, L_PERC};   /* the kick and the clap stay as written */
    int c;
    for (c = 0; c < 2; c++) {                   /* a hit or two, a step either way */
        int l = LN[irand(4)], on[16], m = 0, st;
        uint32_t b = en_pats[p][l];
        for (st = 0; st < 16; st++)
            if (b >> st & 1u)
                on[m++] = st;
        if (m) {
            int k = on[irand(m)], to = (k + (frand() < 0.5f ? 15 : 1)) & 15;
            if (!(b >> to & 1u))
                en_pats[p][l] = (uint16_t)((b & ~(1u << k)) | 1u << to);
        }
    }
    if (par[P_DD_MUTATE] == 2) {                /* Morph: one more, or one fewer */
        int l = LN[irand(4)], st;
        uint32_t b = en_pats[p][l];
        int h = bits16(b);
        if ((frand() < 0.5f && h > 1) || h >= 10) {
            for (st = irand(16); !(b >> st & 1u); st = (st + 1) & 15)
                ;
            b &= ~(1u << st);
        } else if (h < 10) {
            for (st = irand(16); b >> st & 1u; st = (st + 1) & 15)
                ;
            b |= 1u << st;
        }
        en_pats[p][l] = (uint16_t)b;
    }
    en_dd_mutations++;
}

static void dd_bar(void)                        /* at each bar line, before the new pattern is taken */
{
    int len = dd_len();
    if (!par[P_DD_CHANGE] && !par[P_DD_MUTATE]) {
        dd_in = en_dd_left = -1;
        dd_fill = 0;
        dd_bars = 0;
        return;
    }
    if (dd_in > 0)
        dd_in--;
    if (dd_in == 0) {
        int pool = par[P_DD_POOL] < 2 ? 2 : par[P_DD_POOL] > EN_NPAT ? EN_NPAT : par[P_DD_POOL], nb = pat_now;
        if (par[P_DD_CHANGE] == 1)
            nb = pat_now + 1 >= pool ? 0 : pat_now + 1;
        else if (par[P_DD_CHANGE] == 2)
            do
                nb = irand(pool);
            while (nb == pat_now);
        if (par[P_DD_CHANGE]) {
            pat_next = nb;
            par[P_PATTERN] = (int16_t)nb;
            en_dd_changes++;
        }
        if (par[P_DD_MUTATE])
            dd_mutate(pat_next);
        dd_in = -1;
        dd_bars = 0;
    } else if (len && dd_in < 0 && ++dd_bars >= (uint32_t)len - 1u) {
        dd_in = 1;
    }
    dd_fill = dd_in == 1 && par[P_DD_CHANGE];
    en_dd_left = (int8_t)(dd_in >= 0 ? dd_in : len ? len - (int)dd_bars : -1);
}

static void drum_publish(void)
{
    en_pulse_rest = (uint8_t)(sec == SEC_BREAK);
    en_sec = (uint8_t)sec;
    en_sec_req = (uint8_t)sec_req;
    en_pat_now = (uint8_t)pat_now;
    en_pat_next = (uint8_t)pat_next;
}

static void section_cmd(int c)                  /* the player's: on the bar */
{
    if (c == EN_CMD_BREAK) {
        sec = SEC_BREAK;
    } else if (c == EN_CMD_BUILD) {
        if (sec == SEC_BREAK) {
            sec = SEC_BUILD;
            sec_bars = 4;
        }
    } else if (sec != SEC_GROOVE) {             /* the drop */
        sec = SEC_GROOVE;
        if (frand() < 0.4f + 0.5f * evolve)
            change_pending = 1;
    }
}

static int kick_note(void) { return 28 + (par[P_ROOT] + 8) % 12; }   /* E1 .. Eb2, on the piece's root */

/* Held knob 2. An eighth-note sweep on one synth, or a pitch rise on one drum.
 * Zero never adds a term, so a song that has not been turned is the song. */
static uint8_t wob_amt[4], snap_amt[11];
static float wob_ph, wob_s;
static float wob_oct(int part)
{
    if ((unsigned)part > 3u || !wob_amt[part])
        return 0.0f;
    return 4.5f * ((float)wob_amt[part] * 0.01f) * wob_s;
}
static float wob_wag(int part)
{
    float d;
    if ((unsigned)part > 3u || !wob_amt[part])
        return 1.0f;
    d = (float)wob_amt[part] * 0.01f;
    return 1.0f - 0.7f * d * (0.5f - 0.5f * wob_s);
}
static float wob_bend(float note)
{
    if (!wob_amt[3])
        return note;
    return note + 8.0f * ((float)wob_amt[3] * 0.01f) * wob_s;
}
static float snap_rise(int d)
{
    if ((unsigned)d > 10u || !snap_amt[d])
        return 1.0f;
    return fm_exp2f((float)snap_amt[d] * 0.02f * (0.5f + 0.5f * wob_s));
}

/* Closed hats that vanished on a small speaker. House and Break already spoke.
 * Hard did not. Open hats and shakers keep the kit row. This is the closed hat only. */
static void hat_speak(unsigned kit, float *stretch, float *nzlift, float *gain)
{
    static const float S[9] = {1.9f, 1.0f, 2.5f, 1.75f, 1.0f, 2.9f, 2.4f, 2.0f, 3.1f};
    static const float N[9] = {1.45f, 1.0f, 2.4f, 1.5f, 1.0f, 2.7f, 1.75f, 1.65f, 2.3f};
    static const float G[9] = {1.9f, 1.0f, 2.3f, 1.75f, 1.0f, 2.5f, 1.55f, 1.85f, 2.4f};
    if (kit >= 9u)
        kit = 0;
    *stretch = S[kit];
    *nzlift = N[kit];
    *gain = G[kit];
}

static void drum_fire(int d, float vel, int st)
{
    drum_t *v = &drum[d];
    int semi = mel_hit_semi(d, st);
    float tr = semi ? fm_exp2f((float)semi * (1.0f / 12.0f)) : 1.0f;
    const float dec = lane_dec[d], tf = fm_exp2f((lane_tune[d] + (float)semi) * (1.0f / 12.0f));
    const float rise = snap_rise(d);            /* 1 until knob 2 sweeps this drum */
    if ((unsigned)d < (unsigned)EN_NDRUM)
        mel_hold[d] = (int8_t)st;
    v->on = 1;
    v->amp = vel;
    v->att = 0.0f;
    v->t = 0.0f;
    v->note = DRUM_GM[d];
    if (lane_dusty(d)) {                        /* the sampled kit, in place of this lane's oscillator */
        iam_smp_go(&drum_smp[d], IAM_SMP_KIT, DUSTY_LANE[d], tf * rise * tunefac);
        v->adec = fm_expf(-6.9f / (1.4f * EN_SR));
        v->att = 1.0f;
        if (d == L_HAT && lane_dusty(L_OHAT))
            drum[L_OHAT].adec = fm_expf(-6.9f / (0.12f * EN_SR)); /* the closed hat chokes the open one */
        if (d == L_KICK && lane_g[L_KICK] > 0.0f)
            duck_env = 1.0f;
    } else {
    switch (d) {
    case L_KICK: {                              /* a sine dropping fast from (five) times its note */
        float f = mtof((float)kick_note() + lane_tune[0] + (float)semi);
        v->inc = f * KK->k_start * rise * (1.0f / EN_SR);
        v->inc_end = f * (1.0f / EN_SR);
        v->adec = fm_expf(-6.9f / ((KK->k_dec0 + KK->k_dec1 * dec) * EN_SR));
        v->ph = 0.0f;
        v->idx = 1.0f;                          /* the click */
        v->x1 = KK->k_rumble;                    /* the rumble: a slow envelope less a fast one */
        v->x2 = KK->k_rumble;
        if (lane_g[L_KICK] > 0.0f)
            duck_env = 1.0f;
        break;
    }
    case L_SNARE:                               /* two tuned sines and noise */
        v->inc = KS->sn_f1 * tf * rise * (1.0f / EN_SR);
        v->inc2 = KS->sn_f2 * tf * rise * (1.0f / EN_SR);
        v->ph = v->ph2 = 0.0f;
        v->adec = fm_expf(-6.9f / ((0.08f + 0.3f * dec) * KS->sn_dec * EN_SR));
        v->hp = fm_clampf(0.42f * tf, 0.15f, 0.9f);   /* its noise's band */
        break;
    case L_CLAP: {                              /* three bursts of noise, then its tail */
        const kit_t *kc = KC ? KC : KS;
        v->inc = 185.0f * tf * rise * (1.0f / EN_SR);
        v->ph = 0.0f;
        v->adec = fm_expf(-6.9f / ((0.1f + 0.4f * dec) * kc->cl_dec * EN_SR));
        v->inc_end = fm_expf(-6.9f / (0.012f * EN_SR));   /* the bursts' decay */
        v->pend = vel;
        v->hp = fm_clampf(0.18f * tf, 0.08f, 0.5f);
        break;
    }
    case L_HAT: {                               /* closed: it chokes the open one */
        unsigned hk = kit_i[2] < 9u ? kit_i[2] : 0;
        float stretch, nzlift, hgain, nz;
        hat_speak(hk, &stretch, &nzlift, &hgain);
        (void)hgain;
        v->adec = fm_expf(-6.9f / ((0.015f + 0.1f * dec) * KH->h_dec * stretch * EN_SR));
        v->hp = fm_clampf(0.72f * tf, 0.4f, 1.1f);
        v->inc = KH->h_metal * rise;            /* the metal's pitch, for this hit */
        if (semi)
            v->inc *= tr;
        nz = KH->h_noise * nzlift;
        if (nz > 1.4f)
            nz = 1.4f;
        v->idx = nz;
        if (lane_g[L_HAT] > 0.0f) {             /* a hat that is out of the mix does not choke the open one */
            const kit_t *ko = KO ? KO : KH;
            float choke = 0.07f + 0.06f * ko->oh_dec; /* ~110..180 ms: cut, and the kit still speaks */
            drum[L_OHAT].adec = fm_expf(-6.9f / (choke * EN_SR));
        }
        break;
    }
    case L_OHAT: {
        const kit_t *ko = KO ? KO : KH;
        float nz = ko->h_noise > 0.0f ? ko->h_noise : 0.6f;
        /* Longer than the closed hat, and not its metal. The length is still this kit's. */
        v->adec = fm_expf(-6.9f / ((0.08f + 0.6f * dec) * ko->oh_dec * EN_SR));
        v->hp = fm_clampf(0.96f + 0.12f * nz, 0.9f, 1.1f);
        v->inc = (0.88f + 0.34f * ko->h_metal) * rise;
        if (semi)
            v->inc *= tr;
        v->idx = 0.9f + 0.55f * nz;
        break;
    }
    default: {                                  /* perc: an FM blip, falling into its note */
        float f = KP->p_freq * tf;
        v->inc = f * 2.0f * rise * (1.0f / EN_SR);
        v->inc_end = f * (1.0f / EN_SR);
        v->inc2 = KP->p_ratio;                   /* the modulator's ratio */
        v->idx = KP->p_idx;
        v->ph = v->ph2 = 0.0f;
        v->adec = fm_expf(-6.9f / ((0.04f + 0.35f * dec) * EN_SR));
        if (kit_i[3] == 0) {                    /* Hypno: a short clave. The tom keeps the old perc pitch */
            float f = 880.0f * tf;
            v->inc = f * 1.2f * rise * (1.0f / EN_SR);
            v->inc_end = f * (1.0f / EN_SR);
            v->inc2 = 2.03f;
            v->idx = 0.28f;
            v->adec = fm_expf(-6.9f / (0.07f * EN_SR));
        } else if (kit_i[3] == 7) {             /* 808: a falling conga, not the same FM blip */
            float f = 168.0f * tf;
            v->inc = f * 2.6f * rise * (1.0f / EN_SR);
            v->inc_end = f * 0.85f * (1.0f / EN_SR);
            v->inc2 = 1.0f;
            v->idx = 0.12f;
            v->adec = fm_expf(-6.9f / (0.26f * EN_SR));
        }
        break;
    }
    }
    if (sleep_on && (d == L_HAT || d == L_OHAT)) {
        v->adec = fm_expf(-6.9f / ((d == L_OHAT ? 2.8f : 1.2f) * EN_SR)); /* a chime: the open one rings on */
        v->ph = 0.0f;
        v->ph2 = 0.17f;
        v->mph[0] = 0.41f;
        v->att = 1.0f;
    } else if (sleep_on && d == L_PERC) {
        v->adec = fm_expf(-6.9f / (18.0f * EN_SR)); /* a bowl: the hum stays while the room is quiet */
        v->ph = v->ph2 = 0.0f;
        v->mph[0] = 0.2f;
        v->mph[1] = 0.55f;
        v->idx = 1.0f;
        v->att = 1.0f;
        v->inc = 174.6f * rise / EN_SR;     /* F3, and the sweep if knob 2 is open */
        if (semi)
            v->inc *= tr;
    }
    }
    if (links_on()) {                           /* the links: the hits into the voices */
        if (d == L_KICK)
            lk_kick = fm_maxf(lk_kick, vel);
        else if (d == L_SNARE || d == L_CLAP)
            lk_snare = fm_maxf(lk_snare, vel);
    }
    en_drum_glow[d] = 1.0f;
    midi(0x99, v->note, (uint32_t)(vel * 100.0f));
    midi(0x89, v->note, 0);
}

static void drum_hit(int d, float vel, int32_t wait)   /* wait: a swung sixteenth's delay */
{
    int st = (int)(sixteenth & 15u);
    if ((unsigned)d < (unsigned)EN_NDRUM)
        mel_hold[d] = (int8_t)st;
    if (wait > 0) {
        drum[d].wait = wait;
        drum[d].pend = vel;
    } else {
        drum_fire(d, vel, st);
    }
}

static void xdrum_fire(int i, float vel, int st)
{
    drum_t *v = &xdrum[i];
    unsigned kit = xkit_i[i] < EN_NKIT ? xkit_i[i] : 0;
    const kit_t *k;
    int semi = mel_hit_semi(6 + i, st);
    float tf = fm_exp2f(((float)xtune[i] + (float)semi) * (1.0f / 12.0f));
    if ((unsigned)i < (unsigned)EN_XDRUM)
        xmel_hold[i] = (int8_t)st;
    float scale, rise = snap_rise(6 + i);
    v->on = 1;
    v->amp = vel;
    v->att = 0.0f;
    v->t = 0.0f;
    v->ph = v->ph2 = 0.0f;
    v->note = 0;                                /* not the sleep chime, and not the bowl */
    v->wait = 0;
    /* 50 keeps the length these hits already had. 0 is a quarter of it. 100 is 1.75 times. */
    scale = xdecay[i] == 50 ? 1.0f : 0.25f + (float)xdecay[i] * 0.015f;
    if (kit == EN_KIT_DUSTY) {
        iam_smp_go(&xdrum_smp[i], IAM_SMP_KIT, DUSTY_X[i], tf * rise * tunefac);
        v->adec = fm_expf(-6.9f / (1.4f * EN_SR));
        v->att = 1.0f;
        en_xdrum_glow[i] = 1.0f;
        return;
    }
    k = &KITS[kit < EN_NKIT_OSC ? kit : 0];
    if (i == 0) {                               /* rim: a short, high snare */
        v->inc = k->sn_f1 * 2.35f * tf * rise * (1.0f / EN_SR);
        v->inc2 = k->sn_f2 * 1.8f * tf * rise * (1.0f / EN_SR);
        v->adec = fm_expf(-6.9f / (0.045f * scale * EN_SR));
        v->hp = fm_clampf(0.62f * tf, 0.2f, 0.95f);
        v->idx = 1.0f;
    } else if (i == 1 && (kit == 3 || kit == 7)) {
        /* Acid: a short hard tom. 808: a long pitch drop. Other kits keep the perc's tom. */
        float f = (kit == 7 ? 128.0f : 250.0f) * tf;
        v->inc = f * (kit == 7 ? 3.1f : 1.45f) * rise * (1.0f / EN_SR);
        v->inc_end = f * (kit == 7 ? 0.62f : 1.0f) * (1.0f / EN_SR);
        v->inc2 = kit == 7 ? 1.0f : 1.48f;
        v->idx = kit == 7 ? 0.18f : 1.7f;
        v->adec = fm_expf(-6.9f / ((kit == 7 ? 0.62f : 0.14f) * scale * EN_SR));
    } else if (i == 1) {                        /* tom: the perc, lower and longer */
        float f = k->p_freq * 0.55f * tf;
        v->inc = f * 1.6f * rise * (1.0f / EN_SR);
        v->inc_end = f * (1.0f / EN_SR);
        v->inc2 = 1.0f;
        v->idx = k->p_idx * 0.8f;
        v->adec = fm_expf(-6.9f / (0.28f * scale * EN_SR));
    } else if (i == 2 && (kit == 0 || kit == 3 || kit == 4)) {
        /* Hypno: a low agogo. Acid: a short bright bell. Break: a noisy clip. */
        float f = (kit == 0 ? 300.0f : kit == 3 ? 640.0f : 460.0f) * tf;
        float secs = kit == 0 ? 0.32f : kit == 3 ? 0.09f : 0.11f;
        v->inc = f * 1.04f * rise * (1.0f / EN_SR);
        v->inc_end = f * (1.0f / EN_SR);
        v->inc2 = kit == 4 ? 1.34f : 1.51f;
        v->idx = kit == 4 ? 1.15f : kit == 3 ? 0.85f : 0.18f;
        v->ph = 0.11f;
        v->ph2 = 0.43f;
        v->adec = fm_expf(-6.9f / (secs * scale * EN_SR));
    } else if (i == 2) {                        /* cowbell: two squares, a fifth and a bit apart.
                                                 * The perc's FM voice made this a tom. */
        float f = (340.0f + k->p_freq * 0.48f) * tf;
        float ratio = k->p_ratio;
        if (f < 320.0f)
            f = 320.0f;
        if (f > 860.0f)
            f = 860.0f;
        if (ratio < 1.4f)
            ratio = 1.47f + 0.06f * ratio;      /* a 1:1 tom is not a bell */
        else if (ratio > 2.2f)
            ratio = 1.55f + 0.08f * (ratio - 2.2f);
        if (ratio > 1.9f)
            ratio = 1.9f;
        v->inc = f * 1.06f * rise * (1.0f / EN_SR); /* the strike, just sharp of the note */
        v->inc_end = f * (1.0f / EN_SR);
        v->inc2 = ratio;
        v->idx = 0.42f;                         /* the click, gone in a few milliseconds */
        v->ph = 0.11f;
        v->ph2 = 0.43f;
        v->adec = fm_expf(-6.9f / (0.13f * scale * EN_SR));
    } else if (i == 3) {                        /* shaker: beads, not a hat and not one puff of noise */
        float body = k->h_dec > 0.25f ? k->h_dec : 0.45f;
        float bright = k->h_noise > 0.05f ? k->h_noise : 0.55f;
        v->hp = fm_clampf(0.72f + 0.38f * bright, 0.7f, 1.1f);
        v->adec = fm_expf(-6.9f / ((0.12f + 0.22f * body) * scale * EN_SR));
        v->inc = 0.75f + 0.7f * bright;         /* how fast the beads come */
        v->idx = 0.35f;                         /* the fine rattle, beside the louder beads */
        v->t = 4.0f + 6.0f * bright;            /* beads. pend is a swung hit's velocity */
        v->ph = 10000.0f;                       /* a bead on the first sample */
        v->inc_end = 70.0f;
        v->att = 1.0f;
        v->x1 = v->x2 = 0.0f;
        v->mph[0] = 10000.0f;
        v->mph[1] = 40.0f;
        v->mph[2] = v->mph[3] = 0.0f;
        if (semi) {                             /* no note to walk. The beads lean brighter or darker */
            float tr = fm_exp2f((float)semi * (1.0f / 12.0f));
            v->hp = fm_clampf(v->hp * tr, 0.7f, 1.1f);
        }
    } else {                                    /* crash: a different cymbal on each kit, not a long hat */
        /* Techno, Break, Minimal, and 909. The unused slots stay, so 909 does not move.
         * Hypno, House, Acid, Hard, and the 808 have no crash. */
        static const float hz[9] = {220.0f, 470.0f, 890.0f, 510.0f, 200.0f, 1100.0f, 190.0f, 155.0f, 1280.0f};
        static const float rat[9] = {1.73f, 1.68f, 2.41f, 1.81f, 1.91f, 2.27f, 2.71f, 3.17f, 1.33f};
        static const float nz[9] = {0.95f, 0.92f, 0.85f, 0.88f, 1.05f, 1.10f, 1.15f, 0.55f, 0.62f};
        static const float len[9] = {1.80f, 1.7f, 0.42f, 1.10f, 0.48f, 0.55f, 1.60f, 2.15f, 0.26f};
        unsigned ki;
        if (kit == 0 || kit == 1 || kit == 3 || kit == 6 || kit == 7) {
            v->on = 0;
            en_xdrum_glow[i] = 0.0f;
            return;
        }
        ki = kit < 9u ? kit : 0u;
        float metal = k->h_metal > 0.2f ? k->h_metal : 1.0f;
        float secs = len[ki] * (0.7f + 0.3f * k->oh_dec) * scale;
        float f = hz[ki] * tf * (0.92f + 0.08f * metal);
        if (secs < 0.16f)
            secs = 0.16f;
        if (secs > 2.3f)
            secs = 2.3f;
        if (f < 120.0f)
            f = 120.0f;
        if (f > 1400.0f)
            f = 1400.0f;
        v->inc = f * (1.0f / EN_SR);
        v->inc2 = rat[ki];
        v->idx = nz[ki];
        v->hp = fm_clampf(0.42f + 0.4f * nz[ki], 0.35f, 1.05f);
        v->adec = fm_expf(-6.9f / (secs * EN_SR));
        v->t = 2.0f;
        v->att = 0.0f;
        v->ph = 0.08f;
        v->ph2 = 0.37f;
        v->mph[0] = 0.61f;
        v->x1 = v->x2 = 0.0f;
    }
    en_xdrum_glow[i] = 1.0f;
}

static void xdrum_hit(int i, float vel, int32_t wait)
{
    int st = (int)(sixteenth & 15u);
    if ((unsigned)i < (unsigned)EN_XDRUM)
        xmel_hold[i] = (int8_t)st;
    if (wait > 0) {
        xdrum[i].wait = wait;
        xdrum[i].pend = vel;
    } else {
        xdrum_fire(i, vel, st);
    }
}

void en_xdrum(int i, int on, int kit, unsigned steps, int level, int tune, int decay)
{
    float lv;
    if (i < 0 || i >= EN_XDRUM)
        return;
    if (kit < 0)
        kit = 0;
    if (kit >= EN_NKIT)
        kit = EN_NKIT - 1;
    if (level < 0)
        level = 0;
    if (level > 100)
        level = 100;
    if (tune > 24)
        tune = 24;
    if (tune < -24)
        tune = -24;
    if (decay < 0)
        decay = 0;
    if (decay > 100)
        decay = 100;
    xon[i] = on ? 1u : 0u;
    xkit_i[i] = (uint8_t)kit;
    xpat[i] = (uint16_t)steps;
    xlvl[i] = (uint8_t)level;
    xtune[i] = (int8_t)tune;
    xdecay[i] = (uint8_t)decay;
    lv = (float)level;
    xg[i] = lv * lv * 0.0001f;
    if (!on || level <= 0)
        xdrum[i].on = 0;
}

void en_lane_kit(int lane, int kit)
{
    if (kit < 0)
        kit = 0;
    if (kit >= EN_NKIT)
        kit = EN_NKIT - 1;
    if (lane == L_CLAP) {
        kit_c = (uint8_t)kit;
        kit_c_set = 1;
        KC = kit_ptr((unsigned)kit);
    } else if (lane == L_OHAT) {
        kit_o = (uint8_t)kit;
        kit_o_set = 1;
        KO = kit_ptr((unsigned)kit);
    }
}

static int32_t drums_wait(void)                 /* samples to the next hit that waits; 0: none */
{
    int32_t m = 0;
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        if (drum[d].wait > 0 && (!m || drum[d].wait < m))
            m = drum[d].wait;
    for (d = 0; d < EN_XDRUM; d++)
        if (xdrum[d].wait > 0 && (!m || xdrum[d].wait < m))
            m = xdrum[d].wait;
    return m;
}

static void drums_wait_step(int32_t k)          /* k samples went by: hits whose wait is over */
{
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        if (drum[d].wait > 0 && (drum[d].wait -= k) <= 0) {
            drum[d].wait = 0;
            drum_fire(d, drum[d].pend, mel_hold[d]);
        }
    for (d = 0; d < EN_XDRUM; d++)
        if (xdrum[d].wait > 0 && (xdrum[d].wait -= k) <= 0) {
            xdrum[d].wait = 0;
            xdrum_fire(d, xdrum[d].pend, xmel_hold[d]);
        }
}

/* the grid: one call a sixteenth (MIDI clock: 6 ticks) */
static int32_t step_t;                          /* samples into this sixteenth (the gate's clock) */
static uint8_t rpt_arm;                         /* a beat repeat waiting for the next sixteenth: its rate + 1 */
static void rpt_engage(int r);
static void sleep_chirp(float hz0, float hz1, float secs)
{
    float n = secs * EN_SR;
    if (n < 1.0f)
        n = 1.0f;
    sl_bird_f = hz0 / EN_SR;
    sl_bird_df = (hz1 - hz0) / EN_SR / n;
    sl_bird_left = (int)n;
    sl_bird_n = sl_bird_left;
    sl_bird_ph = 0.0f;
}
static void tick(void)
{
    static const uint8_t TRIG_BARS[6] = {0, 1, 2, 4, 8, 0};
    int l, st = (int)(sixteenth & 15u);
    const float pa = fm_clampf(1.0f + 0.4f * arcm, 0.6f, 1.1f);   /* the arc: the hats a little softer */
    const int32_t sw = (st & 1) ? (int32_t)(fm_minf(swing, 0.5f) * step_len) : 0;
    const float chop = KK->chop * (0.4f + 0.6f * evolve);   /* breakcore: how much the drums are chopped */
    int perc_hit = 0;
    step_t = 0;
    if (rpt_arm) {                              /* the beat repeat starts on the sixteenth */
        rpt_engage(rpt_arm - 1);
        rpt_arm = 0;
    }
    if (st == 0) {                              /* a bar */
        int tb = TRIG_BARS[(unsigned)par[P_DE_TRIG] < 6u ? par[P_DE_TRIG] : 0];
        if (sec == SEC_BUILD && --sec_bars <= 0)
            section_cmd(EN_CMD_DROP);           /* the build's end: the drop */
        if (sec_req) {
            section_cmd(sec_req - 1);
            sec_req = 0;
        }
        dd_bar();                               /* drum drift: a fill coming, or a change now */
        pat_now = pat_next;                     /* a new pattern: on the bar */
        if (tb && bars % (uint32_t)tb == 0u)
            bass.env_up = 1;                    /* the drone's envelope: every 1 .. 8 bars */
        bars++;
        if (sleep_on && sl_bird_left <= 0 && sl_bird_wait <= 0 && (bars & 3u) == 2u) {
            sleep_chirp(1900.0f, 3200.0f, 0.085f); /* one call, sometimes answered */
            if ((bars & 7u) == 2u)
                sl_bird_wait = (int)(0.48f * EN_SR);
        }
    }
    drum_publish();
    lk_hat = 0;
    for (l = 0; l < EN_NDRUM; l++) {
        int hit = (int)(en_pats[pat_now][l] >> st & 1u);
        float vel = 0.9f;
        if (sec == SEC_BREAK) {
            hit = 0;
        } else if (sec == SEC_BUILD) {
            if (l == L_KICK || l == L_CLAP || l == L_PERC)
                hit = 0;
            else if (l == L_SNARE) {            /* the roll: quarters, eighths, sixteenths */
                hit = sec_bars > 2 ? (st & 3) == 0 : sec_bars == 2 ? (st & 1) == 0 : 1;
                vel = 0.3f + 0.7f * ((float)(4 - sec_bars) * 16.0f + (float)st) / 64.0f;
            }
        }
        if ((fill_hold || (dd_fill && st >= 8 && sec == SEC_GROOVE)) && (l == L_SNARE || l == L_HAT)) {   /* FILL; drift's */
            hit = 1;
            vel = l == L_SNARE ? 0.35f + 0.04f * (float)st : 0.5f;
        }
        if (chop > 0.0f && sec == SEC_GROOVE && !fill_hold) {   /* breakcore: ghosts, and some hits rolled */
            if (!hit && l == L_SNARE && frand() < 0.16f * chop) {
                hit = 1;
                vel = 0.25f + 0.25f * frand();
            } else if (!hit && l == L_KICK && (st & 1) && frand() < 0.1f * chop) {
                hit = 1;
                vel = 0.7f;
            } else if (l == L_HAT && frand() < 0.25f * chop) {
                hit = !hit;
                vel = 0.5f;
            }
        }
        if (sleep_on && (l == L_HAT || l == L_OHAT || l == L_PERC)) {
            /* The pattern only arms the downbeat. The bar decides whether it speaks.
             * A dark lane does not flash: the cue waits until that part is in the song. */
            int down = st == 0;
            int cue = l == L_PERC ? sleep_bell : sleep_chime;
            int rare = l == L_PERC ? (bars % 16u) == 4u : (bars % 8u) == 0u;
            int allow = down && (cue || rare);
            if (!allow || lane_g[l] <= 0.0f)
                hit = 0;
            else
                hit = 1;
            if (hit && l == L_PERC)
                sleep_bell = 0;
            if (hit && l == L_OHAT)
                sleep_chime = 0;
            if (hit)
                vel = 0.85f;
        }
        if (hit && (l == L_HAT || l == L_OHAT))
            lk_hat = 1;
        if (hit && l == L_PERC)
            perc_hit = 1;
        if (hit) {
            vel *= l == L_HAT || l == L_OHAT ? pa : 1.0f;
            if (chop > 0.0f && !sw && (l == L_SNARE || l == L_KICK || l == L_HAT) && frand() < 0.2f * chop) {
                drum_fire(l, vel, st);          /* a 32nd roll: the hit, and again half a step on */
                drum[l].wait = (int32_t)(0.5f * step_len);
                drum[l].pend = vel * 0.7f;
            } else {
                drum_hit(l, vel, sw);
            }
        }
    }
    if (sec != SEC_BREAK) {                     /* the five drums above the perc. No frand: the rolls stay put */
        for (l = 0; l < EN_XDRUM; l++) {
            int hit = xon[l] && xlvl[l] > 0 && (int)(xpat[l] >> st & 1u);
            if (hit)
                xdrum_hit(l, 0.9f, sw);
        }
    }
    if (pad_stabs() && !par[P_PAD_MAN] && en_playing && sec != SEC_BREAK &&   /* the pad's stabs: the off-beats, more with Evolve */
        ((st & 3) == 2 || ((st == 7 || st == 15) && frand() < 0.35f * evolve))) {
        int i;
        stab_env = st & 1 ? 0.7f : 1.0f;
        stab_k = fm_expf(-6.9f * 32.0f / (0.08f * fm_exp2f((float)par[P_PE_REL] * 0.04f) * EN_SR));
        for (i = 0; i < EN_NPAD; i++) {
            pad_t *v = &pad[i];
            if (v->play)                        /* played from the keys: not stabbed */
                continue;
            if (v->midi_note)
                midi(0x80, v->midi_note, 0);
            v->note = v->target;
            v->hand = 0;
            v->hf = 1.0f;
            v->voices = (uint8_t)pad_osc();
            v->midi_note = (uint8_t)v->target;
            midi(0x90, v->midi_note, (uint32_t)(60.0f + 40.0f * stab_env));
        }
    }
    en_drum_pos = (uint8_t)st;
    if (links_on() && par[P_LK_PERC] && sec == SEC_GROOVE) {
        if (perc_hit)
            acid_step();                        /* the links: the perc lane is the acid's clock */
    } else {
        acid_step();
    }
    if (st == 0 && change_pending && !en_frozen) {   /* chords move on the bar */
        change_pending = 0;
        chord_change();
        if (!par[P_DD_EVERY] && (par[P_DD_CHANGE] || par[P_DD_MUTATE]) && dd_in < 0)
            dd_in = 2;                          /* drum drift with the chord: a fill next bar, the change after */
    }
    sixteenth++;
    en_bars = sixteenth;
}

/* ------------------------------------------------- reverb: Dattorro's plate --- */
/* J. Dattorro, "Effect Design, Part 1", JAES 1997 (as in FoMni): a predelay, four input diffusers,
 * then a figure-eight tank of two halves, each a modulated allpass, a delay, a damping lowpass, an
 * allpass and a delay, feeding the other half. Stereo out from seven taps a side. The paper's lengths
 * are for 29761 Hz; DS() scales them to 44.1 kHz. */
#define DS(n) (((n) * 14818 + 5000) / 10000)          /* 44100 / 29761 */
#define PRE_N 1323                                   /* 30 ms */
#define EXC DS(16)
enum { L_PRE, L_IN1, L_IN2, L_IN3, L_IN4, L_APL, L_D1L, L_AP2L, L_D2L, L_APR, L_D1R, L_AP2R, L_D2R, L_N };
static const int DL_LEN[L_N] = {PRE_N, DS(142), DS(107), DS(379), DS(277), DS(672) + EXC + 2, DS(4453), DS(1800),
                                DS(3720), DS(908) + EXC + 2, DS(4217), DS(2656), DS(3163)};
#define DL_TOTAL (PRE_N + DS(142) + DS(107) + DS(379) + DS(277) + DS(672) + DS(4453) + DS(1800) + DS(3720) + DS(908) + \
                  DS(4217) + DS(2656) + DS(3163) + 2 * (EXC + 2))
static float pl_buf[DL_TOTAL] EN_POOL;
typedef struct {
    float *b;
    int n, i;
} dl_t;
static dl_t dl[L_N];
static float pl_bw, pl_damp_l, pl_damp_r, pl_decay = 0.5f, pl_decay_t = 0.5f, pl_dd2 = 0.5f, lfo_s, lfo_c = 1.0f;

static inline float tap(const dl_t *d, int k)
{
    int j = d->i - k;
    return d->b[j < 0 ? j + d->n : j];
}
static inline void push(dl_t *d, float x)
{
    d->b[d->i] = x;
    if (++d->i >= d->n)
        d->i = 0;
}
static inline float allpass(dl_t *d, int len, float x, float g)
{
    float z = tap(d, len), v = x - g * z;
    push(d, fm_flush(v));
    return z + g * v;
}
static inline float allpass_mod(dl_t *d, float len, float x, float g)
{
    int k = (int)len;
    float f = len - (float)k, z = tap(d, k) + (tap(d, k + 1) - tap(d, k)) * f, v = x - g * z;
    push(d, fm_flush(v));
    return z + g * v;
}

static inline void reverb(float in, float *ol, float *or_)
{
    float x, a, b, fb_l = tap(&dl[L_D2R], DS(3163)), fb_r = tap(&dl[L_D2L], DS(3720));
    push(&dl[L_PRE], in);
    x = tap(&dl[L_PRE], PRE_N);
    pl_bw += 0.6f * (x - pl_bw);
    x = allpass(&dl[L_IN1], DS(142), pl_bw, 0.75f);
    x = allpass(&dl[L_IN2], DS(107), x, 0.75f);
    x = allpass(&dl[L_IN3], DS(379), x, 0.625f);
    x = allpass(&dl[L_IN4], DS(277), x, 0.625f);
    {
        float s = lfo_s + 0.0001f * lfo_c, c = lfo_c - 0.0001f * s;
        lfo_s = s;
        lfo_c = c;
    }
    a = allpass_mod(&dl[L_APL], (float)DS(672) + (float)EXC * lfo_s, x + pl_decay * fb_l, -0.7f);
    push(&dl[L_D1L], a);
    b = tap(&dl[L_D1L], DS(4453));
    pl_damp_l += 0.45f * (b - pl_damp_l);
    b = allpass(&dl[L_AP2L], DS(1800), pl_damp_l * pl_decay, pl_dd2);
    push(&dl[L_D2L], b);
    a = allpass_mod(&dl[L_APR], (float)DS(908) + (float)EXC * lfo_c, x + pl_decay * fb_r, -0.7f);
    push(&dl[L_D1R], a);
    b = tap(&dl[L_D1R], DS(4217));
    pl_damp_r += 0.45f * (b - pl_damp_r);
    b = allpass(&dl[L_AP2R], DS(2656), pl_damp_r * pl_decay, pl_dd2);
    push(&dl[L_D2R], b);
    *ol = 0.6f * (tap(&dl[L_D1R], DS(266)) + tap(&dl[L_D1R], DS(2974)) - tap(&dl[L_AP2R], DS(1913)) +
                  tap(&dl[L_D2R], DS(1996)) - tap(&dl[L_D1L], DS(1990)) - tap(&dl[L_AP2L], DS(187)) -
                  tap(&dl[L_D2L], DS(1066)));
    *or_ = 0.6f * (tap(&dl[L_D1L], DS(353)) + tap(&dl[L_D1L], DS(3627)) - tap(&dl[L_AP2L], DS(1228)) +
                   tap(&dl[L_D2L], DS(2673)) - tap(&dl[L_D1R], DS(2111)) - tap(&dl[L_AP2R], DS(335)) -
                   tap(&dl[L_D2R], DS(121)));
}

static void plate_init(void)
{
    float *p = pl_buf;
    int k, i;
    for (k = 0; k < L_N; k++) {
        dl[k].b = p;
        dl[k].n = DL_LEN[k];
        dl[k].i = 0;
        p += DL_LEN[k];
    }
    for (i = 0; i < DL_TOTAL; i++)
        pl_buf[i] = 0.0f;
    pl_bw = pl_damp_l = pl_damp_r = 0.0f;
    lfo_s = 0.0f;
    lfo_c = 1.0f;
}

/* ------------------------------------------------- echo: tape and Echoplex --- */
/* After Discreet Music's chain: an Echoplex (a short, smearing echo, ~160 ms, fed back) and a tape loop
 * (up to a bar, synced to the tempo), each repeat a little darker and thinner, with a little
 * wow. The tape runs at a quarter rate (11 kHz, 16-bit, mono): it loses its highs anyway. Two heads:
 * the first plays the loop's length, the second a part of it (3/4 at first). With Motion the echo
 * evolves: each head wanders across the field, the feedback leans now on one head, now on the other
 * (the repeats fall in changing patterns, left and right), and now and then the second head slides to
 * another part of the loop, as a head moved by hand (the repeats bend a little in pitch on the way).
 * The Echoplex has a second tap, for the right. */
#define TAPE_N 21800                             /* 2.0 s at 11.025 kHz: a bar at 122 BPM, the longest echo */
#define TAPE_SR 11025.0f
static int16_t tape[TAPE_N] EN_POOL;
static int tape_w, tape_ph;
static float tape_acc, tape_lp, tape_hp, tape_p1, tape_p2, tape_c1, tape_c2;
static const float HEAD2[5] = {0.75f, 0.6667f, 0.625f, 0.5f, 0.375f};   /* the second head's parts */
static float head2 = 0.75f, head2_t = 0.75f, head2_v;   /* its place (of the loop), target, speed */
static float fb_lean;                            /* 0..1: how much of the feedback is the second head's */
static float hg[4] = {1.0f, 0.0f, 0.0f, 1.0f};   /* the heads' pan gains: 1 left, 1 right, 2 left, 2 right */
static uint32_t head_left;                       /* samples until the second head may move */
#define PLEX_N 7100                              /* 160 ms at 44.1 kHz */
#define PLEX_TAP 2700                            /* the right's tap: ~100 ms */
static int16_t plex[PLEX_N];
static int plex_w;
static float plex_lp, plex_lp2;

static inline float tape_read(float d)           /* d quarter-rate samples ago */
{
    int k = (int)d, i0, i1;
    float f = d - (float)k, a, c;
    i0 = tape_w - k;
    i1 = i0 - 1;
    if (i0 < 0)
        i0 += TAPE_N;
    if (i1 < 0)
        i1 += TAPE_N;
    a = (float)tape[i0];
    c = (float)tape[i1];
    return (a + (c - a) * f) * (1.0f / 12000.0f);
}
static inline int16_t s16(float x)
{
    x *= 12000.0f;
    return (int16_t)(x > 32767.0f ? 32767.0f : x < -32767.0f ? -32767.0f : x);
}

/* Shimmer: two grains of the song. The age falls by one sample while the write
 * advances by one, so each grain is read twice as fast as it was written, an octave
 * up. The windows sit half a grain apart and add to one. The lows are taken out
 * before this, so a kick does not come back as a thud. The buffer is the recent
 * mix only. Nothing here touches the plate's own memory. */
#define SH_N 2048
static int16_t sh_buf[SH_N];
static int sh_w;
static float sh_ph, sh_lp, sh_tail;

static void shimmer_clear(void)
{
    int i;
    for (i = 0; i < SH_N; i++)
        sh_buf[i] = 0;
    sh_w = 0;
    sh_ph = 0.0f;
    sh_lp = 0.0f;
    sh_tail = 0.0f;
}

static float shimmer_read(float age)
{
    int k = (int)age;
    float f = age - (float)k, a, c;
    int i0 = sh_w - k, i1 = i0 - 1;
    if (i0 < 0)
        i0 += SH_N;
    if (i1 < 0)
        i1 += SH_N;
    a = (float)sh_buf[i0];
    c = (float)sh_buf[i1];
    return (a + (c - a) * f) * (1.0f / 12000.0f);
}

static float shimmer_up(float x)
{
    float p, p2, a1, a2;
    sh_buf[sh_w] = s16(x);
    if (++sh_w >= SH_N)
        sh_w = 0;
    sh_ph += 1.0f / 1024.0f;
    if (sh_ph >= 1.0f)
        sh_ph -= 1.0f;
    p = sh_ph;
    p2 = p + 0.5f;
    if (p2 >= 1.0f)
        p2 -= 1.0f;
    a1 = (1.0f - p) * 1024.0f;
    a2 = (1.0f - p2) * 1024.0f;
    if (a1 < 1.0f)
        a1 = 1.0f;
    else if (a1 > 1023.0f)
        a1 = 1023.0f;
    if (a2 < 1.0f)
        a2 = 1.0f;
    else if (a2 > 1023.0f)
        a2 = 1023.0f;
    return shimmer_read(a1) * (0.5f - 0.5f * fm_cosf(p * FM_TWO_PI))
         + shimmer_read(a2) * (0.5f - 0.5f * fm_cosf(p2 * FM_TWO_PI));
}

static void echo_time(void)
{
    int i = par[P_ETIME];
    float beat, t;
    if (par[P_TEMPO] <= 0)
        return;                                  /* not yet (en_init applies in order) */
    beat = 60.0f / (float)par[P_TEMPO];
    t = ETIME_BEATS[i] * beat;
    while (t * TAPE_SR > (float)(TAPE_N - 8) && i > 0)
        t = ETIME_BEATS[--i] * beat;             /* too long for the tape: the next shorter */
    if (t * TAPE_SR > (float)(TAPE_N - 8))
        t = (float)(TAPE_N - 8) / TAPE_SR;
    echo_len_t = t * TAPE_SR;
    if (echo_len <= 0.0f)
        echo_len = echo_len_t;
}

/* once a chunk: where the heads sit in the field, how the feedback leans, the second head's slide */
static void heads_step(uint32_t n)
{
    float p, want, vmax;
    int i;
    for (i = 0; i < 2; i++) {
        p = fm_clampf((i ? 0.7f : -0.7f) + 0.9f * motion * mod[6 + i].v, -1.0f, 1.0f);
        p = (p + 1.0f) * (FM_PI / 4.0f);
        hg[2 * i] = fm_cosf(p);
        hg[2 * i + 1] = fm_sinf(p);
    }
    fb_lean = 0.25f * motion * (1.0f + mod[8].v);
    if (head_left <= n) {
        head_left = (uint32_t)((20.0f + 40.0f * frand()) * EN_SR);
        if (en_playing && !en_frozen && frand() < motion)
            head2_t = HEAD2[(int)(frand() * 4.999f)];
    } else {
        head_left -= n;
    }
    /* its speed (a part of the loop per tape sample): at most a bend of ~14 cents, easing in and out */
    vmax = 0.008f / fm_maxf(echo_len, 100.0f);
    want = fm_clampf((head2_t - head2) * (1.0f / (2.0f * TAPE_SR)), -vmax, vmax);
    head2_v += (want - head2_v) * fm_minf((float)n * (1.0f / EN_SR), 1.0f);
}

/* one sample of both: the input (a mono send) to the tape's heads (el, er) and the Echoplex (xl, xr) */
static inline void echoes(float in, float *el, float *er, float *xl, float *xr)
{
    float y = (float)plex[plex_w] * (1.0f / 12000.0f);         /* the Echoplex: 160 ms, fed back, darker */
    int t = plex_w - PLEX_TAP;
    plex_lp += 0.35f * (y - plex_lp);
    plex_lp2 += 0.35f * ((float)plex[t < 0 ? t + PLEX_N : t] * (1.0f / 12000.0f) - plex_lp2);
    /* Shut: the same write as before. Open: the short slap feeds back further, so it sings. */
    if (echo_sm > 0.0f)
        plex[plex_w] = s16(in + (0.55f + 0.28f * echo_sm) * plex_lp);
    else
        plex[plex_w] = s16(in + 0.55f * plex_lp);
    if (++plex_w >= PLEX_N)
        plex_w = 0;
    *xl = plex_lp;
    *xr = plex_lp2;
    tape_acc += in + 0.3f * plex_lp;
    if (++tape_ph >= 4) {                        /* the tape, a quarter of the time */
        float d, h1, h2, fb;
        tape_ph = 0;
        echo_len += (echo_len_t - echo_len) * 0.001f;
        head2 += head2_v;
        d = echo_len * (1.0f + 0.003f * motion * mod[5].v);   /* wow */
        if (d > (float)(TAPE_N - 4))
            d = (float)(TAPE_N - 4);
        h1 = tape_read(d);
        h2 = tape_read(d * head2);
        tape_lp += 0.55f * (h1 + fb_lean * (h2 - h1) - tape_lp);   /* each time round: darker, thinner */
        tape_hp += 0.03f * (tape_lp - tape_hp);
        fb = tape_lp - tape_hp;
        if (++tape_w >= TAPE_N)
            tape_w = 0;
        tape[tape_w] = s16(tape_acc * 0.25f + echo_fbx * fb);
        tape_acc = 0.0f;
        tape_p1 = tape_c1;
        tape_p2 = tape_c2;
        tape_c1 = h1;
        tape_c2 = h2;
    }
    {   /* between the tape's samples: a line from the last to the next; then to the field */
        float f = (float)tape_ph * 0.25f;
        float h1 = tape_p1 + (tape_c1 - tape_p1) * f, h2 = tape_p2 + (tape_c2 - tape_p2) * f;
        *el = hg[0] * h1 + hg[2] * h2;
        *er = hg[1] * h1 + hg[3] * h2;
    }
}

/* ------------------------------------------------------------ parameters --- */
static int pulse;
static uint32_t ext_age;

static void tempo_set(void)
{
    step_len = EN_SR * 60.0f / ((float)par[P_TEMPO] * 4.0f);
    duck_dec = fm_expf(-4.0f / (step_len * 3.0f));   /* the duck: back up in about three sixteenths */
}

static void apply(int p, int v)
{
    const en_param_t *pi = &PARAMS[p];
    if (v < pi->lo)
        v = pi->lo;
    if (v > pi->hi)
        v = pi->hi;
    par[p] = (int16_t)v;
    if (p >= P_LANE && p < P_ECHO) {             /* a drum lane's level, tune or decay */
        int l = (p - P_LANE) / 3, k = (p - P_LANE) % 3;
        if (k == 0)
            lane_g[l] = (float)(v * v) * 0.0001f;
        else if (k == 1)
            lane_tune[l] = (float)v * 0.24f;     /* semitones: an octave either way */
        else
            lane_dec[l] = (float)v * 0.01f;
        return;
    }
    switch (p) {
    case P_PAD: pad_vol = (float)(v * v) * 0.0001f; break;
    case P_ACID: acid_vol = (float)(v * v) * 0.0001f; break;
    case P_BASS: bass_vol = (float)(v * v) * 0.0001f; break;
    case P_PULSE: pulse_vol = (float)(v * v) * 0.0001f; break;
    case P_NOISE: noise_vol = (float)(v * v) * 0.0001f; break;
    case P_DETUNE: detune = 0.00144f * fm_exp2f((float)v * 0.0332f); break;   /* 2.5 .. 25 cents */
    case P_NZ_COLOR: nz_color = v; break;
    case P_NZ_TONE: nz_lp_k = 1.0f - fm_expf(-FM_TWO_PI * 300.0f * fm_exp2f((float)v * 0.06f) * (1.0f / EN_SR)); break;
    case P_NZ_PUMP: nz_pump = (float)v * 0.01f; break;
    case P_BASSOCT:
        if (nsteps)
            bass_to(bass.midi_note % 12);
        break;
    case P_OCT: case P_PAD_OCT:                  /* the pad's voices cross over as for a new key */
        if (nsteps) {
            int i;
            for (i = 0; i < EN_NPAD; i++)       /* a voice's next note: from its register's middle */
                pad[i].target = (float)(55 + 12 * (par[P_PAD_OCT] + par[P_OCT]));
            pad_retarget();
            pad_handoff();
            if (p == P_OCT)
                bass_to(bass.midi_note % 12);
        }
        break;
    case P_DR_RES: reso_k = 1.5f - 0.0138f * (float)v; break;   /* none .. near singing (k 1.5 .. 0.12) */
    case P_AC_LEN:
        if (ac_pos >= v)
            ac_pos = 0;
        en_acid_len = (uint8_t)v;
        break;
    case P_AC_DENS: case P_AC_RANGE:             /* heard at once: the line made again */
        if (nsteps)
            ac_regrow();
        break;
    case P_DRIVE: drive_k = 1.0f + 0.04f * (float)v; break;
    case P_SWING: swing = (float)v * 0.005f; break;   /* the off sixteenths up to half a step late */
    case P_PATTERN:                              /* waits for the bar (now, while stopped) */
        pat_next = v;
        if (!en_playing)
            pat_now = v;
        drum_publish();
        break;
    case P_DRIFT:
        if (drift_total > 0.0f) {                /* keep the progress, at the new pace */
            float frac = drift_left / drift_total;
            drift_total = drift_secs(v) * EN_SR;
            drift_left = frac * drift_total;
        }
        break;
    case P_ECHO: echo_send = (float)(v * v) * 0.0001f; break;
    case P_ETIME: echo_time(); break;
    case P_REVERB: rev_send = (float)(v * v) * 0.0001f; break;
    case P_SIZE:
        pl_decay_t = 0.6f + 0.375f * (float)v * 0.01f;     /* to 0.975: a long hall */
        echo_fb = 0.5f + 0.32f * (float)v * 0.01f;
        break;
    case P_BRIGHT: bright = (float)v; break;
    case P_EVOLVE: evolve = (float)v * 0.01f; break;
    case P_PADTONE:                              /* the voices cross over as they did for a new key */
        if (stabs_were && !pad_stabs()) {        /* out of the stabs: the breaths start over, overlapping */
            int k;
            for (k = 0; k < EN_NPAD; k++) {
                pad[k].t = START_T[k];
                pad_breathe(&pad[k], k);
            }
        }
        stabs_were = (uint8_t)pad_stabs();
        pad_handoff();
        break;
    case P_MOTION: motion = (float)v * 0.01f; break;
    case P_AC_MAN:                               /* the acid off the algorithm: the line's note let go */
        if (v && !acid.play) {
            acid.amp_t = 0.0f;
            acid.gate_left = 0;
        }
        break;
    case P_DR_MAN:                               /* the drone back: to the chord's root */
        if (!v && !bass.play)
            bass_to(chord_pcs[0]);
        break;
    case P_KIT_KICK: kit_i[0] = (uint8_t)((unsigned)v < EN_NKIT ? v : 0); KK = kit_ptr(kit_i[0]); break;
    case P_KIT_SNARE: kit_i[1] = (uint8_t)((unsigned)v < EN_NKIT ? v : 0); KS = kit_ptr(kit_i[1]); break;
    case P_KIT_PERC: kit_i[3] = (uint8_t)((unsigned)v < EN_NKIT ? v : 0); KP = kit_ptr(kit_i[3]); break;
    case P_KIT_HAT: {
        int k;
        kit_i[2] = (uint8_t)((unsigned)v < EN_NKIT ? v : 0);
        KH = kit_ptr(kit_i[2]);
        for (k = 0; k < 4; k++)
            hat_inc[k] = HAT_INC[k] * KH->h_metal;
        break;
    }    case P_TUNE: tunefac = fm_exp2f((float)v * (1.0f / 1200.0f)); break;
    case P_TEMPO:
        tempo_set();
        echo_time();
        break;
    case P_ROOT: case P_CHORD: case P_VOICING:   /* the chord again: each voice moves at its next breath */
        if (nsteps) {
            pad_retarget();
            pad_handoff();
            bass_to(chord_pcs[0]);
        }
        break;
    case P_MIDI:
        if (!v)
            en_midi_out(0xB0, 123, 0);
        break;
    default: break;
    }
}

static void pool_set(uint32_t p)
{
    pool = (p | 1u) & 0xFFFu;
    scale_set();
    if (deg >= nsteps)
        deg = 0;
    pad_retarget();
    pad_handoff();
    bass_to(chord_pcs[0]);
}

static void regrow(void)                        /* a new piece: a fresh acid line, the home chord */
{
    deg = 0;
    pad_retarget();
    pad_handoff();
    bass_to(chord_pcs[0]);
    bass.rest = 0;
    ac_regrow();
    ac_pos = 0;
    sec = SEC_GROOVE;
    sec_req = 0;
    drum_publish();
    drift_reset();
    change_pending = 0;
}

static void notes_off(void)
{
    int i;
    for (i = 0; i < 4; i++)
        en_midi_out(0xB0 | (uint32_t)i, 123, 0);
    en_midi_out(0xB9, 123, 0);
}

/* KEYS PLAYED ON A VOICE'S PAGE (en_note, from the UI): the pad takes up to four at once, each on one of its
 * voices, which breathes again when let go; the drone and the acid take the last key held (the drone
 * two octaves down, the acid one; a second key while one is held slides to it), the drone's root and the
 * acid's line waiting meanwhile. The voice's octaves and the song's move them, as the generative notes. */
static uint8_t held[3][8], nheld[3];
static uint32_t play_seq;
static int fold_note(int n, int lo, int hi)
{
    while (n < lo)
        n += 12;
    while (n > hi)
        n -= 12;
    return n;
}
static void pad_play(int key, int on)
{
    int i, best = -1;
    float bv = 1e9f;
    if (!on) {
        for (i = 0; i < EN_NPAD; i++)
            if (pad[i].play == 1 && pad[i].pnote == key) {
                pad[i].play = 2;
                midi(0x80, pad[i].midi_note, 0);
            }
        return;
    }
    for (i = 0; i < EN_NPAD; i++)               /* a free voice, the quietest; else one ringing out; else the oldest */
        if (pad[i].play == 0 && pad[i].amp < bv)
            bv = pad[i].amp, best = i;
    if (best < 0)
        for (i = 0, bv = 1e9f; i < EN_NPAD; i++)
            if (pad[i].play == 2 && pad[i].pamp < bv)
                bv = pad[i].pamp, best = i;
    if (best < 0)
        for (i = 0, bv = 4e9f; i < EN_NPAD; i++)
            if ((float)pad[i].pord < bv)
                bv = (float)pad[i].pord, best = i;
    {
        pad_t *v = &pad[best];
        if (v->midi_note)
            midi(0x80, v->midi_note, 0);
        v->play = 1;
        v->pnote = (uint8_t)key;
        v->pord = ++play_seq;
        v->note = (float)fold_note(key + 12 * (par[P_PAD_OCT] + par[P_OCT]), 24, 108);
        en_hand_note = (uint8_t)v->note;
        if (pad_lock) {                         /* the phrase is what was heard. The private breath may be */
            v->amp = v->out;                    /* peaking, and switching to it is a spike */
            v->pamp = fm_clampf(v->out / 0.8f, 0.0f, 1.0f);
        } else {
            v->pamp = fm_minf(v->amp / 0.8f, 1.0f);
        }
        v->hand = 0;
        v->hf = 1.0f;
        v->voices = (uint8_t)pad_osc();
        v->midi_note = (uint8_t)v->note;
        pad_tag[best]++;
        midi(0x90, v->midi_note, 100);
    }
}
static void note_cmd(int voice, int key, int on)
{
    int k, t, legato;
    if (voice < 0 || voice > 2)
        return;
    for (k = 0; k < nheld[voice]; k++)          /* the held keys: this one out of the list, and in at the top */
        if (held[voice][k] == key) {
            for (; k + 1 < nheld[voice]; k++)
                held[voice][k] = held[voice][k + 1];
            nheld[voice]--;
            break;
        }
    legato = nheld[voice] > 0;
    if (on) {
        if (nheld[voice] == 8) {
            for (k = 0; k < 7; k++)
                held[voice][k] = held[voice][k + 1];
            nheld[voice] = 7;
        }
        held[voice][nheld[voice]++] = (uint8_t)key;
    }
    t = nheld[voice] ? held[voice][nheld[voice] - 1] : -1;
    en_held = (uint8_t)(nheld[0] + nheld[1] + nheld[2]);
    if (voice == EN_V_PAD) {
        pad_play(key, on);
    } else if (voice == EN_V_DRONE) {
        if (t >= 0) {
            int m = fold_note(t - 24 + 12 * (par[P_BASSOCT] + par[P_OCT]), 24, 83);
            en_hand_note = (uint8_t)m;
            if (on && !legato)
                bass.env_up = 1;                /* a new press: the envelope again */
            if (bass.amp < 0.02f)
                bass.note = (float)m;           /* from silence: straight there, no glide */
            if (bass.midi_note != m) {
                midi(0x82, bass.midi_note, 0);
                midi(0x92, (uint32_t)m, 100);
            }
            bass.play = 1;
            bass.target = (float)m;
            bass.midi_note = (uint8_t)m;
        } else if (bass.play) {                 /* let go: back to the chord's root */
            bass.play = 0;
            bass_to((int)bass.gtarget % 12);
        }
    } else if (t >= 0) {                        /* the acid */
        int m = fold_note(t - 12 + 12 * (par[P_AC_OCT] + par[P_OCT]), 28, 96);
        en_hand_note = (uint8_t)m;
        acid.inc_t = mtof((float)m) * (1.0f / EN_SR);
        if (legato && acid.play && acid.amp_t > 0.0f) {
            acid.sliding = 1;                   /* legato: a slide, no new attack */
        } else {
            float dec = 0.06f * fm_exp2f((float)par[P_AC_DEC] * 0.05f);
            acid.inc = acid.inc_t;
            acid.env = 1.0f;
            acid.acc = 0.6f;
            acid.edec = fm_expf(-6.9f * 32.0f / (dec * EN_SR));
            acid.sliding = 0;
            acid_tag++;
        }
        if (acid.note)
            midi(0x83, acid.note, 0);
        midi(0x93, (uint32_t)m, 110);
        acid.note = (uint8_t)m;
        acid.amp_t = 1.0f;
        acid.gate_left = -1;                    /* held while the key is */
        acid.play = 1;
        en_acid_glow = 1.0f;
    } else if (acid.play) {
        acid.play = 0;
        acid.amp_t = 0.0f;
        acid.gate_left = 0;
        acid.sliding = 0;
        if (acid.note)
            midi(0x83, acid.note, 0);
    }
}
/* A finger that is up does not keep the line. The pad finishes its release and
 * the phrase comes back. The drone and the acid go back to the song. */
static void hands_off(void)
{
    int i;
    nheld[0] = nheld[1] = nheld[2] = 0;
    en_held = 0;
    for (i = 0; i < EN_NPAD; i++)
        if (pad[i].play == 1) {
            pad[i].play = 2;
            midi(0x80, pad[i].midi_note, 0);
        }
    if (bass.play) {
        bass.play = 0;
        bass_to((int)bass.gtarget % 12);
    }
    if (acid.play) {
        acid.play = 0;
        acid.amp_t = 0.0f;
        acid.gate_left = 0;
        acid.sliding = 0;
        if (acid.note)
            midi(0x83, acid.note, 0);
    }
}

static void lay_quiet(void);
static void quiet(void)
{
    int i;
    lay_quiet();
    nheld[0] = nheld[1] = nheld[2] = 0;
    en_held = 0;
    for (i = 0; i < EN_NPAD; i++)
        pad[i].play = 0, pad[i].pamp = 0.0f;
    bass.play = 0;
    acid.play = 0;
    for (i = 0; i < EN_NDRUM; i++)
        drum[i].on = 0, drum[i].wait = 0;
    for (i = 0; i < EN_XDRUM; i++)
        xdrum[i].on = 0, xdrum[i].wait = 0;
    for (i = 0; i < EN_NPAD; i++)
        pad[i].amp = 0.0f;
    bass.amp = 0.0f;
    acid.amp = acid.amp_t = 0.0f;
    nz_env = 0.0f;
    notes_off();
}

static void cloud_reset(void);
static void hop_reset(void);

void en_init(void)
{
    int i, l;
    KC = KO = 0;
    for (i = 0; i < EN_XDRUM; i++) {
        xon[i] = 0;
        xpat[i] = 0;
        xlvl[i] = 0;
        xg[i] = 0.0f;
        xdrum[i].on = 0;
        xdrum[i].wait = 0;
        en_xdrum_glow[i] = 0.0f;
    }
    for (i = 0; i <= SN; i++)
        sint[i] = fm_sinf((float)i * (FM_TWO_PI / (float)SN));
    plate_init();
    for (i = 0; i < TAPE_N; i++)
        tape[i] = 0;
    for (i = 0; i < PLEX_N; i++)
        plex[i] = 0;
    tape_w = plex_w = tape_ph = 0;
    head2 = head2_t = 0.75f;
    head2_v = 0.0f;
    echo_len = 0.0f;
    q_r = q_w = 0;
    for (i = 0; i < NMOD; i++) {
        mod[i].v = mod[i].tgt = 0.0f;
        mod[i].left = 0;
    }
    for (i = 0; i < EN_NPAD; i++) {
        pad[i].note = pad[i].target = (float)(50 + 7 * i);
        pad[i].pan = PAD_PAN[i];
        pad[i].amp = 0.0f;
        pad[i].out = 0.0f;
        pad[i].ph[0] = 0.0f;
        pad[i].ph[1] = 0.37f;
        pad[i].vib = 0.3f * (float)i;
        pad[i].midi_note = 0;
        pad[i].hf = 1.0f;
        pad[i].hand = 0;
        pad[i].fa = 0.4f;
        pad[i].fh = 0.15f;
        pad[i].fr = 0.35f;
    }
    for (i = 0; i < EN_NPAT; i++)               /* the factory patterns (the app loads the player's after) */
        for (l = 0; l < EN_NLANE; l++)
            en_pats[i][l] = EN_PIECE_PATS[0][i][l];
    arcm = 0.0f;
    pad_gain = 0.0f;
    for (i = 0; i < P_NPARAMS; i++)
        apply(i, PARAMS[i].def);
    pool_set(EN_MODE_POOL[par[P_MODE]]);
    for (i = 0; i < EN_NPAD; i++)
        pad[i].note = pad[i].target;
    bass.note = bass.target;
    ac_regrow();
    ac_pos = 0;
    sec = SEC_GROOVE;
    sec_req = 0;
    pat_now = pat_next = 0;
    drum_publish();
    drift_reset();
    pl_decay = pl_decay_t;
    nz_shape = 0;                               /* HYP's bed, until IAM asks for a part */
    nz_oct = 0;
    vgate[0] = vgate[1] = vgate[2] = vgate[3] = 0xFFFFu;
    mel_d[0] = mel_d[1] = mel_d[2] = mel_d[3] = 0;
    for (i = 0; i < 11; i++)
        dmel_d[i] = 0;
    vgate_sm[0] = vgate_sm[1] = vgate_sm[2] = 1.0f;
    vgate_nz = 1.0f;
    vgate_on3 = 0;
    pad_lock = 0;                               /* HYP's four breaths, until IAM asks for one phrase */
    sleep_on = 0;
    sleep_bell = sleep_chime = 0;
    sl_l = sl_r = sl_wh = sl_wb = 0.0f;
    sl_ph = sl_gust = sl_hum = sl_form = sl_vib = sl_fl = 0.0f;
    sl_bird_ph = sl_bird_f = sl_bird_df = 0.0f;
    sl_bird_left = sl_bird_wait = sl_bird_n = 0;
    nz_env = 0.0f;
    nz_ph = 0.0f;
    nz_hp[0] = nz_hp[1] = 0.0f;
    nz_hum_ph[0] = nz_hum_ph[1] = nz_hum_ph[2] = nz_hum_ph[3] = 0.0f;
    nz_rev = 0.15f;
    nz_echo = 0.0f;
    cloud_reset();                              /* a reboot must not keep a cloud that was on */
    echo_amt = 0;
    echo_sm = 0.0f;                             /* nor an echo the previous song had opened */
    verb_amt = 0;
    verb_sm = 0.0f;
    shim_amt = 0;
    shim_sm = 0.0f;
    phase_amt = 0;
    phase_sm = 0.0f;
    kn1_amt = 0;
    kn1_sm = 0.0f;
    kn1_ph = 0.0f;
    kn2_amt = 0;
    kn2_sm = 0.0f;
    kn2_ph = 0.0f;
    kn2_n = 0;
    kn2_c[0] = kn2_c[1] = kn2_c[2] = kn2_c[3] = 0.0f;
    kn2_z[0][0] = kn2_z[0][1] = kn2_z[1][0] = kn2_z[1][1] = 0.0f;
    keys_amt = 1.0f;                            /* a reboot is the voice, until IAM sets the tenth */
    lay_quiet();
    phase_ph = 0.0f;
    phase_yl = phase_yr = 0.0f;
    phase_z[0][0] = phase_z[0][1] = phase_z[0][2] = phase_z[0][3] = 0.0f;
    phase_z[1][0] = phase_z[1][1] = phase_z[1][2] = phase_z[1][3] = 0.0f;
    shimmer_clear();                            /* nor a shimmer tail */
    hop_reset();                                /* nor a crossing from the song before */
}

/* ------------------------------------------------------------ transport --- */
static void clock_pulse(void)
{
    if (pulse == 0)
        tick();
    if (!en_ext)
        midi(0xF8, 0, 0);
    if (++pulse >= 6)
        pulse = 0;
}

static void play_start(int from_top)
{
    int i;
    if (from_top) {
        pulse = 0;
        sixteenth = 0;
        bars = 0;
        ac_pos = 0;
        pat_now = pat_next;
        pad_lfo.ph = drone_lfo.ph = 0.0f;
    }
    pulse_left = 0.0f;
    en_playing = 1;
    pad_gate = 1.0f;
    for (i = 0; i < EN_NPAD; i++)               /* from silence: the breaths start over, overlapping */
        if (pad[i].amp < 1e-4f) {
            pad[i].t = START_T[i];
            pad[i].midi_note = 0;
            pad_breathe(&pad[i], i);
        }
    if (bass.amp < 1e-4f)
        bass.note = bass.target;
    bass.env_up = 1;
    midi(0x92, bass.midi_note, 80);
    if (!en_ext)
        midi(from_top ? 0xFA : 0xFB, 0, 0);
}

static void play_stop(void)
{
    int d;
    for (d = 0; d < EN_NDRUM; d++)
        drum[d].wait = 0;                        /* a swung hit still waiting stays unplayed */
    en_playing = 0;
    pad_gate = 0.0f;
    acid.amp_t = 0.0f;
    acid.gate_left = 0;
    if (!en_ext)
        midi(0xFC, 0, 0);
    notes_off();
}

/* --------------------------------------------------------- performance FX --- */
/* Held effects (en_fx). The master is recorded, always, into a ring of 0.95 s (22.05 kHz, stereo:
 * a half note at 126 BPM; the pool holds no more beside the reverb, the tape and the screen's canvas), which three of them play back instead of the live mix while the engine
 * runs on underneath, in time:
 *   beat repeat   from the next sixteenth, the last 1/2 .. 1/32 note looped (the shortest held wins;
 *                 the ring stops recording while it loops);
 *   reverse       the ring played backwards from now (stopped recording);
 *   tape stop     the live mix slowing to a stop over three beats (still recording, read behind).
 *                 Letting go speeds that same tape back up. The live mix returns only once the
 *                 read has caught the write, so the playhead does not jump.
 * The rest shape the live mix: mute and solo for six tracks (5 ms ramps), a low-pass and a high-pass
 * closing in over half a second, a trance gate on the synths (1/16, 1/8), an echo throw (everything
 * into the tape echo, its feedback up), a crusher (5.5 kHz, 5 bits), a drum fill (the pulse's),
 * a flange (a few milliseconds of the master, sweeping, one side against the other),
 * and a flanger (a slow sweep out to about ten milliseconds, the two sides against each other).
 * GLO's crowd keys are a second mask (en_dj): dj_mix, after the master, and only while one is up. */
#define RB_N 21000
static int16_t rb[RB_N * 2] EN_POOL;
static uint32_t rb_w;                            /* frames written (they wrap: an index is taken mod RB_N) */
static uint8_t rb_odd;
static float rb_hl, rb_hr;                       /* the first sample of each pair, waiting for the second */
static uint32_t fx_req, fx_on;
enum { BM_LIVE, BM_RPT, BM_REV, BM_STOP };
static int bmode, brate = -1;                    /* the ring's mode; the repeat's rate (FX_RPT2 ..) */
static uint32_t bm_start;                        /* the frame the reading counts from */
static float bm_off, bm_len, bm_speed, bm_mix, bm_tgt;   /* samples from bm_start; the slice; 0 live .. 1 ring */
static uint8_t bm_hold;                          /* tape stop is down: the speed falls. Up: it climbs back */
static float trk_g[EN_NTRACK] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, trk_g1[EN_NTRACK] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static float tg_inv = 1.0f / 256.0f;             /* 1 / the chunk: the tracks' gains ramp across it */
#define TG(t, j) (trk_g[t] + (trk_g1[t] - trk_g[t]) * (float)(j) * tg_inv)
enum { TR_KICK, TR_HATS, TR_SNARE, TR_DRONE, TR_ACID, TR_PAD };   /* TR_SNARE: snare, clap, perc; TR_PAD: pad, noise */
static float lp_amt, hp_amt, lp_s[4], hp_s[4], lp_c[4], hp_c[4], throw_amt, crush_amt, crush_l, crush_r, gate_g = 1.0f;
static int crush_n;
#define FL_N 512                                 /* a few milliseconds, sweeping. Not the held-FX ring */
static int16_t fl_dl[FL_N], fl_dr[FL_N];
static uint32_t fl_w;
static float fl_amt, fl_tgt, fl_ph;
static uint8_t fl_live;
static float fg_amt, fg_tgt, fg_ph;              /* the seventh key's flanger. The short jet above stays the eighth */
static uint8_t fg_live;
static const float RPT_BEATS[6] = {2.0f, 1.0f, 2.0f / 3.0f, 0.5f, 0.25f, 0.125f};
const char *const EN_FX_NAME[EN_NFX] = {
    "Rpt 1/2", "Rpt 1/4", "Rpt 1/6", "Rpt 1/8", "Rpt 1/16", "Rpt 1/32", "Reverse", "Tape stop",
    "Mute kick", "Mute hats", "Mute snare", "Mute drone", "Mute acid", "Mute pad", "Low-pass", "High-pass",
    "Gate 1/16", "Gate 1/8", "Echo throw", "Crush", "Fill",
    "Solo kick", "Solo hats", "Solo snare", "Solo drone", "Solo acid", "Solo pad", "Flange", "Flanger"};

static void rpt_engage(int r)                   /* the slice that just ended, looped from its start */
{
    float len = RPT_BEATS[r] * 4.0f * step_len;
    while (len > (float)(RB_N - 8) * 2.0f && r < 5)   /* too long for the ring: the next shorter, in time */
        len = RPT_BEATS[++r] * 4.0f * step_len;
    if (len > (float)(RB_N - 8) * 2.0f)
        len = (float)(RB_N - 8) * 2.0f;
    bmode = BM_RPT;
    brate = r;
    bm_len = len;
    bm_start = rb_w - (uint32_t)(len * 0.5f + 0.5f);
    bm_off = 0.0f;
    bm_tgt = 1.0f;
}

static inline void rb_read(float off, float *l, float *r)   /* off samples (44.1 kHz) after bm_start */
{
    float f = off * 0.5f, t;
    uint32_t k = (uint32_t)f, i0, i1;
    t = f - (float)k;
    i0 = (bm_start + k) % RB_N;
    i1 = (i0 + 1u) % RB_N;
    *l = ((float)rb[2 * i0] + ((float)rb[2 * i1] - (float)rb[2 * i0]) * t) * (1.0f / 12000.0f);
    *r = ((float)rb[2 * i0 + 1] + ((float)rb[2 * i1 + 1] - (float)rb[2 * i0 + 1]) * t) * (1.0f / 12000.0f);
}

static inline void rb_write(float l, float r)
{
    if (!rb_odd) {
        rb_hl = l;
        rb_hr = r;
        rb_odd = 1;
        return;
    }
    rb_odd = 0;
    {
        uint32_t i = rb_w % RB_N;
        rb[2 * i] = s16(0.5f * (rb_hl + l));
        rb[2 * i + 1] = s16(0.5f * (rb_hr + r));
        rb_w++;
    }
}

static void fx_control(uint32_t n)              /* once a chunk: what the held effects ask for */
{
    uint32_t m = fx_req, solo = m >> FX_SOLO0 & 63u;
    float dt = (float)n * (1.0f / EN_SR);
    int t, want = -1;
    fx_on = m;
    for (t = 0; t < EN_NTRACK; t++) {           /* mute and solo: 5 ms ramps */
        float tgt = (!(m >> (FX_MUTE0 + t) & 1u) && (!solo || (solo >> t & 1u))) ? 1.0f : 0.0f, d = (float)n * (1.0f / 220.0f);
        trk_g[t] = trk_g1[t];
        trk_g1[t] = trk_g[t] + fm_clampf(tgt - trk_g[t], -d, d);
    }
    tg_inv = 1.0f / (float)n;
    lp_amt = fm_clampf(lp_amt + (m >> FX_LP & 1u ? dt * 2.0f : -dt * 6.0f), 0.0f, 1.0f);   /* in 0.5 s, out in 0.17 */
    hp_amt = fm_clampf(hp_amt + (m >> FX_HP & 1u ? dt * 2.0f : -dt * 6.0f), 0.0f, 1.0f);
    throw_amt = fm_clampf(throw_amt + (m >> FX_THROW & 1u ? dt * 60.0f : -dt * 60.0f), 0.0f, 1.0f);
    crush_amt = fm_clampf(crush_amt + (m >> FX_CRUSH & 1u ? dt * 100.0f : -dt * 100.0f), 0.0f, 1.0f);
    fl_tgt = (m >> FX_FLANGE & 1u) ? 1.0f : 0.0f;
    if ((m >> FX_FLANGER & 1u) && fg_tgt == 0.0f)
        fg_ph = 0.20f;                           /* mid sweep, so the comb is already there */
    fg_tgt = (m >> FX_FLANGER & 1u) ? 1.0f : 0.0f;
    if (fl_tgt == 0.0f && fl_amt == 0.0f && fg_tgt == 0.0f && fg_amt == 0.0f && fl_live) {
        uint32_t i;
        for (i = 0; i < FL_N; i++)
            fl_dl[i] = fl_dr[i] = 0;
        fl_w = 0;
        fl_ph = 0.0f;
        fg_ph = 0.0f;
        fl_live = 0;
    }
    if (fg_tgt == 0.0f && fg_amt == 0.0f && fg_live) {
        fg_ph = 0.0f;
        fg_live = 0;
    }
    echo_fbx = echo_fb + (0.86f - echo_fb) * throw_amt;
    if (fill_hold != (uint8_t)(m >> FX_FILL & 1u)) {
        fill_hold = (uint8_t)(m >> FX_FILL & 1u);
        drum_publish();
    }
    if (lp_amt > 0.0f)                          /* 16 kHz down to ~250 Hz */
        svf_coefs(lp_c, 16000.0f * fm_exp2f(-6.0f * lp_amt), 0.8f);
    if (hp_amt > 0.0f)                          /* 20 Hz up to ~1.6 kHz */
        svf_coefs(hp_c, 20.0f * fm_exp2f(6.3f * hp_amt), 0.8f);
    for (t = 0; t < 4; t++) {
        lp_s[t] = fm_flush(lp_s[t]);
        hp_s[t] = fm_flush(hp_s[t]);
    }
    for (t = FX_RPT32; t >= FX_RPT2; t--)       /* the ring: the shortest repeat held */
        if (m >> t & 1u) {
            want = t;
            break;
        }
    if (m >> FX_STOP & 1u) {
        rpt_arm = 0;
        if (bmode != BM_STOP || bm_tgt < 1.0f) {
            bmode = BM_STOP;
            bm_start = rb_w - 2u;
            bm_off = 0.0f;
            bm_speed = 1.0f;
            bm_tgt = 1.0f;
        }
        bm_hold = 1;
    } else if (m >> FX_REV & 1u) {
        bm_hold = 0;
        rpt_arm = 0;
        if (bmode != BM_REV || bm_tgt < 1.0f) {
            bmode = BM_REV;
            bm_len = (float)(RB_N - 8) * 2.0f;
            bm_start = rb_w - (RB_N - 8);
            bm_off = bm_len - 1.0f;
            bm_tgt = 1.0f;
        }
    } else if (want >= 0) {
        bm_hold = 0;
        if (bmode != BM_RPT || brate != want || bm_tgt < 1.0f) {
            if (!en_playing || en_ext)
                rpt_engage(want);               /* no grid to wait for */
            else
                rpt_arm = (uint8_t)(want + 1);
        }
    } else if (bmode == BM_STOP) {
        rpt_arm = 0;
        bm_hold = 0;
        bm_tgt = 1.0f;                          /* same tape: the speed comes back, the playhead stays */
    } else {
        rpt_arm = 0;
        bm_hold = 0;
        bm_tgt = 0.0f;                          /* back to the live mix (the ring fades out) */
    }
}

/* Free knobs on the song, after the mix. The held-FX ring above is the buffer: it is
 * already the master, half rate, and this only reads it. Off is the dry path unchanged.
 * Ping-pong: taps on the beat, an eighth, a dotted eighth, then a quarter. Each
 * repeat answers on the other side, and they darken. Helix: one slice of the song, repeated, fading unless the
 * knob is at the top. The song screen leaves the ping-pong and the helix at zero.
 * Loops: halfway is the old full loop, a few slices, then an octave and a fifth,
 * and the slices jump. Past halfway the slices shorten into grains, then little beeps. */
static volatile uint8_t cloud_dly, cloud_hel, cloud_mos, cloud_live;
static float dly_sm, hel_sm, mos_sm;
static float dly_lp_l, dly_lp_r, dly_xf;
static uint32_t dly_len, dly_from, mos_seq;
static uint8_t dly_pole;
typedef struct {
    uint32_t start, len;
    float cursor, gain;
    uint8_t div, live;
} hel_g;
typedef struct {
    uint32_t start, len, snd; /* snd sounds. The rest of len is the gap */
    uint32_t scrap;           /* 0: play straight through. Else the beep loops this */
    float cursor, rate, pan;
    uint16_t lap;
    uint8_t live;
} mos_g;
static hel_g helg;
static mos_g mosg[3];
static const float MOS_DIV[3] = {2.0f, 4.0f, 1.0f};
static const float MOS_RATE[3] = {0.50f, 1.00f, 0.75f}; /* unison, octave, fifth. The ring is half rate */

static void cloud_reset(void)
{
    int i;
    cloud_dly = cloud_hel = cloud_mos = cloud_live = 0;
    dly_sm = hel_sm = mos_sm = 0.0f;
    dly_lp_l = dly_lp_r = dly_xf = 0.0f;
    dly_len = dly_from = mos_seq = 0;
    dly_pole = 0;
    for (i = 0; i < 3; i++)
        mosg[i].live = 0;
    helg.live = 0;
}

void en_cloud(int delay, int helix, int loops)
{
    if (delay < 0)
        delay = 0;
    if (delay > 100)
        delay = 100;
    if (helix < 0)
        helix = 0;
    if (helix > 100)
        helix = 100;
    if (loops < 0)
        loops = 0;
    if (loops > 100)
        loops = 100;
    cloud_dly = (uint8_t)delay;
    cloud_hel = (uint8_t)helix;
    cloud_mos = (uint8_t)loops;
    if (delay || helix || loops)
        cloud_live = 1;
}

static void rb_at(uint32_t frame, float frac, float *l, float *r)
{
    uint32_t i0 = frame % RB_N, i1 = i0 + 1u;
    float a, b, c, d;
    if (i1 >= RB_N)
        i1 = 0;
    if (frac < 0.0f)
        frac = 0.0f;
    else if (frac > 1.0f)
        frac = 1.0f;
    a = (float)rb[2 * i0];
    b = (float)rb[2 * i1];
    c = (float)rb[2 * i0 + 1];
    d = (float)rb[2 * i1 + 1];
    *l = (a + (b - a) * frac) * (1.0f / 12000.0f);
    *r = (c + (d - c) * frac) * (1.0f / 12000.0f);
}

/* Eighth, dotted eighth, then a quarter. Fewer bounces once the ring cannot hold them. */
static uint32_t dly_plan(int amt, int *taps, float *fb)
{
    float six = step_len * 0.5f;
    float d;
    int sixteenths, n;
    if (six < 16.0f)
        six = 16.0f;
    if (amt < 34) {
        sixteenths = 2;
        n = 4;
    } else if (amt < 67) {
        sixteenths = 3;
        n = 3;
    } else {
        sixteenths = 4;
        n = 2;
    }
    *fb = 0.62f + 0.30f * ((float)amt * 0.01f); /* the far side stays in the conversation */
    d = six * (float)sixteenths;
    while (n > 1 && d * (float)n > (float)(RB_N - 64))
        n--;
    if (d > (float)(RB_N - 64))
        d = (float)(RB_N - 64);
    if (d < 32.0f)
        d = 32.0f;
    *taps = n;
    return (uint32_t)d;
}

static void dly_taps(uint32_t len, int taps, float fb, float *sl, float *sr)
{
    float g, acc = 1.0f, p = 1.0f, norm;
    int tap, to_right = 1;
    *sl = 0.0f;
    *sr = 0.0f;
    for (tap = 1; tap < taps; tap++) {
        p *= fb;
        acc += p;
    }
    norm = 0.70f / acc;
    g = norm;
    for (tap = 0; tap < taps; tap++) {
        uint32_t back = len * (uint32_t)(tap + 1);
        float al, ar, m;
        if (back < 1u)
            back = 1u;
        if (back > (uint32_t)(RB_N - 8))
            break;
        rb_at(rb_w - back, 0.0f, &al, &ar);
        m = (al + ar) * 0.5f;                   /* the whole hit, then one side only */
        if (tap == 0) {
            float bal = al - ar;
            int k;
            /* One sample of a centered hit jitters across the middle, and that
             * would spray the bounce onto both sides. Look across the hit. */
            for (k = 1; k < 8; k++) {
                uint32_t at = back + (uint32_t)(k * 48);
                float bl, br;
                if (at >= rb_w)
                    break;
                rb_at(rb_w - at, 0.0f, &bl, &br);
                bal += bl - br;
            }
            to_right = bal > -0.04f;            /* centered, or left, answers on the right */
        }
        if (to_right)
            *sr += m * g;
        else
            *sl += m * g;
        to_right = !to_right;
        g *= fb;
    }
}

static int hel_div(void)
{
    if (cloud_hel >= 80)
        return 16; /* one beat */
    if (cloud_hel >= 55)
        return 8;
    if (cloud_hel >= 30)
        return 4;
    return 2;
}

static void hel_capture(int div)
{
    float six = step_len * 0.5f;
    float lenf;
    uint32_t len, back;
    if (six < 16.0f)
        six = 16.0f;
    lenf = six * (float)div;
    if (lenf > (float)(RB_N - 64))
        lenf = (float)(RB_N - 64);
    len = (uint32_t)lenf;
    if (len < 32u)
        len = 32u;
    back = len + 8u;
    if (back > (uint32_t)(RB_N - 8))
        back = (uint32_t)(RB_N - 8);
    helg.start = rb_w - back;
    helg.len = len;
    helg.cursor = 0.0f;
    helg.gain = 1.0f;
    helg.div = (uint8_t)div;
    helg.live = 1;
}

/* Knob 0..50 is the old 0..100. Past 50, t climbs 0..1 and the slices shrink. */
static int mos_classic(float *t_out)
{
    int mos = (int)cloud_mos;
    int drive = mos <= 50 ? mos * 2 : 100 + (mos - 50) * 2;
    if (t_out)
        *t_out = drive > 100 ? (float)(drive - 100) * 0.01f : 0.0f;
    return drive > 100 ? 100 : drive;
}

static void mos_capture(mos_g *g, int gi, int classic, float t)
{
    float six = step_len * 0.5f;
    float lenf, shrink = 1.0f;
    uint32_t len, span, slide, back;
    if (six < 16.0f)
        six = 16.0f;
    if (t > 0.0f)
        shrink = fm_expf(-t * 3.9f);           /* 51 barely shorter, the top about a fiftieth */
    lenf = six * MOS_DIV[gi] * shrink;
    if (lenf > (float)(RB_N - 64))
        lenf = (float)(RB_N - 64);
    len = (uint32_t)lenf;
    if (t > 0.0f) {
        if (len < 12u)
            len = 12u;                          /* a fast tempo still has a body, not one sample */
    } else if (len < 32u)
        len = 32u;
    span = len;
    if (shrink < 0.5f) {
        float wide = six * 4.0f;                /* grains pick anywhere in the last quarter note */
        if (wide > (float)(RB_N - 64))
            wide = (float)(RB_N - 64);
        if ((float)span < wide)
            span = (uint32_t)wide;
    }
    slide = (mos_seq * 173u + (uint32_t)gi * 91u) % span;
    back = len + slide + 8u;
    if (back > (uint32_t)(RB_N - 8))
        back = (uint32_t)(RB_N - 8);
    g->start = rb_w - back;
    g->len = len;
    g->snd = len;
    g->scrap = 0;
    g->pan = 0.0f;
    g->rate = MOS_RATE[gi];
    if (t <= 0.55f) {
        if (gi == 2 && classic >= 78)
            g->rate = -0.50f;                   /* the fifth stays reversed until the beeps climb */
    } else {
        float u = (t - 0.55f) * (1.0f / 0.45f);
        float lift, wobble, target, reps_f, air;
        uint32_t wob, reps, snd, gap, period, j;
        if (u > 1.0f)
            u = 1.0f;
        lift = u * u * 0.80f;                   /* a little higher. The pitch is the scrap itself */
        wob = (mos_seq * 17u + (uint32_t)gi * 5u) & 7u;
        wobble = 1.0f + ((float)wob - 3.5f) * (0.035f * u);
        g->rate = MOS_RATE[gi] * (1.0f + lift) * wobble;
        if (g->rate < 0.15f)
            g->rate = 0.15f;
        target = 180.0f + u * 420.0f;           /* about 4 ms, growing to about 14 ms */
        reps_f = target * g->rate / (float)len;
        if (reps_f < 1.0f)
            reps_f = 1.0f;
        if (reps_f > 14.0f)
            reps_f = 14.0f;
        reps = (uint32_t)(reps_f + 0.5f);
        if (reps < 1u)
            reps = 1u;
        snd = len * reps;
        air = u * u * 6.0f;                     /* the beep, then a gap, so it does not buzz */
        j = (mos_seq * 11u + (uint32_t)gi * 7u) & 3u;
        air *= 0.70f + 0.20f * (float)j;
        gap = (uint32_t)((float)snd * air);
        period = snd + gap;
        if (period < snd)
            period = snd;
        if (period > 12000u)
            period = 12000u;
        if (snd > period)
            snd = period;
        g->scrap = len;
        g->snd = snd;
        g->len = period;
    }
    if (t > 0.45f) {
        float u = (t - 0.45f) * (1.0f / 0.55f);
        uint32_t p = (mos_seq * 29u + (uint32_t)gi * 13u) & 7u;
        if (u > 1.0f)
            u = 1.0f;
        g->pan = ((float)p - 3.5f) * (0.16f * u);
    }
    if (t > 0.40f && t <= 0.55f) {
        float u = (t - 0.40f) * (1.0f / 0.60f);
        float air;
        uint32_t j, period;
        if (u > 1.0f)
            u = 1.0f;
        /* Grains: the slice stays, and a gap opens after it. */
        air = u * u * 12.0f;
        j = (mos_seq * 11u + (uint32_t)gi * 7u) & 3u;
        air *= 0.70f + 0.20f * (float)j;
        period = len + (uint32_t)((float)len * air);
        if (period < len)
            period = len;
        if (period > (uint32_t)(RB_N - 64))
            period = (uint32_t)(RB_N - 64);
        g->snd = len;
        g->len = period;
    }
    g->cursor = g->rate < 0.0f ? (float)g->len - 1.0f : 0.0f;
    g->lap = 0;
    g->live = 1;
    mos_seq++;
}

static void cloud_mix(float *l, float *r)
{
    float dt = cloud_dly ? 0.28f + 0.72f * ((float)cloud_dly * 0.01f) : 0.0f;
    float ht = cloud_hel ? 0.28f + 0.72f * ((float)cloud_hel * 0.01f) : 0.0f;
    float loop_t = 0.0f;
    int classic = mos_classic(&loop_t);
    float mt = classic ? 0.28f + 0.72f * ((float)classic * 0.01f) : 0.0f;
    float dl = 0.0f, dr = 0.0f, hl = 0.0f, hr = 0.0f, ml = 0.0f, mr = 0.0f, keep, dip;
    int gi, nl;
    dly_sm += (dt - dly_sm) * 0.0015f;
    hel_sm += (ht - hel_sm) * 0.0015f;
    mos_sm += (mt - mos_sm) * 0.0015f;
    if (dly_sm < 0.0005f && dt == 0.0f) {
        dly_sm = 0.0f;
        dly_len = 0;
        dly_pole = 0;
    }
    if (hel_sm < 0.0005f && ht == 0.0f)
        hel_sm = 0.0f;
    if (mos_sm < 0.0005f && mt == 0.0f)
        mos_sm = 0.0f;
    if (dly_sm == 0.0f && hel_sm == 0.0f && mos_sm == 0.0f) {
        if (!cloud_dly && !cloud_hel && !cloud_mos)
            cloud_live = 0;
        return;                                 /* dry, including the sample the fade finishes */
    }
    if (dly_sm > 0.0f) {
        int taps;
        float fb, c, al, ar, bl, br;
        uint32_t tgt = dly_plan((int)cloud_dly, &taps, &fb);
        if (dly_len == 0u) {
            dly_len = tgt;
            dly_xf = 1.0f;
        } else if (tgt + 8u < dly_len || dly_len + 8u < tgt) {
            if (dly_xf >= 1.0f) {
                dly_from = dly_len;
                dly_xf = 0.0f;
            }
            dly_len = tgt;
        } else if (dly_xf >= 1.0f)
            dly_len = tgt;
        dly_taps(dly_len, taps, fb, &al, &ar);
        if (dly_xf < 1.0f) {
            dly_taps(dly_from, taps, fb, &bl, &br);
            al = bl * (1.0f - dly_xf) + al * dly_xf;
            ar = br * (1.0f - dly_xf) + ar * dly_xf;
            dly_xf += 1.0f / 512.0f;
            if (dly_xf > 1.0f)
                dly_xf = 1.0f;
        }
        c = 0.45f - 0.30f * ((float)cloud_dly * 0.01f); /* repeats sit back, and further back as it opens */
        if (!dly_pole) {
            dly_lp_l = al;
            dly_lp_r = ar;
            dly_pole = 1;
        }
        dly_lp_l += c * (al - dly_lp_l);
        dly_lp_r += c * (ar - dly_lp_r);
        dl = dly_lp_l;
        dr = dly_lp_r;
    }
    if (hel_sm > 0.0f) {
        int div = hel_div();
        float decay = 0.62f + 0.30f * ((float)cloud_hel * 0.01f);
        float rel, edge, tail, w, frac, al, ar;
        uint32_t off;
        if (!helg.live)
            hel_capture(div);
        helg.cursor += 0.50f;
        if (helg.cursor >= (float)helg.len) {
            helg.cursor -= (float)helg.len;
            if (cloud_hel < 92) {
                helg.gain *= decay;
                if (helg.gain < 0.12f || helg.div != (uint8_t)div)
                    hel_capture(div);           /* the slice lets go, then a new one */
            } else if (helg.div != (uint8_t)div)
                hel_capture(div);               /* the top holds this grab until the length changes */
        }
        rel = helg.cursor;
        if (rel < 0.0f)
            rel = 0.0f;
        if (helg.len > 0u && rel >= (float)helg.len)
            rel = (float)helg.len - 1.0f;
        off = (uint32_t)rel;
        frac = rel - (float)off;
        edge = rel;
        tail = (float)helg.len - rel;
        w = (edge < tail ? edge : tail) * (1.0f / 40.0f);
        if (w > 1.0f)
            w = 1.0f;
        if (w < 0.0f)
            w = 0.0f;
        rb_at(helg.start + off, frac, &al, &ar);
        hl = al * w * helg.gain;
        hr = ar * w * helg.gain;
    } else
        helg.live = 0;
    if (mos_sm > 0.0f) {
        float t = loop_t;
        int period = classic >= 70 ? 1 : classic >= 36 ? 2 : 4;
        nl = classic < 28 ? 1 : classic < 62 ? 2 : 3;
        for (gi = 0; gi < 3; gi++) {
            mos_g *g = &mosg[gi];
            float rel, body, edge, tail, knee, smear, w, frac, al, ar, bl, br, lg, rg;
            uint32_t off, frame;
            int every = gi == 2 ? 1 : period;
            if (!g->live) {
                if (gi < nl)
                    mos_capture(g, gi, classic, t);
                else
                    continue;
            }
            g->cursor += g->rate;
            if (g->cursor >= (float)g->len || g->cursor < 0.0f) {
                if (gi >= nl) {
                    g->live = 0;                /* this slice ends. A lower knob does not keep it */
                    continue;
                }
                if (++g->lap >= (uint16_t)every)
                    mos_capture(g, gi, classic, t);
                else if (g->cursor >= (float)g->len)
                    g->cursor -= (float)g->len;
                else
                    g->cursor += (float)g->len;
            }
            rel = g->cursor;
            if (rel < 0.0f)
                rel = 0.0f;
            if (rel >= (float)g->len)
                rel = (float)g->len - 1.0f;
            if (g->snd > 0u && g->snd < g->len && rel >= (float)g->snd)
                continue;                       /* the gap after a beep */
            body = (g->snd > 0u && g->snd < g->len) ? (float)g->snd : (float)g->len;
            off = (uint32_t)rel;
            frac = rel - (float)off;
            if (g->scrap > 1u) {
                float cycle = (float)g->scrap;
                float pos = rel;
                while (pos >= cycle)
                    pos -= cycle;               /* the scrap repeats, and that pitch is the beep */
                off = (uint32_t)pos;
                frac = pos - (float)off;
                edge = pos;
                tail = cycle - pos;
                body = cycle;
            } else {
                edge = rel;
                tail = body - rel;
            }
            frame = g->start + off;
            knee = body > 80.0f ? 40.0f : body * 0.5f;
            if (knee < 1.0f)
                knee = 1.0f;
            w = (edge < tail ? edge : tail) * (1.0f / knee);
            if (w > 1.0f)
                w = 1.0f;
            if (w < 0.0f)
                w = 0.0f;
            if (g->scrap > 1u && g->snd > g->scrap) {
                float env = 1.0f - 0.45f * (rel / (float)g->snd);
                if (env < 0.40f)
                    env = 0.40f;
                w *= env;                       /* the beep lets go, instead of holding a buzz */
            }
            smear = body > 600.0f ? 0.42f : 0.42f * (body * (1.0f / 600.0f));
            lg = 1.0f;
            rg = 1.0f;
            if (g->pan > 0.0f)
                lg = 1.0f - g->pan;
            else if (g->pan < 0.0f)
                rg = 1.0f + g->pan;
            rb_at(frame, frac, &al, &ar);
            rb_at(frame - 320u, frac, &bl, &br); /* a short smear, from the same ring */
            {
                float presence = 1.0f;
                if (body < 700.0f)
                    presence = 1.0f + 1.8f * (1.0f - body * (1.0f / 700.0f));
                ml += (al + bl * smear) * w * lg * presence;
                mr += (ar + br * smear) * w * rg * presence;
            }
        }
    }
    dip = dly_sm;
    if (hel_sm > dip)
        dip = hel_sm;
    if (loop_t > 0.40f) {
        float u = (loop_t - 0.40f) * (1.0f / 0.60f);
        float mos_dip = mos_sm;
        if (u > 1.0f)
            u = 1.0f;
        mos_dip *= 1.0f - 0.78f * u;       /* the song returns as the grains thin out */
        if (mos_dip > dip)
            dip = mos_dip;
    } else if (mos_sm > dip)
        dip = mos_sm;
    keep = 1.0f - 0.82f * dip;
    {
        int hot = (dly_sm > 0.2f ? 1 : 0) + (hel_sm > 0.2f ? 1 : 0) + (mos_sm > 0.2f ? 1 : 0);
        float ds = 0.85f, hs = 0.90f, ms = 0.78f;
        if (hot > 1) {
            ds = 0.55f;
            hs = 0.55f;
            ms = 0.55f;
        }
        *l = *l * keep + dl * ds * dly_sm + hl * hs * hel_sm + ml * ms * mos_sm;
        *r = *r * keep + dr * ds * dly_sm + hr * hs * hel_sm + mr * ms * mos_sm;
    }
}

/* A new song keeps the last moments of the one leaving, then lets them go.
 * The ring is already that song. This only reads it, and it does not record
 * while the crossing runs, so the slice stays put.
 *   cue      one beat, looped, and a filter closing as the new song rises
 *   throw    that beat again, darker and quieter each time round
 *   stutter  a sixteenth, then the new song
 *   reverse  the last half note, backwards, and the new song under it */
enum { HOP_OFF, HOP_CUE, HOP_THROW, HOP_STUTTER, HOP_REVERSE };
static uint8_t hop_kind, hop_rev;
static uint32_t hop_n, hop_len, hop_a, hop_span;
static float hop_pos, hop_gain, hop_dl, hop_dr;

static void hop_reset(void)
{
    hop_kind = hop_rev = 0;
    hop_n = hop_len = hop_span = 0;
    hop_pos = hop_gain = hop_dl = hop_dr = 0.0f;
}

static uint32_t hop_frames(float sixteenths)
{
    float six = step_len * 0.5f;
    float d;
    if (six < 16.0f)
        six = 16.0f;
    d = six * sixteenths;
    if (d > (float)(RB_N - 64))
        d = (float)(RB_N - 64);
    if (d < 32.0f)
        d = 32.0f;
    return (uint32_t)d;
}

static void hop_arm(int kind)
{
    float bar;
    if (kind < HOP_CUE || kind > HOP_REVERSE) {
        hop_reset();
        return;
    }
    bar = step_len * 16.0f;                     /* one bar of the song just chosen */
    if (bar < 22050.0f)
        bar = 22050.0f;
    if (bar > 88200.0f)
        bar = 88200.0f;
    hop_kind = (uint8_t)kind;
    hop_rev = 0;
    hop_n = 0;
    hop_len = (uint32_t)bar;
    hop_gain = 1.0f;
    hop_dl = hop_dr = 0.0f;
    if (kind == HOP_STUTTER)
        hop_span = hop_frames(1.0f);
    else if (kind == HOP_REVERSE)
        hop_span = hop_frames(8.0f);
    else
        hop_span = hop_frames(4.0f);
    hop_a = rb_w - hop_span;
    /* Reverse keeps going back from the sample that just played. The others
     * start at the top of the slice: the beat, or the sixteenth, just heard. */
    hop_pos = kind == HOP_REVERSE ? (float)hop_span - 1.0f : 0.0f;
}

static void hop_mix(float *l, float *r)
{
    float t, al, ar, bl, br, oldg, newg, fade, tail, u, frac;
    uint32_t off;
    if (!hop_kind || !hop_len || !hop_span)
        return;
    t = (float)hop_n / (float)hop_len;
    if (t > 1.0f)
        t = 1.0f;
    if (hop_kind == HOP_REVERSE) {
        if (!hop_rev) {
            hop_pos -= 0.50f;
            if (hop_pos < 0.0f)
                hop_rev = 1;
        }
    } else {
        hop_pos += 0.50f;
        if (hop_pos >= (float)hop_span) {
            hop_pos -= (float)hop_span;
            if (hop_kind == HOP_THROW && hop_n > 64u) { /* the opening wrap is the handoff, not a repeat yet */
                hop_gain *= 0.72f;
                if (hop_gain < 0.04f)
                    hop_gain = 0.04f;
            }
        }
    }
    if (hop_pos < 0.0f)
        hop_pos = 0.0f;
    if (hop_pos >= (float)hop_span)
        hop_pos = (float)hop_span - 1.0f;
    off = (uint32_t)hop_pos;
    frac = hop_pos - (float)off;
    rb_at(hop_a + off, frac, &al, &ar);
    fade = 48.0f;
    if (fade > (float)hop_span * 0.25f)
        fade = (float)hop_span * 0.25f;
    if (fade < 4.0f)
        fade = 4.0f;
    if (hop_kind == HOP_REVERSE) {
        if (hop_pos < fade) {                   /* out, rather than a click at the end of the buffer */
            u = hop_pos / fade;
            al *= u;
            ar *= u;
        }
    } else {
        tail = (float)hop_span - hop_pos;
        if (tail < fade) {                      /* into the top of the loop, level stays up */
            u = tail / fade;
            rb_at(hop_a, 0.0f, &bl, &br);
            al = al * u + bl * (1.0f - u);
            ar = ar * u + br * (1.0f - u);
        }
        if (hop_n < 192u) {                     /* from the sample just heard into the top of the loop */
            u = (float)hop_n * (1.0f / 192.0f);
            rb_at(hop_a + hop_span - 1u, 0.0f, &bl, &br);
            al = bl * (1.0f - u) + al * u;
            ar = br * (1.0f - u) + ar * u;
        }
    }
    if (hop_n == 0) {                           /* the filter opens on this sample, not from silence */
        hop_dl = al;
        hop_dr = ar;
    }
    if (hop_kind == HOP_CUE) {
        float c = 0.70f - 0.58f * t;
        if (c < 0.08f)
            c = 0.08f;
        hop_dl += c * (al - hop_dl);
        hop_dr += c * (ar - hop_dr);
        al = hop_dl;
        ar = hop_dr;
        oldg = 1.0f - t;
        newg = 0.22f + 0.78f * t;
    } else if (hop_kind == HOP_THROW) {
        float d = 1.0f - hop_gain;
        hop_dl += 0.18f * (al - hop_dl);
        hop_dr += 0.18f * (ar - hop_dr);
        if (d < 0.0f)
            d = 0.0f;
        if (d > 1.0f)
            d = 1.0f;
        al += (hop_dl - al) * d;
        ar += (hop_dr - ar) * d;
        oldg = hop_gain * (1.0f - t);
        newg = 0.40f + 0.60f * t;
    } else if (hop_kind == HOP_STUTTER) {
        if (t < 0.55f) {
            oldg = 1.0f;
            newg = 0.10f;
        } else {
            u = (t - 0.55f) * (1.0f / 0.45f);
            oldg = 1.0f - u;
            newg = 0.10f + 0.90f * u;
        }
    } else {
        oldg = hop_rev ? 0.0f : (t < 0.72f ? 1.0f : (1.0f - t) * (1.0f / 0.28f));
        newg = t < 0.40f ? 0.18f : 0.18f + 0.82f * ((t - 0.40f) * (1.0f / 0.60f));
    }
    if (oldg < 0.0f)
        oldg = 0.0f;
    if (newg > 1.0f)
        newg = 1.0f;
    *l = *l * newg + al * oldg;
    *r = *r * newg + ar * oldg;
    if (++hop_n >= hop_len)
        hop_kind = 0;
}

/* Lets Find Out keeps one moment of this song and throws it across the one
 * that is playing. The song stays at full level. It only reads the ring, and
 * while the beat is looping it does not record, so that slice stays put.
 * Sleep adds a blur and does not duck the room. A hop owns the master, and
 * this lets go. The cloud knobs are not this. */
enum { FIND_OFF, FIND_SEARCH, FIND_DISTANT };
static uint8_t find_mode, find_rel, find_lap;
static uint32_t find_span, find_a;
static float find_pos, find_w, find_pan, find_lp_l, find_lp_r;

static void find_reset(void)
{
    find_mode = find_rel = find_lap = 0;
    find_span = find_a = 0;
    find_pos = find_w = find_pan = find_lp_l = find_lp_r = 0.0f;
}

static void find_arm(int mode)
{
    if (mode <= 0) {
        if (find_mode)
            find_rel = 1;                       /* the loop keeps playing while it fades */
        else
            find_reset();
        return;
    }
    if (mode != FIND_DISTANT)
        mode = FIND_SEARCH;
    find_mode = (uint8_t)mode;
    find_rel = find_lap = 0;
    find_w = find_pan = find_pos = 0.0f;
    find_lp_l = find_lp_r = 0.0f;
    if (mode == FIND_SEARCH) {
        find_span = hop_frames(4.0f);          /* one beat, the same length a cue uses */
        find_a = rb_w - find_span;
    } else {
        find_span = 0;
    }
}

static void find_mix(float *l, float *r)
{
    float al, ar, bl, br, tgt, step, u, frac, tail, fade, pan_tgt, s;
    uint32_t off;
    if (hop_kind) {
        find_reset();                           /* the crossing is the whole master */
        return;
    }
    if (!find_mode)
        return;
    if (find_mode == FIND_SEARCH && find_span) {
        find_pos += 0.5f;                       /* the ring is half rate: this is realtime */
        if (find_pos >= (float)find_span) {
            find_pos -= (float)find_span;
            find_lap ^= 1u;
        }
        if (find_pos < 0.0f)
            find_pos = 0.0f;
        off = (uint32_t)find_pos;
        frac = find_pos - (float)off;
        rb_at(find_a + off, frac, &al, &ar);
        fade = 48.0f;
        if (fade > (float)find_span * 0.25f)
            fade = (float)find_span * 0.25f;
        tail = (float)find_span - find_pos;
        if (tail < fade) {                      /* into the top of the beat, rather than a click */
            u = tail / fade;
            rb_at(find_a, 0.0f, &bl, &br);
            al = al * u + bl * (1.0f - u);
            ar = ar * u + br * (1.0f - u);
        }
        pan_tgt = (find_lap & 1u) ? 1.0f : 0.0f; /* odd laps arrive on the other side */
        if (find_pan < pan_tgt) {
            find_pan += 0.002f;
            if (find_pan > pan_tgt)
                find_pan = pan_tgt;
        } else if (find_pan > pan_tgt) {
            find_pan -= 0.002f;
            if (find_pan < pan_tgt)
                find_pan = pan_tgt;
        }
        s = al;
        al = al * (1.0f - find_pan) + ar * find_pan;
        ar = ar * find_pan + s * (1.0f - find_pan);
        find_lp_l += 0.38f * (al - find_lp_l);  /* dark, still a beat, not a mute */
        find_lp_r += 0.38f * (ar - find_lp_r);
        al = find_lp_l;
        ar = find_lp_r;
    } else {
        find_lp_l += 0.06f * (*l - find_lp_l);  /* a blur beside the room */
        find_lp_r += 0.06f * (*r - find_lp_r);
        al = find_lp_l;
        ar = find_lp_r;
    }
    tgt = find_rel ? 0.0f : 1.0f;
    step = find_rel ? 0.00012f : 0.00080f;     /* about 180 ms out, about 30 ms in */
    if (find_w < tgt) {
        find_w += step;
        if (find_w > tgt)
            find_w = tgt;
    } else if (find_w > tgt) {
        find_w -= step;
        if (find_w < tgt)
            find_w = tgt;
    }
    if (find_rel && find_w < 0.001f) {
        find_reset();
        return;
    }
    if (find_mode == FIND_SEARCH) {
        float gate = find_pos < (float)find_span * 0.55f ? 1.0f : 0.40f;
        float wet = find_w * 0.62f * gate;     /* the beat on top. The song is not turned down */
        *l += al * (wet * (1.0f - find_pan));
        *r += ar * (wet * find_pan);
    } else {
        float wet = find_w * 0.28f;
        *l += al * wet;
        *r += ar * wet;
    }
}

static float flange_at(const int16_t *d, uint32_t w, float back)
{
    uint32_t k = (uint32_t)back;
    float t = back - (float)k;
    uint32_t i0 = (w - k) & (FL_N - 1u);
    uint32_t i1 = (w - k - 1u) & (FL_N - 1u);
    float a = (float)d[i0], b = (float)d[i1];
    return (a + (b - a) * t) * (1.0f / 12000.0f);
}

/* Seventh black key. A triangle walks the delay from about half a millisecond
 * out to about ten and back, and the right side walks the other way. The eighth
 * key's jet is shorter and faster, and this does not run its write while that
 * jet is the one filling the line. */
static void fg_read(float *dl, float *dr)
{
    float up = fg_ph < 0.5f ? fg_ph * 2.0f : 2.0f - fg_ph * 2.0f;
    *dl = flange_at(fl_dl, fl_w, 20.0f + 430.0f * up);
    *dr = flange_at(fl_dr, fl_w, 20.0f + 430.0f * (1.0f - up));
    fg_ph += 0.40f * (1.0f / EN_SR);
    if (fg_ph >= 1.0f)
        fg_ph -= 1.0f;
}

/* Knob 3. One slow sweep, four allpasses a side, mixed back with the dry song.
 * Off, this is not called, and the sample stays as it was. */
static inline float phase_ap(float x, float *z, float a)
{
    float y = -a * x + *z;
    *z = x + a * y;
    return y;
}
static void phase_mix(float *pl, float *pr)
{
    float tgt = phase_amt ? 0.35f + 0.65f * ((float)phase_amt * 0.01f) : 0.0f;
    float u, s, span, a, ar, fb, mix, x, y;
    int i;
    phase_sm += (tgt - phase_sm) * 0.0015f;
    if (phase_sm < 0.0005f && tgt == 0.0f) {
        if (phase_sm != 0.0f) {
            phase_sm = 0.0f;
            phase_ph = 0.0f;
            phase_yl = phase_yr = 0.0f;
            for (i = 0; i < 4; i++)
                phase_z[0][i] = phase_z[1][i] = 0.0f;
        }
        return;
    }
    u = phase_sm;
    s = sine(phase_ph);
    span = 0.18f + 0.28f * u;
    a = 0.50f + span * s;
    ar = 0.50f + span * sine(phase_ph + 0.13f);
    if (a < 0.05f)
        a = 0.05f;
    if (a > 0.93f)
        a = 0.93f;
    if (ar < 0.05f)
        ar = 0.05f;
    if (ar > 0.93f)
        ar = 0.93f;
    fb = 0.18f + 0.42f * u;
    mix = 0.50f * u;
    x = *pl + fb * phase_yl;
    y = x;
    for (i = 0; i < 4; i++)
        y = phase_ap(y, &phase_z[0][i], a);
    if (y > 2.0f)
        y = 2.0f;
    else if (y < -2.0f)
        y = -2.0f;
    phase_yl = y;
    *pl += (y - *pl) * mix;
    x = *pr + fb * phase_yr;
    y = x;
    for (i = 0; i < 4; i++)
        y = phase_ap(y, &phase_z[1][i], ar);
    if (y > 2.0f)
        y = 2.0f;
    else if (y < -2.0f)
        y = -2.0f;
    phase_yr = y;
    *pr += (y - *pr) * mix;
    phase_ph += (0.10f + 0.40f * u) * (1.0f / EN_SR);
    if (phase_ph >= 1.0f)
        phase_ph -= 1.0f;
}

/* GLO mixer keys (en_dj). Fourteen bits, beside the Felucca mask: the mutes, the solos, the
 * crusher and the sixteenth gate on that page. Zero leaves the sample alone. Each one warps
 * the music already in the master. A wow, a flange, a chorus and a bend read the ring this
 * file already writes and crossfade to it. The rest reshape the sample in place. No second
 * ring, and no pool buffer. Knob 3 keeps its own phaser. Knobs 1 and 2 keep
 * their own ring and wah, so a key and a knob do not share one sweep. */
enum {
    DJ_SWEEP, DJ_WOW, DJ_RING, DJ_TRANS, DJ_PHASER, DJ_WAH, DJ_RADIO, DJ_FLANGE,
    DJ_PAN, DJ_VOWEL, DJ_FILTER, DJ_CHORUS, DJ_BEND, DJ_CHOP, DJ_N
};
static uint16_t dj_req, dj_was;
static uint8_t dj_hot;
static float dj_a[DJ_N];
static float chop_hz, chop_ph, chop_g, pan_ph, vow_t;
static int vow_i;
static float vow_c[2][4];
static float vow_z[2][2][2];                 /* channel, formant, the filter */
static float sweep_u, sweep_c[4], sweep_z[2][2];
static uint8_t sweep_n;
static float wow_ph;
static float ring_ph, ring_f, ring_u;
static uint8_t ring_n;
static float tr_z[2];
static float pha_z[2][4], pha_y[2], pha_ph;  /* own allpasses. Knob 3 keeps phase_z */
static float rad_ph, rad_c[4], rad_z[2][2];
static uint8_t rad_n;
static float dfl_ph;                         /* not the jam flange: that one owns fl_dl */
static float cho_ph, bend_u;
static float flt_ph, flt_c[4], flt_z[2][2];
static uint8_t flt_n;
static float wah_ph, wah_c[4], wah_z[2][2];
static uint8_t wah_n;

static float dj_step(void)
{
    return step_len > 8.0f ? step_len : 8.0f;
}

/* behind: 44.1 kHz samples before the write. Still is the live mix, late. +1 a sample is frozen.
 * +2 a sample is reverse, at the speed it was written. Silence once it leaves the ring. */
static void rb_behind(float behind, float *l, float *r)
{
    float f, t;
    uint32_t k, i0, i1;
    *l = 0.0f;
    *r = 0.0f;
    if (behind < 0.0f)
        behind = 0.0f;
    f = behind * 0.5f;
    k = (uint32_t)f;
    if (k + 2u >= (uint32_t)RB_N || rb_w <= k + 2u)
        return;
    t = f - (float)k;
    i0 = (rb_w - 1u - k) % (uint32_t)RB_N;      /* the newer of the two */
    i1 = (rb_w - 2u - k) % (uint32_t)RB_N;
    *l = ((float)rb[2u * i0] + ((float)rb[2u * i1] - (float)rb[2u * i0]) * t) * (1.0f / 12000.0f);
    *r = ((float)rb[2u * i0 + 1u] + ((float)rb[2u * i1 + 1u] - (float)rb[2u * i0 + 1u]) * t) * (1.0f / 12000.0f);
}

static float dj_bp(float x, const float *c, float *s)   /* the band of the same filter. svf_lp stays the low-pass */
{
    float v3 = x - s[1], v1 = c[0] * s[0] + c[1] * v3, v2 = s[1] + c[1] * s[0] + c[2] * v3;
    s[0] = 2.0f * v1 - s[0];
    s[1] = 2.0f * v2 - s[1];
    return v1;
}

static void dj_vowel_coefs(void)
{
    static const float F[3][2] = {{730.0f, 1090.0f}, {530.0f, 1840.0f}, {570.0f, 840.0f}};
    int i = vow_i;
    if (i < 0 || i > 2)
        i = 0;
    svf_coefs(vow_c[0], F[i][0], 0.25f);
    svf_coefs(vow_c[1], F[i][1], 0.25f);
}

/* The resonant low-pass, crossed in. The peak is held so a Q on the master does not square the tanh. */
static void dj_lp(float *l, float *r, const float *c, float *zl, float *zr, float a)
{
    float wl = svf_lp(*l, c, zl), wr = svf_lp(*r, c, zr);
    if (wl > 1.4f)
        wl = 1.4f;
    else if (wl < -1.4f)
        wl = -1.4f;
    if (wr > 1.4f)
        wr = 1.4f;
    else if (wr < -1.4f)
        wr = -1.4f;
    *l += (wl - *l) * a;
    *r += (wr - *r) * a;
}

/* Not inline: the audio interrupt's frame stays the render, and this is its own. */
static void dj_mix(float *pl, float *pr) __attribute__((noinline));
static void dj_mix(float *pl, float *pr)
{
    uint16_t req, rose;
    int i, any;
    float l, r, st, a, bl, br;

    if (!dj_req && !dj_hot)
        return;
    req = dj_req;
    rose = (uint16_t)(req & (uint16_t)~dj_was);
    dj_was = req;
    any = req != 0;
    for (i = 0; i < DJ_N; i++) {
        float tgt = (req >> i) & 1u ? 1.0f : 0.0f;
        dj_a[i] += (tgt - dj_a[i]) * 0.018f;
        if (dj_a[i] < 0.0005f && tgt == 0.0f)
            dj_a[i] = 0.0f;
        if (dj_a[i] > 0.0f)
            any = 1;
    }
    dj_hot = (uint8_t)any;
    if (!any)
        return;

    if (rose & (uint16_t)(1u << DJ_SWEEP)) {
        sweep_u = 0.0f;
        sweep_n = 1;
        sweep_z[0][0] = sweep_z[0][1] = sweep_z[1][0] = sweep_z[1][1] = 0.0f;
        svf_coefs(sweep_c, 140.0f, 0.32f);
    }
    if (rose & (uint16_t)(1u << DJ_WOW))
        wow_ph = 0.0f;
    if (rose & (uint16_t)(1u << DJ_RING)) {
        ring_ph = ring_u = 0.0f;
        ring_f = 55.0f;
        ring_n = 1;
    }
    if (rose & (uint16_t)(1u << DJ_TRANS))
        tr_z[0] = tr_z[1] = 0.0f;
    if (rose & (uint16_t)(1u << DJ_PHASER)) {
        pha_y[0] = pha_y[1] = pha_ph = 0.0f;
        for (i = 0; i < 4; i++)
            pha_z[0][i] = pha_z[1][i] = 0.0f;
    }
    if (rose & (uint16_t)(1u << DJ_WAH)) {
        wah_ph = 0.75f;                        /* closed, so the first move opens */
        wah_n = 1;
        wah_z[0][0] = wah_z[0][1] = wah_z[1][0] = wah_z[1][1] = 0.0f;
        svf_coefs(wah_c, 400.0f, 0.28f);
    }
    if (rose & (uint16_t)(1u << DJ_RADIO)) {
        rad_ph = 0.0f;
        rad_n = 1;
        rad_z[0][0] = rad_z[0][1] = rad_z[1][0] = rad_z[1][1] = 0.0f;
        svf_coefs(rad_c, 900.0f, 0.55f);
    }
    if (rose & (uint16_t)(1u << DJ_FLANGE))
        dfl_ph = 0.0f;
    if (rose & (uint16_t)(1u << DJ_PAN))
        pan_ph = 0.0f;
    if (rose & (uint16_t)(1u << DJ_VOWEL)) {
        vow_t = 0.0f;
        vow_i = 0;
        for (i = 0; i < 2; i++) {
            vow_z[0][i][0] = vow_z[0][i][1] = 0.0f;
            vow_z[1][i][0] = vow_z[1][i][1] = 0.0f;
        }
        dj_vowel_coefs();
    }
    if (rose & (uint16_t)(1u << DJ_FILTER)) {
        flt_ph = 0.0f;
        flt_n = 1;
        flt_z[0][0] = flt_z[0][1] = flt_z[1][0] = flt_z[1][1] = 0.0f;
        svf_coefs(flt_c, 240.0f, 0.36f);
    }
    if (rose & (uint16_t)(1u << DJ_CHORUS))
        cho_ph = 0.0f;
    if (rose & (uint16_t)(1u << DJ_BEND))
        bend_u = 0.0f;
    if (rose & (uint16_t)(1u << DJ_CHOP)) {
        chop_hz = 7.0f;
        chop_ph = 0.0f;
        chop_g = 1.0f;
    }

    l = *pl;
    r = *pr;
    st = dj_step();

    /* Moving the read head warps the pitch and the comb. A repeat, a reverse or a
     * tape stop already owns the ring, so these stay out of its way. */
    if (bmode == BM_LIVE) {
        a = dj_a[DJ_WOW];
        if (a > 0.0f) {
            float s = sine(wow_ph), sr = sine(wow_ph + 0.23f), mix = 0.90f * a;
            rb_behind(1800.0f + 1400.0f * s, &bl, &br);     /* one slow drift a bar */
            l += (bl - l) * mix;
            rb_behind(1800.0f + 1400.0f * sr, &bl, &br);
            r += (br - r) * mix;
            wow_ph += 1.0f / (16.0f * st);
            if (wow_ph >= 1.0f)
                wow_ph -= 1.0f;
        }
        a = dj_a[DJ_FLANGE];
        if (a > 0.0f) {
            float tri = dfl_ph < 0.5f ? dfl_ph * 2.0f : 2.0f - dfl_ph * 2.0f;
            float mix = 0.62f * a;
            rb_behind(28.0f + tri * 280.0f, &bl, &br);      /* about 0.6 .. 7 ms */
            l += (bl - l) * mix;
            rb_behind(28.0f + (1.0f - tri) * 280.0f, &bl, &br);
            r += (br - r) * mix;
            dfl_ph += 1.0f / (4.0f * st);                   /* once a beat, sides opposed */
            if (dfl_ph >= 1.0f)
                dfl_ph -= 1.0f;
        }
        a = dj_a[DJ_CHORUS];
        if (a > 0.0f) {
            float s = sine(cho_ph), mix = 0.48f * a;
            rb_behind(780.0f + 240.0f * s, &bl, &br);
            l += (bl - l) * mix;
            rb_behind(980.0f - 240.0f * s, &bl, &br);
            r += (br - r) * mix;
            cho_ph += 1.5f * (1.0f / EN_SR);
            if (cho_ph >= 1.0f)
                cho_ph -= 1.0f;
        }
        a = dj_a[DJ_BEND];
        if (a > 0.0f) {
            float depth = sine(bend_u * 0.5f), mix;
            rb_behind(80.0f + depth * 5000.0f, &bl, &br);   /* down and back, once a bar */
            mix = a * (0.20f + 0.75f * depth);
            l += (bl - l) * mix;
            r += (br - r) * mix;
            bend_u += 1.0f / (16.0f * st);
            if (bend_u >= 1.0f)
                bend_u -= 1.0f;
        }
    }

    a = dj_a[DJ_VOWEL];
    if (a > 0.0f) {
        float bp, bpr;
        vow_t += 1.0f / EN_SR;
        if (vow_t >= 0.14f) {
            vow_t -= 0.14f;
            vow_i = (vow_i + 1) % 3;
            dj_vowel_coefs();
        }
        bp = dj_bp(l, vow_c[0], vow_z[0][0]) + dj_bp(l, vow_c[1], vow_z[0][1]);
        bpr = dj_bp(r, vow_c[0], vow_z[1][0]) + dj_bp(r, vow_c[1], vow_z[1][1]);
        if (bp > 2.0f)
            bp = 2.0f;
        else if (bp < -2.0f)
            bp = -2.0f;
        if (bpr > 2.0f)
            bpr = 2.0f;
        else if (bpr < -2.0f)
            bpr = -2.0f;
        l = l * (1.0f - 0.72f * a) + bp * a;
        r = r * (1.0f - 0.72f * a) + bpr * a;
    }
    a = dj_a[DJ_SWEEP];
    if (a > 0.0f) {
        if (sweep_u < 1.0f) {
            sweep_u += 1.0f / (1.5f * EN_SR);  /* open across a second and a half */
            if (sweep_u > 1.0f)
                sweep_u = 1.0f;
        }
        if ((sweep_n++ & 15u) == 0u)
            svf_coefs(sweep_c, 140.0f * fm_exp2f(sweep_u * 6.0f), 0.32f);
        dj_lp(&l, &r, sweep_c, sweep_z[0], sweep_z[1], a);
    }
    a = dj_a[DJ_FILTER];
    if (a > 0.0f) {
        float tri = flt_ph < 0.5f ? flt_ph * 2.0f : 2.0f - flt_ph * 2.0f;
        if ((flt_n++ & 15u) == 0u)
            svf_coefs(flt_c, 240.0f * fm_exp2f(tri * 3.6f), 0.36f);
        dj_lp(&l, &r, flt_c, flt_z[0], flt_z[1], a);
        flt_ph += 1.0f / (4.0f * st);          /* one cycle a beat, and it keeps going */
        if (flt_ph >= 1.0f)
            flt_ph -= 1.0f;
    }
    a = dj_a[DJ_PHASER];
    if (a > 0.0f) {
        float s, c0, c1, x, y;
        s = sine(pha_ph);
        c0 = 0.48f + 0.22f * s;
        c1 = 0.48f + 0.22f * sine(pha_ph + 0.17f);
        if (c0 < 0.08f)
            c0 = 0.08f;
        else if (c0 > 0.92f)
            c0 = 0.92f;
        if (c1 < 0.08f)
            c1 = 0.08f;
        else if (c1 > 0.92f)
            c1 = 0.92f;
        x = l + 0.32f * pha_y[0];
        y = x;
        for (i = 0; i < 4; i++)
            y = phase_ap(y, &pha_z[0][i], c0);
        if (y > 1.6f)
            y = 1.6f;
        else if (y < -1.6f)
            y = -1.6f;
        pha_y[0] = y;
        l += (y - l) * (0.5f * a);             /* half wet: that is where the notches are */
        x = r + 0.32f * pha_y[1];
        y = x;
        for (i = 0; i < 4; i++)
            y = phase_ap(y, &pha_z[1][i], c1);
        if (y > 1.6f)
            y = 1.6f;
        else if (y < -1.6f)
            y = -1.6f;
        pha_y[1] = y;
        r += (y - r) * (0.5f * a);
        pha_ph += 1.0f / (4.0f * st);
        if (pha_ph >= 1.0f)
            pha_ph -= 1.0f;
    }
    a = dj_a[DJ_TRANS];
    if (a > 0.0f) {
        float body, at, y;
        tr_z[0] += (l - tr_z[0]) * 0.08f;      /* the body. What is left is the hit */
        body = tr_z[0];
        at = l - body;
        y = body * 0.42f + at * 1.85f;
        if (y > 1.4f)
            y = 1.4f;
        else if (y < -1.4f)
            y = -1.4f;
        l += (y - l) * a;
        tr_z[1] += (r - tr_z[1]) * 0.08f;
        body = tr_z[1];
        at = r - body;
        y = body * 0.42f + at * 1.85f;
        if (y > 1.4f)
            y = 1.4f;
        else if (y < -1.4f)
            y = -1.4f;
        r += (y - r) * a;
    }
    a = dj_a[DJ_WAH];
    if (a > 0.0f) {
        float s = 0.5f + 0.5f * sine(wah_ph), bp, bpr; /* closed to open, twice a beat */
        if ((wah_n++ & 15u) == 0u)
            svf_coefs(wah_c, 400.0f * fm_exp2f(s * 2.35f), 0.28f); /* about 400 Hz .. 2 kHz */
        bp = dj_bp(l, wah_c, wah_z[0]) * 1.55f;
        bpr = dj_bp(r, wah_c, wah_z[1]) * 1.55f;
        if (bp > 1.3f)
            bp = 1.3f;
        else if (bp < -1.3f)
            bp = -1.3f;
        if (bpr > 1.3f)
            bpr = 1.3f;
        else if (bpr < -1.3f)
            bpr = -1.3f;
        l += (bp - l) * a;
        r += (bpr - r) * a;
        wah_ph += 2.0f / (4.0f * st);
        if (wah_ph >= 1.0f)
            wah_ph -= 1.0f;
    }
    a = dj_a[DJ_RADIO];
    if (a > 0.0f) {
        float wob = 0.5f + 0.5f * sine(rad_ph), bp, bpr;
        if ((rad_n++ & 15u) == 0u)
            svf_coefs(rad_c, 800.0f * fm_exp2f(wob * 1.5f), 0.50f); /* about 0.8 .. 2.3 kHz */
        bp = dj_bp(l, rad_c, rad_z[0]) * 1.8f;
        bpr = dj_bp(r, rad_c, rad_z[1]) * 1.8f;
        if (bp > 1.3f)
            bp = 1.3f;
        else if (bp < -1.3f)
            bp = -1.3f;
        if (bpr > 1.3f)
            bpr = 1.3f;
        else if (bpr < -1.3f)
            bpr = -1.3f;
        l += (bp - l) * a;
        r += (bpr - r) * a;
        rad_ph += 1.0f / (32.0f * st);         /* a slow wander, two bars */
        if (rad_ph >= 1.0f)
            rad_ph -= 1.0f;
    }
    a = dj_a[DJ_RING];
    if (a > 0.0f) {
        float car;
        if (ring_u < 1.0f) {
            ring_u += 1.0f / (2.0f * EN_SR);   /* 55 Hz out to about 160, then it holds */
            if (ring_u > 1.0f)
                ring_u = 1.0f;
        }
        if ((ring_n++ & 15u) == 0u)
            ring_f = 55.0f * fm_exp2f(ring_u * 1.54f);
        ring_ph += ring_f * (1.0f / EN_SR);
        if (ring_ph >= 1.0f)
            ring_ph -= 1.0f;
        car = sine(ring_ph);
        l *= 1.0f - a * 0.85f * (1.0f - car);
        r *= 1.0f - a * 0.85f * (1.0f - car);
    }
    a = dj_a[DJ_PAN];
    if (a > 0.0f) {
        float s, gl, gr;
        pan_ph += 1.0f / (4.0f * st);          /* one cycle a beat */
        if (pan_ph >= 1.0f)
            pan_ph -= 1.0f;
        s = 0.5f + 0.5f * sine(pan_ph);
        gl = 0.2f + 0.8f * s;
        gr = 0.2f + 0.8f * (1.0f - s);
        l *= 1.0f - a * (1.0f - gl);
        r *= 1.0f - a * (1.0f - gr);
    }
    a = dj_a[DJ_CHOP];
    if (a > 0.0f) {
        float gate;
        if (chop_hz < 28.0f)
            chop_hz += 21.0f * (1.0f / EN_SR); /* 7 Hz to 28 Hz across a second */
        chop_ph += chop_hz * (1.0f / EN_SR);
        if (chop_ph >= 1.0f)
            chop_ph -= 1.0f;
        gate = sine(chop_ph) > 0.0f ? 1.0f : 0.0f;
        chop_g += (gate - chop_g) * 0.2f;
        l *= 1.0f - a * (1.0f - chop_g);
        r *= 1.0f - a * (1.0f - chop_g);
    }
    *pl = l;
    *pr = r;
}

/* Knobs 1 and 2. The ring multiplies the mix by a tone. The wah is a resonant
 * band rocking twice a beat, and the peak grits as it opens. Both are the song
 * that is already playing. Zero is not called, and the sample stays. Neither
 * is the GLO key of the same name, and neither writes the plate or the echo. */
static void warp_mix(float *pl, float *pr) __attribute__((noinline));
static void warp_mix(float *pl, float *pr)
{
    float l = *pl, r = *pr, st = step_len > 8.0f ? step_len : 8.0f;
    float tgt = kn1_amt ? 0.18f + 0.82f * ((float)kn1_amt * 0.01f) : 0.0f;
    kn1_sm += (tgt - kn1_sm) * 0.002f;
    if (kn1_sm < 0.0005f && tgt == 0.0f) {
        kn1_sm = 0.0f;
        kn1_ph = 0.0f;
    } else if (kn1_sm > 0.0f) {
        float hz = 32.0f * fm_exp2f(kn1_sm * 2.55f); /* about 40 Hz, out to about 190 */
        float car = sine(kn1_ph);
        float mix = kn1_sm * 0.92f;
        l = l * ((1.0f - mix) + car * mix);
        r = r * ((1.0f - mix) + car * mix);
        kn1_ph += hz * (1.0f / EN_SR);
        if (kn1_ph >= 1.0f)
            kn1_ph -= 1.0f;
    }
    tgt = kn2_amt ? 0.20f + 0.80f * ((float)kn2_amt * 0.01f) : 0.0f;
    kn2_sm += (tgt - kn2_sm) * 0.002f;
    if (kn2_sm < 0.0005f && tgt == 0.0f) {
        if (kn2_sm != 0.0f) {
            kn2_sm = 0.0f;
            kn2_ph = 0.0f;
            kn2_n = 0;
            kn2_z[0][0] = kn2_z[0][1] = kn2_z[1][0] = kn2_z[1][1] = 0.0f;
        }
    } else if (kn2_sm > 0.0f) {
        float s = 0.5f + 0.5f * sine(kn2_ph);
        float a = kn2_sm, bp, bpr;
        if ((kn2_n++ & 15u) == 0u)
            svf_coefs(kn2_c, 320.0f * fm_exp2f(s * (1.5f + a * 1.3f)), 0.40f - 0.16f * a);
        bp = dj_bp(l, kn2_c, kn2_z[0]) * (1.20f + 0.85f * a);
        bpr = dj_bp(r, kn2_c, kn2_z[1]) * (1.20f + 0.85f * a);
        if (bp > 1.35f)
            bp = 1.35f;
        else if (bp < -1.35f)
            bp = -1.35f;
        if (bpr > 1.35f)
            bpr = 1.35f;
        else if (bpr < -1.35f)
            bpr = -1.35f;
        l += (bp - l) * a;
        r += (bpr - r) * a;
        kn2_ph += 2.0f / (4.0f * st);
        if (kn2_ph >= 1.0f)
            kn2_ph -= 1.0f;
    }
    *pl = l;
    *pr = r;
}

static inline void fx_master(float *pl, float *pr)   /* one sample of the master, before the gain */
{
    float l = *pl, r = *pr;
    if ((bmode == BM_LIVE || bmode == BM_STOP) && !hop_kind && find_mode != FIND_SEARCH)
        rb_write(l, r);
    if (bmode != BM_LIVE) {
        float bl = 0.0f, br = 0.0f, w = 1.0f;
        if (bmode == BM_RPT) {
            rb_read(bm_off, &bl, &br);
            w = fm_minf(fm_minf(bm_off, bm_len - bm_off) * (1.0f / 64.0f), 1.0f);   /* 1.5 ms edges */
            if ((bm_off += 1.0f) >= bm_len)
                bm_off -= bm_len;
        } else if (bmode == BM_REV) {
            rb_read(bm_off, &bl, &br);
            w = fm_minf(fm_minf(bm_off, bm_len - bm_off) * (1.0f / 64.0f), 1.0f);
            if ((bm_off -= 1.0f) < 0.0f)
                bm_off += bm_len;
        } else if (bmode == BM_STOP) {
            float rate = 1.0f / (12.0f * step_len); /* three beats, full speed down to a stop */
            float slip = (float)(rb_w - bm_start) * 2.0f - bm_off;
            if (bm_hold) {
                if (bm_speed > 0.0f) {         /* slowing. At rest the mix is silent */
                    rb_read(bm_off, &bl, &br);
                    w = bm_speed;
                    bm_off += bm_speed;
                    bm_speed -= rate;
                }
            } else {
                float limit = (float)(RB_N - 8) * 2.0f;
                float cap = 1.12f, acc = rate;
                if (bm_speed < 0.0f)
                    bm_speed = 0.0f;
                if (slip > limit) {            /* held past the ring: the head's audio was overwritten */
                    bm_off += slip - limit;
                    slip = limit;
                }
                if (slip > (float)(RB_N / 2) * 2.0f) {
                    acc = rate * 6.0f;         /* spin up before the write laps the read */
                    cap = 1.25f;
                }
                /* Back to the speed it had, then a little over so the read meets the write. */
                if (bm_speed < 1.0f) {
                    bm_speed += acc;
                } else {
                    float over = bm_speed - 1.0f;
                    float land = over * over / (2.0f * rate);
                    if (slip - 8.0f > land) {
                        if (bm_speed < cap)
                            bm_speed += rate;
                        else if (bm_speed > cap)
                            bm_speed -= rate;
                    } else if (bm_speed > 1.0f) {
                        bm_speed -= rate;
                        if (bm_speed < 1.0f)
                            bm_speed = 1.0f;
                    }
                }
                if (slip - bm_speed <= 48.0f && bm_speed >= 0.995f && bm_speed <= 1.02f) {
                    bmode = BM_LIVE;           /* this sample is the live one */
                    bm_mix = 0.0f;
                    bm_tgt = 0.0f;
                    brate = -1;
                    bl = l;
                    br = r;
                    w = 1.0f;
                } else {
                    rb_read(bm_off, &bl, &br);
                    w = bm_speed > 1.0f ? 1.0f : bm_speed;
                    bm_off += bm_speed;
                    slip = (float)(rb_w - bm_start) * 2.0f - bm_off;
                    if (slip <= 4.0f) {        /* the step landed on the write */
                        bmode = BM_LIVE;
                        bm_mix = 0.0f;
                        bm_tgt = 0.0f;
                        brate = -1;
                        bl = l;
                        br = r;
                        w = 1.0f;
                    }
                }
            }
        }
        bm_mix += (bm_tgt - bm_mix) * 0.02f;    /* ~1 ms */
        l += (bl * w - l) * bm_mix;
        r += (br * w - r) * bm_mix;
        if (bm_tgt <= 0.0f && bm_mix < 0.0005f) {
            bm_mix = 0.0f;
            bmode = BM_LIVE;
            brate = -1;
        }
    }
    if (lp_amt > 0.0f) {
        l = svf_lp(l, lp_c, &lp_s[0]);
        r = svf_lp(r, lp_c, &lp_s[2]);
    }
    if (hp_amt > 0.0f) {                        /* the high-pass: what the low-pass leaves out */
        l -= svf_lp(l, hp_c, &hp_s[0]);
        r -= svf_lp(r, hp_c, &hp_s[2]);
    }
    if (crush_amt > 0.0f) {
        if (++crush_n >= 8) {                   /* 5.5 kHz, 5 bits */
            crush_n = 0;
            crush_l = (float)(int32_t)(l * 16.0f + (l < 0.0f ? -0.5f : 0.5f)) * (1.0f / 16.0f);
            crush_r = (float)(int32_t)(r * 16.0f + (r < 0.0f ? -0.5f : 0.5f)) * (1.0f / 16.0f);
        }
        l += (crush_l - l) * crush_amt;
        r += (crush_r - r) * crush_amt;
    }
    {
        int fg_on = 0, fl_busy = fl_amt > 0.0f || fl_tgt > 0.0f;
        if (fg_amt > 0.0f || fg_tgt > 0.0f) {   /* off: the sample is left as it was */
            fg_amt += (fg_tgt - fg_amt) * 0.006f;
            if (fg_amt < 0.0005f && fg_tgt == 0.0f)
                fg_amt = 0.0f;
            else
                fg_on = 1;
            if (fg_on && !fl_busy) {            /* this key owns the line while the eighth is up */
                float dl, dr, mix = 0.68f * fg_amt, fb = 0.58f * fg_amt;
                uint32_t i;
                fg_read(&dl, &dr);
                i = fl_w & (FL_N - 1u);
                fl_dl[i] = s16(l + dl * fb);
                fl_dr[i] = s16(r + dr * fb);
                fl_w++;
                l += (dl - l) * mix;
                r += (dr - r) * mix;
                fg_live = 1;
                fl_live = 1;
            }
        }
        if (fl_amt > 0.0f || fl_tgt > 0.0f) {   /* eighth key. Off: the sample is left as it was */
            fl_amt += (fl_tgt - fl_amt) * 0.006f;
            if (fl_amt < 0.0005f && fl_tgt == 0.0f)
                fl_amt = 0.0f;
            else {
                float s = 0.5f + 0.5f * sine(fl_ph); /* one jet, a little wider on the right */
                float base = 16.0f + 150.0f * s;    /* about 0.4 .. 3.8 ms */
                float dl = flange_at(fl_dl, fl_w, base);
                float dr = flange_at(fl_dr, fl_w, base + 14.0f);
                float mix = 0.55f * fl_amt;
                uint32_t i = fl_w & (FL_N - 1u);
                fl_dl[i] = s16(l + dl * (0.45f * fl_amt));
                fl_dr[i] = s16(r + dr * (0.45f * fl_amt));
                fl_w++;
                fl_ph += 1.5f * (1.0f / EN_SR);
                if (fl_ph >= 1.0f)
                    fl_ph -= 1.0f;
                l += (dl - l) * mix;
                r += (dr - r) * mix;
                fl_live = 1;
            }
        }
        if (fg_on && fl_busy) {                 /* both down: the jet wrote. This tap still sweeps */
            float dl, dr, mix = 0.68f * fg_amt;
            fg_read(&dl, &dr);
            l += (dl - l) * mix;
            r += (dr - r) * mix;
            fg_live = 1;
        }
    }
    if (phase_amt || phase_sm > 0.0f)
        phase_mix(&l, &r);                      /* off: the sample is left as it was */
    if (kn1_amt || kn2_amt || kn1_sm > 0.0f || kn2_sm > 0.0f)
        warp_mix(&l, &r);                       /* off: the sample is left as it was */
    if (cloud_live)
        cloud_mix(&l, &r);                      /* off: this is not taken, and the sample stays */
    if (hop_kind)
        hop_mix(&l, &r);                        /* off: the new song is already the whole master */
    if (find_mode)
        find_mix(&l, &r);                       /* a hop inside there lets this go */
    dj_mix(&l, &r);                             /* off: the sample is left as it was */
    *pl = l;
    *pr = r;
}

/* Twelve named keys, beside the four engines. Bit i is one sound (Glass through Soft).
 * Each is a single voice on that engine's note. A second four-note pad per key
 * does not fit the block, so a pad key sings the pad's first note in that sound. */
static const uint8_t LAY_PART[12] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2};
static const uint8_t LAY_SND[12] = {3, 7, 8, 6, 9, 4, 11, 6, 4, 8, 9, 15};
typedef struct {
    float ph[4], amp, env, s[4], c1[4], c2[4];
    uint8_t fired;
} lay_t;
static lay_t lay[12];
static uint16_t lay_on;
static uint8_t lay_lin[3]; /* pad, bass, acid: 0..100, before Glass, the stabs, and the House pluck */
static int lay_house, lay_alive;
volatile uint16_t en_layer_hot;

static void lay_quiet(void)
{
    int i, k;
    lay_on = 0;
    lay_lin[0] = lay_lin[1] = lay_lin[2] = 0;
    lay_house = 0;
    lay_alive = 0;
    en_layer_hot = 0;
    for (i = 0; i < 12; i++) {
        lay[i].amp = lay[i].env = 0.0f;
        lay[i].fired = 0;
        for (k = 0; k < 4; k++) {
            lay[i].ph[k] = 0.1f * (float)(i + k);
            lay[i].s[k] = 0.0f;
            lay[i].c1[k] = 0.0f;
            lay[i].c2[k] = 0.0f;
        }
    }
}

static void drain(void)
{
    while (q_r != q_w) {
        uint32_t c = q[q_r % QN];
        int a = (int)(c >> 16 & 0xFFu), b = (int)(int16_t)(c & 0xFFFFu);
        BARRIER();
        q_r++;
        switch (c >> 24) {
        case C_SET:
            if (a < P_NPARAMS)
                apply(a, b);
            break;
        case C_POOL: pool_set((uint32_t)b & 0xFFFu); break;
        case C_PLAY:
            if (a && !en_playing)
                play_start(1);
            else if (!a && en_playing)
                play_stop();
            break;
        case C_NEXT:
            if (a || !en_playing)
                chord_change();                 /* a place that landed on this bar does not wait for the next */
            else
                change_pending = 1;
            break;
        case C_FREEZE: en_frozen = (uint8_t)(a != 0); break;
        case C_SEED:
            rng ^= (uint32_t)(b & 0xFFFF) * 2654435761u;
            if (!rng)
                rng = 1u;
            break;
        case C_CLOCK:
            en_ext = 1;
            ext_age = 0;
            if (a == EN_CLK_TICK) {
                if (en_playing)
                    clock_pulse();
            } else if (a == EN_CLK_START) {
                play_start(1);
            } else if (a == EN_CLK_CONTINUE) {
                if (!en_playing)
                    play_start(0);
            } else if (en_playing) {
                play_stop();
            }
            break;
        case C_PANIC: quiet(); break;
        case C_MUTE: muted = (uint8_t)(a != 0); break;
        case C_HUSH: hush = (uint8_t)(a != 0); break;
        case C_SHED: shed = (uint8_t)(a != 0); break;
        case C_NOTE:
            if (a == 0xFF)
                hands_off();
            else
                note_cmd(a, b & 0x7F, b >> 8);
            break;
        case C_REGROW: regrow(); break;
        case C_SECTION:
            if (a == EN_CMD_BREAK && sec != SEC_GROOVE && sec != SEC_BUILD)
                break;                          /* already down */
            sec_req = a + 1;
            en_sec_req = (uint8_t)sec_req;
            if (!en_playing) {                  /* resting: now */
                section_cmd(a);
                sec_req = 0;
                drum_publish();
            }
            break;
        case C_FX:
            fx_req = a ? (fx_req & 0xFFFFu) | (uint32_t)(b & 0xFFFF) << 16 : (fx_req & 0xFFFF0000u) | (uint32_t)(b & 0xFFFF);
            break;
        case C_HOP:
            hop_arm(a);
            break;
        case C_FIND:
            find_arm(a);
            break;
        case C_LAYER:
            if (a == 1)
                lay_house = b != 0;
            else if (a >= 2 && a <= 4)
                lay_lin[a - 2] = (uint8_t)(b < 0 ? 0 : b > 100 ? 100 : b);
            else
                lay_on = (uint16_t)(b & 0xFFF);
            break;
        case C_DJ:
            dj_req = (uint16_t)(b & 0x3FFF);
            break;
        }
    }
}

/* --------------------------------------------------------------- render --- */
#define BLK 256
static float mixl[BLK], mixr[BLK], rsend[BLK], esend[BLK];

/* the pad's bus: a 12 dB low-pass, darker than it sounds on paper (Brian Eno darkened everything: the
 * Synthi's filter below half, then tape). No chorus: a modulated delay against pure tones beats (on
 * the FM-1's speaker above all), and that read as pulsing. The width comes from the voices' places,
 * the echo's two heads and the plate. */
static float padl[BLK], padr[BLK];
static float plp[4];

/* A closed step eases out, about 6 ms, so the voice does not click. An open row never multiplies. */
static float vgate_mul(int part)
{
    int st;
    float tgt;
    if ((unsigned)part > 2u || vgate[part] == 0xFFFFu)
        return 1.0f;
    st = (int)((sixteenth - 1u) & 15u);
    tgt = ((vgate[part] >> st) & 1) ? 1.0f : 0.0f;
    vgate_sm[part] += (tgt - vgate_sm[part]) * 0.004f;
    if (vgate_sm[part] < 1.0e-5f)
        vgate_sm[part] = 0.0f;
    else if (tgt > 0.5f && vgate_sm[part] > 0.9995f)
        vgate_sm[part] = 1.0f;
    return vgate_sm[part];
}

static void pad_bus(uint32_t n)
{
    const float fc = fm_clampf(700.0f * (pad_is_smp() ? 6.0f : PAD_SOUND[pad_snd()].fc) * fm_exp2f(bright * 0.03f + 0.6f * arcm + 1.5f * pad_filt +
                                                         wob_oct(0) + 3.0f * (float)par[P_LK_SNARE] * 0.01f * lk_snare), 200.0f, 12000.0f);
    const float c = 1.0f - fm_expf(-FM_TWO_PI * fc * (1.0f / EN_SR));
    uint32_t j;
    for (j = 0; j < n; j++) {
        float l, r;
        plp[0] += c * (padl[j] - plp[0]);
        plp[1] += c * (plp[0] - plp[1]);
        plp[2] += c * (padr[j] - plp[2]);
        plp[3] += c * (plp[2] - plp[3]);
        l = plp[1] * TG(TR_PAD, j) * wob_wag(0);
        r = plp[3] * TG(TR_PAD, j) * wob_wag(0);
        if (vgate[0] != 0xFFFFu) {
            float vg = vgate_mul(0);
            l *= vg;
            r *= vg;
        }
        mixl[j] += l * 0.8f;
        mixr[j] += r * 0.8f;
        rsend[j] += (l + r) * 0.6f;
        esend[j] += (l + r) * 0.2f;
    }
    for (j = 0; j < 4; j++)
        plp[j] = fm_flush(plp[j]);
}

static void render_pad(uint32_t n)
{
    int i;
    float lvl = 0.0f;
    uint32_t j0;
    for (j0 = 0; j0 < n; j0++)
        padl[j0] = padr[j0] = 0.0f;
    /* Level 0 is already silent. Skip the oscillators so an off key does not
     * spend the block. A key that is still held in GLO keeps its voice. */
    if (pad_vol <= 0.0f) {
        int keyed = 0, kv;
        for (kv = 0; kv < EN_NPAD; kv++)
            if (pad[kv].play)
                keyed = 1;
        if (!keyed) {
            en_pad_level = 0.0f;
            pad_bus(n);
            return;
        }
    }
    const int smp = pad_is_smp();
    const float g = pad_vol * 0.5f * pad_lvl * (1.0f + 0.9f * fm_maxf(brk, 0.0f)) * (smp ? pad_smp_gain() : PAD_SOUND[pad_snd()].g) *   /* the pad rises in a breakdown */
                    (1.0f + 0.35f * (float)par[P_LK_SNARE] * 0.01f * lk_snare);                  /* and swells with the links */
    uint32_t nb = (n + 31u) >> 5;
    pad_gain += (pad_gate - pad_gain) * fm_minf((float)n / ((pad_gate > 0.0f ? 2.0f : 5.0f) * EN_SR), 1.0f);
    for (i = 0; i < EN_NPAD; i++) {
        pad_t *v = &pad[i];
        float p0 = v->ph[0], p1 = v->ph[1], amp = v->amp, heard = v->out, s0 = v->ic[0], s1 = v->ic[1], s2 = v->ic[2],
              s3 = v->ic[3], gl, gr, pn, tgt = 0.0f, ptgt = 0.0f, idx = 0.0f, se = stab_env, ar = 0.01f;
        int use_phrase = 0;
        uint32_t j;
        if (amp < 1e-5f && pad_gain < 1e-4f && !v->play) {
            v->amp = 0.0f;
            continue;
        }
        pn = fm_clampf(v->pan + 0.35f * motion * mod[(i + 3) % NMOD_PAD].v + pad_pan * (i & 1 ? 1.0f : -1.0f), -1.0f, 1.0f);
        gl = fm_cosf((pn + 1.0f) * (FM_PI / 4.0f)) * g;
        gr = fm_sinf((pn + 1.0f) * (FM_PI / 4.0f)) * g;
        for (j = 0; j < n; j++) {
            float x, v1, v2, v3, b1, l1, b2;
            if (!(j & 31u)) {
                float br;
                pad_coefs(v, i);
                if (!v->play && mel_d[0]) {     /* the hand keeps its key. The song's chord walks */
                    int semi = mel_render_semi(0);
                    if (semi) {
                        float r = fm_exp2f((float)semi * (1.0f / 12.0f));
                        v->inc[0] *= r;
                        v->inc[1] *= r;
                    }
                }
                ar = 0.01f;
                if (v->play) {                   /* played from the keys */
                    br = v->pamp;
                    tgt = br * 0.8f;
                    if (keys_amt < 1.0f)
                        tgt *= keys_amt;        /* the song's pad stays. 1 is this line as it was */
                } else if (par[P_PAD_MAN]) {     /* off the algorithm: quiet (fading, not cut) */
                    br = 0.0f;
                    tgt = 0.0f;
                    ar = 0.0004f;
                } else if (pad_stabs()) {        /* a stab: the shared envelope */
                    br = se;
                    se *= stab_k;
                    tgt = br * 0.85f * pad_gain;
                } else {
                    float ph = v->t;
                    br = breath(v, ph);
                    tgt = br * v->peak * pad_gain * v->hf; /* v->amp keeps this, so a chord's handoff
                                                            * breathes as often as the chords expect */
                    if (pad_lock) {
                        /* Two bars, from the downbeat. The shape changes with the phrase,
                         * and the voices enter apart, so it is not one repeating block. */
                        static const float FA[6] = {0.08f, 0.04f, 0.05f, 0.10f, 0.26f, 0.06f};
                        static const float FH[6] = {0.48f, 0.58f, 0.28f, 0.46f, 0.34f, 0.36f};
                        static const float FR[6] = {0.16f, 0.16f, 0.12f, 0.18f, 0.16f, 0.14f};
                        float stepf = step_len > 1.0f ? (float)step_t / step_len : 0.0f;
                        float fa = v->fa, fh = v->fh, fr = v->fr, u, gain = 1.0f;
                        uint32_t st = (sixteenth - 1u) & 15u;
                        uint32_t b = bars - 1u;
                        uint32_t phrase = b >> 1;
                        int shape = (int)(phrase % 6u);
                        ph = (float)(b & 1u) + ((float)st + stepf) / 16.0f;
                        ph *= 0.5f;
                        if (ph >= 1.0f)
                            ph -= 1.0f;
                        if ((phrase % 3u) == 1u && i == 3 && ph < 0.5f)
                            u = -1.0f; /* this voice answers in the second bar */
                        else if ((phrase % 3u) == 2u && i == 1)
                            u = -1.0f; /* one voice sits the phrase out */
                        else
                            u = ph - (i == 0 ? 0.0f : 0.045f * (float)((phrase & 1u) ? (EN_NPAD - 1 - i) : i));
                        if (i != 0)
                            gain = (i == 2 && (phrase % 3u) == 0u) ? 0.62f : 0.78f;
                        if (u < 0.0f) {
                            br = 0.0f;
                        } else {
                            v->fa = FA[shape];
                            v->fh = FH[shape];
                            v->fr = FR[shape];
                            br = breath(v, u) * gain;
                            if (shape == 2 || shape == 5) { /* a second, quieter gesture in the same phrase */
                                float u2 = u - 0.58f, b2;
                                if (u2 > 0.0f) {
                                    v->fa = 0.06f;
                                    v->fh = 0.10f;
                                    v->fr = 0.12f;
                                    b2 = breath(v, u2) * gain * 0.7f;
                                    if (b2 > br)
                                        br = b2;
                                }
                            }
                        }
                        v->fa = fa;
                        v->fh = fh;
                        v->fr = fr;
                        if (!v->hand)
                            v->note = v->target;
                        ptgt = br * v->peak * pad_gain * v->hf;
                        use_phrase = 1;
                    }
                }
                /* Breath index, in turns. The stabs' Rhodes and Bloom open with it. */
                idx = (0.04f + 0.1f * bright * 0.01f + 0.1f * br) * (1.0f + 0.4f * arcm);
                if (smp)
                    iam_smp_at(&pad_smp[i], IAM_SMP_FMPI + pad_tone() - EN_NSOUND, (int)(v->note + 0.5f), tunefac, pad_tag[i]);
                if (smp && !v->play) {
                    static uint8_t bent[EN_NPAD];
                    int semi = mel_render_semi(0);
                    if (semi || bent[i]) {
                        iam_smp_tune(&pad_smp[i], semi);
                        bent[i] = (uint8_t)(semi != 0);
                    }
                }
            }
            amp += (tgt - amp) * ar;
            if (use_phrase)
                heard += (ptgt - heard) * ar;
            if (smp)
                x = iam_smp_read(&pad_smp[i]);
            else switch (v->voices) {
            case PO_RHODES: {                    /* FM, the modulator at the carrier's pitch: the stabs */
                float m = sine(p0) * idx;
                x = (sine(p0 + m + 1.0f) + 0.12f * sine(2.0f * p0)) * 1.2f;   /* one carrier: no beating */
                break;
            }
            case PO_BLOOM:                       /* two sines, detuned, an octave that opens with the breath */
                x = (sine(p0) + sine(p1)) * 0.5f + sine(2.0f * p0) * (0.08f + 1.4f * idx);
                break;
            case PO_STRINGS:                     /* two saws, detuned (Detune) */
                x = 0.62f * (blep_saw(p0, v->inc[0]) + blep_saw(p1, v->inc[1]));
                break;
            case PO_GLASS: {
                if (pad_stabs()) {               /* Pings: a mallet bar. Inharmonic partials, no FM */
                    float b = p0 * 2.76f, c = p0 * 5.40f;
                    b -= (float)(int)b;
                    c -= (float)(int)c;
                    x = (0.70f * sine(p0) + 0.40f * sine(b) + 0.16f * sine(c)) * 1.05f;
                } else {                         /* Glass: a sung crystal. A fifth, a little octave, no FM shriek */
                    float p5 = p0 * 1.498307f;
                    p5 -= (float)(int)p5;
                    x = (0.70f * sine(p0) + 0.26f * sine(p5) + 0.14f * sine(2.0f * p0)) * 1.2f;
                }
                break;
            }
            case PO_ORGAN:                       /* drawbars: 8', 4', 2 2/3', 2' */
                x = (0.62f * sine(p0) + 0.38f * sine(2.0f * p0) + 0.26f * sine(3.0f * p0) + 0.16f * sine(4.0f * p0) +
                     0.2f * sine(p1)) * 1.05f;
                break;
            case PO_WARM:                        /* two triangles, detuned, and a sine: soft and round */
                x = ((p0 < 0.5f ? 4.0f * p0 - 1.0f : 3.0f - 4.0f * p0) + (p1 < 0.5f ? 4.0f * p1 - 1.0f : 3.0f - 4.0f * p1)) *
                    0.7f + 0.5f * sine(p0);
                break;
            case PO_SUPER: {                     /* two saws detuned and one an octave up: a supersaw */
                float p2 = 2.0f * p0;
                p2 -= (float)(int)p2;
                x = 0.5f * (blep_saw(p0, v->inc[0]) + blep_saw(p1, v->inc[1])) + 0.3f * blep_saw(p2, 2.0f * v->inc[0]);
                break;
            }
            case PO_PWM: {                       /* a saw less another, detuned: a pulse whose width drifts */
                float q = p1 + 0.5f;
                if (q >= 1.0f)
                    q -= 1.0f;
                x = 0.55f * (blep_saw(p0, v->inc[0]) - blep_saw(q, v->inc[1])) + 0.3f * sine(p0);
                break;
            }
            case PO_MIST: {                      /* a sine, its detuned pair, and a quiet fifth */
                float p5 = p0 * 1.498307f;
                p5 -= (float)(int)p5;
                x = 0.5f * sine(p0) + 0.38f * sine(p1) + 0.22f * sine(p5);
                break;
            }
            case PO_SINE:                        /* a sine and a little of its octave */
                x = (sine(p0) + 0.18f * sine(2.0f * p0)) * 1.3f;
                break;
            default:                             /* Voices: as Discreet Music's Synthi, through two formants */
                x = (p0 < 0.5f ? 4.0f * p0 - 1.0f : 3.0f - 4.0f * p0) * 1.4f + 0.45f * blep_saw(p0, v->inc[0]) + 0.6f * sine(p0);
                v3 = x - s1;                     /* the first formant: TPT SVF (Zavalishin), bp and lp */
                v1 = v->c1[0] * s0 + v->c1[1] * v3;
                v2 = s1 + v->c1[1] * s0 + v->c1[2] * v3;
                s0 = 2.0f * v1 - s0;
                s1 = 2.0f * v2 - s1;
                b1 = v1 * v->c1[3];
                l1 = v2;
                v3 = x - s3;                     /* the second */
                v1 = v->c2[0] * s2 + v->c2[1] * v3;
                v2 = s3 + v->c2[1] * s2 + v->c2[2] * v3;
                s2 = 2.0f * v1 - s2;
                s3 = 2.0f * v2 - s3;
                b2 = v1 * v->c2[3];
                x = b1 + 0.5f * b2 + 0.45f * l1;
                break;
            }
            p0 += v->inc[0];
            if (p0 >= 1.0f)
                p0 -= 1.0f;
            p1 += v->voices == PO_GLASS ? v->inc[0] * 2.0f : v->inc[1];
            if (p1 >= 1.0f)
                p1 -= (float)(int)p1;
            if (smp && pad_smp[i].z && !pad_smp[i].z->looped && !pad_smp[i].done)
                x *= v->play ? 0.85f : (pad_gain > 0.35f ? pad_gain : 0.35f); /* a pluck has its own decay */
            else
                x *= use_phrase ? heard : amp;
            padl[j] += x * gl;                   /* to the pad's bus: smoothed below */
            padr[j] += x * gr;
        }
        v->ic[0] = fm_flush(s0);
        v->ic[1] = fm_flush(s1);
        v->ic[2] = fm_flush(s2);
        v->ic[3] = fm_flush(s3);
        v->amp = amp;
        if (use_phrase)
            v->out = heard;
        v->ph[0] = p0;
        v->ph[1] = p1;
        en_pad_note[i] = (uint8_t)v->note;
        en_pad_amp[i] = use_phrase ? heard : amp;
        lvl = fm_maxf(lvl, use_phrase ? heard : amp);
    }
    en_pad_level = lvl;
    while (nb--)
        stab_env *= stab_k;
    stab_env = fm_flush(stab_env);
    pad_bus(n);
}

static int bass_is_smp(void)
{
    unsigned v = (unsigned)par[P_DR_INST];
    return v >= EN_NSOUND && v < EN_NBASS_SND;
}
static int acid_is_smp(void)
{
    unsigned v = (unsigned)par[P_AC_WAVE];
    return v >= EN_NSOUND && v < EN_NACID_SND;
}
static void render_bass_smp(uint32_t n)
{
    uint32_t j;
    const float tgt = (en_playing && !par[P_DR_MAN]) || bass.play ? 1.0f : 0.0f;
    const float rate = tgt > 0.0f ? 1.0f / ((bass.play ? 0.03f : 2.0f) * EN_SR) : 1.0f / ((par[P_DR_MAN] ? 0.3f : 4.0f) * EN_SR);
    float amp = bass.amp, g = bass_vol * 2.1f; /* the pizz recording sits under the saws */
    int note;
    if (amp < 1e-5f && !en_playing && !bass.play) {
        bass.amp = 0.0f;
        en_bass_level = 0.0f;
        return;
    }
    if (bass_vol <= 0.0f && !bass.play) {
        bass.note += (bass.target - bass.note) * fm_minf((float)n * GLIDE_K, 1.0f);
        en_bass_level = 0.0f;
        return;
    }
    bass.note += (bass.target - bass.note) * fm_minf((float)n * (bass.play ? GLIDE_K * 12.0f : GLIDE_K), 1.0f);
    note = bass.midi_note ? (int)bass.midi_note : (int)(bass.note + 0.5f);
    if (bass.env_up || !bass_smp.live || bass_smp.set != IAM_SMP_UPBASS || bass_smp.note != (uint8_t)note) {
        iam_smp_go(&bass_smp, IAM_SMP_UPBASS, note, tunefac);
        bass.env_up = 0;
        if (tgt > 0.0f)
            amp = 1.0f;                         /* the pluck is the attack. A two-second swell would miss it */
    }
    if (!bass.play) {
        static uint8_t bent;
        int semi = mel_render_semi(1);
        if (semi || bent) {
            iam_smp_tune(&bass_smp, semi);
            bent = (uint8_t)(semi != 0);
        }
    }
    for (j = 0; j < n; j++) {
        float x = iam_smp_read(&bass_smp);
        amp += (tgt - amp) * rate;
        x *= amp * g * TG(TR_DRONE, j) * wob_wag(1);
        if (vgate[1] != 0xFFFFu)
            x *= vgate_mul(1);
        if (bass.play && keys_amt < 1.0f)
            x *= keys_amt;
        mixl[j] += x;
        mixr[j] += x;
        rsend[j] += x * 0.06f;
        esend[j] += x * 0.04f;
    }
    bass.amp = amp;
    en_bass_level = amp;
}
static void render_acid_smp(uint32_t n)
{
    uint32_t j;
    int idx = (int)par[P_AC_WAVE] - EN_NSOUND;
    int set = IAM_SMP_FLUTE + (idx < 0 ? 0 : idx > 3 ? 3 : idx);
    static const float AG[4] = {2.90f, 1.15f, 0.95f, 0.75f}; /* flute, tenor sax, muted trumpet, harmonica */
    float amp = acid.amp, g = acid_vol * AG[idx < 0 ? 0 : idx > 3 ? 3 : idx], lvl = 0.0f;
    int note = acid.note ? (int)acid.note : 67;
    if (acid_vol <= 0.0f && !acid.play) {
        en_acid_level = 0.0f;
        return;
    }
    if (amp < 1e-5f && acid.amp_t <= 0.0f) {
        acid.amp = 0.0f;
        en_acid_level = 0.0f;
        return;
    }
    iam_smp_at(&acid_smp, set, note, tunefac, acid_tag);
    for (j = 0; j < n; j++) {
        float y;
        if (acid.gate_left > 0 && --acid.gate_left == 0)
            acid.amp_t = 0.0f;
        if (!(j & 31u))
            acid.env *= acid.edec;
        amp += (acid.amp_t - amp) * 0.006f;
        y = iam_smp_read(&acid_smp) * amp * g * TG(TR_ACID, j) * wob_wag(2);
        if (vgate[2] != 0xFFFFu)
            y *= vgate_mul(2);
        if (acid.play && keys_amt < 1.0f)
            y *= keys_amt;
        mixl[j] += y;
        mixr[j] += y;
        rsend[j] += y * 0.15f;
        esend[j] += y * 0.35f;
        if (amp > lvl)
            lvl = amp;
    }
    acid.amp = amp;
    en_acid_level = lvl * (0.6f + 0.4f * acid.env);
}
static void render_bass(uint32_t n)
{
    uint32_t j;
    if (bass_is_smp()) {
        render_bass_smp(n);
        return;
    }
    const drone_snd_t *DS = &DRONE_SOUND[(unsigned)par[P_DR_INST] < EN_NSOUND ? par[P_DR_INST] : 0];
    const int dest = par[P_DL_DEST], osc = DS->osc;
    const float depth = (float)par[P_DL_DEPTH] * 0.01f, lv = dest == 2 ? 1.0f - 0.8f * depth * (0.5f - 0.5f * drone_lfo.v) : 1.0f,
                drv = 1.4f * DS->drive * (dest == 3 ? 1.0f + 2.5f * depth * (0.5f + 0.5f * drone_lfo.v) : 1.0f),
                g = bass_vol * 0.36f * lv * DS->g / fm_minf(1.0f + 0.15f * (DS->drive - 1.0f), 1.3f),
                dsaw = DS->saw, dsub = DS->sub,
                tgt = (en_playing && !par[P_DR_MAN]) || bass.play ? 1.0f : 0.0f,
                rate = tgt > 0.0f ? 1.0f / ((bass.play ? 0.03f : 2.0f) * EN_SR) : 1.0f / ((par[P_DR_MAN] ? 0.3f : 4.0f) * EN_SR),
                fmi = 0.5f + 2.2f * bass.env + 0.3f * arcm, pw = 0.5f + 0.3f * mod[9].v;
    float p0 = bass.ph[0], p1 = bass.ph[1], p2 = bass.ph[2], sp = bass.sph, mp = bass.mph, amp = bass.amp, f, i0, i1, i2, mi;
    if (amp < 1e-5f && !en_playing && !bass.play) {
        bass.amp = 0.0f;
        en_bass_level = 0.0f;
        return;
    }
    /* Off in the mix: the note still moves, for a named key on this engine.
     * The oscillator does not. GLO holding the drone still runs it. */
    if (bass_vol <= 0.0f && !bass.play) {
        bass.note += (bass.target - bass.note) * fm_minf((float)n * GLIDE_K, 1.0f);
        en_bass_level = 0.0f;
        return;
    }
    bass.note += (bass.target - bass.note) * fm_minf((float)n * (bass.play ? GLIDE_K * 12.0f : GLIDE_K), 1.0f);
    f = mtof(bass.note + (float)(bass.play ? 0 : mel_render_semi(1)) + (dest == 1 ? depth * drone_lfo.v : 0.0f));   /* the LFO on the pitch: a semitone at most */
    i0 = f * (osc == DO_FIFTHS ? 1.49831f : osc == DO_REESE || osc == DO_HOOVER ? 0.9862f : 0.99481f) *
         (1.0f / EN_SR);                        /* -9, 0, +9 cents (Reese and Hoover: 24; Fifths: a fifth) */
    i1 = f * (1.0f / EN_SR);
    i2 = f * (osc == DO_REESE || osc == DO_HOOVER ? 1.0140f : 1.00521f) * (1.0f / EN_SR);
    mi = osc == DO_METAL ? i1 * 1.4142f : i1 * (1.5f + 2.5f * bass.env);   /* Metal's modulator; Sync's slave */
    for (j = 0; j < n; j++) {
        float x;
        if (!(j & 31u)) {                       /* the filter, every 32 samples */
            float oct, fc;
            oct = (float)par[P_DR_CUT] * 0.075f - 1.5f + DS->cut + (dest == 0 ? 2.5f * depth * drone_lfo.v : 0.0f) +
                  wob_oct(1) + 4.0f * (float)par[P_DE_AMT] * 0.01f * bass.env + 0.8f * motion * mod[9].v + 0.8f * arcm -
                  2.0f * fm_maxf(brk, 0.0f) - 1.6f * fm_minf(brk, 0.0f) - 0.35f * duck_env * pulse_vol +
                  4.0f * (float)par[P_LK_KICK] * 0.01f * lk_kick;   /* the links: the kick opens it */
            fc = fm_clampf(f * 2.0f * fm_exp2f(oct), 45.0f, 9000.0f);
            svf_coefs(bass.c1, fc, reso_k);
            svf_coefs(bass.c2, fc * 1.3f, 1.4f);
        }
        switch (osc) {
        case DO_REESE:                          /* two saws far apart: a slow phasing growl */
            x = (blep_saw(p0, i0) + blep_saw(p2, i2)) * dsaw * 1.35f;
            break;
        case DO_PULSE: {                        /* a pulse, its width wandering */
            float q = p1 + pw;
            if (q >= 1.0f)
                q -= 1.0f;
            x = (blep_saw(p1, i1) - blep_saw(q, i1)) * dsaw * 1.4f;
            break;
        }
        case DO_FM:                             /* two-operator FM, the envelope opening the index */
            x = sine_any(p1 + fmi * sine(p0)) * dsaw * 2.2f;
            break;
        case DO_ORGAN: {                        /* a stopped pipe: the twelfth, almost no octave */
            float p3 = p1 * 3.0f, p5 = p1 * 5.0f;
            p3 -= (float)(int)p3;
            p5 -= (float)(int)p5;
            x = (0.80f * sine(p1) + 0.62f * sine(p3) + 0.14f * sine(p5)) * dsaw * 1.6f;
            break;
        }
        case DO_DIRTY: {                        /* a narrow pulse and a sine an octave up. Not the three saws */
            float q = p1 + 0.12f + 0.10f * bass.env, up = p1 + p1;
            if (q >= 1.0f)
                q -= 1.0f;
            if (up >= 1.0f)
                up -= 1.0f;
            x = ((blep_saw(p1, i1) - blep_saw(q, i1)) * 0.85f + 0.40f * sine(up)) * dsaw * 1.2f;
            break;
        }
        case DO_SINE:
            x = (sine(p1) + 0.15f * sine(2.0f * p1)) * dsaw * 1.8f;
            break;
        case DO_SQUARE: {                       /* a square: the saw less itself half a turn on */
            float q = p1 + 0.5f;
            if (q >= 1.0f)
                q -= 1.0f;
            x = (blep_saw(p1, i1) - blep_saw(q, i1)) * dsaw * 1.2f;
            break;
        }
        case DO_HOOVER: {                       /* two pulses (a third wide), far apart, and the saw's octave down */
            float q0 = p0 + 0.3f, q2 = p2 + 0.3f;
            if (q0 >= 1.0f)
                q0 -= 1.0f;
            if (q2 >= 1.0f)
                q2 -= 1.0f;
            x = (blep_saw(p0, i0) - blep_saw(q0, i0) + blep_saw(p2, i2) - blep_saw(q2, i2)) * dsaw * 0.9f +
                0.5f * dsaw * (2.0f * sp - 1.0f);
            break;
        }
        case DO_FIFTHS:                         /* the root and the fifth (p0 runs at 3/2) */
            x = (blep_saw(p1, i1) + 0.75f * blep_saw(p0, i0)) * dsaw * 1.1f;
            break;
        case DO_METAL:                          /* FM, the modulator at 1.41: off the harmonics */
            x = sine_any(p1 + (0.3f + 1.6f * bass.env) * sine(mp)) * dsaw * 2.0f;
            break;
        case DO_SYNC:                           /* a saw synced to the note, the envelope sweeping it */
            x = blep_saw(mp, mi) * dsaw * 1.3f;
            break;
        default:                                /* three saws a few cents apart */
            x = (blep_saw(p0, i0) + blep_saw(p1, i1) + blep_saw(p2, i2)) * dsaw;
            break;
        }
        if (sleep_on) {
            /* Throat: the drone, and one harmonic walking the formant. */
            float h, fp;
            sl_form += 0.04f / EN_SR;
            if (sl_form >= 1.0f)
                sl_form -= 1.0f;
            sl_vib += 0.33f / EN_SR;
            if (sl_vib >= 1.0f)
                sl_vib -= 1.0f;
            h = 5.0f + 6.0f * (0.5f + 0.5f * sine(sl_form));
            fp = p1 * h;
            fp -= (float)(int)fp;
            x = (sine(p1) * 0.55f + sine(fp) * (0.40f + 0.10f * sine(sl_vib))) * 1.35f;
        } else
            x += dsub * sine(sp);
        p0 += i0;
        p1 += sleep_on ? i1 * (1.0f + 0.004f * sine(sl_vib)) : i1;
        p2 += i2;
        sp += i1 * 0.5f;                        /* the sub: an octave down */
        mp += mi;
        if (mp >= 1.0f)
            mp -= (float)(int)mp;
        if (osc == DO_SYNC && p1 >= 1.0f)
            mp = (p1 - 1.0f) * (mi / i1);       /* the sync: the slave starts again with the note */
        if (p0 >= 1.0f)
            p0 -= 1.0f;
        if (p1 >= 1.0f)
            p1 -= 1.0f;
        if (p2 >= 1.0f)
            p2 -= 1.0f;
        if (sp >= 1.0f)
            sp -= 1.0f;
        x = svf_lp(x, bass.c1, &bass.s[0]);
        x = svf_lp(x, bass.c2, &bass.s[2]);
        amp += (tgt - amp) * rate;
        x = fm_tanhf(x * drv) * amp * g * TG(TR_DRONE, j) * wob_wag(1);
        if (vgate[1] != 0xFFFFu)
            x *= vgate_mul(1);
        if (bass.play && keys_amt < 1.0f)
            x *= keys_amt;                      /* the key has the drone. Let go, and the song is whole */
        mixl[j] += x;
        mixr[j] += x;
        rsend[j] += x * 0.06f;
        esend[j] += x * 0.04f;
    }
    for (j = 0; j < 4; j++)
        bass.s[j] = fm_flush(bass.s[j]);
    bass.ph[0] = p0;
    bass.ph[1] = p1;
    bass.ph[2] = p2;
    bass.sph = sp;
    bass.mph = mp;
    bass.amp = amp;
    en_bass_level = amp;
}

static void render_acid(uint32_t n)
{
    uint32_t j;
    if (acid_is_smp()) {
        render_acid_smp(n);
        return;
    }
    const acid_snd_t *AS = &ACID_SOUND[(unsigned)par[P_AC_WAVE] < EN_NSOUND ? par[P_AC_WAVE] : 0];
    const int osc = AS->osc;
    const float g = acid_vol * 0.75f * AS->g, adrv = AS->drive, res = (float)par[P_AC_RES] * 0.01f, envmod = (float)par[P_AC_ENV] * 0.06f,
                accamt = (float)par[P_AC_ACC] * 0.01f,
                base = (float)par[P_AC_CUT] * 0.066f, k1 = 1.45f - 1.38f * res,
                vel = 0.75f + 0.5f * accamt * acid.acc, glide = acid.sliding ? 0.0012f : 1.0f;
    float ph = acid.ph, inc = acid.inc, amp = acid.amp, lvl = 0.0f, ps = acid.ps, p3 = acid.p3, sratio = 1.0f, pw = 0.5f, lz = 1.0f;
    /* Off in the mix: keep the pitch moving for Pluck and the other acid keys.
     * The oscillator stays out. A held GLO key still plays it. */
    if (acid_vol <= 0.0f && !acid.play) {
        for (j = 0; j < n; j++)
            inc += (acid.inc_t - inc) * glide;
        acid.inc = inc;
        en_acid_level = 0.0f;
        return;
    }
    if (amp < 1e-5f && acid.amp_t <= 0.0f) {
        acid.amp = 0.0f;
        en_acid_level = 0.0f;
        return;
    }
    for (j = 0; j < n; j++) {
        float x, y;
        if (!(j & 31u)) {                       /* the cutoff: its knob, the envelope, an accent's push */
            float oct = base + envmod * acid.env * (1.0f + accamt * acid.acc) + 1.2f * accamt * acid.acc * acid.env +
                        wob_oct(2);
            float fc = fm_clampf(60.0f * fm_exp2f(oct), 50.0f, 14000.0f);
            svf_coefs(acid.c1, fc, k1);
            svf_coefs(acid.c2, fc * 1.15f, 1.2f);
            sratio = 1.6f + 2.6f * acid.env;    /* Sync: the slave's pitch, swept by the envelope */
            acid.lph += 32.0f * 0.35f / EN_SR;  /* PWM: its width, 0.35 Hz */
            if (acid.lph >= 1.0f)
                acid.lph -= 1.0f;
            pw = 0.5f + 0.38f * sine(acid.lph);
            lz = 1.0f + 7.0f * acid.env * acid.env;   /* Laser: its pitch, falling as the envelope does */
            acid.env *= acid.edec;
        }
        if (acid.gate_left > 0 && --acid.gate_left == 0)
            acid.amp_t = 0.0f;                  /* the gate closes */
        inc += (acid.inc_t - inc) * glide;      /* a slide: ~50 ms */
        switch (osc) {
        case AO_SQUARE: case AO_PWM: {          /* a square, or a pulse with a sub under it */
            float p2 = ph + (osc == AO_PWM ? pw : 0.5f);
            if (p2 >= 1.0f)
                p2 -= 1.0f;
            x = 0.7f * (blep_saw(ph, inc) - blep_saw(p2, inc));
            if (osc == AO_PWM) {                /* the width still drifts. The sub keeps it off the thin buzz */
                x = 0.62f * (blep_saw(ph, inc) - blep_saw(p2, inc)) + 0.40f * (ps < 0.5f ? 1.0f : -1.0f);
                ps += inc * 0.5f;
                if (ps >= 1.0f)
                    ps -= 1.0f;
            }
            break;
        }
        case AO_SYNC:                           /* a saw hard-synced to the note, swept up by the envelope */
            x = blep_saw(ps, inc * sratio);
            ps += inc * sratio;
            if (ps >= 1.0f)
                ps -= (float)(int)ps;
            break;
        case AO_PLUCK:                          /* FM, the index falling with the envelope */
            x = sine_any(ph + (0.2f + 2.2f * acid.env) * sine(2.0f * ph)) * 1.2f;
            break;
        case AO_CHIP:                           /* a raw square, as an 8-bit game's */
            x = ph < 0.5f ? 0.75f : -0.75f;
            break;
        case AO_SUB:                            /* the saw and a square an octave down */
            x = blep_saw(ph, inc) + 0.7f * (ps < 0.5f ? 1.0f : -1.0f);
            ps += inc * 0.5f;
            if (ps >= 1.0f)
                ps -= 1.0f;
            break;
        case AO_TRANCE:                         /* three saws, -15, 0, +15 cents */
            x = 0.5f * (blep_saw(ph, inc) + blep_saw(ps, inc * 1.0087f) + blep_saw(p3, inc * 0.9913f));
            ps += inc * 1.0087f;
            p3 += inc * 0.9913f;
            if (ps >= 1.0f)
                ps -= (float)(int)ps;
            if (p3 >= 1.0f)
                p3 -= (float)(int)p3;
            break;
        case AO_BLEEP:
            x = sine(ph) * 1.3f;
            break;
        case AO_FIFTH: {                        /* a square and a sine a fifth up, a little octave */
            x = (0.55f * (ph < 0.5f ? 1.0f : -1.0f) + 0.42f * sine(ps) + 0.16f * sine(ph + ph)) * 0.95f;
            ps += inc * 1.49831f;
            if (ps >= 1.0f)
                ps -= (float)(int)ps;
            break;
        }
        case AO_BELL: {                         /* a tube: the octave and the twelfth, brighter as it starts */
            float p2 = ph + ph, p3h = ph * 3.0f, br = 0.15f + 0.85f * acid.env;
            if (p2 >= 1.0f)
                p2 -= 1.0f;
            p3h -= (float)(int)p3h;
            x = (0.62f * sine(ph) + br * 0.40f * sine(p2) + br * 0.28f * sine(p3h)) * 1.2f;
            break;
        }
        case AO_ORGAN: {                        /* the twelfth shouts at the start, then an 8' and a 4' */
            float perc = acid.env * acid.env;
            x = (0.70f * sine(ph) + 0.42f * sine(ph + ph) + (0.12f + 0.70f * perc) * sine(ph * 3.0f)) * 0.95f;
            break;
        }
        case AO_LASER:                          /* a sine, up to three octaves above the note as it starts, falling */
            x = sine(ps) * 1.25f;
            ps += inc * lz;
            if (ps >= 1.0f)
                ps -= (float)(int)ps;
            break;
        case AO_FOLD:                           /* a sine through a sine: it folds over as the envelope opens */
            x = sine_any((0.3f + 1.2f * acid.env) * sine(ph)) * 1.1f;
            break;
        case AO_SOFT:                           /* a triangle */
            x = (ph < 0.5f ? 4.0f * ph - 1.0f : 3.0f - 4.0f * ph) * 1.2f;
            break;
        case AO_DRIVE: {                        /* a saw and its octave, folded. Not a hotter copy of Saw */
            float up = blep_saw(ps, inc * 2.0f);
            ps += inc * 2.0f;
            if (ps >= 1.0f)
                ps -= (float)(int)ps;
            x = sine_any(1.35f * (0.62f * blep_saw(ph, inc) + 0.48f * up)) * 1.15f;
            break;
        }
        default:
            x = blep_saw(ph, inc);
            break;
        }
        if (sleep_on) {
            /* A flute: the sine, a little octave, a slow vibrato. No squelch. */
            float p2 = ph + ph;
            float air;
            if (p2 >= 1.0f)
                p2 -= 1.0f;
            sl_fl += 5.1f / EN_SR;
            if (sl_fl >= 1.0f)
                sl_fl -= 1.0f;
            air = 0.04f * sine(sl_fl * 6.0f);
            x = (sine(ph) * 1.05f + 0.16f * sine(p2) + air) * (0.94f + 0.06f * sine(sl_fl));
        }
        ph += inc;
        if (ph >= 1.0f) {
            ph -= 1.0f;
            if (osc == AO_SYNC)
                ps = ph * sratio;               /* the sync: the slave starts again with the note */
        }
        if (sleep_on) {
            y = x;
        } else {
            y = svf_lp(x, acid.c1, &acid.s[0]);
            y = fm_tanhf(y * (1.6f + 1.5f * res) * adrv) / fm_minf(adrv, 1.3f);   /* the squelch: driven between the two */
            y = svf_lp(y, acid.c2, &acid.s[2]);
        }
        amp += (acid.amp_t - amp) * (sleep_on ? 0.00028f : 0.006f); /* sleep: about 80 ms, not a pluck */
        y *= amp * vel * g * TG(TR_ACID, j) * wob_wag(2);
        if (vgate[2] != 0xFFFFu)
            y *= vgate_mul(2);
        if (acid.play && keys_amt < 1.0f)
            y *= keys_amt;                      /* the key has the acid. Let go, and the line is whole */
        mixl[j] += y;
        mixr[j] += y;
        rsend[j] += y * 0.15f;
        esend[j] += y * 0.35f;
        lvl = fm_maxf(lvl, amp);
    }
    for (j = 0; j < 4; j++)
        acid.s[j] = fm_flush(acid.s[j]);
    acid.ph = ph;
    acid.ps = ps;
    acid.p3 = p3;
    acid.inc = inc;
    acid.amp = amp;
    en_acid_level = lvl * (0.6f + 0.4f * acid.env);
}

/* the noise: one sample of a colour from a generator */
static inline float nz_white(nz_t *z)
{
    z->rng = z->rng * 1664525u + 1013904223u;
    return (float)(int32_t)z->rng * (1.0f / 2147483648.0f);
}
static inline float nz_sample(nz_t *z, int c)
{
    float w = nz_white(z), pk, o;
    z->b0 = 0.99765f * z->b0 + w * 0.0990460f;  /* pink: Paul Kellet's economy filter */
    z->b1 = 0.96300f * z->b1 + w * 0.2965164f;
    z->b2 = 0.57000f * z->b2 + w * 1.0526913f;
    pk = (z->b0 + z->b1 + z->b2 + w * 0.1848f) * 0.22f;
    switch (c) {
    case 0: o = w * 0.5f; break;
    case 1: o = pk; break;
    case 2: z->br = (z->br + 0.02f * w) * (1.0f / 1.02f); o = z->br * 3.0f; break;
    case 3: o = (pk - z->pp) * 1.6f; break;
    case 4: o = (w - z->pw) * 0.35f; break;
    case 5: z->br = (z->br + 0.02f * w) * (1.0f / 1.02f); o = 1.5f * z->br + 0.15f * (w - z->pw) + 0.2f * pk; break;
    case 6:                                     /* black: crackle */
        if (w > 0.9965f || w < -0.9965f)
            z->crack = w * 1.6f;
        o = z->crack;
        z->crack *= 0.86f;
        break;
    default: {                                  /* yellow: pink through a band-pass at ~1.8 kHz */
        float hp = pk - z->y2 - 0.9f * z->y1;
        z->y1 += 0.256f * hp;
        z->y2 += 0.256f * z->y1;
        o = z->y1 * 1.4f;
        break;
    }
    }
    z->pw = w;
    z->pp = pk;
    return o;
}

static float nzl[BLK], nzr[BLK];

void en_haze(int shape)
{
    if (shape < 0)
        shape = 0;
    if (shape > 5)
        shape = 5;
    if (shape != nz_shape)
        nz_env = 0.0f;                          /* a new part starts from silence */
    nz_shape = shape;
}

void en_haze_oct(int oct)
{
    if (oct < -3)
        oct = -3;
    if (oct > 3)
        oct = 3;
    nz_oct = oct;
}

/* After a colour's own drop, before the speaker folds. Zero leaves the note. */
static int nz_shift(int base)
{
    return base + 12 * nz_oct;
}

void en_sleep(int on)
{
    sleep_on = on ? 1 : 0;
    if (!sleep_on) {
        sl_bird_left = 0;
        sl_bird_wait = 0;
        sleep_bell = sleep_chime = 0;
    }
}

void en_sleep_cue(int which)
{
    if (which & 1)
        sleep_chime = 1;
    if (which & 2)
        sleep_bell = 1;
}

/* Chop is the skank (the & of each beat): a short chord, not a noise.
 * Air swells a tone across each eighth.
 * Hush breathes a low tone, about six seconds, and is the one part that stays up.
 * Dust is its own shaker, in render_noise. */
static float nz_gate(int shape, int st, float pos)
{
    float eg, a;
    int hit = nz_step_on(shape, st);
    if (shape == 2) {
        eg = hit ? fm_expf(-pos * 4.2f) : 0.0f; /* the skank, or whichever steps the row names */
        a = 0.03f;
    } else if (shape == 3) {
        float pair = (st & 1) ? 1.0f + pos : pos;
        if (pair > 2.0f)
            pair = 2.0f;
        eg = 0.5f - 0.5f * fm_cosf(pair * FM_PI);
        if (!hit)
            eg = 0.0f;                          /* the swell stays. A rest eases out through nz_env */
        a = 0.05f;
    } else {
        nz_ph += 1.0f / (6.4f * EN_SR);
        if (nz_ph >= 1.0f)
            nz_ph -= 1.0f;
        eg = 0.42f + 0.58f * (0.5f + 0.5f * fm_sinf(nz_ph * FM_TWO_PI));
        if (!hit)
            eg = 0.0f;
        a = 0.01f;
    }
    nz_env += (eg - nz_env) * a;
    return nz_env;
}

static void render_noise(uint32_t n)            /* into its own buffer: the pump ducks it on its own */
{
    uint32_t j;
    const float g = noise_vol * 1.1f, k = nz_lp_k;
    const int shape = nz_shape;
    float lvl = 0.0f;
    if (shape == 1)                             /* a tick: a little room, almost no delay */
        nz_rev = 0.12f, nz_echo = 0.04f;
    else if (shape == 2)                        /* one quieter reply, then the next chop */
        nz_rev = 0.22f, nz_echo = 0.20f;
    else if (shape == 3)                        /* a tone, a little space, no hiss in the room */
        nz_rev = 0.16f, nz_echo = 0.03f;
    else if (shape == 4)
        nz_rev = 0.20f, nz_echo = 0.02f;
    else if (shape == 5)
        nz_rev = 0.42f, nz_echo = 0.04f;        /* the wind sits in the room */
    else
        nz_rev = 0.15f, nz_echo = 0.0f;
    if (g <= 0.0f || !en_playing) {
        for (j = 0; j < n; j++)
            nzl[j] = nzr[j] = 0.0f;
        nz_env *= 0.9f;
        if (nz_env < 1.0e-5f)
            nz_env = 0.0f;
        en_noise_level *= 0.9f;
        return;
    }
    if (shape == 5) {                           /* wind: a slow breeze, water, and a bird when one was asked for */
        float bright = (float)par[P_NZ_TONE] * 0.01f;
        const int st = (int)((sixteenth - 1u) & 15u);
        int pc = (int)en_chord_root % 12;
        int hum = 36 + pc + 12 * nz_oct;
        float hum_inc;
        if (hum < 24)
            hum = 24;
        if (hum > 84)
            hum = 84;
        hum += mel_render_semi(3);
        hum_inc = mtof(wob_bend((float)hum)) * (1.0f / EN_SR);
        float cut;
        if (bright < 0.0f)
            bright = 0.0f;
        if (bright > 1.0f)
            bright = 1.0f;
        cut = 0.004f + 0.030f * bright;         /* tone opens the breeze */
        for (j = 0; j < n; j++) {
            float l, r, gust, lap, wind, water, bird, ol, orr, tg;
            l = nz_sample(&nz[0], nz_color);
            r = nz_sample(&nz[1], nz_color);
            sl_l += cut * (l - sl_l);
            sl_r += cut * (r - sl_r);
            sl_wh += 0.040f * ((l + r) * 0.5f - sl_wh);
            sl_wb += 0.070f * (((l + r) * 0.5f - sl_wh) - sl_wb);
            sl_ph += 0.03f / EN_SR;
            sl_gust += 0.05f / EN_SR;
            if (sl_ph >= 1.0f)
                sl_ph -= 1.0f;
            if (sl_gust >= 1.0f)
                sl_gust -= 1.0f;
            sl_hum += hum_inc;
            if (sl_hum >= 1.0f)
                sl_hum -= 1.0f;
            gust = 0.35f + 0.65f * (0.5f + 0.5f * sine(sl_gust));
            lap = 0.45f + 0.55f * sine(sl_ph);
            wind = (sl_l + sl_r) * 0.5f * gust;
            water = sl_wb * 0.35f * lap;
            bird = 0.0f;
            if (sleep_on && sl_bird_left > 0) {
                int span = sl_bird_n > 1 ? sl_bird_n : 1;
                int age = span - sl_bird_left;
                float env = age < 500 ? (float)age / 500.0f : (float)sl_bird_left / (float)span;
                if (env < 0.0f)
                    env = 0.0f;
                if (env > 1.0f)
                    env = 1.0f;
                sl_bird_ph += sl_bird_f;
                sl_bird_f += sl_bird_df;
                if (sl_bird_ph >= 1.0f)
                    sl_bird_ph -= 1.0f;
                if (sl_bird_f < 0.0f)
                    sl_bird_f = 0.0f;
                if (sl_bird_f > 4500.0f / EN_SR)
                    sl_bird_f = 4500.0f / EN_SR;
                bird = sine(sl_bird_ph) * env * 0.55f;
                sl_bird_left--;
            } else if (sleep_on && sl_bird_wait > 0) {
                sl_bird_wait--;
                if (sl_bird_wait == 0)
                    sleep_chirp(1500.0f, 2400.0f, 0.07f);
            }
            tg = TG(TR_PAD, j) * g;
            ol = (wind * 1.5f + water + sine(sl_hum) * 0.10f * gust + bird) * tg * 1.7f;
            orr = (wind * 1.35f + water * 0.8f + sine(sl_hum) * 0.10f * gust + bird * 0.75f) * tg * 1.7f;
            if (vgate_on3 && vgate[3] != 0xFFFFu) {
                float tgt = nz_step_on(5, st) ? 1.0f : 0.0f;
                vgate_nz += (tgt - vgate_nz) * 0.004f;
                if (vgate_nz < 1.0e-5f)
                    vgate_nz = 0.0f;
                ol *= vgate_nz;
                orr *= vgate_nz;
            }
            nzl[j] = ol;
            nzr[j] = orr;
            lvl = fm_maxf(lvl, fm_fabsf(ol));
            lvl = fm_maxf(lvl, fm_fabsf(orr));
        }
        sl_l = fm_flush(sl_l);
        sl_r = fm_flush(sl_r);
        sl_wh = fm_flush(sl_wh);
        sl_wb = fm_flush(sl_wb);
        sl_ph = fm_flush(sl_ph);
        sl_gust = fm_flush(sl_gust);
        sl_hum = fm_flush(sl_hum);
        sl_bird_ph = fm_flush(sl_bird_ph);
        sl_bird_f = fm_flush(sl_bird_f);
    } else if (shape >= 3) {                    /* air and hush: a tone on the chord, not noise */
        int pc = (int)en_chord_root % 12;
        int base = (shape == 4 ? 48 : 60) + pc;
        float bright = (float)par[P_NZ_TONE] * 0.01f;
        float fifth_g, oct_g, scale, inc0, inc1, inc5, inc8;
        /* Brown and vinyl sit an octave under air. Hush is already that low
         * tone, and another octave does not come through the speaker. */
        if ((nz_color == 2 || nz_color == 6) && shape != 4)
            base -= 12;
        base = nz_shift(base);
        if (base < 28)
            base += 12;
        if (bright < 0.0f)
            bright = 0.0f;
        if (bright > 1.0f)
            bright = 1.0f;
        if (nz_color == 6) {                    /* vinyl on hush stays the fundamental. Air warps the disc below */
            fifth_g = 0.0f;
            oct_g = 0.0f;
        } else if (nz_color == 2) {
            fifth_g = 0.10f * bright;
            oct_g = 0.0f;
        } else {                                /* pink, and any other colour: a fifth, then a quiet octave */
            fifth_g = 0.22f + 0.20f * bright;
            oct_g = 0.06f * bright;
        }
        if (shape == 4) {
            fifth_g *= 0.30f;
            oct_g *= 0.30f;
        }
        scale = shape == 4 ? 0.50f : 0.42f;
        base += mel_render_semi(3);
        inc0 = mtof(wob_bend((float)base)) * (1.0f / EN_SR);
        inc1 = mtof(wob_bend((float)base + 0.06f)) * (1.0f / EN_SR);
        inc5 = mtof(wob_bend((float)base + 7.0f)) * (1.0f / EN_SR);
        inc8 = mtof(wob_bend((float)base + 12.0f)) * (1.0f / EN_SR);
        for (j = 0; j < n; j++) {
            float pos = (float)(step_t + (int32_t)j) / (step_len > 8.0f ? step_len : 8.0f);
            float tg, env, fifth, octv, ol, orr;
            int p;
            if (pos < 0.0f)
                pos = 0.0f;
            nz_hum_ph[0] += inc0;
            nz_hum_ph[1] += inc1;
            nz_hum_ph[2] += inc5;
            nz_hum_ph[3] += inc8;
            for (p = 0; p < 4; p++)
                if (nz_hum_ph[p] >= 1.0f)
                    nz_hum_ph[p] -= 1.0f;
            fifth = fm_sin_turns(nz_hum_ph[2]);
            octv = fm_sin_turns(nz_hum_ph[3]);
            tg = TG(TR_PAD, j) * g;
            env = nz_gate(shape, (int)((sixteenth - 1u) & 15u), pos);
            if (shape == 3 && nz_color == 6) {  /* vinyl: a warped disc. A deep drift, sparse pops */
                float w0 = nz_white(&nz[0]), w1 = nz_white(&nz[1]);
                float tri, drift, flutter, wow0, wow1;
                nz_ph += 0.37f / EN_SR;         /* one warp every few seconds, not a chorus */
                if (nz_ph >= 1.0f)
                    nz_ph -= 1.0f;
                tri = nz_ph < 0.5f ? 4.0f * nz_ph - 1.0f : 3.0f - 4.0f * nz_ph;
                drift = 0.028f * tri;
                flutter = 0.006f * sine(nz_ph * 6.0f);
                wow0 = 1.0f + drift + flutter;
                wow1 = 1.0f + drift - flutter;
                nz_hum_ph[0] += inc0 * (wow0 - 1.0f);
                nz_hum_ph[1] += inc1 * (wow1 - 1.0f);
                if (nz_hum_ph[0] >= 1.0f)
                    nz_hum_ph[0] -= 1.0f;
                if (nz_hum_ph[1] >= 1.0f)
                    nz_hum_ph[1] -= 1.0f;
                if (nz_hum_ph[0] < 0.0f)
                    nz_hum_ph[0] += 1.0f;
                if (nz_hum_ph[1] < 0.0f)
                    nz_hum_ph[1] += 1.0f;
                if (w0 > 0.992f || w0 < -0.992f)
                    nz[0].crack = w0 > 0.0f ? 0.85f : -0.85f;
                if (w1 > 0.992f || w1 < -0.992f)
                    nz[1].crack = w1 > 0.0f ? 0.85f : -0.85f;
                nz[0].crack *= 0.70f;
                nz[1].crack *= 0.70f;
                ol = (0.78f * fm_sin_turns(nz_hum_ph[0]) + 0.05f * w0 + 0.42f * nz[0].crack) *
                     scale * tg * env * 1.15f;
                orr = (0.78f * fm_sin_turns(nz_hum_ph[1]) + 0.05f * w1 + 0.42f * nz[1].crack) *
                      scale * tg * env * 1.15f;
            } else {
                ol = (fm_sin_turns(nz_hum_ph[0]) + fifth * fifth_g + octv * oct_g) * scale * tg * env;
                orr = (fm_sin_turns(nz_hum_ph[1]) + fifth * fifth_g * 0.75f + octv * oct_g * 0.5f) * scale * tg * env;
            }
            nzl[j] = ol;
            nzr[j] = orr;
            lvl = fm_maxf(lvl, fm_fabsf(ol));
            lvl = fm_maxf(lvl, fm_fabsf(orr));
        }
        for (j = 0; j < 4u; j++)
            nz_hum_ph[j] = fm_flush(nz_hum_ph[j]);
        nz_env = fm_flush(nz_env);
        nz_ph = fm_flush(nz_ph);
    } else if (shape == 0) {                    /* HYP: the bed, unchanged */
        for (j = 0; j < n; j++) {
            float l = nz_sample(&nz[0], nz_color), r = nz_sample(&nz[1], nz_color), tg = TG(TR_PAD, j) * g;
            nz[0].lp += k * (l - nz[0].lp);     /* Tone: a low-pass, 300 Hz .. 19 kHz */
            nz[1].lp += k * (r - nz[1].lp);
            nzl[j] = nz[0].lp * tg;
            nzr[j] = nz[1].lp * tg;
            lvl = fm_maxf(lvl, fm_fabsf(nzl[j]));
        }
    } else if (shape == 1) {
        /* A shaker on the offbeats, tuned to the chord. Tone opens the grain and
         * shortens the note. Knob 4 is that tone. Knob 2 bends the note. One noise sample
         * a channel. Wood is a short knock on that same note, an octave under the
         * pink shaker: the rumble walk sounded unpitched. Vinyl is mostly the crackle. */
        int pc = (int)en_chord_root % 12;
        int base = 72 + pc;
        float bright = (float)par[P_NZ_TONE] * 0.01f;
        float body, grain, decay, open, k2, inc0, inc1, inc5, inc8;
        const int st = (int)((sixteenth - 1u) & 15u);
        const float sl = step_len > 8.0f ? step_len : 8.0f;
        const int wood = nz_color == 2;
        if (bright < 0.0f)
            bright = 0.0f;
        if (bright > 1.0f)
            bright = 1.0f;
        if (wood) {
            base -= 12;                         /* under the pink shaker, still a note */
            body = 0.72f;
            grain = 0.0f;
        } else if (nz_color == 6) {
            base -= 5;
            body = 0.28f;
            grain = 0.50f + bright * 0.45f;
        } else {
            body = 0.62f - bright * 0.22f;
            grain = 0.10f + bright * 0.75f;
        }
        base = nz_shift(base);
        if (base > 96)
            base -= 12;
        if (base < 36)
            base += 12;
        decay = wood ? 6.0f + bright * 7.0f : 2.0f + bright * 5.0f;
        open = 0.04f + bright * 0.92f;
        k2 = k + (1.0f - k) * open;
        base += mel_render_semi(3);
        inc0 = mtof(wob_bend((float)base)) * (1.0f / EN_SR);
        inc1 = mtof(wob_bend((float)base + 0.07f)) * (1.0f / EN_SR);
        inc5 = mtof(wob_bend((float)base + 7.0f)) * (1.0f / EN_SR);
        inc8 = mtof(wob_bend((float)base + 12.0f)) * (1.0f / EN_SR);
        nz_rev = 0.16f - 0.08f * bright;
        nz_echo = 0.03f;
        for (j = 0; j < n; j++) {
            float pos = (float)(step_t + (int32_t)j) / sl;
            float l, r, tg, eg, env, ol, orr, sine, wide, fifth, gl, gr;
            if (pos < 0.0f)
                pos = 0.0f;
            l = nz_sample(&nz[0], nz_color);
            r = nz_sample(&nz[1], nz_color);
            if (wood) {
                float edge_l = l - nz_hp[0];
                float edge_r = r - nz_hp[1];
                float atk = pos < 0.12f ? 1.0f - pos * (1.0f / 0.12f) : 0.0f;
                float click = (0.20f + 0.65f * bright) * atk;
                nz_hp[0] += 0.40f * edge_l;
                nz_hp[1] += 0.40f * edge_r;
                gl = edge_l * click;
                gr = edge_r * click;
            } else {
                nz_hp[0] += k2 * (l - nz_hp[0]);
                nz_hp[1] += k2 * (r - nz_hp[1]);
                gl = nz_hp[0] * grain;
                gr = nz_hp[1] * grain;
            }
            nz_hum_ph[0] += inc0;
            nz_hum_ph[1] += inc1;
            nz_hum_ph[2] += inc5;
            nz_hum_ph[3] += inc8;
            if (nz_hum_ph[0] >= 1.0f)
                nz_hum_ph[0] -= 1.0f;
            if (nz_hum_ph[1] >= 1.0f)
                nz_hum_ph[1] -= 1.0f;
            if (nz_hum_ph[2] >= 1.0f)
                nz_hum_ph[2] -= 1.0f;
            if (nz_hum_ph[3] >= 1.0f)
                nz_hum_ph[3] -= 1.0f;
            sine = fm_sin_turns(nz_hum_ph[0]);
            wide = fm_sin_turns(nz_hum_ph[1]);
            fifth = fm_sin_turns(nz_hum_ph[2]);
            eg = nz_step_on(1, st) && pos < 0.95f ? fm_expf(-pos * decay) : 0.0f;
            nz_env += (eg - nz_env) * (wood ? 0.20f : 0.05f);
            env = nz_env;
            tg = TG(TR_PAD, j) * g;
            if (wood) {
                float oct = fm_sin_turns(nz_hum_ph[3]);
                float fifth_g = 0.12f + 0.50f * bright;
                ol = (0.70f * sine + 0.24f * oct + fifth_g * fifth + gl) * tg * 2.36f * env;
                orr = (0.70f * wide + 0.18f * oct + fifth_g * fifth * 0.75f + gr) * tg * 2.36f * env;
            } else {
                ol = (body * sine + 0.16f * body * fifth + gl) * tg * 1.55f * env;
                orr = (body * wide + 0.12f * body * fifth + gr) * tg * 1.55f * env;
            }
            nzl[j] = ol;
            nzr[j] = orr;
            lvl = fm_maxf(lvl, fm_fabsf(ol));
            lvl = fm_maxf(lvl, fm_fabsf(orr));
        }
        nz_hp[0] = fm_flush(nz_hp[0]);
        nz_hp[1] = fm_flush(nz_hp[1]);
        nz_hum_ph[0] = fm_flush(nz_hum_ph[0]);
        nz_hum_ph[1] = fm_flush(nz_hum_ph[1]);
        nz_hum_ph[2] = fm_flush(nz_hum_ph[2]);
        nz_hum_ph[3] = fm_flush(nz_hum_ph[3]);
        nz_env = fm_flush(nz_env);
        nz_ph = fm_flush(nz_ph);
    } else {
        /* Chop: a short chord on the offbeat, in the song's key. Root and a fifth.
         * Brown and vinyl sit an octave down. Tone opens the fifth. The old chop
         * was filtered noise at 5.5, loud and out of tune. */
        int pc = (int)en_chord_root % 12;
        int base = 64 + pc;
        float bright = (float)par[P_NZ_TONE] * 0.01f;
        float fifth_g, inc0, inc1, inc5;
        const int st = (int)((sixteenth - 1u) & 15u);
        const float sl = step_len > 8.0f ? step_len : 8.0f;
        const float scale = 0.62f;
        if (nz_color == 2 || nz_color == 6)
            base -= 12;
        base = nz_shift(base);
        if (base > 76)
            base -= 12;
        if (base < 40)
            base += 12;
        if (bright < 0.0f)
            bright = 0.0f;
        if (bright > 1.0f)
            bright = 1.0f;
        fifth_g = 0.40f + 0.25f * bright;
        base += mel_render_semi(3);
        inc0 = mtof(wob_bend((float)base)) * (1.0f / EN_SR);
        inc1 = mtof(wob_bend((float)base + 0.08f)) * (1.0f / EN_SR);
        inc5 = mtof(wob_bend((float)base + 7.0f)) * (1.0f / EN_SR);
        for (j = 0; j < n; j++) {
            float pos = (float)(step_t + (int32_t)j) / sl;
            float tg, env, fifth, ol, orr;
            if (pos < 0.0f)
                pos = 0.0f;
            nz_hum_ph[0] += inc0;
            nz_hum_ph[1] += inc1;
            nz_hum_ph[2] += inc5;
            if (nz_hum_ph[0] >= 1.0f)
                nz_hum_ph[0] -= 1.0f;
            if (nz_hum_ph[1] >= 1.0f)
                nz_hum_ph[1] -= 1.0f;
            if (nz_hum_ph[2] >= 1.0f)
                nz_hum_ph[2] -= 1.0f;
            env = nz_gate(shape, st, pos);
            if (env < 0.0001f) {             /* the rest. The pitch above keeps moving */
                nzl[j] = 0.0f;
                nzr[j] = 0.0f;
                continue;
            }
            fifth = fm_sin_turns(nz_hum_ph[2]);
            tg = TG(TR_PAD, j) * g;
            ol = (fm_sin_turns(nz_hum_ph[0]) + fifth * fifth_g) * scale * tg * env;
            orr = (fm_sin_turns(nz_hum_ph[1]) + fifth * fifth_g * 0.7f) * scale * tg * env;
            nzl[j] = ol;
            nzr[j] = orr;
            lvl = fm_maxf(lvl, fm_fabsf(ol));
            lvl = fm_maxf(lvl, fm_fabsf(orr));
        }
        nz_hum_ph[0] = fm_flush(nz_hum_ph[0]);
        nz_hum_ph[1] = fm_flush(nz_hum_ph[1]);
        nz_hum_ph[2] = fm_flush(nz_hum_ph[2]);
        nz_env = fm_flush(nz_env);
        nz_ph = fm_flush(nz_ph);
    }
    nz[0].lp = fm_flush(nz[0].lp);
    nz[1].lp = fm_flush(nz[1].lp);
    nz[0].br = fm_flush(nz[0].br);
    nz[1].br = fm_flush(nz[1].br);
    en_noise_level = fm_maxf(lvl * 6.0f, en_noise_level * 0.9f);
}

/* a hat: the metal and noise, high-passed (Chamberlin SVF, its corner the lane's tune); little echo */
static void render_hat(drum_t *v, uint32_t n, float g, float pl, float pr)
{
    uint32_t j;
    int k;
    float x1, x2, amp, att;
    if (!v->on)
        return;
    if (sleep_on && v->note) {
        /* Three high sines, about a second. Not the 808's metal. The extra drums
         * leave note at 0, so a shaker during Sleep stays a shaker. */
        float ph = v->ph, ph2 = v->ph2, ph3 = v->mph[0], a = v->amp;
        float det = v->note == 46 ? 0.94f : 1.0f;
        float ring = v->note == 46 ? 3.6f : 1.0f; /* the open hat is the long chime; its lane is quieter */
        const float i0 = 2093.0f / EN_SR * det;
        const float i1 = 2489.0f / EN_SR * det;
        const float i2 = 3136.0f / EN_SR * det;
        for (j = 0; j < n; j++) {
            float x = (sine(ph) + 0.55f * sine(ph2) + 0.28f * sine(ph3)) * a * g * 9.0f * ring * TG(TR_HATS, j);
            ph += i0;
            ph2 += i1;
            ph3 += i2;
            if (ph >= 1.0f)
                ph -= 1.0f;
            if (ph2 >= 1.0f)
                ph2 -= 1.0f;
            if (ph3 >= 1.0f)
                ph3 -= 1.0f;
            mixl[j] += x * pl;
            mixr[j] += x * pr;
            rsend[j] += x * 0.55f;
            esend[j] += x * 0.06f;
            a *= v->adec;
        }
        v->ph = ph;
        v->ph2 = ph2;
        v->mph[0] = ph3;
        v->amp = a;
        v->on = a > 0.0005f;
        return;
    }
    x1 = v->x1, x2 = v->x2, amp = v->amp, att = v->att;
    {
        float metal = v->inc > 0.05f ? v->inc : 1.0f;
        float nzamt = v->idx > 0.0f ? v->idx : KH->h_noise;
        for (j = 0; j < n; j++) {
        float in = 0.0f, hp, x;
        for (k = 0; k < 4; k++) {
            v->mph[k] += HAT_INC[k] * metal;
            if (v->mph[k] >= 1.0f)
                v->mph[k] -= 1.0f;
            in += v->mph[k] < 0.5f ? 0.25f : -0.25f;
        }
        noise = noise * 1664525u + 1013904223u;
        in = 0.5f * in + nzamt * (float)(int32_t)noise * (1.0f / 2147483648.0f);
        hp = in - x2 - 0.8f * x1;
        x1 += v->hp * hp;
        x2 += v->hp * x1;
        x = hp * amp * att * g * TG(TR_HATS, j);
        mixl[j] += x * pl;
        mixr[j] += x * pr;
        rsend[j] += x * 0.2f;
        esend[j] += x * 0.08f;
        amp *= v->adec;
        att += (1.0f - att) * 0.3f;
        }
    }
    v->x1 = fm_flush(x1);
    v->x2 = fm_flush(x2);
    v->amp = amp;
    v->att = att;
    v->on = amp > 0.0005f;
}

/* the open hat: higher partials and more noise than the closed hat. Sleep keeps the chime. */
static void render_ohat(drum_t *v, uint32_t n, float g, float pl, float pr)
{
    uint32_t j;
    int k;
    float x1, x2, amp, att;
    static const float OH[3] = {640.0f / EN_SR, 1010.0f / EN_SR, 1510.0f / EN_SR};
    if (!v->on)
        return;
    if (sleep_on && v->note) {
        render_hat(v, n, g, pl, pr);
        return;
    }
    x1 = v->x1;
    x2 = v->x2;
    amp = v->amp;
    att = v->att;
    {
        float metal = v->inc > 0.2f ? v->inc : 1.0f;
        float nzamt = v->idx > 0.2f ? v->idx : 1.0f;
        float coef = v->hp;
        for (j = 0; j < n; j++) {
            float in = 0.0f, hp, nz, x;
            for (k = 0; k < 3; k++) {
                v->mph[k] += OH[k] * metal;
                if (v->mph[k] >= 1.0f)
                    v->mph[k] -= 1.0f;
                in += v->mph[k] < 0.5f ? 0.18f : -0.18f;
            }
            noise = noise * 1664525u + 1013904223u;
            nz = (float)(int32_t)noise * (1.0f / 2147483648.0f);
            in = 0.22f * in + nzamt * nz;
            hp = in - x2 - 0.28f * x1;
            x1 += coef * hp;
            x2 += coef * x1;
            x = (hp + 0.16f * nz) * amp * att * g * TG(TR_HATS, j);
            mixl[j] += x * pl;
            mixr[j] += x * pr;
            rsend[j] += x * 0.32f;
            esend[j] += x * 0.12f;
            amp *= v->adec;
            att += (1.0f - att) * 0.16f;
        }
    }
    v->x1 = fm_flush(x1);
    v->x2 = fm_flush(x2);
    v->amp = amp;
    v->att = att;
    v->on = amp > 0.0005f;
}

/* cowbell: two soft squares, not quite a fifth apart. Tom and perc stay on the FM voice. */
static void render_bell(drum_t *v, uint32_t n, float g)
{
    uint32_t j;
    float ph = v->ph, ph2 = v->ph2, inc = v->inc, amp = v->amp, att = v->att, click = v->idx;
    float ratio = v->inc2 > 1.1f ? v->inc2 : 1.5f;
    if (!v->on)
        return;
    for (j = 0; j < n; j++) {
        float nz, q, q2, x;
        noise = noise * 1664525u + 1013904223u;
        nz = (float)(int32_t)noise * (1.0f / 2147483648.0f);
        q = sine(ph) + 0.34f * sine(ph * 3.0f);
        q2 = sine(ph2) + 0.26f * sine(ph2 * 3.0f);
        x = (0.56f * q + 0.44f * q2 + nz * click) * amp * att * g * TG(TR_SNARE, j);
        ph += inc;
        ph2 += inc * ratio;
        if (ph >= 1.0f)
            ph -= 1.0f;
        if (ph2 >= 1.0f)
            ph2 -= 1.0f;
        inc += (v->inc_end - inc) * 0.035f;
        mixl[j] += x * 0.84f;
        mixr[j] += x;
        rsend[j] += x * 0.2f;
        esend[j] += x * 0.06f;
        amp *= v->adec;
        click *= 0.93f;
        att += (1.0f - att) * 0.55f;
    }
    v->ph = ph;
    v->ph2 = ph2;
    v->inc = inc;
    v->amp = amp;
    v->att = att;
    v->idx = click;
    v->on = amp > 0.0005f;
}

/* shaker: handfuls of beads in bright noise. The gaps wander. It is not a hat. */
static void render_shaker(drum_t *v, uint32_t n, float g, float pl, float pr)
{
    uint32_t j;
    float s1 = v->x1, s2 = v->x2, b1 = v->mph[2], b2 = v->mph[3];
    float amp = v->amp, att = v->att, fine = v->idx;
    float grain = v->ph, gap = v->inc_end, fg = v->mph[0], fgap = v->mph[1];
    float rate = v->inc > 0.3f ? v->inc : 1.0f;
    float coef = v->hp, beads = v->t;
    if (!v->on)
        return;
    if (gap < 40.0f)
        gap = 40.0f;
    if (fgap < 24.0f)
        fgap = 24.0f;
    for (j = 0; j < n; j++) {
        float u, in, hp, shell, x, base;
        noise = noise * 1664525u + 1013904223u;
        u = (float)(noise >> 8) * (1.0f / 16777216.0f);
        in = (float)(int32_t)noise * (1.0f / 2147483648.0f);
        grain += 1.0f;
        fg += 1.0f;
        if (grain >= gap) {
            grain = 0.0f;
            if (beads >= 1.0f) {
                beads -= 1.0f;
                att = 0.45f + 0.55f * u;
                base = (0.0034f + 0.008f * u) / rate;
            } else {
                beads = 3.0f + 5.0f * u;
                att = 0.7f + 0.3f * u;
                base = 0.013f + 0.012f * u;
            }
            if (amp > 0.5f)
                base *= 0.68f;
            gap = base * EN_SR;
            if (gap < 55.0f)
                gap = 55.0f;
        }
        if (fg >= fgap) {
            fg = 0.0f;
            fine = 0.3f + 0.6f * u;
            fgap = (0.0016f + 0.0026f * u) * EN_SR / rate;
            if (fgap < 30.0f)
                fgap = 30.0f;
        }
        hp = in - s2 - 0.16f * s1;
        s1 += coef * hp;
        s2 += coef * s1;
        shell = in - b2 - 0.45f * b1;
        b1 += coef * 0.38f * shell;
        b2 += coef * 0.38f * b1;
        x = (hp * (att + 0.35f * fine) + 0.1f * shell) * amp * g * 2.4f * TG(TR_HATS, j);
        mixl[j] += x * pl;
        mixr[j] += x * pr;
        rsend[j] += x * 0.06f;
        att *= 0.968f;
        fine *= 0.95f;
        amp *= v->adec;
    }
    v->x1 = fm_flush(s1);
    v->x2 = fm_flush(s2);
    v->mph[2] = fm_flush(b1);
    v->mph[3] = fm_flush(b2);
    v->mph[0] = fg;
    v->mph[1] = fgap;
    v->ph = grain;
    v->inc_end = gap;
    v->att = att;
    v->idx = fine;
    v->t = beads;
    v->amp = amp;
    v->on = amp > 0.0005f;
}

/* crash: a ring of partials that are not the hat's, plus as much noise as that kit wants. */
static void render_crash(drum_t *v, uint32_t n, float g, float pl, float pr)
{
    uint32_t j;
    float p0 = v->ph, p1 = v->ph2, p2 = v->mph[0];
    float s1 = v->x1, s2 = v->x2, amp = v->amp, att = v->att, inc = v->inc;
    float ratio = v->inc2 > 0.5f ? v->inc2 : 1.5f;
    float nzamt = v->idx > 0.0f ? v->idx : 0.4f;
    float coef = v->hp;
    if (!v->on)
        return;
    for (j = 0; j < n; j++) {
        float in, hp, metal, x;
        noise = noise * 1664525u + 1013904223u;
        in = (float)(int32_t)noise * (1.0f / 2147483648.0f);
        hp = in - s2 - 0.45f * s1;
        s1 += coef * hp;
        s2 += coef * s1;
        metal = sine(p0) * (0.48f + 0.52f * sine(p1));
        metal += 0.36f * sine(p2);
        x = (metal * (1.1f - 0.4f * nzamt) + hp * nzamt) * amp * att * g * 1.55f * TG(TR_HATS, j);
        p0 += inc;
        p1 += inc * ratio;
        p2 += inc * (ratio * 0.62f + 1.7f);
        if (p0 >= 1.0f)
            p0 -= 1.0f;
        if (p1 >= 1.0f)
            p1 -= 1.0f;
        if (p2 >= 1.0f)
            p2 -= 1.0f;
        inc *= 0.999996f;
        mixl[j] += x * pl;
        mixr[j] += x * pr;
        rsend[j] += x * 0.42f;
        esend[j] += x * 0.16f;
        amp *= v->adec;
        att += (1.0f - att) * 0.07f;
    }
    v->ph = p0;
    v->ph2 = p1;
    v->mph[0] = p2;
    v->x1 = fm_flush(s1);
    v->x2 = fm_flush(s2);
    v->inc = inc;
    v->amp = amp;
    v->att = att;
    v->on = amp > 0.0005f;
}

/* the clap: noise through a band-pass, three bursts 8 ms apart, the last with a tail */
static void render_clap(drum_t *v, uint32_t n, float g)
{
    uint32_t j;
    float s1 = v->x1, s2 = v->x2, amp = v->amp, t = v->t, ph = v->ph;
    const kit_t *kc = KC ? KC : KS;
    const float gap = kc->cl_gap;
    if (!v->on)
        return;
    for (j = 0; j < n; j++) {
        float in, bp, x;
        if (t == gap || t == 2.0f * gap)
            amp = v->pend;                      /* the next burst */
        noise = noise * 1664525u + 1013904223u;
        in = (float)(int32_t)noise * (1.0f / 2147483648.0f);
        bp = in - s2 - 0.7f * s1;               /* Chamberlin SVF: the band-pass is s1 */
        s1 += v->hp * bp;
        s2 += v->hp * s1;
        x = (s1 * 1.6f + 0.35f * sine(ph) * fm_maxf(1.0f - t * (1.0f / 2500.0f), 0.0f)) * amp * g * TG(TR_SNARE, j);
        ph += v->inc;
        if (ph >= 1.0f)
            ph -= 1.0f;
        mixl[j] += x * 0.95f;
        mixr[j] += x;
        rsend[j] += x * 0.45f;
        esend[j] += x * 0.12f;
        amp *= t < 2.0f * gap ? v->inc_end : v->adec;
        t += 1.0f;
    }
    v->x1 = fm_flush(s1);
    v->x2 = fm_flush(s2);
    v->amp = amp;
    v->t = t;
    v->ph = ph;
    v->on = t < 2.0f * gap || amp > 0.0005f;
}

/* the snare: two tuned sines falling quickly, and noise through a band-pass decaying more slowly */
static void render_snare(drum_t *v, uint32_t n, float g, const kit_t *ks)
{
    uint32_t j;
    float s1 = v->x1, s2 = v->x2, amp = v->amp, ph = v->ph, ph2 = v->ph2, att = v->att, body = v->idx;
    if (!v->on)
        return;
    if (v->t == 0.0f)
        body = 1.0f;
    for (j = 0; j < n; j++) {
        float in, bp, x;
        noise = noise * 1664525u + 1013904223u;
        in = (float)(int32_t)noise * (1.0f / 2147483648.0f);
        bp = in - s2 - 0.5f * s1;
        s1 += v->hp * bp;
        s2 += v->hp * s1;
        x = ((sine(ph) + 0.6f * sine(ph2)) * body * ks->sn_body + s1 * ks->sn_noise) * amp * att * g * TG(TR_SNARE, j);
        ph += v->inc;
        ph2 += v->inc2;
        if (ph >= 1.0f)
            ph -= 1.0f;
        if (ph2 >= 1.0f)
            ph2 -= 1.0f;
        mixl[j] += x;
        mixr[j] += x * 0.95f;
        rsend[j] += x * 0.35f;
        esend[j] += x * 0.1f;
        amp *= v->adec;
        body *= 0.9992f;                        /* the body: ~20 ms */
        att += (1.0f - att) * 0.4f;
    }
    v->x1 = fm_flush(s1);
    v->x2 = fm_flush(s2);
    v->amp = amp;
    v->ph = ph;
    v->ph2 = ph2;
    v->att = att;
    v->idx = body;
    v->t = 1.0f;
    v->on = amp > 0.0005f;
}

/* the perc: an FM blip (the modulator at 1.48 times, its index falling), the pitch falling into place */
static void render_perc(drum_t *v, uint32_t n, float g)
{
    uint32_t j;
    float ph = v->ph, ph2 = v->ph2, inc = v->inc, amp = v->amp, idx = v->idx, att = v->att;
    if (!v->on)
        return;
    if (sleep_on && v->note) {
        /* A singing bowl. The strike dies. The hum stays for a few seconds.
         * A tom or a cowbell leaves note at 0, so Sleep does not turn it into the bowl. */
        float p3 = v->mph[0], p4 = v->mph[1];
        inc = 174.6f / EN_SR;
        for (j = 0; j < n; j++) {
            float x = (sine(ph) + idx * (0.34f * sine(ph2) + 0.16f * sine(p3) + 0.07f * sine(p4))) * amp * g * 6.2f *
                      TG(TR_SNARE, j);
            ph += inc;
            ph2 += inc * 2.76f;
            p3 += inc * 5.40f;
            p4 += inc * 8.93f;
            if (ph >= 1.0f)
                ph -= 1.0f;
            if (ph2 >= 1.0f)
                ph2 -= (float)(int)ph2;
            if (p3 >= 1.0f)
                p3 -= (float)(int)p3;
            if (p4 >= 1.0f)
                p4 -= (float)(int)p4;
            mixl[j] += x * 0.85f;
            mixr[j] += x;
            rsend[j] += x * 0.62f;
            esend[j] += x * 0.08f;
            amp *= v->adec;
            idx *= 0.9992f;                     /* the clang: a third of a second */
        }
        v->ph = ph;
        v->ph2 = ph2;
        v->mph[0] = p3;
        v->mph[1] = p4;
        v->amp = amp;
        v->idx = idx;
        v->on = amp > 0.0005f;
        return;
    }
    for (j = 0; j < n; j++) {
        float m = sine(ph2) * idx, x = sine(ph + m + 4.0f) * amp * att * g * TG(TR_SNARE, j);
        ph += inc;
        ph2 += inc * v->inc2;
        if (ph >= 1.0f)
            ph -= 1.0f;
        if (ph2 >= 1.0f)
            ph2 -= 1.0f;
        inc += (v->inc_end - inc) * 0.004f;
        mixl[j] += x * 0.8f;
        mixr[j] += x;
        rsend[j] += x * 0.3f;
        esend[j] += x * 0.25f;
        amp *= v->adec;
        idx *= 0.9995f;
        att += (1.0f - att) * 0.3f;
    }
    v->ph = ph;
    v->ph2 = ph2;
    v->inc = inc;
    v->amp = amp;
    v->idx = idx;
    v->att = att;
    v->on = amp > 0.0005f;
}

static void render_dsmp(iam_smp *s, drum_t *v, uint32_t n, float g, float gl, float gr)
{
    uint32_t j;
    float amp;
    if (!v->on)
        return;
    amp = v->amp;
    for (j = 0; j < n; j++) {
        float x = iam_smp_read(s) * amp * g;
        mixl[j] += x * gl;
        mixr[j] += x * gr;
        amp *= v->adec;
    }
    v->amp = amp;
    v->on = !s->done && amp > 0.0008f;
}
static void render_drums(uint32_t n)
{
    uint32_t j;
    const float g = pulse_vol * 1.1f;
    drum_t *v;
    /* The cajon and the clap are already as loud as the synth kit. The rope, the hat,
     * the woodblock and the shaker are quiet in the recording. */
    static const float DUSTY_G[EN_NDRUM] = {0.90f, 7.5f, 0.70f, 6.4f, 2.5f, 3.6f};
    static const float DUSTY_XG[EN_XDRUM] = {4.2f, 2.2f, 3.0f, 32.0f, 2.5f};
    if (lane_dusty(L_KICK))
        render_dsmp(&drum_smp[L_KICK], &drum[L_KICK], n, g * lane_g[L_KICK] * DUSTY_G[L_KICK], 1.0f, 1.0f);
    else {
    v = &drum[L_KICK];                           /* kick: a sine falling fast to its note, a click, driven */
    if (v->on) {
        float ph = v->ph, inc = v->inc, amp = v->amp, att = v->att, ck = v->idx, gk = g * lane_g[L_KICK] * 1.15f;
        float r1 = v->x1, r2 = v->x2, rl1 = v->mph[0], rl2 = v->mph[1], rp = v->ph2;
        const float dk = drive_k * KK->k_drive, drop = KK->k_drop, cka = KK->k_click, rum = KK->k_rumble;
        const float norm = 0.85f / fm_minf(1.0f + 0.25f * (dk - 1.0f), 1.6f);
        for (j = 0; j < n; j++) {
            float x, nz;
            noise = noise * 1664525u + 1013904223u;
            nz = (float)(int32_t)noise * (1.0f / 2147483648.0f);
            x = sine(ph) * amp * att + ck * cka * nz;
            if (rum > 0.0f) {                         /* the rumble: low noise and the note, swelling after */
                rl1 += 0.012f * (nz - rl1);           /* the hit and dying slowly, as a reverb's low end */
                rl2 += 0.012f * (rl1 - rl2);
                x += (r1 - r2) * (3.0f * rl2 + 0.5f * sine(rp));
                rp += v->inc_end;
                if (rp >= 1.0f)
                    rp -= 1.0f;
                r1 *= 0.99985f;                       /* ~0.35 s */
                r2 *= 0.9985f;                        /* ~35 ms */
            }
            x = fm_tanhf(x * 1.3f * dk) * norm * gk * TG(TR_KICK, j);
            mixl[j] += x;
            mixr[j] += x;
            ph += inc;
            if (ph >= 1.0f)
                ph -= 1.0f;
            inc += (v->inc_end - inc) * drop;        /* the drop: ~14 ms */
            amp *= v->adec;
            att += (1.0f - att) * 0.25f;
            ck *= 0.985f;                            /* the click: ~2 ms */
        }
        v->ph = ph;
        v->inc = inc;
        v->amp = amp;
        v->att = att;
        v->idx = ck;
        v->x1 = r1;
        v->x2 = r2;
        v->mph[0] = fm_flush(rl1);
        v->mph[1] = fm_flush(rl2);
        v->ph2 = rp;
        v->on = amp > 0.0005f || r1 - r2 > 0.002f;
    }
    }
    if (lane_dusty(L_SNARE))
        render_dsmp(&drum_smp[L_SNARE], &drum[L_SNARE], n, g * lane_g[L_SNARE] * DUSTY_G[L_SNARE], 0.9f, 1.05f);
    else
        render_snare(&drum[L_SNARE], n, g * 0.38f * lane_g[L_SNARE], KS);
    if (lane_dusty(L_CLAP))
        render_dsmp(&drum_smp[L_CLAP], &drum[L_CLAP], n, g * lane_g[L_CLAP] * DUSTY_G[L_CLAP], 1.0f, 1.0f);
    else
        render_clap(&drum[L_CLAP], n, g * 0.42f * lane_g[L_CLAP]);
    if (lane_dusty(L_HAT))
        render_dsmp(&drum_smp[L_HAT], &drum[L_HAT], n, g * lane_g[L_HAT] * DUSTY_G[L_HAT], 0.75f, 1.0f);
    else
        {
            unsigned hk = kit_i[2] < 9u ? kit_i[2] : 0;
            float stretch, nzlift, hg;
            hat_speak(hk, &stretch, &nzlift, &hg);
            render_hat(&drum[L_HAT], n, g * 0.22f * hg * lane_g[L_HAT], 0.75f, 1.0f); /* a little right */
        }
    if (lane_dusty(L_OHAT))
        render_dsmp(&drum_smp[L_OHAT], &drum[L_OHAT], n, g * lane_g[L_OHAT] * DUSTY_G[L_OHAT], 1.0f, 0.75f);
    else
        render_ohat(&drum[L_OHAT], n, g * 0.22f * lane_g[L_OHAT], 1.0f, 0.75f); /* a little left, a little more present */
    if (lane_dusty(L_PERC))
        render_dsmp(&drum_smp[L_PERC], &drum[L_PERC], n, g * lane_g[L_PERC] * DUSTY_G[L_PERC], 1.0f, 1.0f);
    else
        render_perc(&drum[L_PERC], n, g * 0.3f * lane_g[L_PERC]);
    if (xkit_i[0] == EN_KIT_DUSTY)
        render_dsmp(&xdrum_smp[0], &xdrum[0], n, g * xg[0] * DUSTY_XG[0], 1.0f, 1.0f);
    else if (xdrum[0].on)
        render_snare(&xdrum[0], n, g * 0.32f * xg[0], kit_ptr(xkit_i[0]));
    if (xkit_i[1] == EN_KIT_DUSTY)
        render_dsmp(&xdrum_smp[1], &xdrum[1], n, g * xg[1] * DUSTY_XG[1], 1.0f, 1.0f);
    else if (xdrum[1].on)
        render_perc(&xdrum[1], n, g * 0.34f * xg[1]);
    if (xkit_i[2] == EN_KIT_DUSTY)
        render_dsmp(&xdrum_smp[2], &xdrum[2], n, g * xg[2] * DUSTY_XG[2], 1.0f, 1.0f);
    else if (xdrum[2].on)
        render_bell(&xdrum[2], n, g * 0.34f * xg[2]);
    if (xkit_i[3] == EN_KIT_DUSTY)
        render_dsmp(&xdrum_smp[3], &xdrum[3], n, g * xg[3] * DUSTY_XG[3], 0.85f, 1.0f);
    else if (xdrum[3].on)
        /* The beads were peaking about twice the kick, and that hit was what flattened the master. */
        render_shaker(&xdrum[3], n, g * 0.20f * xg[3], 0.85f, 1.0f);
    if (xkit_i[4] == EN_KIT_DUSTY)
        render_dsmp(&xdrum_smp[4], &xdrum[4], n, g * xg[4] * DUSTY_XG[4], 1.0f, 0.8f);
    else if (xdrum[4].on)
        render_crash(&xdrum[4], n, g * 0.24f * xg[4], 1.0f, 0.8f);
}

/* One sample of a named key. The phases belong to the layer. The song's voices are not moved. */
static float lay_wrap(float p)
{
    if (p >= 1.0f)
        p -= (float)(int)p;
    return p;
}
static float lay_osc(int id, lay_t *v, float inc, float env)
{
    float p0 = v->ph[0], p1 = v->ph[1], p2 = v->ph[2], x = 0.0f;
    switch (id) {
    case 0: { /* Glass: FM at 3.5 */
        float m = sine(p1) * (0.25f + 1.4f * env);
        x = (sine_any(p0 + m) + 0.25f * sine(2.0f * p0)) * 1.1f;
        p1 = lay_wrap(p1 + inc * 3.5f);
        p0 = lay_wrap(p0 + inc);
        break;
    }
    case 1: { /* Dark: the singing pad's vowel, one voice */
        float v1, v2, v3, b1, b2, l1;
        x = (p0 < 0.5f ? 4.0f * p0 - 1.0f : 3.0f - 4.0f * p0) * 1.4f + 0.45f * blep_saw(p0, inc) + 0.6f * sine(p0);
        v3 = x - v->s[1];
        v1 = v->c1[0] * v->s[0] + v->c1[1] * v3;
        v2 = v->s[1] + v->c1[1] * v->s[0] + v->c1[2] * v3;
        v->s[0] = 2.0f * v1 - v->s[0];
        v->s[1] = 2.0f * v2 - v->s[1];
        b1 = v1 * v->c1[3];
        l1 = v2;
        v3 = x - v->s[3];
        v1 = v->c2[0] * v->s[2] + v->c2[1] * v3;
        v2 = v->s[3] + v->c2[1] * v->s[2] + v->c2[2] * v3;
        v->s[2] = 2.0f * v1 - v->s[2];
        v->s[3] = 2.0f * v2 - v->s[3];
        b2 = v1 * v->c2[3];
        x = b1 + 0.5f * b2 + 0.45f * l1;
        p0 = lay_wrap(p0 + inc);
        break;
    }
    case 2: /* Dub: two detuned saws */
        x = 0.62f * (blep_saw(p0, inc * 0.994f) + blep_saw(p1, inc * 1.006f));
        p0 = lay_wrap(p0 + inc * 0.994f);
        p1 = lay_wrap(p1 + inc * 1.006f);
        break;
    case 3: { /* Stabs: a Rhodes */
        float m = sine(p0) * (0.08f + 0.35f * env);
        x = (sine(p0 + m + 1.0f) + 0.12f * sine(2.0f * p0)) * 1.2f;
        p0 = lay_wrap(p0 + inc);
        break;
    }
    case 4: /* Sub: the sine and its octave down */
        x = (sine(p0) + 0.15f * sine(2.0f * p0)) * 1.8f + 0.5f * sine(p1);
        p0 = lay_wrap(p0 + inc);
        p1 = lay_wrap(p1 + inc * 0.5f);
        break;
    case 5: /* Reese: two saws, far apart */
        x = (blep_saw(p0, inc * 0.9862f) + blep_saw(p2, inc * 1.014f)) * 0.5f * 1.35f;
        p0 = lay_wrap(p0 + inc * 0.9862f);
        p2 = lay_wrap(p2 + inc * 1.014f);
        break;
    case 6: { /* Hoover: two pulses, far apart, and the octave down */
        float q0 = p0 + 0.3f, q2 = p2 + 0.3f, i0 = inc * 0.9862f, i2 = inc * 1.014f;
        if (q0 >= 1.0f)
            q0 -= 1.0f;
        if (q2 >= 1.0f)
            q2 -= 1.0f;
        x = (blep_saw(p0, i0) - blep_saw(q0, i0) + blep_saw(p2, i2) - blep_saw(q2, i2)) * 0.36f +
            0.2f * (2.0f * p1 - 1.0f);
        p0 = lay_wrap(p0 + i0);
        p2 = lay_wrap(p2 + i2);
        p1 = lay_wrap(p1 + inc * 0.5f);
        break;
    }
    case 7: /* FM bass */
        x = sine_any(p0 + (0.5f + 2.2f * env) * sine(p1)) * 1.21f;
        p0 = lay_wrap(p0 + inc);
        p1 = lay_wrap(p1 + inc);
        break;
    case 8: /* Pluck */
        x = sine_any(p0 + (0.2f + 2.2f * env) * sine(2.0f * p0)) * 1.2f;
        p0 = lay_wrap(p0 + inc);
        break;
    case 9: /* Trance: three saws */
        x = 0.5f * (blep_saw(p0, inc) + blep_saw(p1, inc * 1.0087f) + blep_saw(p2, inc * 0.9913f));
        p0 = lay_wrap(p0 + inc);
        p1 = lay_wrap(p1 + inc * 1.0087f);
        p2 = lay_wrap(p2 + inc * 0.9913f);
        break;
    case 10: { /* Bleep: a narrow pulse and a sine under it. A game blip, not FM */
        float q = p0 + 0.18f;
        if (q >= 1.0f)
            q -= 1.0f;
        x = 0.55f * (blep_saw(p0, inc) - blep_saw(q, inc)) + 0.40f * sine(p1);
        p0 = lay_wrap(p0 + inc);
        p1 = lay_wrap(p1 + inc * 0.5f);
        break;
    }
    default: { /* Soft: a flute. A sine, a quiet fifth, a slow vibrato */
        float p5 = p0 * 1.498307f;
        p5 -= (float)(int)p5;
        x = (0.78f * sine(p0) + 0.22f * sine(p5)) * (0.92f + 0.08f * sine(p1)) * 1.2f;
        p2 = lay_wrap(p2 + 4.5f / EN_SR);
        p0 = lay_wrap(p0 + inc * (1.0f + 0.004f * sine(p2)));
        p1 = lay_wrap(p1 + 2.2f / EN_SR);
        break;
    }
    }
    v->ph[0] = p0;
    v->ph[1] = p1;
    v->ph[2] = p2;
    return x;
}

static void render_layers(uint32_t n)
{
    int id, st = (int)((sixteenth - 1u) & 15u);
    uint16_t hot = 0;
    int alive = 0;
    if (!lay_on && !lay_alive) {
        en_layer_hot = 0;
        return;
    }
    for (id = 0; id < 12; id++) {
        lay_t *v = &lay[id];
        int part = LAY_PART[id], snd = LAY_SND[id];
        int on = (lay_on >> id) & 1;
        int open = vgate[part] == 0xFFFFu || ((vgate[part] >> st) & 1);
        int stab = id == 2 || id == 3;
        float inc, g, drv = 1.0f, phrase, pan, gl, gr, env;
        uint32_t j;
        int k;
        if (!en_playing)
            on = 0;
        if (part == 2 && acid.amp < 1e-3f && acid.amp_t <= 0.0f)
            on = 0;
        if (!on && v->amp < 1e-4f && v->env < 1e-4f)
            continue;
        alive = 1;
        if (part == 0)
            inc = mtof(pad[0].note + (float)mel_render_semi(0)) * (1.0f / EN_SR);
        else if (part == 1)
            inc = mtof(bass.note + (float)mel_render_semi(1)) * (1.0f / EN_SR);
        else
            inc = acid.inc > 1e-5f ? acid.inc : mtof((float)acid.note) * (1.0f / EN_SR);
        if (inc < 1e-5f)
            inc = 1e-5f;
        if (inc > 0.45f)
            inc = 0.45f;
        {
            /* The stored level, not the engine's mute. A dark part still lets its keys sound. */
            int lin = lay_lin[part];
            float base = (float)(lin * lin) * 0.0001f;
            if (base < 1e-8f) {
                if (v->amp < 1e-4f && v->env < 1e-4f)
                    continue;
                on = 0;
            }
            if (part == 0) {
                float pv = base;
                if (snd == 3)
                    pv *= 0.1024f; /* Glass is heard at 32% of the level, and the level is squared */
                else if (snd == 6 || snd == 8)
                    pv *= 0.25f; /* a stab at half the level, squared */
                g = pv * 0.5f * PAD_SOUND[snd].g;
                for (k = 0; k < 4; k++) {
                    v->c1[k] = pad[0].c1[k];
                    v->c2[k] = pad[0].c2[k];
                }
            } else if (part == 1) {
                const drone_snd_t *DS = &DRONE_SOUND[snd];
                g = base * 0.36f * DS->g * 0.55f; /* four of these sit about as loud as the bass */
                drv = DS->drive;
                for (k = 0; k < 4; k++) {
                    v->c1[k] = bass.c1[k];
                    v->c2[k] = bass.c2[k];
                }
            } else {
                const acid_snd_t *AS = &ACID_SOUND[snd];
                float av = base;
                if (lay_house && id == 8) {
                    int p = lin - 40;
                    if (p < 0)
                        p = 0;
                    av = (float)(p * p) * 0.0001f; /* House pluck, forty down, then squared */
                }
                g = av * 0.75f * AS->g * 0.55f;
                drv = AS->drive;
                for (k = 0; k < 4; k++) {
                    v->c1[k] = acid.c1[k];
                    v->c2[k] = acid.c2[k];
                }
            }
        }
        phrase = part == 0 && !stab ? en_pad_amp[0] : 1.0f;
        env = part == 1 ? bass.env : part == 2 ? acid.env : phrase;
        g *= wob_wag(part); /* the named key follows the engine's wobble */
        pan = id == 0 || id == 2 ? -0.35f : id == 1 || id == 3 ? 0.35f : 0.0f;
        gl = fm_cosf((pan + 1.0f) * (FM_PI / 4.0f)) * g;
        gr = fm_sinf((pan + 1.0f) * (FM_PI / 4.0f)) * g;
        if (stab && on && open && (st & 3) == 2 && v->fired != (uint8_t)st) {
            v->env = 1.0f;
            v->fired = (uint8_t)st;
        }
        for (j = 0; j < n; j++) {
            float x, tgt = on && open ? (stab ? v->env : 1.0f) : 0.0f;
            float rate = tgt > v->amp ? 0.00057f : 0.00019f;
            v->amp += (tgt - v->amp) * rate;
            x = lay_osc(id, v, inc, stab ? v->env : env);
            if (stab) {
                v->s[0] += (x - v->s[0]) * 0.22f; /* the saws, darkened a little */
                x = v->s[0];
            } else if (part > 0) {
                x = svf_lp(x, v->c1, &v->s[0]);
                x = svf_lp(x, v->c2, &v->s[2]);
                x = fm_tanhf(x * drv);
            }
            if (stab)
                v->env *= 0.9997f;
            x *= v->amp * (stab ? 1.0f : phrase);
            mixl[j] += x * gl;
            mixr[j] += x * gr;
            rsend[j] += x * 0.5f * (gl + gr) * 0.06f;
        }
        if (v->amp * (stab ? 1.0f : phrase) > 0.2f)
            hot = (uint16_t)(hot | (1u << id));
        for (k = 0; k < 4; k++)
            v->s[k] = fm_flush(v->s[k]);
        v->env = fm_flush(v->env);
        v->amp = fm_flush(v->amp);
    }
    lay_alive = alive;
    en_layer_hot = hot;
}

/* A piece change dips these four, then brings the new levels up. The drums are outside it.
 * At zero the oscillators early-out, so the new piece does not start on top of the old one. */
static void shed_tick(uint32_t n)
{
    if (shed || shed_g < 1.0f) {
        const float step = (float)n / (0.020f * EN_SR);
        float tgt = shed ? 0.0f : 1.0f;
        shed_g = tgt > shed_g ? fm_minf(shed_g + step, 1.0f) : fm_maxf(shed_g - step, 0.0f);
        pad_vol *= shed_g;
        bass_vol *= shed_g;
        acid_vol *= shed_g;
        noise_vol *= shed_g;
    }
    en_shed_done = (uint8_t)(shed && shed_g <= 0.0f);
}

/* Three sines at boot, one after another: G, C, E. A gap between them.
 * Half the level of the first quiet pass, so it sits further under the song.
 * Nothing else, and the song is left alone. The host build does not hear it,
 * so a seed's peak stays the song. Not in ram_text: flash-busy code only. */
#ifndef EN_HOST
static const float JAM_HZ[3] = {392.00f, 523.25f, 659.25f};
static const float JAM_ON[3] = {0.00f, 0.32f, 0.64f};
static const float JAM_OFF[3] = {0.20f, 0.52f, 0.96f};
static struct {
    uint32_t left, t;
    float ph;
    int8_t note;
} jam;

static float jam_sample(void) __attribute__((noinline));
static float jam_sample(void)
{
    float sec, y;
    int i;
    if (!jam.left)
        return 0.0f;
    sec = (float)jam.t * (1.0f / EN_SR);
    y = 0.0f;
    for (i = 0; i < 3; i++) {
        if (sec >= JAM_ON[i] && sec < JAM_OFF[i]) {
            float u = sec - JAM_ON[i];
            float tail = JAM_OFF[i] - sec;
            float env = 1.0f;
            if (jam.note != (int8_t)i) {
                jam.ph = 0.0f;
                jam.note = (int8_t)i;
            }
            if (u < 0.012f)
                env = u * (1.0f / 0.012f);
            if (tail < 0.028f)
                env *= tail * (1.0f / 0.028f);
            jam.ph += JAM_HZ[i] * (1.0f / EN_SR);
            if (jam.ph >= 1.0f)
                jam.ph -= 1.0f;
            y = fm_sin_turns(jam.ph) * env * 0.025f;
            break;
        }
    }
    jam.t++;
    if (--jam.left == 0)
        jam.ph = 0.0f;
    return y;
}
#endif

void en_jam(void)
{
#ifndef EN_HOST
    jam.left = 0; /* the render must not start on a half-written note */
    jam.t = 0;
    jam.ph = 0.0f;
    jam.note = -1;
    jam.left = (uint32_t)(1.05f * EN_SR);
#endif
}

static void render_chunk(int32_t *out, uint32_t n, float gain)
{
    uint32_t j;
    const float rg = rev_send * rev_in_gain * (1.0f - 0.25f * arcm) * (1.0f + 0.7f * fm_maxf(brk, 0.0f)), eg = echo_send;
    float dk[BLK], de = duck_env, gg = gate_g;
    const float depth = fm_minf(0.55f * KK->duck, 0.85f) * fm_minf(pulse_vol * 1.5f, 1.0f) * trk_g1[TR_KICK];   /* a muted kick ducks nothing */
    const int gate = fx_on >> FX_GATE16 & 1u ? 1 : fx_on >> FX_GATE8 & 1u ? 2 : 0;
    const int odd = (int)((sixteenth - 1u) & 1u);         /* the sixteenth playing now: the second of an eighth? */
    float shed_pad = pad_vol, shed_bass = bass_vol, shed_acid = acid_vol, shed_nz = noise_vol;
    for (j = 0; j < n; j++)
        mixl[j] = mixr[j] = rsend[j] = esend[j] = 0.0f;
    shed_tick(n);
    render_pad(n);
    render_bass(n);
    render_acid(n);
    render_layers(n);
    render_noise(n);
    pad_vol = shed_pad;
    bass_vol = shed_bass;
    acid_vol = shed_acid;
    noise_vol = shed_nz;
    for (j = 0; j < n; j++) {                   /* the sidechain: the kick pushes the rest down, briefly */
        float gt = 1.0f;
        if (gate == 1)                          /* the gate: the first half of each sixteenth */
            gt = (float)(step_t + (int32_t)j) < 0.5f * step_len ? 1.0f : 0.0f;
        else if (gate == 2)                     /* or of each eighth */
            gt = odd ? 0.0f : 1.0f;
        gg += (gt - gg) * 0.04f;
        dk[j] = 1.0f - depth * de;
        de *= duck_dec;
        mixl[j] *= dk[j] * gg;
        mixr[j] *= dk[j] * gg;
        rsend[j] *= gg;
        esend[j] *= gg;
        {   /* the noise: its own pump, from none to deep. The shape sets how much room and delay it gets. */
            float pk = (1.0f - nz_pump * 0.95f * de * trk_g1[TR_KICK]) * wob_wag(3);
            float ns = (nzl[j] + nzr[j]) * pk;
            mixl[j] += nzl[j] * pk;
            mixr[j] += nzr[j] * pk;
            rsend[j] += ns * nz_rev;
            esend[j] += ns * 0.5f * nz_echo;
        }
    }
    gate_g = gg;
    if (KK->crush > 0.0f) {                      /* breakcore: the drums crushed (what they add, held and stepped) */
        static float hl, hr;
        static uint32_t hc;
        const uint32_t hold = 2u + (uint32_t)(KK->crush * 4.0f);
        const float c = KK->crush, q = 24.0f;
        static float bl[BLK], br[BLK];      /* static: the audio interrupt's stack is small (and this never re-enters) */
        for (j = 0; j < n; j++) {
            bl[j] = mixl[j];
            br[j] = mixr[j];
        }
        render_drums(n);
        for (j = 0; j < n; j++) {
            float dl = mixl[j] - bl[j], dr = mixr[j] - br[j];
            if (++hc >= hold) {
                hc = 0;
                hl = (float)(int32_t)(dl * q + (dl < 0.0f ? -0.5f : 0.5f)) * (1.0f / q);
                hr = (float)(int32_t)(dr * q + (dr < 0.0f ? -0.5f : 0.5f)) * (1.0f / q);
            }
            mixl[j] = bl[j] + dl + (hl - dl) * c;
            mixr[j] = br[j] + dr + (hr - dr) * c;
        }
    } else {
        render_drums(n);                        /* (the kick sets duck_env for the next chunk) */
    }
    duck_env = fm_flush(de);
    for (j = 0; j < n; j++) {
        float wl, wr, l, r, el, er, xl, xr;
        if (echo_amt || echo_sm > 0.0f) {        /* the open eases in. Shut, this block never runs */
            float et = echo_amt ? 0.28f + 0.72f * ((float)echo_amt * 0.01f) : 0.0f;
            echo_sm += (et - echo_sm) * 0.0015f;
            if (echo_sm < 0.0005f && et == 0.0f) {
                echo_sm = 0.0f;
                echo_time();                     /* the tape length glides back to the song */
            }
        }
        if (echo_sm == 0.0f && (verb_amt || verb_sm > 0.0f || shim_amt || shim_sm > 0.0f)) {
            float vt = verb_amt ? 0.28f + 0.72f * ((float)verb_amt * 0.01f) : 0.0f;
            float st = shim_amt ? 0.28f + 0.72f * ((float)shim_amt * 0.01f) : 0.0f;
            verb_sm += (vt - verb_sm) * 0.0012f;
            shim_sm += (st - shim_sm) * 0.0012f;
            if (verb_sm < 0.0005f && vt == 0.0f)
                verb_sm = 0.0f;
            if (shim_sm < 0.0005f && st == 0.0f) {
                if (shim_sm != 0.0f)
                    shimmer_clear();             /* one clear, as it shuts. Not every sample after */
                shim_sm = 0.0f;
            }
        }
        /* Shut: the song's send alone, and the same return. Open: the mix joins the tape,
         * and the repeats come forward. The plate and the shimmer wait until that tape is shut. */
        if (echo_sm > 0.0f) {
            float em = echo_sm;
            echoes(esend[j] * eg + throw_amt * 0.5f * (mixl[j] + mixr[j]) + em * 0.50f * (mixl[j] + mixr[j]), &el, &er, &xl, &xr);
            reverb((rsend[j] * rg + (el + er + xl + xr) * 0.35f * rev_send) * 0.8f, &wl, &wr);
            l = (mixl[j] + (0.75f * el + 0.15f * xl + em * (0.45f * el + 0.40f * xl) + 1.3f * wl) * dk[j]) * 0.36f;
            r = (mixr[j] + (0.75f * er + 0.15f * xr + em * (0.45f * er + 0.40f * xr) + 1.3f * wr) * dk[j]) * 0.36f;
        } else if (verb_sm > 0.0f || shim_sm > 0.0f) {
            float up = 0.0f, vin;
            echoes(esend[j] * eg + throw_amt * 0.5f * (mixl[j] + mixr[j]), &el, &er, &xl, &xr);
            vin = (rsend[j] * rg + (el + er + xl + xr) * 0.35f * rev_send) * 0.8f
                + verb_sm * 0.34f * (mixl[j] + mixr[j]);
            if (shim_sm > 0.0f) {
                float x = 0.5f * (mixl[j] + mixr[j]) + sh_tail;
                sh_lp += (x - sh_lp) * 0.08f;    /* the kick stays down. The highs go up */
                x -= sh_lp;
                up = shimmer_up(x);
                vin += shim_sm * 0.80f * up;
            }
            reverb(vin, &wl, &wr);
            if (shim_sm > 0.0f)
                sh_tail = shim_sm * 0.22f * 0.5f * (wl + wr);
            l = (mixl[j] + (0.75f * el + 0.15f * xl + 1.3f * wl + verb_sm * 0.70f * wl + shim_sm * 1.8f * up) * dk[j]) * 0.36f;
            r = (mixr[j] + (0.75f * er + 0.15f * xr + 1.3f * wr + verb_sm * 0.70f * wr + shim_sm * 1.8f * up) * dk[j]) * 0.36f;
        } else {
            echoes(esend[j] * eg + throw_amt * 0.5f * (mixl[j] + mixr[j]), &el, &er, &xl, &xr);
            reverb((rsend[j] * rg + (el + er + xl + xr) * 0.35f * rev_send) * 0.8f, &wl, &wr);
            l = (mixl[j] + (0.75f * el + 0.15f * xl + 1.3f * wl) * dk[j]) * 0.36f;
            r = (mixr[j] + (0.75f * er + 0.15f * xr + 1.3f * wr) * dk[j]) * 0.36f;
        }
#ifndef EN_HOST
        if (jam.left) { /* three notes, half the earlier level, then nothing */
            float v = jam_sample();
            l += v;
            r += v;
        }
#endif
        fx_master(&l, &r);
        out[2 * j] = (int32_t)(fm_tanhf(l * gain) * 8300000.0f);
        out[2 * j + 1] = (int32_t)(fm_tanhf(r * gain) * 8300000.0f);
    }
    step_t += (int32_t)n;
}

/* the arc: a slow tide, minutes long, between sparse (-1) and full (+1); Motion sets how far it goes */
static float arc, arc_tgt;
static uint32_t arc_left;

/* The song's echo keeps its send. This opens the same tape: more of the mix,
 * longer repeats, and past the middle a longer loop. Zero never touches that path.
 * The song screen leaves it shut. */
void en_echo(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    echo_amt = (uint8_t)amt;
}

/* The song's plate keeps its send. This puts more of the mix in, and the
 * hall runs longer while it is open. Zero never touches that path. The song
 * screen does not open it. */
void en_verb(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    verb_amt = (uint8_t)amt;
}

/* An octave of the mix, in the same plate. Zero never writes the grains.
 * The song screen does not open it. */
void en_shimmer(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    shim_amt = (uint8_t)amt;
}

/* White keys in GLO. 100 is the voice. Under that, only the key is quieter. */
void en_keys(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    keys_amt = amt >= 100 ? 1.0f : (float)amt * 0.01f;
}

/* Knob 1. The mix times a sine. Further up, the sine is faster and the dry
 * song steps back. Zero leaves the master, and does not write the plate. */
void en_ring(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    kn1_amt = (uint8_t)amt;
}

/* Knob 2. A resonant band through the mix, rocking twice a beat. The peak
 * grits as it opens. Zero leaves the master. Not the wah on a black key. */
void en_wah(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    kn2_amt = (uint8_t)amt;
}

/* Knob 3. Four allpasses, swept, mixed back with the song. Zero leaves the master. */
void en_phase(int amt)
{
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    phase_amt = (uint8_t)amt;
}

void en_wobble(int part, int amt)
{
    if (part < 0 || part > 3)
        return;
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    wob_amt[part] = (uint8_t)amt;
}

void en_snap(int drum, int amt)
{
    if (drum < 0 || drum > 10)
        return;
    if (amt < 0)
        amt = 0;
    if (amt > 100)
        amt = 100;
    snap_amt[drum] = (uint8_t)amt;
}

static void echo_follow(void)
{
    float open = echo_sm;
    if (!echo_amt && open == 0.0f)
        return;
    if (open <= 0.0f)
        return;
    if (echo_fbx < 0.90f)
        echo_fbx += (0.90f - echo_fbx) * open;   /* the repeats stay, and stack as it opens */
    if (par[P_TEMPO] > 0 && open >= 0.45f) {
        float beats = open >= 0.72f ? 4.0f : 2.0f;
        float t = beats * (60.0f / (float)par[P_TEMPO]) * TAPE_SR;
        float cap = (float)(TAPE_N - 8);
        if (t > cap)
            t = cap;
        if (t > echo_len_t)
            echo_len_t = t;                      /* longer than the song's echo, never shorter */
    } else
        echo_time();                             /* back toward the length the song baked */
}

static void control(uint32_t n)                 /* once a chunk: the slow things */
{
    int i;
    mods_step(n);
    heads_step(n);
    fx_control(n);
    echo_follow();
    if (arc_left <= n) {
        float r = frand() * 2.0f - 1.0f;
        arc_tgt = r * (0.6f + 0.4f * frand()) + (r > 0.0f ? 0.25f : -0.25f);   /* away from the middle */
        arc_left = (uint32_t)((50.0f + 100.0f * frand()) * EN_SR);
    } else {
        arc_left -= n;
    }
    if (en_playing)
        arc += (fm_clampf(arc_tgt, -1.0f, 1.0f) - arc) * (float)n * (1.0f / (35.0f * EN_SR));
    arcm = arc * motion;
    en_arc = arcm;
    {   /* the sections, smoothed: a breakdown falls in over ~2 s, a build rises through its four bars */
        float bt = sec == SEC_BREAK ? 1.0f : sec == SEC_BUILD ? -0.6f : 0.0f;
        brk += (bt - brk) * fm_minf((float)n / ((sec == SEC_BUILD ? 4.0f : 2.0f) * EN_SR), 1.0f);
    }
    pl_decay += ((en_frozen ? 0.9995f : pl_decay_t) - pl_decay) * 0.002f;
    if (verb_sm > 0.0f || shim_sm > 0.0f) {      /* longer while a knob holds it. Never written back */
        float open = verb_sm > shim_sm ? verb_sm : shim_sm;
        float t = pl_decay_t + 0.18f * open;
        if (t > 0.985f)
            t = 0.985f;
        if (pl_decay < t)
            pl_decay += (t - pl_decay) * 0.004f;
    }
    pl_dd2 = fm_clampf(pl_decay + 0.15f, 0.25f, 0.5f);
    rev_in_gain += ((en_frozen ? 0.0f : 1.0f) - rev_in_gain) * 0.01f;
    if (en_playing && !en_frozen && !change_pending) {
        drift_left -= (float)n;
        if (drift_left <= 0.0f)
            change_pending = 1;
    }
    en_drift = drift_total > 0.0f ? 1.0f - fm_clampf(drift_left / drift_total, 0.0f, 1.0f) : 0.0f;
    en_acid_glow *= 0.93f;
    {   /* the links' envelopes: the amount sets how far and how long (kick 40 .. 160 ms, snare 80 .. 280 ms) */
        float ak = (float)par[P_LK_KICK] * 0.01f, as = (float)par[P_LK_SNARE] * 0.01f;
        lk_kick *= fm_expf(-(float)n / ((0.04f + 0.12f * ak) * EN_SR));
        lk_snare *= fm_expf(-(float)n / ((0.08f + 0.2f * as) * EN_SR));
    }
    if (!links_on())
        lk_kick = lk_snare = 0.0f;
    for (i = 0; i < EN_NDRUM; i++)
        en_drum_glow[i] *= 0.95f;
    for (i = 0; i < EN_XDRUM; i++)
        en_xdrum_glow[i] *= 0.95f;
    {   /* the LFOs, and where the pad's goes */
        float d = (float)par[P_PL_DEPTH] * 0.01f;
        lfo_step(&pad_lfo, par[P_PL_RATE], par[P_PL_SHAPE], (float)n, step_len);
        lfo_step(&drone_lfo, par[P_DL_RATE], par[P_DL_SHAPE], (float)n, step_len);
        {   /* knob 2: one cycle an eighth note, so a turn is there on the next hit */
            float sp = step_len > 8.0f ? step_len * 2.0f : 16.0f;
            wob_ph += (float)n / sp;
            if (wob_ph >= 1.0f)
                wob_ph -= (float)(int)wob_ph;
            wob_s = fm_sinf(FM_TWO_PI * wob_ph);
        }
        pad_filt = par[P_PL_DEST] == 0 ? d * pad_lfo.v : 0.0f;
        pad_lvl = par[P_PL_DEST] == 1 ? 1.0f - 0.85f * d * (0.5f - 0.5f * pad_lfo.v) : 1.0f;
        pad_pan = par[P_PL_DEST] == 2 ? 0.9f * d * pad_lfo.v : 0.0f;
        pad_pitch = par[P_PL_DEST] == 3 ? 0.5f * d * pad_lfo.v : 0.0f;
    }
    {   /* the drone's envelope: attack (a line), then decay */
        if (bass.env_up) {
            bass.env += (float)n / (de_atk_s(par[P_DE_ATK]) * EN_SR);
            if (bass.env >= 1.0f) {
                bass.env = 1.0f;
                bass.env_up = 0;
            }
        } else {
            bass.env *= fm_expf(-6.9f * (float)n / (de_dec_s(par[P_DE_DEC]) * EN_SR));
        }
        en_bass_env = bass.env;
    }
}

void en_render(int32_t *out, uint32_t n, uint32_t gain_q12)
{
    float gain;
    int32_t *const out0 = out;
    const uint32_t n0 = n;
    drain();
    if (hush && hush_g <= 0.0f) {               /* hushed: the music holds still (the song resumes where it was) */
        for (n = 0; n < 2 * n0; n++)
            out0[n] = 0;
        en_hushed = 1;
        return;
    }
    gain = muted ? 0.0f : (float)gain_q12 * (1.0f / 4096.0f) * 2.0f;
    if (en_ext && ++ext_age > 100u)
        en_ext = 0;
    while (n) {
        uint32_t k = n > BLK ? BLK : n;
        int timed = en_playing && !en_ext;
        if (timed) {
            if (pulse_left <= 0.0f) {
                clock_pulse();
                pulse_left += step_len * (1.0f / 6.0f);
            }
            if ((float)k > pulse_left)
                k = (uint32_t)pulse_left + 1u;
        }
        if (k > n)
            k = n;
        {
            int32_t w = drums_wait();            /* a swung hit lands on its sample */
            if (w > 0 && (uint32_t)w < k)
                k = (uint32_t)w;
        }
        if (timed)
            pulse_left -= (float)k;
        control(k);
        render_chunk(out, k, gain);
        drums_wait_step((int32_t)k);
        out += 2 * k;
        n -= k;
    }
    if (hush || hush_g < 1.0f) {                /* the hush: ~6 ms each way */
        const float step = 1.0f / (0.006f * EN_SR), tgt = hush ? 0.0f : 1.0f;
        uint32_t j;
        for (j = 0; j < n0; j++) {
            hush_g = tgt > hush_g ? fm_minf(hush_g + step, 1.0f) : fm_maxf(hush_g - step, 0.0f);
            out0[2 * j] = (int32_t)((float)out0[2 * j] * hush_g);
            out0[2 * j + 1] = (int32_t)((float)out0[2 * j + 1] * hush_g);
        }
    }
    en_hushed = (uint8_t)(hush && hush_g <= 0.0f);
}
