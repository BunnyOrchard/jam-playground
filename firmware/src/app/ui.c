/* SPDX-License-Identifier: GPL-3.0-only */
/* HYP-1 UI: the panel and the screen. Part of the unity build (after gfx.c and project.c); the host
 * simulator includes it too.
 *
 *   SELECT         the pages, in order (with HOME held: the tempo)
 *   HOME           Tempo (tempo, evolve, motion, drift)
 *   ENV            Pad, then Pad env, Pad LFO (each press the next)
 *   LFO            Drone, then Drone env, Drone LFO
 *   ARP            Acid, then Acid env, Acid line. In IAM's GLO a tap turns the arpeggiator on or off
 *   SEQ            Drums, then Links; held, the sequencer (six lanes, sixteen steps); with SEL held, the
 *                  links (the drums into the voices) on and off
 *   GLO            Global (root, theme, tune, MIDI out); Noise and Space are on the SELECT dial only.
 *                  On Global, SAVE stores the piece as it is (patterns and all); held, the piece goes
 *                  back to its factory settings and out of the store
 *   KNOB 1-4       the page's four values (in the sequencer: the lane's level, tune, decay; the pattern)
 *   FX             the effects menu, on and off: while it is on, each key is an effect while held.
 *                  SAVE held is lock mode: each key tapped locks its effect on (or, locked, off again),
 *                  one at a time or together; keys held as SAVE goes down lock too. A locked effect's
 *                  key tapped (no SAVE) turns that one off; closing FX lets them all go. SAVE tapped
 *                  alone is still the breakdown
 *   SAVE           the breakdown on the next bar; again, a four-bar build and the drop; held, the drop
 *                  (on Global: the piece stored, above)
 *   OCT- / OCT+    the octave (3 down .. 3 up) of the voice whose page it is (pad, drone, acid); on any
 *                  other page, the whole song's; in the sequencer, the lane
 *   keys           the scale (each adds or removes its note); in the sequencer, the sixteen white keys
 *                  (the lower row) are the lane's steps: lit where it hits, tapped to turn a hit on or off
 *   SEL            shift, held: white keys 1-6 the pattern (on the bar); PRESETS the piece, on the next
 *                  bar; ALGORITHM the sound of the voice on screen (pad, drone, acid; on the drums' pages the kit); in the sequencer, black keys 1-6 copy the pattern there, OCT- clears the drum's
 *                  lane, OCT+ puts back the piece's own
 *   PLAY           play / stop         REC  FREEZE (in IAM's GLO: loop the white keys)  EDIT  the next chord
 *   ALGORITHM      the mode            PRESETS  the piece (01 .. 10)
 *
 * Each piece has its own settings and six drum patterns. Power-on starts on a piece at random, as it was
 * stored, or its factory settings nudged a little if it never was (project_start). A store is written
 * at once (store_now: playing, the music hushes around the write); the theme, Tune and MIDI out two
 * seconds after a change. */
#include "../dsp/fastmath.h"

/* --------------------------------------------------------------- pages --- */
enum { V_TEMPO, V_PAD, V_PADENV, V_PADLFO, V_CHORDS, V_NOISE, V_DRONE, V_DRENV, V_DRLFO, V_ACID, V_ACENV, V_ACLINE,
       V_DRUMS, V_LINKS, V_KIT, V_DDRIFT, V_SPACE, V_GLOBAL, NDIAL, V_SEQ = NDIAL, NVIEWS };
static const char *const VIEW_NAME[NVIEWS] = {"Tempo", "Pad", "Pad env", "Pad LFO", "Chords", "Noise", "Drone", "Drone env",
                                              "Drone LFO", "Acid", "Acid env", "Acid line", "Drums", "Links", "Kit", "Drum drift", "Space", "Global",
                                              "Sequencer"};
/* the knobs of each page: a parameter, or one of these (the sequencer's: the lane's) */
enum { K_LVL = 200, K_TUNE, K_DEC, K_KEYS, K_NONE };
static const uint8_t VIEW_KNOB[NVIEWS][4] = {
    {P_TEMPO, P_EVOLVE, P_MOTION, P_DRIFT},
    {P_PAD, P_PADTONE, P_BRIGHT, P_DETUNE},
    {P_PE_ATK, P_PE_HOLD, P_PE_REL, P_PE_REST},
    {P_PL_RATE, P_PL_DEPTH, P_PL_SHAPE, P_PL_DEST},
    {P_CHORD, P_VOICING, P_DRIFT, P_ROOT},         /* the harmony together (Drift is also on Tempo, Root on Global) */
    {P_NOISE, P_NZ_COLOR, P_NZ_TONE, P_NZ_PUMP},
    {P_BASS, P_DR_INST, P_DR_CUT, P_DR_RES},
    {P_DE_TRIG, P_DE_ATK, P_DE_DEC, P_DE_AMT},
    {P_DL_RATE, P_DL_DEPTH, P_DL_SHAPE, P_DL_DEST},
    {P_ACID, P_AC_WAVE, P_AC_CUT, P_AC_RES},
    {P_AC_DEC, P_AC_ACC, P_AC_SLIDE, P_AC_ENV},
    {P_AC_DENS, P_AC_LEN, P_AC_RANGE, P_AC_MUT},
    {P_PULSE, P_DRIVE, P_SWING, P_PATTERN},
    {P_LK_KICK, P_LK_SNARE, P_LK_HAT, P_LK_PERC},
    {P_KIT_KICK, P_KIT_SNARE, P_KIT_HAT, P_KIT_PERC},
    {P_DD_CHANGE, P_DD_EVERY, P_DD_POOL, P_DD_MUTATE},
    {P_ECHO, P_ETIME, P_REVERB, P_SIZE},
    {P_ROOT, P_THEME, P_TUNE, P_MIDI},
    {K_LVL, K_TUNE, K_DEC, P_PATTERN},
};
static const char *const KNOB_LABEL[NVIEWS][4] = {
    {"Tempo", "Evolve", "Motion", "Drift"},
    {"Level", "Sound", "Bright", "Detune"},
    {"Attack", "Hold", "Release", "Rest"},
    {"Rate", "Depth", "Shape", "Target"},
    {"Chord", "Voicing", "Drift", "Root"},
    {"Level", "Colour", "Tone", "Pump"},
    {"Level", "Sound", "Cutoff", "Reso"},
    {"Trigger", "Attack", "Decay", "Amount"},
    {"Rate", "Depth", "Shape", "Target"},
    {"Level", "Sound", "Cutoff", "Reso"},
    {"Decay", "Accent", "Slide", "Env mod"},
    {"Density", "Length", "Range", "Mutate"},
    {"Level", "Drive", "Swing", "Pattern"},
    {"K>Drone", "S>Pad", "H>Acid", "P>Clock"},
    {"Kick", "Snare", "Hats", "Perc"},
    {"Change", "Every", "Pool", "Mutate"},
    {"Echo", "Echo time", "Reverb", "Size"},
    {"Root", "Theme", "Tune", "MIDI out"},
    {"Level", "Tune", "Decay", "Pattern"},
};
/* the buttons that lead to pages: each to a group, pressed again the next page in it */
static const struct {
    uint8_t btn, first, n;
} GROUP[] = {
    {B_HOME, V_TEMPO, 1}, {B_ENV, V_PAD, 4}, {B_LFO, V_DRONE, 3}, {B_ARP, V_ACID, 3}, {B_SEQ, V_DRUMS, 4}, {B_GLO, V_GLOBAL, 1},
};
#define NGROUP (int)(sizeof GROUP / sizeof GROUP[0])

static const char *const LANE_SHORT[EN_NLANE] = {"KICK", "SNARE", "CLAP", "HAT", "OPEN", "PERC"};

/* the FX menu (FX on): each key an effect while it is held. White keys: the beat repeats, reverse,
 * tape stop, the mutes, the filters; black keys: the gates, the throw, the crusher, the fill, the solos
 * (each solo the black key beside its track's mute). */
static const uint8_t KEY_FX[NKEYS] = {
    FX_RPT2, FX_GATE16, FX_RPT4, FX_GATE8, FX_RPT6, FX_THROW, FX_RPT8,                /* F3 F#3 G3 G#3 A3 A#3 B3 */
    FX_RPT16, FX_CRUSH, FX_RPT32, FX_FILL, FX_REV,                                    /* C4 C#4 D4 D#4 E4 */
    FX_STOP, FX_SOLO0, FX_MUTE0, FX_SOLO1, FX_MUTE1, FX_SOLO2, FX_MUTE2,              /* F4 F#4 G4 G#4 A4 A#4 B4 */
    FX_MUTE3, FX_SOLO3, FX_MUTE4, FX_SOLO4, FX_MUTE5,                                 /* C5 C#5 D5 D#5 E5 */
    FX_LP, FX_SOLO5, FX_HP};                                                          /* F5 F#5 G5 */
static const char *const FX_LABEL[EN_NFX] = {      /* M: mute, S: solo (the menu's title says so) */
    "RPT 1/2", "RPT 1/4", "RPT 1/6", "RPT 1/8", "RPT1/16", "RPT1/32", "REVERSE", "TAPESTP",
    "M:KICK", "M:HATS", "M:SNARE", "M:DRONE", "M:ACID", "M:PAD", "LOWPASS", "HIPASS",
    "GATE 16", "GATE 8", "THROW", "CRUSH", "FILL",
    "S:KICK", "S:HATS", "S:SNARE", "S:DRONE", "S:ACID", "S:PAD", "FLANGE", "FLANGER"};
/* GLO's FX page swaps fourteen of those keys. HYP, and GLO with the page closed, keep KEY_FX. */
static const char *const DJ_LABEL[14] = {
    "SWEEP", "WOW", "RING", "TRANS", "PHASER", "WAH", "RADIO", "FLANGE",
    "PAN", "VOWEL", "FILTER", "CHORUS", "BEND", "CHOP"};
/* the white keys, in order: the sequencer's steps; the black keys, in order */
static const uint8_t WHITE_KEY[16] = {0, 2, 4, 6, 7, 9, 11, 12, 14, 16, 18, 19, 21, 23, 24, 26};
static const uint8_t BLACK_KEY[11] = {1, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25};
#define SEQ_HOLD 400u                  /* SEQ held this long: the sequencer */
#define DROP_HOLD 500u                 /* SAVE held this long: the drop, on the next bar */
#define HELP_HOLD 900u
#define KIT_QUIET 3000u                /* anything left unwritten: this long after a change, while stopped */
#define GLOB_QUIET 2000u               /* the theme, Tune, MIDI out: written this long after a change */
#define FACTORY_HOLD 1500u             /* SAVE held this long on Global: the piece to its factory settings */

/* ---------------------------------------------------------------- themes --- */
/* five looks for the screen (Global > Theme): a ground, its panel and lines, the text, an accent for what is
 * happening now, a primary for the music in three shades (the pad and the chord; the acid; the drone),
 * the drums' colour, and a colour for the sections and freeze. Each layer has a dark tint for shapes at rest. */
typedef struct {
    uint16_t bg, panel, line, dim, text, acc, pad, pad_t, acid, acid_t, bass, bass_t, pulse, pulse_t, frost;
} theme_t;
static const theme_t THEMES[EN_NTHEME] = {
    /* Synth: charcoal, a cyberpunk purple, orange */
    {RGB(22, 21, 27), RGB(40, 36, 50), RGB(62, 56, 76), RGB(142, 134, 162), RGB(238, 234, 246), RGB(255, 128, 32),
     RGB(164, 76, 255), RGB(68, 40, 104), RGB(214, 128, 255), RGB(78, 50, 104), RGB(118, 70, 214), RGB(52, 36, 88),
     RGB(206, 196, 226), RGB(66, 60, 82), RGB(255, 150, 64)},
    /* Acid: black, a 303's yellow-green, hot pink */
    {RGB(12, 13, 10), RGB(30, 34, 24), RGB(52, 60, 40), RGB(140, 152, 118), RGB(236, 242, 222), RGB(255, 60, 160),
     RGB(196, 255, 40), RGB(54, 72, 16), RGB(240, 255, 120), RGB(70, 80, 30), RGB(120, 200, 30), RGB(40, 62, 14),
     RGB(210, 220, 190), RGB(56, 62, 46), RGB(255, 90, 180)},
    /* Ice: deep navy, cyan, a cold pink */
    {RGB(10, 16, 28), RGB(22, 34, 54), RGB(38, 56, 84), RGB(120, 150, 186), RGB(226, 240, 255), RGB(255, 84, 130),
     RGB(0, 200, 255), RGB(10, 56, 84), RGB(120, 228, 255), RGB(26, 70, 96), RGB(40, 130, 230), RGB(16, 44, 82),
     RGB(196, 216, 236), RGB(48, 64, 86), RGB(255, 120, 160)},
    /* Ember: black-brown, red, amber */
    {RGB(20, 12, 10), RGB(40, 24, 20), RGB(70, 40, 34), RGB(170, 130, 116), RGB(250, 232, 220), RGB(255, 176, 32),
     RGB(255, 46, 66), RGB(92, 20, 26), RGB(255, 110, 90), RGB(96, 38, 32), RGB(200, 40, 40), RGB(72, 18, 18),
     RGB(230, 200, 186), RGB(82, 56, 50), RGB(255, 200, 80)},
    /* Phosphor: a green screen's black, its green, a brighter green */
    {RGB(6, 14, 8), RGB(14, 32, 18), RGB(26, 58, 32), RGB(80, 160, 96), RGB(170, 255, 190), RGB(220, 255, 120),
     RGB(60, 255, 120), RGB(14, 70, 30), RGB(150, 255, 170), RGB(24, 76, 40), RGB(30, 190, 90), RGB(10, 56, 26),
     RGB(120, 220, 140), RGB(20, 52, 28), RGB(240, 255, 140)},
};
static const theme_t *th = &THEMES[0];
#define K_BG (th->bg)
#define K_PANEL (th->panel)
#define K_LINE (th->line)
#define K_DIM (th->dim)
#define K_TEXT (th->text)
#define K_ACC (th->acc)
#define K_PAD (th->pad)
#define K_PAD_T (th->pad_t)
#define K_ACID (th->acid)
#define K_ACID_T (th->acid_t)
#define K_BASS (th->bass)
#define K_BASS_T (th->bass_t)
#define K_PULSE (th->pulse)
#define K_PULSE_T (th->pulse_t)
#define K_FROST (th->frost)

uint32_t ui_splash_ms = 3200u;           /* the boot cards: RedtailWorks, then HYP-1 (the host's tests set 0) */

static struct {
    uint8_t view, last_dial;
    uint32_t btn, keys, btn_used;
    uint32_t enc_t[NE];
    uint8_t enc_run[NE];               /* detents in the current quick run, per knob (accel) */
    uint32_t down_t[NB];               /* when each button went down */
    uint32_t turn_t;                   /* the last knob turn */
    uint8_t help;                      /* a button's card is up: the button + 1 */
    uint8_t save_held;                 /* SAVE held long enough: the drop was asked for this hold */
    uint8_t seq_held;                  /* SEQ held long enough: the sequencer opened this hold */
    uint8_t fx_menu;                   /* the FX menu is on: the keys are effects */
    uint32_t fx_mask;                  /* the effects held (and locked), as last sent */
    uint32_t fx_lock;                  /* the effects locked on (lock mode: SAVE held in FX) */
    uint16_t dj_mask;                  /* GLO crowd keys held (and locked), as last sent */
    uint16_t dj_lock;                  /* crowd keys locked on. Kept when GLO closes, so they come back */
    uint8_t lock_mode;                 /* SAVE held in the FX menu: keys tapped lock and unlock */
    uint32_t lock_keys;                /* keys pressed in lock mode: not effects while they're held */
    uint8_t lane;                      /* the sequencer's lane (OCT- / OCT+) */
    uint8_t save_global;               /* SAVE went down on Global: the piece is stored when it comes up */
    uint8_t store_wait;                /* a store while playing: the music fading out to write it (store_tick) */
    uint32_t store_t;
    char store_msg[12];
    uint8_t factory_done;              /* SAVE held on Global: the piece went back to its factory settings */
    uint32_t shift_keys;               /* keys pressed while SEL (shift) was held: not notes, not effects */
    uint8_t piece_q;                   /* a piece waiting for the bar (SEL + PRESETS): its number + 1 */
    uint8_t shift_hint;                /* SEL held alone: its hint shown this hold */
    uint8_t key_voice[NKEYS];          /* a key playing a voice: its EN_V_* + 1 (the note-off goes there) */
    uint8_t splash, theme;             /* the boot cards are up (2: the HYP-1 card); the theme drawn */
    uint8_t splash_step;               /* the HYP-1 card's steps lit */
    uint32_t boot_t, act_t;
    char msg[2][24];
    uint32_t msg_until;
    int8_t touched;                    /* the knob turned last (0..3), shown bright for a moment */
    uint32_t touch_until;
    uint32_t frame;
    uint32_t sig[3];                   /* what each band shows: redrawn when it changes */
} ui;

static void say(const char *a, const char *b)
{
    uint32_t i;
    for (i = 0; i < 23u && a && a[i]; i++)
        ui.msg[0][i] = a[i];
    ui.msg[0][i] = 0;
    for (i = 0; i < 23u && b && b[i]; i++)
        ui.msg[1][i] = b[i];
    ui.msg[1][i] = 0;
    ui.msg_until = plat_ms() + 1300u;
}
void ui_say(const char *a, const char *b) { say(a, b); }

static void itoa_u(uint32_t v, char *b)
{
    char t[12];
    int n = 0;
    do {
        t[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v);
    while (n)
        *b++ = t[--n];
    *b = 0;
}

int ui_dirty(void) { return project_pending(); }

static void edited(void) { proj.edited |= (uint16_t)(1u << proj.piece); }   /* the piece changed */

static int pool_count(uint32_t p)
{
    int n = 0, i;
    for (i = 0; i < 12; i++)
        n += (int)(p >> i & 1u);
    return n;
}
static uint32_t pool_abs(void)                 /* the pool as absolute pitch classes */
{
    uint32_t p = proj.pool, r = (uint32_t)proj.par[P_ROOT];
    return ((p << r) | (p >> (12u - r))) & 0xFFFu;
}
static const char *mode_name(void)
{
    return proj.pool == EN_MODE_POOL[proj.par[P_MODE]] ? EN_MODE_NAME[proj.par[P_MODE]] : "Custom";
}

/* ------------------------------------------------------------ values --- */
static int iam_kvol_get(void);                  /* machine.c: GLO white keys, 0..100 */
static void iam_kvol_set(int v);
static int knob_par(int k)                     /* a knob's parameter (the sequencer's: its lane's) */
{
    if (k == K_KEYS)
        return -1;
    if (k == K_LVL)
        return P_LANE_LVL(ui.lane);
    if (k == K_TUNE)
        return P_LANE_TUNE(ui.lane);
    if (k == K_DEC)
        return P_LANE_DEC(ui.lane);
    return k < P_NPARAMS ? k : -1;
}
static int knob_get(int k)
{
    int p;
    if (k == K_KEYS)
        return iam_kvol_get();
    p = knob_par(k);
    return p >= 0 ? proj.par[p] : 0;
}
static void knob_range(int k, int *lo, int *hi)
{
    int p;
    if (k == K_KEYS) {
        *lo = 0;
        *hi = 100;
        return;
    }
    p = knob_par(k);
    *lo = p >= 0 ? en_param_info(p)->lo : 0;
    *hi = p >= 0 ? en_param_info(p)->hi : 0;
}
static void knob_text(int k, char *b)
{
    int p;
    if (k == K_KEYS) {
        itoa_u((uint32_t)iam_kvol_get(), b);
        return;
    }
    p = knob_par(k);
    if (p >= 0)
        en_param_text(p, proj.par[p], b);
    else
        b[0] = 0;
}

static void knob_set(int k, int v)
{
    int lo, hi, p;
    if (k == K_KEYS) {
        char n[8];
        if (v < 0)
            v = 0;
        if (v > 100)
            v = 100;
        if (v == iam_kvol_get())
            return;
        iam_kvol_set(v);
        if (v <= 0)
            say("KEYS", "OFF");
        else if (v >= 100)
            say("KEYS", "FULL");
        else {
            itoa_u((uint32_t)v, n);
            say("KEYS", n);
        }
        return;
    }
    p = knob_par(k);
    if (p < 0)
        return;
    knob_range(k, &lo, &hi);
    v = v < lo ? lo : v > hi ? hi : v;
    if (v == proj.par[p])
        return;
    proj.par[p] = (int16_t)v;
    en_set(p, v);
    if (en_par_global(p))
        project_globals();                       /* the player's: stored at once */
    else
        edited();
}

/* knob acceleration by speed (X0X's): a few clicks are one step each however quick, so small
 * corrections stay exact; a longer quick turn speeds up to 8 a detent, so half a turn sweeps 0-100.
 * Small ranges never accelerate. */
static uint8_t accel_off;              /* the host simulator's "spin": exact steps */
static int32_t accel(int role, int32_t s, int range)
{
    uint32_t now = plat_ms(), dt = now - ui.enc_t[role], a = (uint32_t)(s < 0 ? -s : s), m;
    ui.enc_t[role] = now;
    if (!a)
        return s;
    if (a > 1)
        dt /= a;                                      /* per detent */
    ui.enc_run[role] = (uint8_t)(dt < 60u ? (ui.enc_run[role] + a > 255u ? 255u : ui.enc_run[role] + a) : a);
    if (range <= 24 || accel_off || ui.enc_run[role] < 4u)
        return s;
    m = dt < 12u ? 8u : dt < 25u ? 5u : dt < 45u ? 3u : 1u;
    if (range < 100 && m > 3u)
        m = 3u;
    return s * (int32_t)m;
}

static void turn(int role, int k, int32_t e)
{
    int lo, hi;
    if (!e || k == K_NONE)
        return;
    knob_range(k, &lo, &hi);
    knob_set(k, knob_get(k) + accel(role, e, hi - lo));
}

/* ------------------------------------------------------------- the pool --- */
static void toggle_note(int pc)                 /* a key: its note in or out of the scale */
{
    int rel = (pc - proj.par[P_ROOT] + 12) % 12;
    uint32_t p = proj.pool ^ (1u << rel);
    if (rel == 0) {
        say("THE ROOT STAYS", "(GLOBAL)");
        return;
    }
    if (pool_count(p) < 3) {
        say("3 NOTES", "AT LEAST");
        return;
    }
    proj.pool = (uint16_t)p;
    en_pool(p);
    edited();
    say(p >> rel & 1u ? "ADDED" : "REMOVED", EN_NOTE_NAME[pc]);
}

static void set_mode(int m)
{
    m = (m % EN_NMODES + EN_NMODES) % EN_NMODES;
    proj.par[P_MODE] = (int16_t)m;
    proj.pool = EN_MODE_POOL[m];
    en_set(P_MODE, m);
    en_pool(proj.pool);
    edited();
    say("MODE", EN_MODE_NAME[m]);
}

static void caps(char *d, const char *s, int n);

static void iam_piece_room(void);            /* machine.c: a piece stays inside four synth keys */
static int iam_song(void);                   /* machine.c: seed 32 and up, the IAM panel */

static uint8_t piece_wait, piece_flush;     /* engines fading: the piece lands when they are down */
static char piece_flush_msg[16];
static void glo_fx_drop(void)                /* latched effects off before the new piece. A key still held stays */
{
    ui.fx_lock = 0;
    ui.dj_lock = 0;
    ui.fx_mask = 0;
    ui.dj_mask = 0;
    ui.lock_mode = 0;
    en_fx(0);
    en_dj(0);
}
static void set_piece(int i)                    /* the piece playing kept as it is; piece i as it was left */
{
    char t[12] = "PIECE ";
    project_piece(i);
    iam_piece_room();                        /* named keys still sounding sit on top of the piece */
    caps(t + 6, piece_name(proj.piece), 4);
    say(t, project_saved(proj.piece) ? "STORED" : "FACTORY");
    if (iam_song() && en_playing) {
        /* The four engines dip first. The new levels come up once they are down,
         * so the old pad, bass, acid, and air are not still rendering. */
        glo_fx_drop();
        piece_flush = 0;
        if (!piece_wait)
            en_shed(1);
        piece_wait = 1;
        return;
    }
    project_apply();
    en_seed(plat_ms());
    en_regrow();
}

/* into flash now. Writing it stops the chip a moment (it runs from the same flash), so while the music
 * plays it fades out (en_hush), holds still while the store is written, and fades back in where it was:
 * a short pause, not a cut. Stopped: at once. */
static void store_now(const char *done)
{
    int i;
    for (i = 0; done[i] && i < (int)sizeof ui.store_msg - 1; i++)
        ui.store_msg[i] = done[i];
    ui.store_msg[i] = 0;
    if (en_playing) {
        en_hush(1);
        ui.store_wait = 1;
        ui.store_t = plat_ms();
        say("STORING", 0);
    } else {
        say(ui.store_msg, project_flush() == 0 ? 0 : "NOT WRITTEN");
    }
}
static void store_tick(void)                    /* the music faded out (or long enough): write, then back in */
{
    if (!ui.store_wait || (!en_hushed && plat_ms() - ui.store_t < 150u))
        return;
    ui.store_wait = 0;
    say(ui.store_msg, project_flush() == 0 ? 0 : "NOT WRITTEN");
    en_hush(0);
}

static void piece_store(void)                   /* Global, SAVE: the piece as it is, stored */
{
    char t[12] = "STORED ";
    project_piece_save();
    caps(t + 7, piece_name(proj.piece), 4);
    store_now(t);
}

static void piece_factory(void)                 /* Global, SAVE held: back to the factory settings */
{
    char t[12] = "FACTORY ";
    int n;
    project_piece_factory();
    iam_piece_room();
    caps(t + 8, piece_name(proj.piece), 4);
    if (iam_song() && en_playing) {
        glo_fx_drop();
        for (n = 0; t[n] && n < (int)sizeof piece_flush_msg - 1; n++)
            piece_flush_msg[n] = t[n];
        piece_flush_msg[n] = 0;
        piece_flush = 1;
        if (!piece_wait)
            en_shed(1);
        piece_wait = 1;
        return;
    }
    project_apply();
    en_regrow();
    store_now(t);
}
static void piece_land(void)                     /* the four are down: the piece, then they come back */
{
    int store;
    if (!piece_wait || !en_shed_done)
        return;
    piece_wait = 0;
    store = piece_flush;
    piece_flush = 0;
    project_apply();
    if (!store)
        en_seed(plat_ms());
    en_regrow();
    en_shed(0);
    if (store)
        store_now(piece_flush_msg);
}

/* ------------------------------------------------------------ the kit --- */
static int seq_pat(void) { return proj.par[P_PATTERN]; }   /* the pattern the sequencer edits: the one chosen */

static void step_toggle(int s)                  /* a hit on or off: the lane's step s (its white key) */
{
    volatile uint16_t *p = &en_pats[seq_pat()][ui.lane];
    *p = (uint16_t)(*p ^ (1u << s));
    edited();
}

/* ------------------------------------------------------------- shift --- */
static void pat_name(int i, char *t)            /* "PATTERN 3" */
{
    const char *p = "PATTERN ";
    while (*p)
        *t++ = *p++;
    *t++ = (char)('1' + i);
    *t = 0;
}

static void shift_key(int key)                  /* a key pressed with SEL held */
{
    char t[16];
    int i;
    for (i = 0; i < EN_NPAT; i++)
        if (WHITE_KEY[i] == key) {               /* white 1-6: the pattern */
            knob_set(P_PATTERN, i);
            pat_name(i, t);
            say(t, en_playing && en_pat_now != i ? "NEXT BAR" : 0);
            return;
        }
    if (ui.view != V_SEQ)
        return;
    for (i = 0; i < EN_NPAT; i++)
        if (BLACK_KEY[i] == key) {               /* the sequencer, black 1-6: the pattern copied there */
            int from = seq_pat(), l;
            if (i == from) {
                say("THAT'S THIS", "PATTERN");
                return;
            }
            for (l = 0; l < EN_NLANE; l++)
                en_pats[i][l] = en_pats[from][l];
            edited();
            pat_name(i, t);
            say("COPIED TO", t);
            return;
        }
}

static void shift_lane(int restore)             /* the sequencer: SEL + OCT-, the lane cleared; OCT+, the piece's own */
{
    static uint16_t f[EN_NPAT][EN_NLANE];
    char t[16];
    uint16_t v = 0;
    if (restore) {
        en_piece_pats(proj.piece, f);
        v = f[seq_pat()][ui.lane];
    }
    en_pats[seq_pat()][ui.lane] = v;
    edited();
    caps(t, EN_LANE_NAME[ui.lane], sizeof t);
    say(t, restore ? "FACTORY" : "CLEARED");
}

/* ------------------------------------------------------------ buttons --- */
static void set_view(int v)
{
    ui.view = (uint8_t)v;
    if (v < NDIAL)
        ui.last_dial = (uint8_t)v;
    ui.touched = -1;
}

static int voice_of(int v)                      /* the voice page v belongs to (EN_V_*), or -1 */
{
    if (v >= V_PAD && v <= V_CHORDS)
        return EN_V_PAD;
    if (v >= V_DRONE && v <= V_DRLFO)
        return EN_V_DRONE;
    if (v >= V_ACID && v <= V_ACLINE)
        return EN_V_ACID;
    return -1;
}
static const int SOUND_PAR[3] = {P_PADTONE, P_DR_INST, P_AC_WAVE};
static const int HAND_PAR[3] = {P_PAD_MAN, P_DR_MAN, P_AC_MAN};   /* off the algorithm: played by hand */
static int by_hand(int vc) { return vc >= 0 && proj.par[HAND_PAR[vc]] != 0; }
static int kit_view(int v) { return v == V_DRUMS || v == V_LINKS || v == V_KIT || v == V_SEQ; }

static void shift_sound(int d)                  /* SEL + ALGORITHM: the voice's next sound, or the whole kit's */
{
    char a[16], b[16];
    int vc = voice_of(ui.view);
    if (vc >= 0) {
        static const char *const WHO[3] = {"PAD", "DRONE", "ACID"};
        int p = SOUND_PAR[vc];
        knob_set(p, (proj.par[p] + d + en_bank_n(vc)) % en_bank_n(vc));
        en_param_text(p, proj.par[p], b);
        caps(a, b, sizeof a);
        say(WHO[vc], a);
    } else if (kit_view(ui.view)) {             /* the drums: all four to the next kit */
        int k = (proj.par[P_KIT_KICK] + d + EN_NKIT) % EN_NKIT;
        knob_set(P_KIT_KICK, k);
        knob_set(P_KIT_SNARE, k);
        knob_set(P_KIT_HAT, k);
        knob_set(P_KIT_PERC, k);
        caps(a, EN_KIT_NAME[k], sizeof a);
        say("KIT", a);
    } else {
        say("SOUND", "ON A VOICE'S PAGE");
    }
}

static int oct_par(int v)                       /* what OCT moves on page v */
{
    if (v >= V_PAD && v <= V_CHORDS)
        return P_PAD_OCT;
    if (v >= V_DRONE && v <= V_DRLFO)
        return P_BASSOCT;
    if (v >= V_ACID && v <= V_ACLINE)
        return P_AC_OCT;
    return P_OCT;
}

static int group_of(int v)
{
    int g;
    for (g = 0; g < NGROUP; g++)
        if (v >= GROUP[g].first && v < GROUP[g].first + GROUP[g].n)
            return g;
    return -1;
}

static int iam_song(void);                  /* machine.c: seed 32 and up, the IAM panel */
static int iam_glo_perf(int b);              /* machine.c: IAM GLO keeps the song. 1 = leave Felucca */
static int iam_arp_on(void);                 /* machine.c: GLO arpeggiator latched. leds() is above the include */
static int view_knob(int i)                 /* Tempo's third knob is the white keys, on an IAM song */
{
    if (iam_song() && ui.view == V_TEMPO && i == 2)
        return K_KEYS;
    return VIEW_KNOB[ui.view][i];
}
static void iam_loop_toggle(void);           /* machine.c: GLO REC, the white-key loop */
static void iam_loop_tick(void);
static int iam_loop_button(void);            /* 1: light REC */
static void iam_loop_halt(void);
static void iam_loop_stop(void);             /* REC while the loop is playing */
static int iam_loop_playing(void);           /* 1: a finished loop is running */
static int iam_loop_recording(void);         /* 1: the take is open */
static int iam_loop_voice(int vc);           /* 1: that voice is in the playing loop */
static int iam_jam_voice(void);              /* machine.c: the voice the white keys play */

static void button(int b)
{
    int g;
    if (iam_song() && iam_glo_perf(b))           /* PLAY conducts, SAVE breaks, EDIT mutates */
        return;
    for (g = 0; g < NGROUP; g++)                 /* a page's button: its page, or the next in its group */
        if (GROUP[g].btn == b) {
            int v = ui.view;
            if (group_of(v) == g && GROUP[g].n > 1)
                set_view(GROUP[g].first + (v - GROUP[g].first + 1) % GROUP[g].n);
            else
                set_view(GROUP[g].first);
            return;
        }
    switch (b) {
    case B_FX:
        ui.fx_menu = (uint8_t)!ui.fx_menu;
        say("FX", ui.fx_menu ? "HOLD A KEY" : ui.fx_lock || ui.dj_lock ? "LOCKED ONES STAY ON" : "OFF");
        break;
    case B_EDIT:
        en_next();
        say("NEXT CHORD", en_playing ? "ON THE BAR" : 0);
        break;
    case B_PLAY:
        if (en_playing) {
            en_play(0);
            say("STOPPED", 0);
        } else {
            en_seed(plat_ms());
            en_play(1);
            say("PLAYING", 0);
        }
        break;
    case B_REC:
        if (iam_song()) {                        /* GLO: the white keys, not the freeze */
            iam_loop_toggle();
            break;
        }
        say("FREEZE", en_frozen ? "OFF" : "ON");
        en_freeze(!en_frozen);                   /* the engine takes it at its next block */
        break;
    case B_OCTDN: case B_OCTUP:
        if (ui.view == V_SEQ) {                  /* the sequencer: the lane */
            ui.lane = (uint8_t)((ui.lane + (b == B_OCTUP ? 1 : EN_NLANE - 1)) % EN_NLANE);
            ui.touched = -1;
        } else {                                 /* the page's voice's octave, or the song's */
            int p = oct_par(ui.view);
            char t[8];
            knob_set(p, proj.par[p] + (b == B_OCTUP ? 1 : -1));
            en_param_text(p, proj.par[p], t);
            say(p == P_PAD_OCT ? "PAD OCTAVE" : p == P_BASSOCT ? "DRONE OCTAVE" : p == P_AC_OCT ? "ACID OCTAVE" : "SONG OCTAVE", t);
        }
        break;
    case B_SAVE:                                 /* nothing is saved by it: SAVE is the breakdown (and the build) */
        if (en_sec == EN_SEC_GROOVE && en_sec_req == EN_CMD_BREAK + 1) {
            en_section(EN_CMD_DROP);             /* asked twice before the bar: never mind */
            say("BREAKDOWN", "OFF");
        } else if (en_sec == EN_SEC_GROOVE) {
            en_section(EN_CMD_BREAK);
            say("BREAKDOWN", en_playing ? "NEXT BAR" : 0);
        } else if (en_sec == EN_SEC_BREAK) {
            en_section(EN_CMD_BUILD);
            say("BUILD", "4 BARS, DROP");
        } else {
            en_section(EN_CMD_DROP);
            say("DROP", "NEXT BAR");
        }
        break;
    default: break;
    }
}

/* MIDI in (USB and the TRS jack): channel 1 notes add or remove their note from the scale; clock and
 * start / stop drive the grid */
static void midi_in(void)
{
    uint32_t pkt;
    while (plat_midi_in(&pkt)) {
        uint32_t st = (pkt >> 8) & 0xFFu, n = (pkt >> 16) & 0x7Fu, v = pkt >> 24;
        if (st == 0xF8u || st == 0xFAu || st == 0xFBu || st == 0xFCu)
            en_clock(st == 0xF8u ? EN_CLK_TICK : st == 0xFAu ? EN_CLK_START : st == 0xFBu ? EN_CLK_CONTINUE : EN_CLK_STOP);
        else if (st == 0x90u && v)
            toggle_note((int)(n % 12u));
    }
}

/* HELP CARDS (X0X's): hold one button alone for HELP_HOLD and a card says what it does. A key, a
 * turn or another button puts it away; letting go after its card does nothing else. The buttons whose
 * hold means something else (PLAY, SEL, SEQ, SAVE, FX) have no card. */
static void help_tick(uint32_t btn, uint32_t keys)
{
    uint32_t now = plat_ms();
    int i;
    if (ui.help) {
        uint32_t m = 1u << (ui.help - 1);
        if (btn != m || keys || (int32_t)(ui.turn_t - ui.down_t[ui.help - 1]) > 0)
            ui.help = 0;
        return;
    }
    if (!btn || (btn & (btn - 1u)) || keys)
        return;
    for (i = 0; i < NB; i++)
        if (btn == 1u << i && i != B_PLAY && i != B_SEL && i != B_FX && i != B_SAVE && i != B_SEQ &&
            !(ui.btn_used & btn) && now - ui.down_t[i] >= HELP_HOLD && (int32_t)(ui.turn_t - ui.down_t[i]) <= 0) {
            ui.help = (uint8_t)(i + 1);
            ui.btn_used |= btn;                  /* its release now does nothing */
        }
}

/* REMOTE (SEL + GLO): the panel plays HYP-1 in the browser over USB MIDI, and the FM-1 goes quiet. On
 * channel 16: the keys are notes 48 + k, the buttons CC 102 + b (127 down, 0 up), the knobs CC 85 + role
 * (64 + detents), MASTER CC 7, and CC 116 says hello every second. The browser answers with its lights
 * the same way (notes and CC 102 + b) and CC 117, so the panel lights as the emulator's does. SEL + GLO
 * again: back, stopped. Not in the browser's own build (it has no USB). */
#define RM_ST_NOTE 0x9Fu
#define RM_ST_OFF 0x8Fu
#define RM_ST_CC 0xBFu
#define RM_KEY0 48u
#define RM_BTN0 102u
#define RM_ENC0 85u
#define RM_MASTER 7u
#define RM_HELLO 116u
#define RM_ACK 117u
static struct {
    uint8_t on, link, drawn;
    uint32_t btn, keys, ign_b, ign_k;            /* sent as down; held at the start, not sent till let go */
    uint32_t lit_b, lit_k;                       /* the browser's lights */
    uint32_t hello_t, rx_t;
    int master;
} rm;
int ui_remote(void) { return rm.on; }

static void rm_out(uint32_t st, uint32_t d1, uint32_t d2) { plat_midi_out(st >> 4 | st << 8 | d1 << 16 | d2 << 24); }

static void text_c(int32_t cx, int32_t y, const felucca_font_t *f, const char *s, uint16_t c);

static void rm_draw(void)
{
    lcd_fill(0, 0, 240, 240, K_BG);
    cv_begin(240, 120, K_BG);
    text_c(120, 4, &FONT_L, "REMOTE", K_ACC);
    text_c(120, 44, &FONT_S, rm.link ? "PLAYING THE BROWSER" : "WAITING FOR THE BROWSER", rm.link ? K_PAD : K_DIM);
    text_c(120, 66, &FONT_XS, "USB MIDI  //  CHANNEL 16", K_DIM);
    text_c(120, 96, &FONT_S, "SEL + GLO: BACK", K_TEXT);
    cv_blit(0, 56);
    rm.drawn = (uint8_t)(1u + rm.link);
}

#ifndef EN_WEB
static void rm_enter(void)
{
    int e;
    iam_loop_halt();                             /* the key loop does not follow the panel out */
    memset(&rm, 0, sizeof rm);
    rm.on = 1;
    rm.ign_b = plat_buttons();
    rm.ign_k = plat_keys();
    rm.hello_t = plat_ms() - 1000u;
    rm.master = -1;
    for (e = 0; e < NE; e++)
        (void)plat_enc(e);
    en_play(0);
    en_mute(1);                                  /* after the stop's note-offs */
    plat_leds(0, 0);
    rm_draw();
}
#endif

static void rm_leave(void)
{
    uint32_t i;
    for (i = 0; i < NB; i++)                     /* the browser lets go of what was down */
        if (rm.btn >> i & 1u)
            rm_out(RM_ST_CC, RM_BTN0 + i, 0);
    for (i = 0; i < NKEYS; i++)
        if (rm.keys >> i & 1u)
            rm_out(RM_ST_OFF, RM_KEY0 + i, 0);
    rm_out(RM_ST_CC, RM_HELLO, 0);               /* goodbye */
    rm.on = 0;
    en_mute(0);
    ui.btn = plat_buttons();                     /* what's held now does nothing when let go */
    ui.keys = plat_keys();
    ui.btn_used = ui.btn;
    ui.shift_keys = ui.keys;
    ui.sig[0] = ui.sig[1] = ui.sig[2] = 0;       /* everything drawn again */
    lcd_fill(0, 0, 240, 240, K_BG);
    say("REMOTE", "OFF");
}

static void rm_tick(void)
{
    uint32_t b = plat_buttons(), k = plat_keys(), now = plat_ms(), ch, i, pkt;
    int e;
    while (plat_midi_in(&pkt)) {                 /* the browser's lights (channel 16); the rest, ignored */
        uint32_t st = (pkt >> 8) & 0xFFu, n = (pkt >> 16) & 0x7Fu, v = pkt >> 24;
        if (st == RM_ST_NOTE || st == RM_ST_OFF) {
            if (n >= RM_KEY0 && n < RM_KEY0 + NKEYS) {
                uint32_t m = 1u << (n - RM_KEY0);
                rm.lit_k = st == RM_ST_NOTE && v ? rm.lit_k | m : rm.lit_k & ~m;
            }
        } else if (st == RM_ST_CC) {
            if (n >= RM_BTN0 && n < RM_BTN0 + NB) {
                uint32_t m = 1u << (n - RM_BTN0);
                rm.lit_b = v ? rm.lit_b | m : rm.lit_b & ~m;
            } else if (n == RM_ACK) {
                rm.rx_t = now;
                if (!rm.link) {
                    rm.link = 1;
                    rm_draw();
                }
                if (!v)
                    rm.lit_b = rm.lit_k = 0;
            }
        }
    }
    if (rm.link && now - rm.rx_t > 3000u) {      /* the browser's gone quiet */
        rm.link = 0;
        rm.lit_b = rm.lit_k = 0;
        rm_draw();
    }
    if ((b >> B_SEL & 1u) && (b >> B_GLO & 1u) && !((rm.ign_b | rm.btn) >> B_GLO & 1u)) {
        rm_leave();                              /* SEL + GLO: back (GLO never sent) */
        return;
    }
    rm.ign_b &= b;
    rm.ign_k &= k;
    b &= ~rm.ign_b;
    k &= ~rm.ign_k;
    if (b >> B_GLO & 1u && b >> B_SEL & 1u)
        b &= ~(1u << B_GLO);
    ch = b ^ rm.btn;
    for (i = 0; i < NB; i++)
        if (ch >> i & 1u)
            rm_out(RM_ST_CC, RM_BTN0 + i, b >> i & 1u ? 127u : 0u);
    rm.btn = b;
    ch = k ^ rm.keys;
    for (i = 0; i < NKEYS; i++)
        if (ch >> i & 1u) {
            if (k >> i & 1u)
                rm_out(RM_ST_NOTE, RM_KEY0 + i, 100u);
            else
                rm_out(RM_ST_OFF, RM_KEY0 + i, 0u);
        }
    rm.keys = k;
    for (e = 0; e < NE; e++) {
        int32_t d = plat_enc(e);
        if (d)
            rm_out(RM_ST_CC, RM_ENC0 + (uint32_t)e, (uint32_t)(64 + (d > 63 ? 63 : d < -63 ? -63 : d)));
    }
    {
        int m = (int)(plat_master() >> 5);       /* 0 .. 127, sent when it moves by two (the pot's noise) */
        if (m > 127)
            m = 127;
        if (rm.master < 0 || m - rm.master > 1 || rm.master - m > 1 || ((m == 0 || m == 127) && m != rm.master)) {
            rm.master = m;
            rm_out(RM_ST_CC, RM_MASTER, (uint32_t)m);
        }
    }
    if (now - rm.hello_t >= 1000u) {
        rm.hello_t = now;
        rm_out(RM_ST_CC, RM_HELLO, 127u);
    }
    if (b || k)
        ui.act_t = now;
}

static int runner_on(void);
static void runner_input(uint32_t btn, uint32_t keys);
static void runner_start(void);
static int iam_editor_back(int b); /* machine.c: an IAM song leaves the editor on GLO or HOME */
static int iam_song(void);         /* machine.c: seed 32 and up, the IAM panel */
static void iam_editor_note(int key); /* machine.c: IAM editor keys, chords and effects */
static int iam_editor_key_off(int key); /* machine.c: one GLO chord or effect, let go */
static uint32_t iam_jam_mask(void);     /* machine.c: black-key effects held in GLO */
static void iam_editor_lift_off(void); /* machine.c: a voice raised so a key could be heard */
static void iam_editor_unstick(uint32_t keys); /* machine.c: a key that is up lets the line go */

/* GLO's FX page. -1 keeps the Felucca effect. The order is the keys that were asked for. */
static int glo_dj(int key)
{
    static const uint8_t K[14] = {
        1, 8, 13, 15, 17, 20, 22, 25,          /* black 1, 4, 6, 7, 8, 9, 10, 11 */
        14, 16, 18, 19, 21, 23                 /* white 9 .. 14 */
    };
    int i;
    if (!iam_song())
        return -1;
    for (i = 0; i < 14; i++)
        if (K[i] == (uint8_t)key)
            return i;
    return -1;
}
static void glo_lock_or(int key)
{
    int d = glo_dj(key);
    if (d >= 0)
        ui.dj_lock = (uint16_t)(ui.dj_lock | (uint16_t)(1u << d));
    else
        ui.fx_lock |= 1u << KEY_FX[key];
}
static void glo_lock_xor(int key)
{
    int d = glo_dj(key);
    if (d >= 0)
        ui.dj_lock = (uint16_t)(ui.dj_lock ^ (uint16_t)(1u << d));
    else
        ui.fx_lock ^= 1u << KEY_FX[key];
}
static int glo_lock_has(int key)
{
    int d = glo_dj(key);
    if (d >= 0)
        return (int)((ui.dj_lock >> d) & 1u);
    return (int)((ui.fx_lock >> KEY_FX[key]) & 1u);
}
static void glo_lock_clear(int key)
{
    int d = glo_dj(key);
    if (d >= 0)
        ui.dj_lock = (uint16_t)(ui.dj_lock & (uint16_t)~(1u << d));
    else
        ui.fx_lock &= ~(1u << KEY_FX[key]);
}

static void input(void)
{
    uint32_t btn = plat_buttons(), keys = plat_keys(), ch, i;
    int32_t e;
    if (runner_on()) {                           /* the secret: the game has the panel */
        runner_input(btn, keys);
        return;
    }
    ch = btn ^ ui.btn;
    ui.btn = btn;
    for (i = 0; i < NB; i++) {
        uint32_t m = 1u << i;
        if (!(ch & m))
            continue;
        if (btn & m) {
            ui.btn_used &= ~m;
            ui.down_t[i] = plat_ms();
            if (i == B_SAVE) {
                ui.save_held = 0;
                ui.save_global = 0;
            }
            if (i == B_SAVE && ui.view == V_GLOBAL && !ui.fx_menu) {   /* Global: store the piece */
                ui.save_global = 1;
                ui.factory_done = 0;
                ui.btn_used |= m;              /* not a breakdown */
                ui.save_held = 1;              /* nor a drop */
            } else if (i == B_SAVE && ui.fx_menu) {   /* FX: SAVE held is lock mode */
                uint32_t k, held = keys & ~ui.shift_keys;
                ui.lock_mode = 1;
                ui.save_held = 1;              /* no drop while it's held */
                for (k = 0; k < NKEYS; k++)    /* keys already held: locked */
                    if (held >> k & 1u)
                        glo_lock_or((int)k);
                if (held) {
                    ui.lock_keys |= held;
                    ui.btn_used |= m;          /* not a breakdown */
                }
                say("LOCK MODE", "TAP EFFECTS");
            }
            if (i == B_SEQ)
                ui.seq_held = 0;
#ifndef EN_WEB
            if (i == B_GLO && (btn >> B_SEL & 1u)) {   /* shift: remote, the panel plays the browser */
                rm_enter();
                return;
            }
#endif
            if (i == B_EDIT && (btn >> B_SEL & 1u)) {   /* shift: the secret */
                ui.btn_used |= m | 1u << B_SEL;
                ui.btn = btn;
                ui.keys = keys;
                runner_start();
                return;
            }
            if (i == B_ENV && (btn >> B_SEL & 1u)) {
                /* shift: the voice whose page is on screen off the algorithm (silent but for the keys on its
                 * pages), or back on it */
                static const char *const WHO[3] = {"PAD", "DRONE", "ACID"};
                int vc = voice_of(ui.view), k;
                if (vc < 0) {
                    say("BY HAND", "ON A VOICE'S PAGE");
                } else {
                    knob_set(HAND_PAR[vc], !proj.par[HAND_PAR[vc]]);
                    if (!proj.par[HAND_PAR[vc]]) /* back on the algorithm: any of its keys let go */
                        for (k = 0; k < NKEYS; k++)
                            if (ui.key_voice[k] == vc + 1) {
                                if (iam_song() && iam_editor_key_off((int)k))
                                    continue;
                                en_note(vc, 53 + k, 0);
                                ui.key_voice[k] = 0;
                            }
                    say(WHO[vc], proj.par[HAND_PAR[vc]] ? "BY HAND: PLAY THE KEYS" : "BACK WITH THE MUSIC");
                }
                ui.btn_used |= m | 1u << B_SEL;
            }
            if (i == B_SEQ && (btn >> B_SEL & 1u)) {   /* shift: the links on and off */
                knob_set(P_LINKS, !proj.par[P_LINKS]);
                say("LINKS", proj.par[P_LINKS] ? "ON" : "OFF");
                ui.btn_used |= m | 1u << B_SEL;
                ui.seq_held = 1;               /* no sequencer for this hold */
            }
            if (i == B_SEL)
                ui.shift_hint = 0;
            if ((i == B_OCTDN || i == B_OCTUP) && (btn >> B_SEL & 1u) && ui.view == V_SEQ) {
                shift_lane(i == B_OCTUP);      /* shift: the lane cleared, or the piece's own */
                ui.btn_used |= m | 1u << B_SEL;
            }
            if (i == B_PLAY)                   /* transport on the press */
                button((int)i);
        } else if (i == B_SAVE && ui.lock_mode) {
            ui.lock_mode = 0;                  /* lock mode over; SAVE tapped alone: the breakdown, as ever */
            if (!(ui.btn_used & m))
                button((int)i);
            else
                say("FX", ui.fx_lock || ui.dj_lock ? "LOCKED" : "NONE LOCKED");
        } else if (i == B_SAVE && ui.save_global) {
            ui.save_global = 0;                /* Global: SAVE let go, the piece stored (unless held to factory) */
            if (!ui.factory_done)
                piece_store();
        } else if (!(ui.btn_used & m) && i != B_PLAY) {
            if (iam_editor_back((int)i))
                return;                        /* IAM: GLO again, or HOME, is the song */
            if (i == B_SEQ && ui.view == V_SEQ)
                set_view(V_DRUMS);             /* SEQ tapped in the sequencer: back to Drums */
            else
                button((int)i);
        }
    }
    ch = keys ^ ui.keys;
    ui.keys = keys;
    ui.shift_keys &= keys;                     /* let go: no longer the shift's */
    if ((btn >> B_SEL & 1u) && (ch & keys)) {  /* SEL held: the keys pressed are the shift's */
        ui.shift_keys |= ch & keys;
        ui.btn_used |= 1u << B_SEL;
        for (i = 0; i < NKEYS; i++)
            if ((ch & keys) >> i & 1u)
                shift_key((int)i);
    }
    ui.lock_keys &= keys;
    if (ui.fx_menu && ui.lock_mode && (ch & keys & ~ui.shift_keys)) {   /* lock mode: a key tapped, its effect */
        uint32_t p = ch & keys & ~ui.shift_keys;                         /* locked, or unlocked */
        for (i = 0; i < NKEYS; i++)
            if (p >> i & 1u)
                glo_lock_xor((int)i);
        ui.lock_keys |= p;
        ui.btn_used |= 1u << B_SAVE;
    }
    if (ui.fx_menu && !ui.lock_mode && (ch & keys & ~ui.shift_keys)) {   /* a locked effect's key tapped: that */
        uint32_t p = ch & keys & ~ui.shift_keys;                         /* one off, the others stay locked */
        for (i = 0; i < NKEYS; i++)
            if ((p >> i & 1u) && glo_lock_has((int)i)) {
                glo_lock_clear((int)i);
                ui.lock_keys |= 1u << i;       /* and not played while it's held */
            }
    }
    if (!ui.fx_menu)
        for (i = 0; i < NKEYS; i++)
            if ((ch & keys & ~ui.shift_keys) >> i & 1u) {
                if (ui.view == V_SEQ) {          /* the sequencer: a white key is a step */
                    int s;
                    for (s = 0; s < 16; s++)
                        if (WHITE_KEY[s] == i)
                            step_toggle(s);
                    if (iam_song())              /* a black key: the same held effects as GLO */
                        iam_editor_note((int)i);
                } else if (iam_song()) {         /* chords on the white keys. Not the scale */
                    iam_editor_note((int)i);
                } else if (by_hand(voice_of(ui.view))) {   /* a voice off the algorithm, its page: the key plays it */
                    ui.key_voice[i] = (uint8_t)(voice_of(ui.view) + 1);
                    en_note(voice_of(ui.view), 53 + (int)i, 1);
                } else {
                    toggle_note((int)((5u + i) % 12u));   /* key 0 is F3 */
                }
            }
    for (i = 0; i < NKEYS; i++)                  /* let go: its note off, wherever it went */
        if (ui.key_voice[i] && !(keys >> i & 1u)) {
            if (iam_song() && iam_editor_key_off((int)i))
                continue;
            en_note(ui.key_voice[i] - 1, 53 + (int)i, 0);
            ui.key_voice[i] = 0;
        }
    if (iam_song()) {
        iam_editor_lift_off();
        iam_editor_unstick(keys);              /* before the effect mask, so a cleared key drops out of it */
    }
    {   /* the effects: each key held while the FX menu is on, those locked, and GLO's black keys */
        uint32_t m = 0;
        uint16_t dj = 0;
        if (!ui.fx_menu)
            ui.lock_mode = 0;                  /* FX closed: the locks play on (FX again, a key: that one off) */
        m = ui.fx_lock;
        dj = ui.dj_lock;
        if (ui.fx_menu) {
            for (i = 0; i < NKEYS; i++)
                if ((keys & ~ui.shift_keys & ~ui.lock_keys) >> i & 1u) {
                    int d = glo_dj((int)i);
                    if (d >= 0)
                        dj = (uint16_t)(dj | (uint16_t)(1u << d));
                    else
                        m |= 1u << KEY_FX[i];
                }
        } else if (iam_song())
            m |= iam_jam_mask();
        if (m != ui.fx_mask) {
            ui.fx_mask = m;
            en_fx(m);
        }
        if (dj != ui.dj_mask) {
            ui.dj_mask = dj;
            en_dj(dj);
        }
    }
    if (btn || keys || ch)
        ui.act_t = plat_ms();
    midi_in();
    if ((btn >> B_SEQ & 1u) && !ui.seq_held && ui.view != V_SEQ && plat_ms() - ui.down_t[B_SEQ] >= SEQ_HOLD) {
        ui.seq_held = 1;                       /* SEQ held: the sequencer */
        ui.btn_used |= 1u << B_SEQ;
        set_view(V_SEQ);
    }
    if ((e = plat_enc(EN_SELECT)) != 0) {        /* SELECT: the pages (the steps); with HOME held, the tempo */
        if (btn >> B_HOME & 1u) {
            turn(EN_SELECT, P_TEMPO, e);
            ui.btn_used |= 1u << B_HOME;
            if (ui.view == V_TEMPO) {
                ui.touched = 0;
                ui.touch_until = plat_ms() + 1200u;
            } else {
                char t[8];
                itoa_u((uint32_t)proj.par[P_TEMPO], t);
                say("TEMPO", t);
            }
        } else {
            int v = (ui.view == V_SEQ ? V_DRUMS : (int)ui.view) + (e > 0 ? 1 : -1);   /* (out of the sequencer too) */
            if (v >= 0 && v < NDIAL)
                set_view(v);
        }
        ui.turn_t = plat_ms();
    }
    if ((e = plat_enc(EN_ALGO)) != 0) {
        if (btn >> B_SEL & 1u) {               /* shift: the sound of the voice on screen (drums: the kit) */
            ui.btn_used |= 1u << B_SEL;
            shift_sound(e > 0 ? 1 : -1);
        } else {
            set_mode(proj.par[P_MODE] + (e > 0 ? 1 : -1));
        }
        ui.turn_t = plat_ms();
    }
    if ((e = plat_enc(EN_PRESET)) != 0) {
        if ((btn >> B_SEL & 1u) && en_playing) {   /* shift: the piece on the next bar */
            int t = ((ui.piece_q ? ui.piece_q - 1 : proj.piece) + (e > 0 ? 1 : -1) + NPIECES) % NPIECES;
            char u[12] = "PIECE ";
            ui.btn_used |= 1u << B_SEL;
            caps(u + 6, piece_name(t), 4);
            if (t == proj.piece) {
                ui.piece_q = 0;
                say(u, "STAYS");
            } else {
                ui.piece_q = (uint8_t)(t + 1);
                say(u, "NEXT BAR");
                ui.msg_until = plat_ms() + 60000u;   /* until it comes */
            }
        } else {
            if (btn >> B_SEL & 1u)
                ui.btn_used |= 1u << B_SEL;
            ui.piece_q = 0;
            set_piece(proj.piece + (e > 0 ? 1 : -1));
        }
        ui.turn_t = plat_ms();
    }
    if (ui.piece_q && (!en_playing || en_drum_pos == EN_NSTEP - 1)) {   /* the bar's last step: the piece */
        int t = ui.piece_q - 1;
        ui.piece_q = 0;
        set_piece(t);
    }
    if ((btn >> B_SEL & 1u) && !(ui.btn_used >> B_SEL & 1u) && !ui.shift_hint &&
        plat_ms() - ui.down_t[B_SEL] >= 400u) {
        ui.shift_hint = 1;                     /* SEL held alone: what the shift does */
        say("SHIFT", ui.view == V_SEQ ? "COPY / CLEAR" : "ALGO: SOUND");
    }
    if (ui.save_global && !ui.factory_done && (btn >> B_SAVE & 1u)) {
        uint32_t held = plat_ms() - ui.down_t[B_SAVE];
        if (held >= FACTORY_HOLD) {
            ui.factory_done = 1;
            piece_factory();
        } else if (held >= 400u && plat_ms() >= ui.msg_until) {
            say("HOLD:", "FACTORY");           /* what holding on does */
            ui.msg_until = ui.down_t[B_SAVE] + FACTORY_HOLD;
        }
    }
    if ((btn >> B_SAVE & 1u) && !ui.save_held && plat_ms() - ui.down_t[B_SAVE] >= DROP_HOLD) {
        ui.save_held = 1;                      /* held: straight to the drop, no build */
        ui.btn_used |= 1u << B_SAVE;
        if (iam_song())
            button(B_SAVE);                    /* the break, once. The release does not fire it again */
        else if (en_sec != EN_SEC_GROOVE) {
            en_section(EN_CMD_DROP);
            say("DROP", "NEXT BAR");
        } else {
            button(B_SAVE);
        }
    }
    for (i = 0; i < 4; i++)
        if ((e = plat_enc(EN_K1 + (int)i)) != 0) {
            turn(EN_K1 + (int)i, view_knob((int)i), e);
            ui.touched = (int8_t)i;
            ui.touch_until = plat_ms() + 1200u;
            ui.act_t = ui.turn_t = plat_ms();
        }
    help_tick(btn, keys);
    iam_loop_tick();
}

/* ------------------------------------------------------------- screen --- */
/* three bands: the header (28 px), the main area (144), the knobs (68) */
#define HDR_H 28
#define MAIN_Y 28
#define MAIN_H 144
#define KNB_Y 172
#define KNB_H 68

static uint32_t hash(uint32_t h, uint32_t v) { return (h ^ v) * 16777619u; }

/* a rectangle with rounded corners (radius r, up to 6) */
static void cv_round(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t c)
{
    static const uint8_t IN[7][6] = {{0}, {1}, {1, 0}, {2, 1, 0}, {2, 1, 0, 0}, {3, 2, 1, 0, 0}, {4, 2, 1, 1, 0, 0}};
    int32_t j;
    if (r > 6)
        r = 6;
    if (r > h / 2)
        r = h / 2;
    cv_rect(x, y + r, w, h - 2 * r, c);
    for (j = 0; j < r; j++) {
        int32_t d = IN[r][j];
        cv_rect(x + d, y + j, w - 2 * d, 1, c);
        cv_rect(x + d, y + h - 1 - j, w - 2 * d, 1, c);
    }
}

/* a filled circle of radius r (0..6) centred on (cx, cy): HW[r][d] is the half width d rows from the middle */
static void cv_disc(int32_t cx, int32_t cy, int32_t r, uint16_t c)
{
    static const uint8_t HW[7][7] = {{0}, {1, 0}, {2, 1, 0}, {3, 2, 2, 0}, {4, 4, 3, 2, 0}, {5, 5, 4, 4, 3, 0},
                                     {6, 6, 5, 5, 4, 3, 0}};
    int32_t j;
    if (r <= 0) {
        cv_rect(cx, cy, 1, 1, c);
        return;
    }
    if (r > 6)
        r = 6;
    for (j = -r + 1; j < r; j++) {
        int32_t w = HW[r][j < 0 ? -j : j];
        cv_rect(cx - w, cy + j, 2 * w + 1, 1, c);
    }
}

/* an outline: a cyberpunk frame with its corners cut */
static void cv_frame(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c)
{
    cv_rect(x + 2, y, w - 4, 1, c);
    cv_rect(x + 2, y + h - 1, w - 4, 1, c);
    cv_rect(x, y + 2, 1, h - 4, c);
    cv_rect(x + w - 1, y + 2, 1, h - 4, c);
    cv_rect(x + 1, y + 1, 1, 1, c);
    cv_rect(x + w - 2, y + 1, 1, 1, c);
    cv_rect(x + 1, y + h - 2, 1, 1, c);
    cv_rect(x + w - 2, y + h - 2, 1, 1, c);
}

static void text_c(int32_t cx, int32_t y, const felucca_font_t *f, const char *s, uint16_t c)
{
    cv_text(cx - text_w(f, s) / 2, y, f, s, c);
}

/* anti-aliased drawing (X0X's): a pixel blended over the canvas, a disc, and a knob's ring */
static void px_blend(int32_t x, int32_t y, uint16_t c, uint32_t a)
{
    uint16_t *q;
    uint32_t bg;
    y += cv_oy;
    if ((uint32_t)x >= cv_w || y < cv_y0 || y >= cv_y1 || !a)
        return;
    q = &cv_px[(uint32_t)y * cv_w + (uint32_t)x];
    if (a >= 16u) {
        *q = swap16(c);
        return;
    }
    bg = swap16(*q);
    *q = swap16((uint32_t)(((((c >> 11) * a + (bg >> 11) * (16u - a)) >> 4) << 11) |
                           (((((c >> 5) & 63u) * a + ((bg >> 5) & 63u) * (16u - a)) >> 4) << 5) |
                           (((c & 31u) * a + (bg & 31u) * (16u - a)) >> 4)));
}

static void disc(float cx, float cy, float r, uint16_t c)
{
    int32_t x0 = (int32_t)fm_floorf(cx - r - 1.0f), x1 = (int32_t)(cx + r + 1.0f);
    int32_t y0 = (int32_t)fm_floorf(cy - r - 1.0f), y1 = (int32_t)(cy + r + 1.0f), x, y;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++) {
            float dx = (float)x + 0.5f - cx, dy = (float)y + 0.5f - cy, d = fm_sqrtf(dx * dx + dy * dy);
            px_blend(x, y, c, (uint32_t)(fm_clampf(r - d + 0.5f, 0.0f, 1.0f) * 16.0f + 0.5f));
        }
}

/* atan2 in turns, 0..1 from +x going clockwise on screen (y down) */
static float turns(float y, float x)
{
    float ax = fm_fabsf(x), ay = fm_fabsf(y), a = ax < ay ? ax / (ay + 1e-9f) : ay / (ax + 1e-9f), s = a * a, r;
    r = ((-0.0464964749f * s + 0.15931422f) * s - 0.327622764f) * s * a + a;   /* atan on [0, 1] */
    if (ay > ax)
        r = 1.57079637f - r;
    if (x < 0.0f)
        r = FM_PI - r;
    if (y < 0.0f)
        r = FM_TWO_PI - r;
    return r * (1.0f / FM_TWO_PI);
}

/* a knob: a 270-degree track from 7:30 to 4:30, anti-aliased, round ends; the value filled from
 * the start (or from the top, for a centred value) and a round handle */
static void arc(int32_t cx, int32_t cy, int32_t r, float v, uint16_t track, uint16_t fill, uint16_t bg, int bipolar)
{
    const float th_ = 3.4f, a0 = 0.375f, sweep = 0.75f;
    float ro = (float)r, ri = ro - th_, rm = ro - th_ * 0.5f, fx = (float)cx + 0.5f, fy = (float)cy + 0.5f, lo, hi;
    int32_t x, y;
    v = fm_clampf(v, 0.0f, 1.0f);
    lo = bipolar ? (v < 0.5f ? v : 0.5f) : 0.0f;
    hi = bipolar ? (v < 0.5f ? 0.5f : v) : v;
    for (y = -r - 1; y <= r + 1; y++)
        for (x = -r - 1; x <= r + 1; x++) {
            float dx = (float)x, dy = (float)y, d = fm_sqrtf(dx * dx + dy * dy), t, a;
            a = fm_clampf(ro + 0.5f - d, 0.0f, 1.0f) * fm_clampf(d - ri + 0.5f, 0.0f, 1.0f);
            if (a <= 0.0f)
                continue;
            t = turns(dy, dx) - a0;
            if (t < 0.0f)
                t += 1.0f;
            t /= sweep;
            if (t > 1.0f)
                continue;
            px_blend(cx + x, cy + y, (t >= lo && t <= hi) ? fill : track, (uint32_t)(a * 16.0f + 0.5f));
        }
    {
        float e0 = a0 * FM_TWO_PI, e1 = (a0 + sweep) * FM_TWO_PI, ev = (a0 + sweep * v) * FM_TWO_PI;
        float el = (a0 + sweep * lo) * FM_TWO_PI;
        disc(fx + fm_cosf(e0) * rm - 0.5f, fy + fm_sinf(e0) * rm - 0.5f, th_ * 0.5f, lo <= 0.0f && hi > 0.0f ? fill : track);
        disc(fx + fm_cosf(e1) * rm - 0.5f, fy + fm_sinf(e1) * rm - 0.5f, th_ * 0.5f, hi >= 1.0f ? fill : track);
        if (hi > lo)
            disc(fx + fm_cosf(el) * rm - 0.5f, fy + fm_sinf(el) * rm - 0.5f, th_ * 0.5f, fill);
        disc(fx + fm_cosf(ev) * rm - 0.5f, fy + fm_sinf(ev) * rm - 0.5f, th_ * 0.5f + 1.3f, bg);
        disc(fx + fm_cosf(ev) * rm - 0.5f, fy + fm_sinf(ev) * rm - 0.5f, th_ * 0.5f + 0.6f, fill);
    }
}

static void caps(char *d, const char *s, int n)  /* s in capitals, at most n - 1 characters */
{
    int i;
    for (i = 0; i < n - 1 && s[i]; i++)
        d[i] = (char)(s[i] >= 'a' && s[i] <= 'z' ? s[i] - 32 : s[i]);
    d[i] = 0;
}

static uint16_t mix(uint16_t a, uint16_t b, int t)   /* t 0..16: a -> b */
{
    int r, g, bl;
    if (t <= 0)
        return a;
    if (t >= 16)
        return b;
    r = ((a >> 11) * (16 - t) + (b >> 11) * t) / 16;
    g = (((a >> 5) & 63) * (16 - t) + ((b >> 5) & 63) * t) / 16;
    bl = ((a & 31) * (16 - t) + (b & 31) * t) / 16;
    return (uint16_t)(r << 11 | g << 5 | bl);
}
static int q16(float x) { return x <= 0.0f ? 0 : x >= 1.0f ? 16 : (int)(x * 16.0f + 0.5f); }

/* the header: the piece in a tag, its root and mode; on the right, the tempo with a beat light (a play
 * triangle while stopped), the section and a dot while the kit is unsaved. Messages take its place
 * for a moment. */
static void draw_header(void)
{
    char t[24];
    int msg = plat_ms() < ui.msg_until;
    uint32_t h = hash(hash(hash(2166136261u, proj.piece | (uint32_t)proj.par[P_ROOT] << 8 | (uint32_t)proj.pool << 12),
                           en_playing | (uint32_t)en_sec << 2 | (uint32_t)en_sec_req << 9 | (uint32_t)msg << 5 |
                               (uint32_t)en_ext << 6 | (uint32_t)proj.par[P_MODE] << 12 | (uint32_t)project_pending() << 16 |
                               (uint32_t)(proj.edited >> proj.piece & 1u) << 17),
                      (uint32_t)proj.par[P_TEMPO] | (uint32_t)ui.theme << 8 |
                          (uint32_t)(ui.fx_lock != 0 || ui.dj_lock != 0) << 12);
    if (msg)
        h = hash(hash(h, (uint32_t)ui.msg[0][0] << 8 | ui.msg[0][1]), ui.msg_until);
    if (en_playing)
        h = hash(h, (en_bars / 4u) & 1u);
    if (h == ui.sig[0])
        return;
    ui.sig[0] = h;
    cv_begin(240, HDR_H, K_BG);
    if (msg) {
        int32_t x = cv_text(10, 6, &FONT_B, ui.msg[0], K_TEXT);
        cv_text(x + 8, 6, &FONT_B, ui.msg[1], K_ACC);
    } else {
        int32_t x = 232, w;
        if (project_pending()) {                   /* not yet in flash (the theme, Tune, MIDI out: two seconds on) */
            cv_round(x - 5, 12, 5, 5, 2, K_DIM);
            x -= 11;
        }
        if (en_sec != EN_SEC_GROOVE || en_sec_req) {   /* the section, or the one coming on the bar */
            const char *sn = en_sec == EN_SEC_BREAK ? "BREAK" : en_sec == EN_SEC_BUILD ? "BUILD" :
                             en_sec_req == EN_CMD_BREAK + 1 ? "BREAK>" : "";
            if (en_sec != EN_SEC_GROOVE && en_sec_req == EN_CMD_DROP + 1)
                sn = "DROP>";
            else if (en_sec == EN_SEC_BREAK && en_sec_req == EN_CMD_BUILD + 1)
                sn = "BUILD>";
            if (*sn) {
                x -= text_w(&FONT_XS, sn);
                cv_text(x, 8, &FONT_XS, sn, K_FROST);
                x -= 10;
            }
        }
        if ((ui.fx_lock || ui.dj_lock) && !ui.fx_menu) {   /* effects locked on, the menu closed: they play on */
            x -= text_w(&FONT_XS, "FX");
            cv_text(x, 8, &FONT_XS, "FX", K_PAD);
            x -= 10;
        }
        if (en_ext) {
            t[0] = 'E';
            t[1] = 'X';
            t[2] = 'T';
            t[3] = 0;
        } else {
            itoa_u((uint32_t)proj.par[P_TEMPO], t);
        }
        x -= text_w(&FONT_S, t);
        cv_text(x, 6, &FONT_S, t, en_playing ? K_TEXT : K_DIM);
        x -= 16;
        if (en_playing) {                         /* the beat light */
            int on = ((en_bars / 4u) & 1u) == 0u;
            cv_round(x, 9, 10, 10, 2, on ? K_ACC : K_LINE);
        } else {
            int32_t j;                            /* stopped: a play triangle */
            for (j = 0; j < 5; j++)
                cv_rect(x + 2 + j, 9 + j, 1, 11 - 2 * j, K_DIM);
        }
        w = text_w(&FONT_B, piece_name(proj.piece)) + 12;   /* the piece, in a tag with a cut corner; a */
        if (proj.edited >> proj.piece & 1u)                   /* mark in it while it has unstored changes */
            w += 7;
        cv_rect(4, 4, w, 20, K_PAD);
        cv_rect(4 + w - 4, 4, 4, 1, K_BG);
        cv_rect(4 + w - 3, 5, 3, 1, K_BG);
        cv_rect(4 + w - 2, 6, 2, 1, K_BG);
        cv_rect(4 + w - 1, 7, 1, 1, K_BG);
        cv_text(10, 5, &FONT_B, piece_name(proj.piece), K_BG);
        if (proj.edited >> proj.piece & 1u)
            cv_round(4 + w - 10, 12, 4, 4, 1, K_BG);
        w = cv_text(w + 10, 8, &FONT_XS, EN_NOTE_NAME[proj.par[P_ROOT]], K_TEXT);
        cv_text(w + 3, 8, &FONT_XS, mode_name(), K_DIM);
    }
    cv_blit(0, 0);
}

/* the flow: the scale as a ring of twelve, the chord lit in it; the acid loop around it, its steps (the
 * notes, the accents bigger) and its playhead; the layers' levels on the left; the drum pattern on the
 * right, sixteen steps by six lanes, the playhead going across */
#define CX 112
#define CY 74
static void polar(int32_t cx, int32_t cy, float r, int i, int n, int32_t *x, int32_t *y)
{
    float a = (float)i * (FM_TWO_PI / (float)n);  /* step i of n, clockwise from the top */
    float sx = fm_sinf(a) * r, sy = fm_cosf(a) * r;
    *x = cx + (int32_t)(sx + (sx < 0.0f ? -0.5f : 0.5f));
    *y = cy - (int32_t)(sy + (sy < 0.0f ? -0.5f : 0.5f));
}

/* outer is the acid loop. inner is the scale. The HYP ring passes 46 and 28, so its dots stay put. */
static void draw_ring_at(int32_t cx, int32_t cy, float outer, float inner)
{
    uint32_t abs_pool = pool_abs(), chord = 0;
    int i, len = en_acid_len ? en_acid_len : 16, s, g = q16(en_acid_glow);
    int ow = (int)(outer + 0.5f);
    for (i = 0; i < EN_NPAD; i++)
        chord |= 1u << (en_chord[i] % 12u);
    for (s = 0; s < len; s++) {                 /* the acid loop */
        int32_t x, y;
        int cur = en_playing && s == en_acid_pos;
        polar(cx, cy, outer, s, len, &x, &y);
        if (en_acid_gate >> s & 1u)
            cv_disc(x, y, (en_acid_acc >> s & 1u) ? 3 : 2, cur ? mix(K_ACID, K_ACC, g) : mix(K_ACID_T, K_ACID, 10));
        else
            cv_rect(x, y, 2, 2, cur ? K_ACC : K_LINE);
    }
    for (i = 0; i < 12; i++) {                  /* the scale: C at the top */
        int32_t x, y;
        int in = (int)(abs_pool >> i & 1u), ch = (int)(chord >> i & 1u);
        polar(cx, cy, inner, i, 12, &x, &y);
        if (i == proj.par[P_ROOT])
            cv_disc(x, y, 5, K_TEXT);           /* the root: ringed */
        if (ch)
            cv_disc(x, y, 4, mix(K_PAD_T, K_PAD, 6 + q16(en_pad_level) * 10 / 16));
        else if (in)
            cv_disc(x, y, 3, mix(K_PAD_T, K_PAD, 7));
        else
            cv_disc(x, y, 1, K_LINE);
    }
    {   /* the chord's name, in the middle. The big face fits the hole of a radius-46 ring at width 40. */
        char nm[16];
        int pcs[EN_NPAD + 1], k;
        pcs[0] = en_chord_root;
        for (k = 0; k < EN_NPAD; k++)
            pcs[k + 1] = en_chord[k] % 12;
        en_chord_name(pcs, EN_NPAD + 1, nm);
        if (text_w(&FONT_B, nm) * 46 <= ow * 40)
            text_c(cx, cy - 8, &FONT_B, nm, en_frozen ? K_FROST : K_TEXT);
        else
            text_c(cx, cy - 6, &FONT_XS, nm, en_frozen ? K_FROST : K_TEXT);
    }
}
static void draw_ring(void) { draw_ring_at(CX, CY, 46.0f, 28.0f); }

/* Five levels, pad noise drone acid kit. x, y, bar width, gap, and bar height.
 * The HYP screen calls this as (6, 20, 5, 4, 96): bars on a stride of 9, labels on the bar's x. */
static void draw_meters_box(int32_t x0, int32_t y0, int32_t bw, int32_t gap, int32_t bh)
{
    static const char *const L[5] = {"P", "N", "D", "A", "K"};
    uint16_t c[5], t[5];
    float lv[5], dm = 0.0f;
    int i;
    int32_t room = bh > 4 ? bh - 4 : bh;
    c[0] = K_PAD, c[1] = K_DIM, c[2] = K_BASS, c[3] = K_ACID, c[4] = K_PULSE;
    t[0] = K_PAD_T, t[1] = K_LINE, t[2] = K_BASS_T, t[3] = K_ACID_T, t[4] = K_PULSE_T;
    for (i = 0; i < EN_NDRUM; i++)
        dm = fm_maxf(dm, en_drum_glow[i]);
    lv[0] = en_pad_level * (float)proj.par[P_PAD] * 0.01f;
    lv[1] = en_noise_level;
    lv[2] = en_bass_level * (float)proj.par[P_BASS] * 0.01f;
    lv[3] = en_acid_level * (float)proj.par[P_ACID] * 0.01f;
    lv[4] = dm * (float)proj.par[P_PULSE] * 0.01f;
    for (i = 0; i < 5; i++) {
        int32_t x = x0 + i * (bw + gap);
        int32_t h = (int32_t)(fm_clampf(lv[i], 0.0f, 1.0f) * (float)room + 0.5f);
        int32_t lx = x;
        cv_rect(x, y0, bw, bh, t[i]);
        if (h > 0)
            cv_rect(x, y0 + bh - h, bw, h, c[i]);
        if (bw >= 8) {                          /* a wide bar centres its letter; a thin one stays left */
            int32_t tw = text_w(&FONT_XS, L[i]);
            lx = x + (bw - tw) / 2;
        }
        cv_text(lx, y0 + bh + 4, &FONT_XS, L[i], K_DIM);
    }
}
static void draw_meters(void) { draw_meters_box(6, 20, 5, 4, 96); }

static void draw_kit(void)                       /* right: the pattern playing, small */
{
    int l, s, pos = en_drum_pos, pat = en_pat_now;
    char t[12] = "PAT ";
    for (l = 0; l < EN_NLANE; l++) {
        uint16_t bits = en_pats[pat][l];
        int g = q16(en_drum_glow[l]);
        for (s = 0; s < 16; s++) {
            int32_t x = 170 + s * 4, y = 26 + l * 9;
            int on = (int)(bits >> s & 1u), cur = en_playing && s == pos;
            if (on)
                cv_rect(x, y, 3, 6, en_pulse_rest ? K_LINE : cur ? K_ACC : mix(K_PULSE_T, K_PULSE, 8 + g / 2));
            else
                cv_rect(x + 1, y + 2, 1, 2, cur ? K_DIM : K_LINE);
        }
    }
    itoa_u((uint32_t)pat + 1u, t + 4);
    cv_text(170, 84, &FONT_XS, t, K_TEXT);
    if (en_pat_next != en_pat_now) {             /* the next, waiting for the bar */
        char u[8] = "> ";
        itoa_u((uint32_t)en_pat_next + 1u, u + 2);
        cv_text(206, 84, &FONT_XS, u, K_ACC);
    }
}

/* the sequencer: six lanes of sixteen steps, the lane the keys edit on a tinted band; the playhead a
 * tinted column; the pattern edited above, and the one playing if they differ */
static void draw_seq(void)
{
    int l, s, pat = seq_pat(), pos = en_drum_pos;
    char t[24] = "PATTERN ";
    itoa_u((uint32_t)pat + 1u, t + 8);
    cv_text(6, 1, &FONT_XS, t, K_ACC);
    if (en_pat_now != pat || en_pat_next != pat) {
        char u[24] = "PLAYING ";
        itoa_u((uint32_t)en_pat_now + 1u, u + 8);
        cv_text(120, 1, &FONT_XS, u, K_DIM);
    }
    for (l = 0; l < EN_NLANE; l++) {
        uint16_t bits = en_pats[pat][l];
        int32_t y = 18 + l * 20, sel = l == ui.lane;
        if (sel)                                /* the keys' lane */
            cv_rect(0, y + 1, 240, 17, K_PANEL);
        cv_text(4, y + 3, &FONT_XS, LANE_SHORT[l], sel ? K_TEXT : K_DIM);
        if (sel)
            cv_rect(1, y + 4, 2, 9, K_ACC);
        for (s = 0; s < 16; s++) {
            int32_t x = 44 + s * 12;
            int on = (int)(bits >> s & 1u), cur = en_playing && s == pos && en_pat_now == pat;
            if (cur)
                cv_rect(x - 1, y, 12, 18, sel ? K_LINE : K_PANEL);
            if (on)
                cv_rect(x + 1, y + 3, 9, 12, sel ? (cur ? K_ACC : K_PULSE) : mix(K_PULSE_T, K_PULSE, 9));
            else
                cv_rect(x + 4, y + 8, 3, 3, (s & 3) == 0 ? K_DIM : K_LINE);
        }
    }
    for (s = 1; s < 4; s++)                     /* the beats */
        cv_rect(44 + s * 48 - 2, 18, 1, 118, K_LINE);
}

/* a button's card (help_tick): its name, then what it does, in the main area's place */
#define HELP_LINES 4
static const char *const BTN_LABEL[NB] = {"FX", "SEL", "ENV", "LFO", "EDIT", "GLO", "HOME", "SAVE", "ARP", "SEQ",
                                          "PLAY", "REC", "OCT-", "OCT+"};
static int help_lines(int b, const char *l[HELP_LINES])
{
    int n = 0;
#define L(t) (l[n++] = (t))
    switch (b) {
    case B_HOME:
        L("TEMPO");
        L(iam_song() ? "TEMPO, EVOLVE, KEYS, DRIFT" : "TEMPO, EVOLVE, MOTION, DRIFT");
        L("+ SELECT: TEMPO");
        break;
    case B_ENV: L("PAD; AGAIN: ENV, LFO,"); L("THEN THE CHORDS"); L("SEL + ENV ON A VOICE'S"); L("PAGE: PLAY IT BY HAND"); break;
    case B_ARP:
        if (iam_song()) {
            L("ARP: ON OR OFF"); L("HELD KEYS, ONE NOTE"); L("LOW TO HIGH, THE BEAT"); L("SELECT STILL FINDS ACID");
        } else {
            L("THE ACID; AGAIN: ITS"); L("ENVELOPE, THEN THE LINE"); L("SEL + ENV: BY HAND"); L("SEL + ALGO: ITS SOUND");
        }
        break;
    case B_LFO: L("THE DRONE; AGAIN: ITS"); L("ENVELOPE, THEN ITS LFO"); L("SEL + ENV: BY HAND"); L("SEL + ALGO: ITS SOUND"); break;
    case B_GLO: {                                /* and the audio's load, for testing on the device */
        static char cpu[24];
        uint32_t c = plat_cpu_pct(), x = plat_xruns(), j = 4;
        cpu[0] = 'C', cpu[1] = 'P', cpu[2] = 'U', cpu[3] = ' ';
        itoa_u(c, cpu + j);
        while (cpu[j])
            j++;
        cpu[j++] = '%', cpu[j++] = ' ', cpu[j++] = ' ', cpu[j++] = 'X', cpu[j++] = 'R', cpu[j++] = 'U', cpu[j++] = 'N', cpu[j++] = 'S',
        cpu[j++] = ' ';
        itoa_u(x > 9999u ? 9999u : x, cpu + j);
        L("GLOBAL");
        L("ROOT, THEME, TUNE, MIDI OUT");
        L("+ SEL: REMOTE (BROWSER)");
        L(cpu);
        break;
    }
    case B_EDIT:
        if (iam_song()) {
            L("MUTATE THE SONG"); L("HEARD NOW");
        } else {
            L("THE NEXT CHORD,"); L("ON THE NEXT BAR");
        }
        break;
    case B_SEQ: L("DRUMS; AGAIN: LINKS, KIT,"); L("THEN DRUM DRIFT"); L("HOLD: THE SEQUENCER"); break;
    case B_REC:
        if (iam_song()) {
            L("LOOP THE WHITE KEYS"); L("THE LAST FOUR BARS"); L("AGAIN: THE LOOP PLAYS"); L("STAYS UNTIL REC");
        } else {
            L("FREEZE: THE CHORD HOLDS"); L("AND THE REVERB RINGS ON"); L("AGAIN: THAW");
        }
        break;
    case B_OCTDN: L("AN OCTAVE DOWN: THIS"); L("PAGE'S VOICE, OR THE SONG"); L("SEQUENCER: THE DRUM ABOVE"); break;
    case B_OCTUP: L("AN OCTAVE UP: THIS"); L("PAGE'S VOICE, OR THE SONG"); L("SEQUENCER: THE DRUM BELOW"); break;
    default: break;
    }
#undef L
    return n;
}

static void draw_help(void)
{
    const char *l[HELP_LINES];
    int n = help_lines(ui.help - 1, l), k;
    cv_round(16, 14, 208, 116, 6, K_PANEL);
    cv_rect(16, 14, 208, 26, K_PAD);
    text_c(120, 18, &FONT_B, BTN_LABEL[ui.help - 1], K_BG);
    for (k = 0; k < n; k++)
        text_c(120, 56 + k * 20, &FONT_S, l[k], K_TEXT);
}

/* The name, centred. A box that cannot hold the whole word keeps the letters that fit. */
static void fx_name(int x, int y, int w, int h, const char *s, uint16_t c)
{
    char b[16];
    int n = 0;
    int32_t limit = w - 4, tw, tx, ty;
    if (limit < 8 || !s || !s[0])
        return;
    tw = text_w(&FONT_XS, s);
    if (tw > limit) {
        while (s[n] && n < (int)sizeof b - 1) {
            b[n] = s[n];
            b[n + 1] = 0;
            if (text_w(&FONT_XS, b) > limit) {
                b[n] = 0;
                break;
            }
            n++;
        }
        s = b;
        tw = text_w(&FONT_XS, s);
    }
    tx = (w - 4 - tw) / 2;
    if (tx < 0)
        tx = 0;
    ty = y + (h - (int)FONT_XS.h) / 2;
    if (ty < y)
        ty = y;
    cv_text(x + 2 + tx, ty, &FONT_XS, s, c);
}
/* One key. Held fills the bar on the left, the same way a lit song key does. */
static void fx_box(int x, int y, int w, int h, int key)
{
    int d = glo_dj(key), on, locked;
    const char *lab;
    uint16_t fg;
    if (d >= 0) {
        on = (int)((ui.dj_mask >> d) & 1u);
        locked = (int)((ui.dj_lock >> d) & 1u);
        lab = DJ_LABEL[d];
    } else {
        on = (int)((ui.fx_mask >> KEY_FX[key]) & 1u);
        locked = (int)((ui.fx_lock >> KEY_FX[key]) & 1u);
        lab = FX_LABEL[KEY_FX[key]];
    }
    fg = on ? (locked ? K_PAD : K_ACC) : K_DIM;
    cv_rect(x, y, w, h, K_LINE);
    if (w > 2 && h > 2)
        cv_rect(x + 1, y + 1, w - 2, h - 2, on ? K_PANEL : K_BG);
    if (on && h > 2)
        cv_rect(x + 1, y + 1, 2, h - 2, fg);
    fx_name(x, y, w, h, lab, fg);
}
/* GLO: the song screen's boxes. Black keys above, five then six, a bar after
 * the third, the eighth and the tenth. White keys below, four to a row. */
static void draw_fx_grid(void)
{
    static const int top_w[5] = {52, 54, 56, 42, 33};
    static const int bot_w[6] = {31, 42, 49, 29, 38, 47};
    int i, x, row, y = 16, h = 20;
    for (i = 0, x = 0; i < 5; i++) {
        fx_box(x, y, top_w[i], h, BLACK_KEY[i]);
        x += top_w[i];
        if (i == 2) {
            cv_rect(x, y, 3, h, K_TEXT);
            x += 3;
        }
    }
    y += h + 1;
    for (i = 0, x = 0; i < 6; i++) {
        fx_box(x, y, bot_w[i], h, BLACK_KEY[5 + i]);
        x += bot_w[i];
        if (i == 2 || i == 4) {          /* two bars: the row is full once the names fit */
            cv_rect(x, y, 2, h, K_TEXT);
            x += 2;
        }
    }
    y += h + 3;
    for (row = 0; row < 4; row++) {
        for (i = 0, x = 0; i < 4; i++) {
            fx_box(x, y, 60, h, WHITE_KEY[row * 4 + i]);
            x += 60;
        }
        y += h + 1;
    }
}
/* the FX menu (FX on). GLO uses the key grid. HYP lists the note beside the effect. */
static void draw_fx(void)
{
    int k;
    cv_text(4, 1, &FONT_XS, ui.lock_mode ? "LOCK: TAP KEYS" : "FX: HOLD A KEY", ui.lock_mode ? K_PAD : K_ACC);
    if (ui.lock_mode)
        cv_text(118, 1, &FONT_XS, "TAP AGAIN: OFF", K_PAD);
    else if (ui.fx_lock || ui.dj_lock)
        cv_text(118, 1, &FONT_XS, "LOCKED: SAVE+KEY", K_PAD);
    else if (iam_song())
        cv_text(118, 1, &FONT_XS, "CROWD KEYS", K_DIM);
    else
        cv_text(118, 1, &FONT_XS, "M: MUTE  S: SOLO", K_DIM);
    if (iam_song()) {
        draw_fx_grid();
        return;
    }
    for (k = 0; k < NKEYS; k++) {
        int32_t x = 2 + (k / 9) * 79, y = 16 + (k % 9) * 14;
        int d = glo_dj(k), pc = (5 + k) % 12, oct = 3 + (5 + k) / 12, on, locked;
        int black = pc == 1 || pc == 3 || pc == 6 || pc == 8 || pc == 10;
        char t[24];
        int n = 0;
        const char *nm = EN_NOTE_NAME[pc];
        const char *lab;
        if (d >= 0) {
            on = (int)((ui.dj_mask >> d) & 1u);
            locked = (int)((ui.dj_lock >> d) & 1u);
            lab = DJ_LABEL[d];
        } else {
            on = (int)(ui.fx_mask >> KEY_FX[k] & 1u);
            locked = (int)(ui.fx_lock >> KEY_FX[k] & 1u);
            lab = FX_LABEL[KEY_FX[k]];
        }
        while (*nm)
            t[n++] = *nm++;
        t[n++] = (char)('0' + oct);
        t[n] = 0;
        if (on)
            cv_rect(x - 1, y - 1, 77, 14, locked ? K_PAD : K_ACC);   /* locked: the primary; held: the accent */
        cv_text(x, y, &FONT_XS, t, on ? K_BG : black ? K_DIM : K_TEXT);
        cv_text(x + 24, y, &FONT_XS, lab, on ? K_BG : K_TEXT);
    }
}

#include "runner.c"
static int runner_on(void) { return kr.on; }

/* The synth the white keys play, and its sound, for the knob row. A held effect
 * takes the sound's place. *hand is set when that voice is off the algorithm. */
static void iam_keys_caption(char *u, int n, uint16_t *col, int *hand)
{
    static const char *const WHO[3] = {"PAD", "DRONE", "ACID"};
    static const int SND[3] = {P_PADTONE, P_DR_INST, P_AC_WAVE};
    static const char *const *const BANK[3] = {EN_PAD_SOUND, EN_DRONE_SOUND, EN_ACID_SOUND};
    int vc, snd, i, nfx = 0, ndj = 0, hot, first = -1, fdj = -1, j = 0;
    char sound[16];
    const char *line, *w;
    vc = iam_jam_voice();
    if (vc < 0 || vc > 2)
        vc = EN_V_PAD;
    snd = proj.par[SND[vc]];
    if ((unsigned)snd >= (unsigned)en_bank_n(vc))
        snd = 0;
    caps(sound, BANK[vc][snd], (int)sizeof sound);
    for (i = 0; i < EN_NFX; i++)
        if ((ui.fx_mask >> i) & 1u) {
            if (first < 0)
                first = i;
            nfx++;
        }
    for (i = 0; i < 14; i++)
        if ((ui.dj_mask >> i) & 1u) {
            if (fdj < 0)
                fdj = i;
            ndj++;
        }
    hot = nfx + ndj;
    if (hot == 1 && ndj == 1)
        line = DJ_LABEL[fdj];
    else if (hot == 1)
        line = FX_LABEL[first];
    else if (hot > 1) {
        sound[0] = (char)('0' + (hot > 9 ? 9 : hot));
        sound[1] = ' ';
        sound[2] = 'F';
        sound[3] = 'X';
        sound[4] = 0;
        line = sound;
    } else {
        line = sound;
    }
    w = WHO[vc];
    if (n < 2) {
        if (n > 0)
            u[0] = 0;
        *col = K_TEXT;
        *hand = 0;
        return;
    }
    while (*w && j < n - 1)
        u[j++] = *w++;
    if (j < n - 1)
        u[j++] = ' ';
    while (*line && j < n - 1)
        u[j++] = *line++;
    u[j] = 0;
    *col = hot ? K_ACC : vc == EN_V_DRONE ? K_BASS : vc == EN_V_ACID ? K_ACID : K_PAD;
    *hand = !hot && by_hand(vc);
}

/* GLO, on an IAM song. The four beats run across the bottom, so a take can land
 * on the bar. The level meters sit on the left, the chord ring on the right.
 * Bar, phrase, and step share the top line. The synth the keys play is named
 * on the knob row, under the beats. */
static void draw_iam_bars(void)
{
    uint32_t step = en_bars & 15u, phrase = (en_bars / 16u) & 3u;
    int beat = (int)(step >> 2), sub = (int)(step & 3u), i, rec, looping;
    char n[8];
    const char *lab;
    int32_t tw;
    rec = iam_loop_recording();
    looping = iam_loop_playing();
    lab = rec ? "REC" : looping ? "LOOP" : en_playing ? "BAR" : "STOP";
    tw = cv_text(6, 2, &FONT_XS, lab, rec ? K_ACC : looping ? K_PAD : K_DIM);
    n[0] = (char)('1' + phrase);
    n[1] = 0;
    cv_text(tw + 4, 2, &FONT_XS, n, !en_playing ? K_DIM : step == 0u ? K_ACC : K_TEXT);
    n[0] = 'S';
    n[1] = 'T';
    n[2] = 'E';
    n[3] = 'P';
    n[4] = ' ';
    n[5] = (char)('0' + (step + 1u) / 10u);
    n[6] = (char)('0' + (step + 1u) % 10u);
    n[7] = 0;
    tw = text_w(&FONT_XS, n);
    cv_text(234 - tw, 2, &FONT_XS, n, !en_playing ? K_DIM : (step & 3u) == 0u ? K_ACC : K_TEXT);
    draw_meters_box(8, 18, 14, 8, 68);          /* left: shorter than the HYP meters, clear of the beats */
    draw_ring_at(176, 62, 40.0f, 40.0f * (28.0f / 46.0f));
    for (i = 0; i < 4; i++) {
        int32_t x = 12 + i * 58, w = (sub + 1) * 46 / 4;
        cv_rect(x, 108, 50, 28, K_LINE);
        cv_rect(x + 2, 110, 46, 24, K_BG);
        if (!en_playing)
            continue;
        if (i < beat)
            cv_rect(x + 2, 110, 46, 24, K_DIM);
        else if (i == beat)
            cv_rect(x + 2, 110, w > 46 ? 46 : w, 24, K_ACC);
    }
}

static void draw_main(void)
{
    uint32_t h = hash(2166136261u, pool_abs() | (uint32_t)proj.par[P_ROOT] << 12 | (uint32_t)en_frozen << 16 | proj.piece << 24 |
                                        (uint32_t)en_playing << 17 | (uint32_t)proj.par[P_MODE] << 20), i;
    if (ui.frame % 2u)                           /* 30 frames a second is plenty */
        return;
    h = hash(h, ui.theme);
    if (ui.fx_menu) {
        h = hash(hash(0x46584D4Eu, ui.fx_mask ^ (uint32_t)ui.theme << 28 ^ (uint32_t)ui.lock_mode << 31), ui.fx_lock);
        h = hash(h, (uint32_t)ui.dj_mask | (uint32_t)ui.dj_lock << 16);
        if (h != ui.sig[1]) {
            ui.sig[1] = h;
            cv_begin(240, MAIN_H, K_BG);
            draw_fx();
            cv_blit(0, MAIN_Y);
        }
        return;
    }
    if (ui.help) {
        h = hash(0x48454C50u, ui.help | (uint32_t)ui.theme << 8);
        if (ui.help - 1 == B_GLO)                /* the load, as it changes */
            h = hash(h, plat_cpu_pct() | plat_xruns() << 8);
        if (h != ui.sig[1]) {
            ui.sig[1] = h;
            cv_begin(240, MAIN_H, K_BG);
            draw_help();
            cv_blit(0, MAIN_Y);
        }
        return;
    }
    for (i = 0; i < EN_NLANE; i++)
        h = hash(h, en_pats[ui.view == V_SEQ ? seq_pat() : en_pat_now][i] | (uint32_t)q16(en_drum_glow[i]) << 16);
    h = hash(h, en_drum_pos | (uint32_t)en_pat_now << 8 | (uint32_t)en_pat_next << 12 | (uint32_t)en_pulse_rest << 16 |
                    (uint32_t)ui.view << 20);
    if (ui.view == V_SEQ) {
        h = hash(h, ui.lane | (uint32_t)proj.par[P_PATTERN] << 16);
        if (h == ui.sig[1])
            return;
        ui.sig[1] = h;
        cv_begin(240, MAIN_H, K_BG);
        draw_seq();
        cv_blit(0, MAIN_Y);
        return;
    }
    if (iam_song()) {
        uint32_t bars = en_bars;
        int vc = iam_jam_voice();
        h = hash(2166136261u, (bars & 15u) | ((bars / 16u) & 3u) << 8 | (uint32_t)en_playing << 12 |
                                  (uint32_t)iam_loop_recording() << 13 | (uint32_t)iam_loop_playing() << 14 |
                                  (uint32_t)ui.theme << 16 | (uint32_t)ui.view << 20);
        h = hash(h, (uint32_t)proj.par[P_PAD] | (uint32_t)proj.par[P_BASS] << 8 |
                        (uint32_t)proj.par[P_ACID] << 16 | (uint32_t)vc << 24);
        h = hash(h, (uint32_t)proj.par[P_PADTONE] | (uint32_t)proj.par[P_DR_INST] << 8 |
                        (uint32_t)proj.par[P_AC_WAVE] << 16);
        h = hash(h, (uint32_t)(proj.par[P_PAD_MAN] | proj.par[P_DR_MAN] << 1 | proj.par[P_AC_MAN] << 2));
        h = hash(h, ui.fx_mask);
        h = hash(h, ui.dj_mask);
        h = hash(h, pool_abs());
        h = hash(h, (uint32_t)proj.par[P_ROOT] | (uint32_t)en_chord_root << 8 | (uint32_t)en_frozen << 16);
        for (i = 0; i < EN_NPAD; i++)
            h = hash(h, en_chord[i]);
        h = hash(h, en_acid_gate | (uint32_t)en_acid_acc << 16);
        h = hash(h, en_acid_pos | (uint32_t)en_acid_len << 8 | (uint32_t)q16(en_acid_glow) << 16);
        h = hash(h, (uint32_t)q16(en_pad_level) | (uint32_t)q16(en_bass_level) << 8 |
                        (uint32_t)q16(en_acid_level) << 16 | (uint32_t)q16(en_noise_level) << 24);
        {
            uint32_t g = 0;
            for (i = 0; i < EN_NDRUM; i++)
                g |= (uint32_t)q16(en_drum_glow[i]) << (5u * i);
            h = hash(h, g);
        }
        h = hash(h, (uint32_t)proj.par[P_NOISE] | (uint32_t)proj.par[P_PULSE] << 8);
        if (h == ui.sig[1])
            return;
        ui.sig[1] = h;
        cv_begin(240, MAIN_H, K_BG);
        draw_iam_bars();
        cv_blit(0, MAIN_Y);
        return;
    }
    for (i = 0; i < EN_NPAD; i++)
        h = hash(h, en_chord[i]);
    h = hash(h, en_acid_gate | (uint32_t)en_acid_acc << 16);
    h = hash(h, en_acid_pos | (uint32_t)en_acid_len << 8 | (uint32_t)q16(en_acid_glow) << 16);
    h = hash(h, (uint32_t)(en_drift * 40.0f) | (uint32_t)q16(en_pad_level) << 8 | (uint32_t)q16(en_bass_level) << 13 |
                    (uint32_t)q16(en_acid_level) << 18 | (uint32_t)q16(en_noise_level) << 23);
    for (i = 0; i < 5; i++)
        h = hash(h, (uint32_t)proj.par[i]);
    if (h == ui.sig[1])
        return;
    ui.sig[1] = h;
    cv_begin(240, MAIN_H, K_BG);
    if (en_frozen) {                             /* the way to the next chord, top right (held: frozen) */
        cv_text(232 - text_w(&FONT_XS, "held"), 4, &FONT_XS, "held", K_FROST);
    } else {
        int32_t w = 30, f = (int32_t)(en_drift * (float)w + 0.5f);
        cv_rect(232 - w, 8, w, 4, K_LINE);
        if (f > 0)
            cv_rect(232 - w, 8, f, 4, K_PAD);
    }
    draw_ring();
    draw_meters();
    draw_kit();
    cv_blit(0, MAIN_Y);
}

/* the knob strip (X0X's): a rule with the page's name and a dot a page at its right end; under it a
 * cell a knob: its name, a ring (or pips, for a switch of a few positions), the value. The knob being
 * turned is drawn on a tint. Only redrawn when what it shows changes. */
static void draw_knobs(void)
{
    uint32_t h = hash(2166136261u, ui.view | (uint32_t)ui.touched << 8 | (uint32_t)ui.lane << 16 | (uint32_t)ui.theme << 24), i;
    h = hash(h, (uint32_t)proj.par[P_PADTONE] | (uint32_t)proj.par[P_DR_INST] << 3 | (uint32_t)proj.par[P_AC_WAVE] << 6 |
                (uint32_t)(proj.par[P_KIT_KICK] | proj.par[P_KIT_SNARE] << 3 | proj.par[P_KIT_HAT] << 6) << 9 |
                (uint32_t)proj.par[P_KIT_PERC] << 18);
    h = hash(h, (uint32_t)(proj.par[P_PAD_MAN] | proj.par[P_DR_MAN] << 1 | proj.par[P_AC_MAN] << 2) << 24 |
                (uint32_t)proj.par[P_LINKS] << 8 |
                    (uint32_t)(proj.par[oct_par(ui.view)] + 8) << 12);
    if (iam_song()) {                            /* the knob row names the synth the keys play */
        h = hash(h, (uint32_t)proj.par[P_PAD] | (uint32_t)proj.par[P_BASS] << 8 | (uint32_t)proj.par[P_ACID] << 16);
        h = hash(h, ui.fx_mask);
        h = hash(h, (uint32_t)ui.dj_mask);
    }
    if (ui.view == V_GLOBAL)
        h = hash(h, (uint32_t)proj.piece | (uint32_t)(proj.edited >> proj.piece & 1u) << 8 |
                        (uint32_t)project_saved(proj.piece) << 9);
    int now_touch = plat_ms() < ui.touch_until;
    for (i = 0; i < 4; i++)
        h = hash(h, (uint32_t)knob_get(view_knob((int)i)));
    h = hash(h, (uint32_t)now_touch);
    if (h == ui.sig[2])
        return;
    ui.sig[2] = h;
    cv_begin(240, KNB_H, K_BG);
    {   /* the rule; at its right end, where you are: the page, then the dots */
        char t[24];
        int k;
        int32_t xd = 237 - NDIAL * 5, xt;
        if (ui.view == V_SEQ) {
            char u[24] = "SEQ: ";
            caps(u + 5, EN_LANE_NAME[ui.lane], (int)sizeof u - 5);
            caps(t, u, sizeof t);
        } else {
            caps(t, VIEW_NAME[ui.view], sizeof t);
        }
        xt = xd - 5 - text_w(&FONT_XS, t);
        if (iam_song() && ui.view == V_TEMPO)
            xt = xd + 5;                         /* no page name: the rule runs up to the dots */
        if (ui.view == V_GLOBAL) {               /* Global: the piece's state, and what SAVE does */
            char u[24];
            int32_t xs;
            caps(u, piece_name(proj.piece), 4);
            if (proj.edited >> proj.piece & 1u) {
                const char *q = " SAVE: STORE";
                int j = 2;
                while (*q && j < 23)
                    u[j++] = *q++;
                u[j] = 0;
            } else {
                const char *q = project_saved(proj.piece) ? " STORED" : " FACTORY";
                int j = 2;
                while (*q && j < 23)
                    u[j++] = *q++;
                u[j] = 0;
            }
            xs = cv_text(4, -1, &FONT_XS, u, proj.edited >> proj.piece & 1u ? K_TEXT : K_DIM);
            if (xt - 9 > xs + 4)
                cv_rect(xs + 4, 5, xt - 9 - xs - 4, 1, K_LINE);
        } else {                                 /* the voice's sound (the drums: the kit), then the rule */
            char u[24], t[12];
            int32_t xs;
            int vc = voice_of(ui.view);
            uint16_t lc = K_DIM;
            int jam_hand = 0;
            if (vc >= 0) {
                en_param_text(SOUND_PAR[vc], proj.par[SOUND_PAR[vc]], t);
                caps(u, t, sizeof u);
            } else if (kit_view(ui.view)) {
                int k = proj.par[P_KIT_KICK];
                int same = proj.par[P_KIT_SNARE] == k && proj.par[P_KIT_HAT] == k && proj.par[P_KIT_PERC] == k;
                caps(u, same ? EN_KIT_NAME[(unsigned)k < EN_NKIT ? k : 0] : "Mixed kit", sizeof u);
            } else if (iam_song()) {
                iam_keys_caption(u, (int)sizeof u, &lc, &jam_hand);
            } else {
                caps(u, "HYP-1", sizeof u);
            }
            xs = cv_text(4, -1, &FONT_XS, u, lc);
            if (by_hand(vc))                     /* the voice off the algorithm: said beside its sound */
                xs = cv_text(xs + 5, -1, &FONT_XS, "BY HAND", K_ACC);
            else if (jam_hand && xs + 5 + text_w(&FONT_XS, "BY HAND") < xt - 8)
                xs = cv_text(xs + 5, -1, &FONT_XS, "BY HAND", K_ACC);
            else if (proj.par[P_LINKS])          /* the links on: said beside it */
                xs = cv_text(xs + 5, -1, &FONT_XS, "+LINK", K_ACC);
            else if (ui.view == V_LINKS)
                xs = cv_text(xs + 5, -1, &FONT_XS, "LINKS OFF", K_DIM);
            {   /* the octave OCT moves here, if it isn't home: "OCT+1" (the song's, off a voice: "SONG-2") */
                int op = oct_par(ui.view), ov = proj.par[op];
                if (ov) {
                    char o[10];
                    const char *pre = op == P_OCT ? "SONG" : "OCT";
                    int j = 0;
                    while (*pre)
                        o[j++] = *pre++;
                    o[j++] = ov > 0 ? '+' : '-';
                    o[j++] = (char)('0' + (ov > 0 ? ov : -ov));
                    o[j] = 0;
                    if (xs + 5 + text_w(&FONT_XS, o) < xt - 8)
                        xs = cv_text(xs + 5, -1, &FONT_XS, o, K_TEXT);
                }
            }
            if (xt - 9 > xs + 4)
                cv_rect(xs + 4, 5, xt - 9 - xs - 4, 1, K_LINE);
        }
        if (!(iam_song() && ui.view == V_TEMPO))
            cv_text(xt, -1, &FONT_XS, t, K_ACC);
        for (k = 0; k < NDIAL; k++) {
            int cur = k == ui.view || (ui.view == V_SEQ && k == V_DRUMS);
            int same = group_of(k) >= 0 && group_of(k) == group_of(ui.view == V_SEQ ? V_DRUMS : ui.view);
            cv_rect(xd + k * 5, 3, 3, 4, cur ? K_ACC : same ? K_DIM : K_LINE);
        }
    }
    for (i = 0; i < 4; i++) {
        int k = view_knob((int)i), lo, hi, v, hot = now_touch && ui.touched == (int)i;
        int32_t x = (int32_t)i * 60, cx = x + 30;
        char t[16];
        uint16_t bg = hot ? K_PANEL : K_BG, fill = hot ? K_ACC : K_PAD;
        if (k == K_NONE)
            continue;
        if (i)
            cv_rect(x, 16, 1, 46, K_LINE);
        if (hot)
            cv_round(x + 3, 11, 54, 57, 3, K_PANEL);
        knob_range(k, &lo, &hi);
        v = knob_get(k);
        caps(t, k == K_KEYS ? "Keys" : KNOB_LABEL[ui.view][i], sizeof t);
        text_c(cx, 12, &FONT_XS, t, hot ? K_TEXT : K_DIM);
        if (hi - lo < 6) {                       /* a switch: its positions as pips */
            int n = hi - lo + 1, j, pw = n > 4 ? 6 : 9, gap = 3, w0 = n * (pw + gap) - gap;
            for (j = 0; j < n; j++)
                cv_rect(cx - w0 / 2 + j * (pw + gap), 33, pw, 8, j == v - lo ? fill : K_LINE);
        } else {
            arc(cx, 37, 12, hi > lo ? (float)(v - lo) / (float)(hi - lo) : 0.0f, K_LINE, fill, bg, lo < 0);
        }
        knob_text(k, t);
        text_c(cx, 50, &FONT_B, t, hot ? K_ACC : K_TEXT);
    }
    cv_blit(0, KNB_Y);
}

static void leds(void)
{
    uint32_t b = 0, k = 0, i, ap = pool_abs();
    int g = group_of(ui.view == V_SEQ ? V_DRUMS : ui.view);
    if (g >= 0)
        b |= 1u << GROUP[g].btn;
    if (iam_arp_on())
        b |= 1u << B_ARP;                        /* GLO: the arpeggiator is on, on any page */
    if (en_playing)
        b |= 1u << B_PLAY;
    if (iam_loop_button())
        b |= 1u << B_REC;                        /* recording solid, the loop blinking */
    else if (en_frozen)
        b |= 1u << B_REC;
    if ((ui.fx_lock || ui.dj_lock) && !ui.fx_menu && (ui.frame / 16u) & 1u)
        b |= 1u << B_FX;                         /* effects locked on, the menu closed: FX blinks */
    if (en_sec != EN_SEC_GROOVE || (en_sec_req && (ui.frame / 8u) & 1u))   /* a section; blinking: on its way */
        b |= 1u << B_SAVE;
    if (ui.btn >> B_SEL & 1u) {                  /* shift: the pattern chosen; the one playing blinks if not */
        b |= 1u << B_SEL;
        if (ui.fx_menu)
            b |= 1u << B_FX;
        k |= 1u << WHITE_KEY[proj.par[P_PATTERN] % EN_NPAT];
        if (en_pat_now != proj.par[P_PATTERN] && (ui.frame / 8u) & 1u)
            k |= 1u << WHITE_KEY[en_pat_now % EN_NPAT];
        if (ui.view == V_SEQ)                    /* and the copy's targets */
            for (i = 0; i < EN_NPAT; i++)
                if ((int)i != seq_pat())
                    k |= 1u << BLACK_KEY[i];
    } else if (ui.fx_menu) {                     /* FX: lit; the keys held */
        b |= 1u << B_FX;
        for (i = 0; i < NKEYS; i++) {
            int d = glo_dj((int)i);
            if (d >= 0) {
                if ((ui.dj_mask >> d) & 1u)
                    k |= 1u << i;
            } else if (ui.fx_mask >> KEY_FX[i] & 1u)
                k |= 1u << i;
        }
    } else if (ui.view == V_SEQ) {               /* the sequencer: the lane's hits on the white keys, the playhead */
        uint16_t bits = en_pats[seq_pat()][ui.lane];
        for (i = 0; i < 16; i++)
            if ((bits >> i & 1u) ^ (uint32_t)(en_playing && i == en_drum_pos))
                k |= 1u << WHITE_KEY[i];
        if (iam_song())                          /* a black key held: its effect, and only that key */
            for (i = 0; i < NKEYS; i++)
                if (ui.key_voice[i])
                    k |= 1u << i;
    } else if (iam_song()) {                     /* the key under the finger. The scale stays put */
        for (i = 0; i < NKEYS; i++)
            if (ui.key_voice[i])
                k |= 1u << i;
    } else {
        for (i = 0; i < NKEYS; i++)
            if (ap >> ((5u + i) % 12u) & 1u)
                k |= 1u << i;
    }
    plat_glow(1);
    plat_leds(b, k);
}

/* what's left waiting for flash (a store is written at once, store_now): stopped, all of it after a
 * quiet moment; playing, the player's three (theme, Tune, MIDI out), with a moment's gap */
static void autosave(void)
{
    uint32_t now = plat_ms();
    if (!project_pending() || ui.btn || ui.keys || ui.store_wait)
        return;
    if (!en_playing && now - ui.act_t >= KIT_QUIET)
        project_flush();                         /* stopped: all of it */
    else if (project_pending_globals() && now - ui.act_t >= GLOB_QUIET)
        project_flush_globals();                 /* playing: the theme, Tune, MIDI out (a moment's gap) */
}

/* THE BOOT CARDS. First RedtailWorks: a pixel red panda (3x) sitting on the name, its ringed tail
 * curled up behind it, and the rings again beneath; then HYP-1 in big pixel letters, sixteen steps
 * filling in (the white keys lighting with them) until the music starts. Any button or key: straight
 * to the music. */
#define PANDA_W 32
#define PANDA_H 25
static const char *const PANDA[PANDA_H] = {
    ".WWW..............WWW...........",
    ".WBBW............WBBW...........",
    ".WBBRRRRRRRRRRRRRRBBW...........",
    "..RRRRRRRRRRRRRRRRRR............",
    ".RRRWWWRRRRRRRRWWWRRR...........",
    ".RRRRRRRRRRRRRRRRRRRR...........",
    ".RRRRKKRRRRRRRRKKRRRR...........",
    ".RWWDKKDRRRRRRDKKDWWR...........",
    ".WWWWDDWWWWWWWWDDWWWW...........",
    ".WWWWWDDWWKKWWDDWWWWW...........",
    "..WWWWWDDWWWWDDWWWWW........DDD.",
    "...DWWWWDDWWDDWWWWD........DDDDD",
    "....DDWWWWWWWWWWDD.........TTTTT",
    "......DDDDDDDDDD...........TTTTT",
    "......DRRRRRRRRD...........RRRRR",
    ".....BRRRRRRRRRRB..........RRRRR",
    "....BBRRRRRRRRRRBB.........TTTTT",
    "....BBBRRRRRRRRBBB........TTTTTT",
    "...BBBBBRRRRRRBBBBB.......RRRRRR",
    "...BBBBBBBBBBBBBBBB......RRRRRR.",
    "...BBBBBBBBBBBBBBBBRRTTRRTTTTT..",
    "...BBBBBBBBBBBBBBBBRRTTRRTTTTT..",
    "....BBBB.BBBB.BBBBRRRTTRRRRRR...",
    "...KKBBB..BB..BBBKKDDTTDDDD.....",
    "...KKKK...KK...KKKK............."
};
static uint16_t panda_col(char c)
{
    switch (c) {
    case 'R': return RGB(200, 84, 32);           /* rust */
    case 'D': return RGB(140, 52, 20);           /* its shade */
    case 'T': return RGB(232, 156, 92);          /* the tail's pale rings */
    case 'W': return RGB(246, 236, 222);         /* the face's white */
    case 'B': return RGB(64, 30, 22);            /* the dark legs and belly, the ears inside */
    case 'K': return RGB(24, 16, 18);            /* eyes, nose, paws */
    default: return 0;
    }
}

static void draw_splash(void)
{
    int i, x, y;
    lcd_fill(0, 0, 240, 240, K_BG);
    cv_begin(240, 96, K_BG);                     /* the panda */
    for (y = 0; y < PANDA_H; y++)
        for (x = 0; PANDA[y][x]; x++)
            if (PANDA[y][x] != '.')
                cv_rect(120 - PANDA_W * 3 / 2 + x * 3, 96 - PANDA_H * 3 - 4 + y * 3, 3, 3, panda_col(PANDA[y][x]));
    cv_blit(0, 0);
    cv_begin(240, 144, K_BG);                    /* the name, the rings, and what's next */
    text_c(120, 0, &FONT_L, "REDTAIL", K_PAD);
    text_c(120, 36, &FONT_L, "WORKS", K_ACC);
    for (i = 0; i < 9; i++)
        cv_rect(48 + i * 16, 86, 12, 6, i & 1 ? K_ACC : K_PAD);
    cv_frame(40, 80, 160, 18, K_LINE);
    text_c(120, 110, &FONT_XS, "PRESENTS", K_DIM);
    cv_blit(0, 96);
}

/* the HYP-1 card: big 5 x 7 pixel letters (7 px a pixel), an orange shadow under the purple */
static const uint8_t BIG_GLYPH[5][7] = {
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},  /* H */
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},  /* Y */
    {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},  /* P */
    {0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00},  /* - */
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},  /* 1 */
};
#define SPLASH_STEP_Y 184
static void draw_hyp(void)
{
    int g, r, c, pass;
    lcd_fill(0, 0, 240, 240, K_BG);
    cv_begin(240, 144, K_BG);
    for (pass = 0; pass < 2; pass++)             /* the shadow, then the letters */
        for (g = 0; g < 5; g++)
            for (r = 0; r < 7; r++)
                for (c = 0; c < 5; c++)
                    if (BIG_GLYPH[g][r] >> (4 - c) & 1u)
                        cv_rect(18 + (g * 6 + c) * 7 + (pass ? 0 : 3), 10 + r * 7 + (pass ? 0 : 3), 7, 7,
                                pass ? K_PAD : K_ACC);
    text_c(120, 76, &FONT_S, "HYPNOTIC TECHNO  //  ACID", K_TEXT);
    text_c(120, 102, &FONT_XS, EN_VERSION[0] == 'D' ? "FOR THE M-VAVE FM-1" : "FOR THE FM-1  //  " EN_VERSION, K_DIM);
    cv_blit(0, 30);
    for (c = 0; c < 16; c++)
        lcd_fill(8 + c * 14, SPLASH_STEP_Y, 11, 11, K_PANEL);
}

static void splash_tick(void)                    /* the HYP-1 card's steps, filling in to the music */
{
    uint32_t t = plat_ms() - ui.boot_t, t1 = ui_splash_ms * 17u / 32u, n, k = 0, i;
    if (ui.splash == 1) {
        if (t < t1)
            return;
        ui.splash = 2;
        draw_hyp();
    }
    n = (t - t1) * 17u / (ui_splash_ms - t1 + 1u);
    if (n > 16u)
        n = 16u;
    for (; ui.splash_step < n; ui.splash_step++)
        lcd_fill(8 + ui.splash_step * 14, SPLASH_STEP_Y, 11, 11, ui.splash_step % 4u ? K_PAD : K_ACC);
    for (i = 0; i < n; i++)
        k |= 1u << WHITE_KEY[i];
    plat_glow(1);
    plat_leds(n ? 1u << B_PLAY : 0u, k);
}

#include "machine.c"

void ui_init(void)
{
    if (rm.on)
        en_mute(0);
    memset(&rm, 0, sizeof rm);
    memset(&ui, 0, sizeof ui);
    ui.touched = -1;
    ui.boot_t = plat_ms();
    ui.theme = (uint8_t)proj.par[P_THEME];
    th = &THEMES[ui.theme < EN_NTHEME ? ui.theme : 0];
    ui.splash = ui_splash_ms > 0u;
    if (project_seed >= IAM_SEED_MIN) {          /* IAM-1: a song, already playing, no splash */
        ui.splash = 0;
        iam_boot(project_seed);
        lcd_fill(0, 0, 240, 240, K_BG);
        iam.dirty = 1;
        if (!en_playing)
            en_play(1);
        return;
    }
    iam.on = 0;
    if (ui.splash) {
        draw_splash();
    } else {
        lcd_fill(0, 0, 240, 240, K_BG);
        if (!en_playing)
            en_play(1);                          /* it plays from the start */
    }
}

void ui_frame(void)
{
    piece_land();
    if (ui.splash) {
        uint32_t held = plat_buttons() | plat_keys();
        if (plat_ms() - ui.boot_t < ui_splash_ms && !held) {
            splash_tick();
            return;
        }
        ui.splash = 0;                           /* the cards' time is up (or skipped): the UI, and the music */
        ui.btn = plat_buttons();                 /* a press that skipped them does nothing more */
        ui.keys = plat_keys();
        ui.btn_used = ui.btn;
        ui.shift_keys = ui.keys;
        lcd_fill(0, 0, 240, 240, K_BG);
        if (!en_playing)
            en_play(1);
    }
    if (project_seed >= IAM_SEED_MIN && !iam.on)
        iam_home_watch();                        /* deep editor: hold HOME, and the song returns */
    if (iam.on) {
        store_tick();
        iam_frame();
        ui.frame++;
        return;
    }
    if (ui.theme != (uint8_t)proj.par[P_THEME]) {    /* a new theme: everything again */
        ui.theme = (uint8_t)proj.par[P_THEME];
        th = &THEMES[ui.theme < EN_NTHEME ? ui.theme : 0];
        ui.sig[0] = ui.sig[1] = ui.sig[2] = 0;
        lcd_fill(0, 0, 240, 240, K_BG);
    }
    store_tick();
    {   /* drum drift changed the pattern: the knob follows, and says so */
        static uint32_t seen;
        if (en_dd_changes != seen) {
            char t[12] = "PATTERN ";
            seen = en_dd_changes;
            proj.par[P_PATTERN] = (int16_t)en_pat_next;
            t[8] = (char)('1' + en_pat_next % 10u);
            t[9] = 0;
            if (plat_ms() >= ui.msg_until)
                say("DRUM DRIFT", t);
        }
    }
    if (rm.on) {                                 /* remote: the browser's screen, the browser's lights */
        rm_tick();
        if (rm.on) {
            plat_glow(1);
            plat_leds(rm.lit_b, rm.lit_k);
            return;
        }
    }
    input();
    if (iam_song() && !iam.on)
        iam_arp_tick();                          /* same frame as the key: one note, on the sixteenths */
    if (iam.on) {                                /* GLO or HOME just handed the panel back */
        store_tick();
        iam_frame();
        ui.frame++;
        return;
    }
    autosave();
    if (kr.on) {                                 /* the secret: the whole screen, twenty frames a second */
        runner_update();
        if (ui.frame % 3u == 0u) {
            runner_draw(0);
            cv_blit(0, 0);
            runner_draw(1);
            cv_blit(0, 120);
        }
        runner_leds();
        ui.frame++;
        return;
    }
    draw_header();
    draw_main();
    draw_knobs();
    leds();
    ui.frame++;
}

void ui_input_only(void)
{
    if (iam.on) {
        if (iam.primed)
            iam_keys(plat_keys(), plat_buttons()); /* between frames too: a click is an edge */
        return;                                  /* encoders stay once a frame */
    }
    if (rm.on)
        rm_tick();
    else
        input();
}
