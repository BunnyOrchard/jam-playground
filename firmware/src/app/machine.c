/* SPDX-License-Identifier: GPL-3.0-only */
/* IAMM-1: a generative music machine on the HYP-1 engine (itself a fork of AMB-1).
 * Included from ui.c, so the panel, the drawing and the engine are already in scope.
 *
 * Power-on (any seed from 32 up) loads a song and starts it. The first four white
 * keys are the song's own pad, bass, acid, and the vibe's fourth part. Each later
 * white key is one more voice on that engine's note. Glass can play with the pad,
 * Sub with the bass, Pluck with the acid. Four of those synth keys can sound
 * at once. A fifth turns the oldest off, on the same bar, and the new one
 * stays. The drums are not in that count. A key that stays on is the full voice.
 * Each of those is one note, not a second chord. Every black key is one drum, low to
 * high: kick, snare, clap, hat, open hat, perc, rim, tom, cowbell, shaker, crash.
 * The song screen draws those eleven as five, then six, with a bar between the
 * third and the fourth, the eighth and the ninth, and the tenth and the eleventh.
 * A click turns that drum on or off, and only that drum. A white key turns only
 * its synth. A song draws its own keys, anywhere on the row: two or three of the
 * sixteen synths, and a handful of the eleven drums. Each drum that is on takes
 * its own kit and its own pattern from that vibe. Sleep keeps the room. A thin
 * room can take a colour. A room of three engines does not. The seed-42 boot
 * is the known drum-and-bass piece.
 * Until FX or a step edit takes a drum the song left off, its steps stay with the
 * beat. A section does not relight a drum the draw left off. Knob 1, held, is its
 * kit. Knob 2 wobbles a synth and sweeps a drum. A click mutes a part.
 * Hold one and the knobs edit it. The screen names that key, and under the name
 * the sound or the kit knob 1 is aiming at. That line moves as the knob turns.
 * The stored level sits on the right of the name, as a percent.
 * The pattern is drawn while the key is held: a drum's steps, and a white
 * key's steps. Each engine also has sixteen steps. Dust opens on the offbeats,
 * a chop on the & of the beat, and the rest
 * open on every step until one is taken out. While a drum is held, OCT- turns
 * the marked step and OCT+ moves to the next. The mark starts on step 1.
 * SEQ opens every track that is on. SELECT moves through those tracks.
 * OCT- and OCT+ shift the selected track by an octave. SEL solos that
 * track, and SEL again brings the song back. The white keys are that
 * track's sixteen steps: lit where it plays, and a press turns the step.
 * The title shows that track's stored level, and knob 3 changes it.
 * SEQ, GLO, or HOME leaves that screen, and a solo leaves with it. HOME
 * leaves the chord where it is.
 * A hold without SEQ still lets go. A click of FX mutates
 * those rows with the rest of the song. PLAY conducts a song.
 * The PRESETS dial steps thirteen vibes, from drum and bass out through the decades,
 * and then Sleep. Turning it back onto a vibe lands another song in that character:
 * another sound, another pattern, another noise. A named white key adds that sound. The recipe stays in the pool.
 * A new song picks among the first twelve. Sleep is slow: wind,
 * birds, a drone that comes and goes, a flute, and now and then a bell.
 * Break swaps two sounding parts for two quiet ones. SAVE crosses
 * into that break: a cue, a throw, a stutter, or a reverse. The intro's
 * thinner mix lasts four bars, then the groove. The form's
 * drop sits one or two parts out for one bar, then they come back. The
 * rest keeps playing. A change that would leave nothing playing keeps the
 * parts that are already on. Back returns the music.
 * In auto mode the pad sings on a two-bar clock whose shape keeps changing.
 * A new note comes every four bars. A section brings a new sound and chord,
 * and later steps leave that style's four sounds and move a drum pattern.
 * The fourth key follows the vibe: dust, a chop, air, hush, or wind. A new song
 * replaces the screen.
 * Holding FX does not retarget the knobs, and a turn while it is down is ignored.
 * The title row carries a clock: sixteen steps in this bar, and which of four bars.
 * A part, its sound, its kit and its pattern, and the scene land on the next bar.
 * That is the quantize. Level, cutoff, tempo, scale, effects and PLAY stay live.
 * With nothing held, and with SEQ closed, knob 1 is a ring and knob 2 is
 * a wah. Both warp the song already playing. Knob 3 is a phaser, and knob
 * 4 opens every synth filter. A click of FX mutates.
 * A held key still owns all four.
 * ENV is a new song on the next bar. The way across is a cue, a throw, a
 * stutter, or a reverse, and each press picks one. The PRESETS dial uses
 * that same crossing into the vibe it lands on.
 * LFO is Lets Find Out. The last beat is thrown across the song as you
 * press it, and the song stays loud. Sleep adds a blur. On the next bar
 * this song moves: a new chord, a new part brought up, a clearly busier
 * beat, or the air brought forward. Sleep steps into the next room.
 * OCT+ is excite, and two quiet drums come in with it. Once the song is
 * already excited, OCT+ pumps: one more part, more drum steps, a little
 * tempo, and the line mutates. OCT- is back, one move at a time.
 * Neither does that while SEQ's screen is up: SELECT moves the track, and
 * OCT shifts that track by an octave. SEL solos the selected track.
 * Knob 3 there is that track's level, not the phaser.
 * A drum still held, with that screen closed, keeps OCT on its steps.
 * SEL with that screen closed still walks the tips, and a held key still solos.
 * GLO opens the editor under the song. The arrangement keeps its bars there:
 * the sections, the pad's note, and the chord still move. White keys play the
 * scale on the part the track has forward, and the sixteen keys keep climbing
 * to the end.
 * A low octave does not fold the top keys back onto the bottom ones.
 * The pad's chord when the pad leads, or one note on
 * the drone or the acid when that voice is louder, so the key uses the filter
 * and the LFO already moving in the song. ENV and LFO pick the pad and the
 * drone. SELECT reaches the acid. ARP toggles an arpeggiator: held white
 * keys play one note at a time, low to high, on the sixteenths. A chord
 * key walks its notes, on the voice the page already plays.
 * Knob 2 on that page is its sound, one of that voice's sixteen. The white keys
 * open a tenth under the voice. On Tempo, knob 3 is that level: 0 is silent
 * and 100 is as loud as the voice. The part's own level stays. Black keys hold effects while they
 * are down. The FX page draws them in the song screen's groups, the effect name only.
 * Nothing there edits the scale. REC there records the white keys and, on the
 * next press, loops the last four bars. It does not stack. One more press
 * lets the loop go. Leaving GLO keeps a finished loop until REC is pressed
 * again. The song screen's REC still freezes when nothing is looping.
 * GLO again, or HOME, brings the song back. OCT there is the octave.
 * The four beats run across the bottom of that screen. The level meters
 * sit on the left, and the chord ring on the right. Under the beats, the
 * row names the synth the keys play and its sound. On Tempo it does not
 * also say the page.
 *
 * Seeds 0..31 stay on the HYP-1 panel: the scenario suite boots at 0, 2 and 3.
 * A real FM-1 seed is a hash of the MASTER pot and the clock, so it lands above
 * this. The host asks for the machine with seed 42 and up. */
#define IAM_SEED_MIN 32u
#define IAM_NPART 8
#define IAM_CAP 4 /* four synth keys. A fifth turns the oldest off. Not the drums */
#define IAM_OPEN 3 /* a song lights two or three. The player can add the fourth */
/* Thirteen on the dial. A new song draws among the first twelve.
 * Seed 42's first roll is 11355432: 0 modulo 12, and 10 modulo 13, which would
 * boot as dubstep. The unpicked bake stays `% IAM_NDANCE`. Sleep is the last row. */
#define IAM_NSTYPE 13
#define IAM_NDANCE 12
#define IAM_NVISIT 8 /* songs in one vibe. The first is the recipe */
#define IAM_NPAT 8
#define IAM_UNDO 4
#define IAM_CLICK_MS 750u /* a finger on the pad; a longer hold, with no twist, leaves the part */
#define IAM_SETTLE_MS 48u /* detents in this first moment of a press are the key's contact */
#define IAM_NTIP 27
#define IAM_TIP_LINES 7
enum { IAM_INTRO, IAM_GROOVE, IAM_EXCITE, IAM_BREAK, IAM_DROP, IAM_GLIDE, IAM_NSEC };
/* one arrangement change can be waiting for the bar. The last scene replaces it. */
enum { IAM_Q_NONE, IAM_Q_BREAK, IAM_Q_EXCITE, IAM_Q_DROP, IAM_Q_BACK, IAM_Q_CHANGE, IAM_Q_VIBE };

static const char *const IAM_STYLE_NAME[IAM_NSTYPE] = {
    "DNB", "REGGAE", "TRANCE", "STILL", "DISCO", "FUNK", "ELECTRO", "HOUSE", "HIPHOP", "GARAGE", "DUBSTEP", "WAVE",
    "SLEEP",
};
static const char *const IAM_SEC_NAME[IAM_NSEC] = {"INTRO", "GROOVE", "EXCITE", "BREAK", "DROP", "GLIDE"};
static const char *const IAM_PART_NAME[IAM_NPART] = {"PAD", "BASS", "ACID", "AIR", "KICK", "SNARE", "HAT", "PERC"};
/* engine level, sound, and the "surprise" character of each part */
static const int IAM_LVL[IAM_NPART] = {P_PAD, P_BASS, P_ACID, P_NOISE, P_LANE_LVL(0), P_LANE_LVL(1), P_LANE_LVL(3),
                                       P_LANE_LVL(5)};
static const int IAM_SND[IAM_NPART] = {P_PADTONE, P_DR_INST, P_AC_WAVE, P_NZ_COLOR, P_KIT_KICK, P_KIT_SNARE, P_KIT_HAT,
                                       P_KIT_PERC};
static const int IAM_SUR[IAM_NPART] = {P_BRIGHT, P_DR_RES, P_AC_MUT, P_NZ_PUMP, P_LANE_DEC(0), P_LANE_TUNE(1),
                                       P_LANE_DEC(3), P_LANE_TUNE(5)};
/* knob 4: the filter of that part. A drum's tune is its brightness (the hat's high-pass) */
static const int IAM_CUT[IAM_NPART] = {P_BRIGHT, P_DR_CUT, P_AC_CUT, P_NZ_TONE, P_LANE_TUNE(0), P_LANE_TUNE(1),
                                       P_LANE_TUNE(3), P_LANE_TUNE(5)};
/* Seven centred lines. A line much past 220px of FONT_S clips on the 240 panel.
 * Tap SEL to walk them. The last tap, or any other button, returns to the song. */
static const char *const IAM_TIP[IAM_NTIP][IAM_TIP_LINES] = {
    {"THE TIPS", "1 OF 27", "TAP SEL FOR THE NEXT", "THE LAST IS THE SONG", "ANY BUTTON LEAVES", "HOLD SEL AND A KEY", "SOLO WHILE YOU HOLD"},
    {"A CLICK", "2 OF 27", "A SHORT TAP TOGGLES", "IT LANDS ON THE BAR", "HOLD ON TO READ IT", "LET GO, AND IT STAYS", "A TURN IS NOT A TAP"},
    {"THE ENGINES", "3 OF 27", "FOUR WHITE KEYS FIRST", "PAD, BASS, THEN ACID", "THE FOURTH IS THE VIBE", "DUST CHOP AIR HUSH WIND", "SEQ: THE LIVE ROWS"},
    {"THE SOUNDS", "4 OF 27", "FIFTH TURNS OLDEST OFF", "PAD BASS ACID: THE SONG", "GLASS DARK DUB, STABS", "SUB REESE, HOOVER FM", "PLUCK TRANCE BLEEP SOFT"},
    {"THE DRUMS", "5 OF 27", "ELEVEN BLACK KEYS", "EACH KEY, ONE DRUM", "KICK OUT TO THE CRASH", "A TAP TURNS THAT DRUM", "AND ONLY THAT DRUM"},
    {"EACH DRUM", "6 OF 27", "THE BEAT STAYS LINKED", "A SONG, AND AUTO TOO", "FX OR STEPS UNLINK IT", "A TAP TURNS ONE DRUM", "EACH KEEPS ITS OWN KIT"},
    {"THE KITS", "7 OF 27", "HYPNO, HOUSE, TECHNO", "ACID, BREAK, MINIMAL", "HARD, THEN 808, 909", "HOLD THE DRUM, KNOB 1", "YOU HEAR IT NEXT BAR"},
    {"HOLD A KEY", "8 OF 27", "KNOB 1: SOUND OR KIT", "2: WOBBLE, HEARD NOW", "A DRUM: KNOB 2 SWEEP", "DOES NOT TURN A KEY ON", "PAD COLOUR: WAITING"},
    {"LEVEL AND FILTER", "9 OF 27", "3 LEVEL. 4 THE FILTER", "A DRUM: KNOB 4 TUNES", "ONLY THE DRUM YOU HOLD", "NOT THE ONE BESIDE IT", "YOU HEAR IT AS YOU TURN"},
    {"FROM SILENT", "10 OF 27", "KNOB 3 UP WAKES LOW", "TURN MORE, GET LOUDER", "DOWN ON DARK: NO CHANGE", "KNOB 4, NO KEY HELD", "OPENS EVERY FILTER"},
    {"WAKING A PART", "11 OF 27", "SOUND OR FILTER WAKES", "AT THE SAVED LEVEL", "KNOB 3 IS THE SOFT WAY", "A DRUM'S KNOB WAKES", "ONLY THAT DRUM"},
    {"THE PICTURE", "12 OF 27", "HOLD SHOWS THE NAME", "SOUND OR KIT UNDER IT", "KNOB 1 MOVES THAT LINE", "THE STEPS STAY DRAWN", "SEQ: SELECT OCT SEL"},
    {"PLAY", "13 OF 27", "STARTS ON AUTO", "TAP PLAY TO SWITCH", "MUSIC STAYS EITHER WAY", "INTRO GROOVE EXCITE", "BREAK DROP GLIDE, AROUND"},
    {"THE BREAK", "14 OF 27", "SAVE IS THE BREAK", "TWO LOUD PARTS STEP OUT", "TWO QUIET ONES STEP IN", "ONE NEW PAD NOTE", "CUE THROW STUTTER REV"},
    {"EXCITE", "15 OF 27", "EDIT IS EXCITE", "THE FILTERS OPEN", "A NEW PAD SOUND", "AND A NEW CHORD", "NO DRUMS. NEXT BAR"},
    {"OCT PLUS", "16 OF 27", "OCT+ IS EXCITE", "TWO QUIET DRUMS JOIN", "AGAIN: A TRACK, FASTER", "STEPS, AND IT MUTATES", "EACH ONE, NEXT BAR"},
    {"THE STEPS", "17 OF 27", "SELECT PICKS THE TRACK", "OCT MOVES AN OCTAVE", "SEL SOLOS THE TRACK", "WHITE KEYS TURN STEPS", "SEQ GLO HOME, ARP BACK"},
    {"A NEW SONG", "18 OF 27", "ENV IS A NEW SONG", "CUE, THROW, STUTTER", "OR REVERSE, AT RANDOM", "PRESETS CROSSES TOO", "BOTH ON THE NEXT BAR"},
    {"THE VIBES", "19 OF 27", "DNB, REGGAE, TRANCE", "STILL, DISCO, FUNK", "ELECTRO, HOUSE, HIPHOP", "GARAGE, DUBSTEP, WAVE", "THEN SLEEP, THEN DNB"},
    {"MORE BUTTONS", "20 OF 27", "LFO: LETS FIND OUT", "AN EFFECT, THEN A PLACE", "REC: HELD, OR MOVING", "HOME: NEXT CHORD, NOW", "HOLD PAD: AUTO WAITS"},
    {"GLO", "21 OF 27", "SONG PLAYS UNDERNEATH", "KEYS FOLLOW THE TRACK", "ARP ON: ONE NOTE EACH", "KNOB 2 SOUND, 3 KEYS", "GLO OR HOME RETURNS"},
    {"INSIDE GLO", "22 OF 27", "BLACK KEYS HOLD EFFECTS", "REPEAT REV GATE THROW", "FLANGER FLANGE 8TH FILT", "HOLD SEQ: DRUM STEPS", "LOOP STAYS UNTIL REC"},
    {"TEMPO AND SCALE", "23 OF 27", "1 RING  2 WAH", "3 PHASER  4 FILTERS", "SELECT TEMPO, ALGO SCALE", "FX CLICK IS MUTATE", "A HELD KEY OWNS 1 TO 4"},
    {"FX", "24 OF 27", "A SHORT TAP MUTATES", "A HELD KEY: JUST THAT", "NO KEY: THE WHOLE SONG", "HOLDING DOES NOT MOVE", "A KNOB. A TURN IS LOST"},
    {"MUTATE", "25 OF 27", "NO KEY WALKS THE SONG", "BEAT ACID PAD BASS AIR", "KIT, FILTER, OR ROOM", "A HELD KEY: JUST THAT", "TEMPO AND PARTS STAY"},
    {"THE CLOCK", "26 OF 27", "SIXTEEN PIPS: THIS BAR", "ONE PIP IS THE STEP", "FOUR PIPS MAKE A BEAT", "FOUR WIDER: THE PHRASE", "WHICH OF THE FOUR BARS"},
    {"WHAT WAITS", "27 OF 27", "BAR: TAP SOUND AND KIT", "SCENE VIBE FIND OCT ENV", "NOW: WOBBLE LEVEL FILTER", "TEMPO SCALE FX PLAY", "REC HOME SEL AND GLO"},
};
/* boot arrangement. Dnb is pad and hat, so a drop has a rhythm to bring.
 * Reggae is the one-drop band, trance is already studying, still is the chord. */
static const uint8_t IAM_SPINE[IAM_NSTYPE] = {0x41, 0x5B, 0x57, 0x0B, 0x51, 0x31, 0x54, 0x51, 0x31, 0x52, 0x32, 0x13, 0x0A};
/* a song from the dial. The drums become these. Voices already on stay.
 * Still is the chord alone: the drop opens the room, it does not start a kit.
 * A boot uses the spine. The dial of a later vibe uses the arrive mask whole. */
/* Sleep's arrive is the groove: drone, flute, wind, and the chimes. The bell waits for the break. */
static const uint8_t IAM_ARRIVE[IAM_NSTYPE] = {0x73, 0xDB, 0x57, 0x0B, 0x7B, 0x7B, 0x7E, 0x7B, 0x7B, 0xFB, 0x7B, 0x7B, 0x4E};
static const int IAM_FORM_LEN[IAM_NSEC] = {4, 16, 8, 8, 1, 8}; /* intro four bars. The drop is one */
static const int IAM_FORM_SEC[IAM_NSEC] = {IAM_INTRO, IAM_GROOVE, IAM_EXCITE, IAM_BREAK, IAM_DROP, IAM_GLIDE};
/* singing sounds only. Stabs, dub, pings, hits and rave gate on the off-beat and die. */
static const uint8_t IAM_PAD_SND[IAM_NSTYPE][4] = {
    {1, 2, 5, 4},    /* Dnb: Voices, Strings, Warm, Organ. This row is the seed-42 song */
    {4, 5, 0, 8},    /* Reggae: Organ, Warm, Bloom, Dub */
    {9, 10, 14, 5},  /* Trance: Trance, Poly, Rave, Warm */
    {15, 11, 1, 5},  /* Still: Sine, Mist, Voices, Warm */
    {4, 0, 2, 5},    /* Disco: Organ, Bloom, Strings, Warm */
    {6, 4, 0, 8},    /* Funk: Stabs, Organ, Bloom, Dub */
    {7, 6, 3, 10},   /* Electro: Dark, Stabs, Glass, Poly */
    {4, 0, 5, 1},    /* House: Organ, Bloom, Warm, Voices */
    {7, 0, 11, 5},   /* Hip hop: Dark, Bloom, Mist, Warm */
    {4, 5, 0, 6},    /* Garage: Organ, Warm, Bloom, Stabs */
    {7, 11, 15, 14}, /* Dubstep: Dark, Mist, Sine, Rave */
    {2, 5, 1, 3},    /* Wave: Strings, Warm, Voices, Glass */
    {15, 11, 1, 5},  /* Sleep: Sine, Mist, Voices, Warm */
};
/* chord shapes a style may become. The first is the one the song starts on. */
static const uint8_t IAM_PAD_CH[IAM_NSTYPE][4] = {
    {EN_CH_9TH, EN_CH_7TH, EN_CH_SUS, EN_CH_4THS},
    {EN_CH_DUB, EN_CH_7TH, EN_CH_9TH, EN_CH_SUS},
    {EN_CH_SUS, EN_CH_9TH, EN_CH_7TH, EN_CH_4THS},
    {EN_CH_4THS, EN_CH_SUS, EN_CH_9TH, EN_CH_DUB},
    {EN_CH_7TH, EN_CH_9TH, EN_CH_TRIAD, EN_CH_SUS},
    {EN_CH_9TH, EN_CH_7TH, EN_CH_DUB, EN_CH_SUS},
    {EN_CH_SUS, EN_CH_4THS, EN_CH_7TH, EN_CH_9TH},
    {EN_CH_7TH, EN_CH_9TH, EN_CH_TRIAD, EN_CH_SUS},
    {EN_CH_SUS, EN_CH_7TH, EN_CH_9TH, EN_CH_4THS},
    {EN_CH_9TH, EN_CH_7TH, EN_CH_SUS, EN_CH_DUB},
    {EN_CH_4THS, EN_CH_SUS, EN_CH_7TH, EN_CH_DUB},
    {EN_CH_SUS, EN_CH_9TH, EN_CH_7TH, EN_CH_4THS},
    {EN_CH_4THS, EN_CH_SUS, EN_CH_TRIAD, EN_CH_SUS}, /* Sleep: the groove steps onto the suspended chord */
};
/* After a style's own first change, the pad leaves that row. Singing sounds,
 * then one stabbed colour, so a later phrase is not the same four again.
 * Sleep stays sung: no stab. */
static const uint8_t IAM_PAD_MORE[IAM_NSTYPE][8] = {
    {0, 3, 11, 15, 10, 9, 7, 6},  {1, 3, 11, 15, 2, 10, 7, 12}, {0, 11, 15, 1, 3, 5, 7, 14},
    {0, 3, 2, 4, 10, 9, 7, 6},    {1, 3, 11, 15, 10, 9, 7, 13}, {0, 11, 15, 2, 3, 10, 7, 8},
    {0, 11, 15, 1, 5, 2, 4, 14},  {3, 11, 15, 2, 10, 9, 7, 13}, {1, 3, 2, 4, 10, 9, 6, 12},
    {0, 11, 15, 1, 3, 10, 7, 8},  {0, 1, 5, 2, 10, 9, 4, 14},   {0, 11, 15, 4, 10, 9, 7, 6},
    {0, 3, 4, 2, 10, 9, 7, 15},
};
/* Chord colours the later steps can land on. The first step of a song stays on its row. */
static const uint8_t IAM_CH_MORE[6] = {EN_CH_TRIAD, EN_CH_7TH, EN_CH_9TH, EN_CH_SUS, EN_CH_DUB, EN_CH_4THS};
/* attack, hold, release, rest. A long hold and almost no rest: the four unequal
 * breaths overlap, so the chord stays up instead of swelling off. */
static const uint8_t IAM_BREATH[IAM_NSTYPE][4] = {
    {26, 68, 42, 3}, {18, 52, 36, 6}, {22, 70, 48, 4}, {46, 92, 72, 2},
    {20, 60, 40, 4}, {14, 44, 28, 8}, {16, 36, 24, 8}, {22, 64, 44, 4}, /* funk's hold stays above 40: seed 99 draws it */
    {24, 58, 40, 6}, {16, 44, 30, 6}, {18, 50, 36, 6}, {32, 80, 60, 3},
    {40, 70, 90, 80}, /* Sleep: a slow attack, a long hold, a long release, a rest of several seconds */
};
/* The bass hits and leaves. Trigger is 1 bar, 1 bar, 1 bar, 2 bars.
 * Decay is about half a second, four fifths, two fifths, and a second and a half.
 * Still's dress asks for a tail of several seconds. The bake replaces that. */
static const uint8_t IAM_DR_TRIG[IAM_NSTYPE] = {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 4};
static const uint8_t IAM_DR_DEC[IAM_NSTYPE] = {40, 48, 36, 60, 44, 36, 40, 42, 48, 40, 64, 66, 94};
/* Eleven black keys, one drum each. The first four of kick, snare, hat and perc
 * are arrangement parts. Clap, the open hat, and the five above perc are their own
 * on/off, beside the mask, so a tap never moves the drum next to them. */
#define IAM_NDRUMKEY 11
static const uint8_t IAM_DRUM_PART[6] = {4, 5, 5, 6, 6, 7}; /* clap shares the snare's part number only
                                                            * as a column. Its on/off does not. */
static const char *const IAM_DRUM_NAME[IAM_NDRUMKEY] = {"KICK", "SNARE", "CLAP", "HAT", "OPEN HAT", "PERC",
                                                        "RIM", "TOM", "COWBELL", "SHAKER", "CRASH"};
static const char *const IAM_DRUM_ROW[IAM_NDRUMKEY] = {"KICK", "SNARE", "CLAP", "HAT", "OPEN", "PERC",
                                                       "RIM", "TOM", "BELL", "SHAKE", "CRASH"};
/* Five across, then six. Each bar is three pixels, so the cells sum to 237 and 234. */
static const int IAM_DROW1_W[5] = {44, 52, 44, 50, 47}; /* kick snare clap | hat open. CHIME fits the hat */
static const int IAM_DROW2_W[6] = {40, 30, 34, 38, 46, 46}; /* perc rim tom | bell shake | crash */
#define IAM_DRUM_BAR 3
/* Twelve sounds, each its own key. Four play on the pad, four on the bass, four on the acid.
 * The engine key is the song's own sound, and it is never one of these twelve. */
static const uint8_t IAM_PRE_PART[12] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2};
static const uint8_t IAM_PRE_SND[12] = {3, 7, 8, 6, 9, 4, 11, 6, 4, 8, 9, 15};
static const char *const IAM_PRE_NAME[12] = {"GLASS", "DARK", "DUB",  "STABS", "SUB",   "REESE",
                                             "HOOVER", "FM",  "PLUCK", "TRANCE", "BLEEP", "SOFT"};
/* haze colours: pink, wood, vinyl. White, blue, violet, grey and yellow hiss.
 * The roll uses these. Seed 42 lands on wood, the knock that replaced brown. */
static const uint8_t IAM_HAZE_COL[4] = {2, 1, 6, 2};
/* pattern, per vibe: tone (the low-pass) and how hard the kick ducks it.
 * Seed 42's dust pattern is row 1: tone 14, pump 22. The rows beside it open
 * and close the shaker. FX on the fourth key steps to the next row. Electro's
 * boot row is 0 (22, 26). Dubstep's boot row is 0 (18, 34). */
static const uint8_t IAM_HAZE[IAM_NSTYPE][8][2] = {
    {{6, 8}, {14, 22}, {62, 48}, {34, 20}, {8, 30}, {74, 56}, {26, 16}, {48, 36}},
    {{46, 8}, {40, 6}, {52, 10}, {36, 8}, {58, 12}, {44, 4}, {34, 14}, {48, 8}},
    {{56, 24}, {64, 18}, {50, 28}, {70, 16}, {48, 30}, {60, 20}, {62, 18}, {54, 22}},
    {{24, 4}, {20, 4}, {32, 6}, {30, 4}, {36, 8}, {22, 4}, {28, 6}, {18, 2}},
    {{48, 10}, {42, 8}, {56, 14}, {38, 6}, {52, 12}, {44, 8}, {60, 16}, {40, 10}},
    {{36, 16}, {30, 12}, {44, 18}, {28, 10}, {40, 20}, {34, 14}, {48, 22}, {32, 12}},
    {{22, 26}, {68, 44}, {10, 12}, {46, 32}, {30, 24}, {76, 50}, {14, 18}, {40, 28}},
    {{44, 8}, {38, 6}, {52, 12}, {34, 4}, {48, 10}, {40, 6}, {56, 14}, {36, 8}},
    {{26, 6}, {20, 4}, {32, 8}, {18, 4}, {28, 6}, {22, 4}, {34, 10}, {16, 2}},
    {{40, 14}, {34, 10}, {48, 18}, {30, 8}, {44, 16}, {36, 12}, {52, 20}, {32, 10}},
    {{18, 34}, {66, 50}, {8, 18}, {42, 36}, {24, 28}, {78, 58}, {12, 22}, {36, 40}},
    {{30, 6}, {24, 4}, {36, 8}, {20, 4}, {32, 6}, {26, 4}, {40, 10}, {22, 2}},
    {{28, 0}, {16, 0}, {42, 0}, {8, 0}, {36, 0}, {20, 0}, {52, 0}, {12, 0}}, /* Sleep: the wind's tone. No duck */
};

/* 16-step hats, kicks and snares. Bit 0 is the downbeat. Slot 4 is the one-drop. */
static const uint16_t IAM_KICK[IAM_NPAT] = {0x1111, 0x0401, 0x0441, 0x0001, 0x0100, 0x0451, 0x0041, 0x1511};
/* kick, snare, hat, perc index into the tables above. Pattern 0 is what plays. */
static const uint8_t IAM_BEAT[IAM_NSTYPE][4] = {
    {6, 1, 0, 0}, /* 1 and the a of 2, snare on 3, rushing hats */
    {4, 1, 1, 0}, /* one-drop, rim on 3, offbeat hat */
    {0, 0, 2, 4}, /* four on the floor, backbeat, straight hats */
    {3, 7, 3, 5}, /* one kick, a rare snare, a slow hat, one perc */
    {0, 0, 0, 4}, /* four on the floor, backbeat, sixteenth hats */
    {2, 0, 0, 4}, /* a syncopated kick, the backbeat, swung sixteenths */
    {0, 0, 4, 6}, /* a machine kick, a clap, dry sixteenths */
    {0, 0, 1, 4}, /* four on the floor, the hat on the offbeat */
    {2, 0, 4, 5}, /* a boom-bap kick, the backbeat, a swung hat */
    {6, 0, 6, 2}, /* two-step kick, skippy hats */
    {3, 1, 2, 5}, /* one kick, the snare on three: half time */
    {0, 0, 1, 3}, /* a soft floor, an offbeat hat */
    {3, 7, 0, 0}, /* Sleep writes its own sparse lanes. These indices are unused */
};
static const uint8_t IAM_BASS_SND[IAM_NSTYPE] = {4, 9, 9, 1, 1, 9, 6, 9, 1, 9, 4, 5, 9};
static const uint8_t IAM_ACID_SND[IAM_NSTYPE] = {2, 4, 8, 15, 15, 4, 9, 4, 15, 4, 14, 11, 15};
static const uint8_t IAM_KIT[IAM_NSTYPE] = {4, 7, 2, 5, 8, 7, 7, 8, 4, 1, 6, 7, 5};
static const uint8_t IAM_ACID_PAT[IAM_NSTYPE] = {3, 5, 6, 2, 2, 4, 6, 5, 2, 5, 2, 7, 2};
/* haze pattern and colour. Style 0 keeps the roll: seed 42's tone and pump are that roll.
 * Still's colour is black, and the hush plays it as a dark tone. */
static const uint8_t IAM_HAZE_PAT[IAM_NSTYPE] = {0, 0, 6, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const uint8_t IAM_HAZE_SET[IAM_NSTYPE] = {2, 2, 1, 6, 1, 2, 4, 1, 6, 2, 2, 1, 1};
/* which air part. 1 dust, 2 chop, 3 air, 4 hush, 5 wind. Not style + 1: that only fits four. */
static const uint8_t IAM_AIR[IAM_NSTYPE] = {1, 2, 3, 4, 3, 2, 1, 3, 4, 2, 1, 3, 5};
/* floors and the filter, for the eight appended vibes. 0 leaves that level as rolled.
 * bass, kick, acid, haze level, pad, LFO depth, acid cut, bright, drone cut, attack, LFO rate, reso, LFO shape */
static const uint8_t IAM_DRESS[8][13] = {
    {74, 78, 0, 42, 70, 12, 40, 48, 42, 18, 3, 55, 0},
    {82, 74, 0, 48, 0, 18, 48, 40, 36, 12, 4, 62, 0},
    {76, 80, 76, 46, 0, 14, 64, 26, 50, 10, 5, 84, 0},
    {76, 82, 0, 40, 72, 8, 36, 46, 34, 16, 2, 48, 0},
    {80, 76, 0, 34, 0, 16, 30, 24, 32, 20, 3, 40, 0},
    {84, 72, 0, 46, 0, 20, 44, 42, 38, 14, 4, 58, 0},
    {88, 84, 0, 48, 0, 52, 32, 20, 44, 16, 6, 50, 1},
    {74, 64, 0, 44, 80, 22, 38, 38, 30, 28, 3, 45, 0},
};
/* A landing draws from these. Eight songs. The first of each row is the recipe.
 * Style 0's boot does not draw: those rolls are the seed-42 piece. Every other
 * new song does, and so does the dial. Beats stay in the family: a one-drop
 * stays a one-drop, half time stays half time, a floor stays a floor.
 * Sleep's lanes are its own clock, so its beat row is unused. Electro's acid
 * pattern stays the one the line can follow. */
static const uint8_t IAM_VISIT_BASS[IAM_NSTYPE][IAM_NVISIT] = {
    {4, 9, 8, 6, 14, 2, 10, 5},  /* Dnb: reese, sub, rumble, FM, sync, grit, square, pulse */
    {9, 1, 7, 5, 8, 15, 12, 3},  /* Reggae */
    {9, 5, 0, 1, 10, 15, 7, 14}, /* Trance */
    {1, 7, 9, 5, 15, 12, 8, 10}, /* Still */
    {1, 7, 9, 5, 15, 0, 10, 12}, /* Disco */
    {9, 6, 1, 4, 7, 5, 10, 3},  /* Funk */
    {6, 10, 5, 14, 0, 2, 13, 12}, /* Electro: FM stays in the pool */
    {9, 1, 7, 5, 15, 10, 0, 12}, /* House */
    {1, 9, 4, 8, 3, 6, 5, 2},    /* Hip hop */
    {9, 4, 5, 1, 10, 8, 6, 0},   /* Garage */
    {4, 8, 6, 11, 14, 2, 13, 3}, /* Dubstep: reese and hoover stay */
    {5, 1, 9, 0, 15, 7, 12, 10}, /* Wave */
    {9, 1, 7, 5, 15, 12, 8, 10}, /* Sleep */
};
static const uint8_t IAM_VISIT_ACID[IAM_NSTYPE][IAM_NVISIT] = {
    {2, 0, 1, 14, 3, 5, 6, 10},
    {4, 15, 12, 11, 5, 7, 1, 10},
    {8, 5, 0, 10, 1, 2, 14, 6},
    {15, 11, 12, 4, 5, 7, 10, 1},
    {15, 12, 4, 1, 5, 11, 0, 8},
    {4, 5, 1, 12, 15, 11, 0, 7},
    {9, 11, 15, 4, 6, 0, 14, 10}, /* bleep stays in the pool */
    {4, 15, 12, 8, 5, 1, 11, 0},
    {15, 4, 7, 11, 12, 5, 1, 0},
    {4, 1, 5, 15, 12, 11, 0, 8},
    {14, 3, 0, 2, 6, 13, 10, 5},
    {11, 15, 4, 10, 12, 5, 8, 7},
    {15, 11, 4, 12, 5, 7, 10, 1},
};
static const uint8_t IAM_VISIT_KIT[IAM_NSTYPE][IAM_NVISIT] = {
    {4, 7, 8, 6, 2, 3, 0, 5},
    {7, 1, 0, 8, 5, 4, 2, 3},
    {2, 8, 5, 7, 1, 0, 3, 6},
    {5, 0, 1, 7, 2, 8, 4, 3},
    {8, 1, 7, 2, 0, 3, 5, 4},
    {7, 4, 8, 1, 0, 2, 6, 5},
    {7, 8, 2, 5, 0, 6, 4, 3}, /* 808 stays */
    {8, 1, 2, 7, 0, 5, 3, 4},
    {4, 7, 0, 8, 1, 5, 6, 2},
    {1, 8, 2, 7, 0, 4, 5, 6},
    {6, 7, 4, 2, 8, 3, 0, 1},
    {7, 5, 1, 8, 0, 2, 4, 3},
    {5, 0, 1, 7, 2, 8, 4, 3},
};
static const uint8_t IAM_VISIT_COL[IAM_NSTYPE][IAM_NVISIT] = {
    {2, 1, 6, 5, 0, 4, 3, 7},
    {2, 1, 6, 5, 0, 4, 3, 7},
    {1, 0, 3, 4, 2, 6, 5, 7},
    {6, 2, 1, 5, 0, 4, 3, 7},
    {1, 0, 2, 4, 6, 5, 3, 7},
    {2, 1, 6, 5, 0, 4, 3, 7},
    {4, 1, 2, 5, 0, 6, 3, 7},
    {1, 0, 2, 3, 6, 4, 5, 7},
    {6, 2, 1, 5, 0, 4, 3, 7},
    {2, 1, 0, 6, 4, 5, 3, 7},
    {2, 4, 1, 5, 6, 0, 3, 7},
    {1, 3, 0, 2, 6, 5, 4, 7},
    {1, 2, 0, 3, 6, 5, 4, 7},
};
static const uint8_t IAM_VISIT_APAT[IAM_NSTYPE][IAM_NVISIT] = {
    {3, 0, 4, 7, 1, 2, 5, 6},
    {5, 0, 7, 4, 1, 2, 3, 6},
    {6, 1, 3, 4, 0, 2, 5, 7},
    {2, 5, 0, 7, 1, 3, 4, 6},
    {2, 0, 5, 7, 1, 3, 4, 6},
    {4, 0, 3, 7, 1, 2, 5, 6},
    {6, 6, 6, 6, 6, 6, 6, 6},
    {5, 2, 0, 7, 1, 3, 4, 6},
    {2, 5, 0, 7, 1, 3, 4, 6},
    {5, 0, 4, 7, 1, 2, 3, 6},
    {2, 5, 0, 7, 1, 3, 4, 6},
    {7, 0, 4, 3, 1, 2, 5, 6},
    {2, 2, 2, 2, 2, 2, 2, 2},
};
/* kick, snare, hat, perc. Eight cousins. The first is IAM_BEAT. */
static const uint8_t IAM_VISIT_BEAT[IAM_NSTYPE][IAM_NVISIT][4] = {
    {{6, 1, 0, 0}, {6, 7, 6, 2}, {5, 1, 2, 4}, {1, 7, 5, 6}, {6, 0, 2, 4}, {5, 7, 0, 6}, {1, 1, 6, 2}, {6, 7, 5, 4}},
    {{4, 1, 1, 0}, {4, 7, 3, 2}, {4, 1, 6, 4}, {4, 7, 5, 5}, {4, 0, 1, 2}, {4, 5, 6, 6}, {4, 2, 3, 1}, {4, 6, 7, 4}},
    {{0, 0, 2, 4}, {0, 4, 5, 2}, {0, 2, 0, 1}, {0, 6, 1, 6}, {0, 1, 4, 3}, {0, 7, 2, 5}, {0, 5, 6, 0}, {0, 2, 7, 1}},
    {{3, 7, 3, 5}, {3, 1, 5, 3}, {3, 7, 5, 2}, {3, 1, 3, 6}, {3, 7, 1, 0}, {3, 1, 2, 4}, {3, 7, 0, 1}, {3, 1, 6, 2}},
    {{0, 0, 0, 4}, {0, 4, 2, 1}, {0, 2, 6, 6}, {0, 6, 1, 2}, {0, 1, 0, 3}, {0, 5, 4, 5}, {0, 7, 0, 6}, {0, 4, 7, 1}},
    {{2, 0, 0, 4}, {5, 4, 6, 1}, {2, 2, 2, 6}, {5, 6, 1, 2}, {2, 1, 4, 3}, {5, 0, 2, 5}, {2, 7, 6, 0}, {5, 2, 0, 4}},
    {{0, 0, 4, 6}, {0, 4, 2, 2}, {0, 2, 5, 4}, {0, 6, 6, 1}, {0, 1, 0, 3}, {0, 7, 4, 5}, {0, 5, 2, 6}, {0, 4, 7, 0}},
    {{0, 0, 1, 4}, {0, 4, 2, 6}, {0, 2, 6, 1}, {0, 6, 5, 3}, {0, 1, 1, 2}, {0, 7, 0, 5}, {0, 5, 1, 6}, {0, 4, 7, 0}},
    {{2, 0, 4, 5}, {2, 4, 6, 2}, {5, 2, 1, 3}, {5, 6, 2, 4}, {2, 1, 4, 0}, {5, 7, 6, 6}, {2, 5, 0, 1}, {5, 0, 4, 2}},
    {{6, 0, 6, 2}, {6, 4, 1, 5}, {6, 2, 0, 4}, {6, 6, 2, 1}, {6, 1, 6, 3}, {6, 7, 1, 4}, {6, 5, 0, 6}, {6, 0, 4, 5}},
    {{3, 1, 2, 5}, {3, 7, 6, 2}, {3, 1, 0, 6}, {3, 7, 5, 3}, {3, 1, 4, 0}, {3, 7, 2, 6}, {3, 1, 6, 4}, {3, 7, 0, 1}},
    {{0, 0, 1, 3}, {0, 4, 2, 6}, {0, 2, 5, 1}, {0, 6, 3, 2}, {0, 1, 1, 4}, {0, 7, 2, 0}, {0, 5, 0, 5}, {0, 4, 7, 6}},
    {{3, 7, 0, 0}, {3, 7, 0, 0}, {3, 7, 0, 0}, {3, 7, 0, 0}, {3, 7, 0, 0}, {3, 7, 0, 0}, {3, 7, 0, 0}, {3, 7, 0, 0}},
};
typedef char iam_rows_ok[(sizeof IAM_SPINE == IAM_NSTYPE && sizeof IAM_ARRIVE == IAM_NSTYPE &&
                          sizeof IAM_DR_TRIG == IAM_NSTYPE && sizeof IAM_DR_DEC == IAM_NSTYPE &&
                          sizeof IAM_AIR == IAM_NSTYPE && sizeof IAM_BASS_SND == IAM_NSTYPE &&
                          sizeof IAM_ACID_SND == IAM_NSTYPE && sizeof IAM_KIT == IAM_NSTYPE &&
                          sizeof IAM_ACID_PAT == IAM_NSTYPE && sizeof IAM_HAZE_PAT == IAM_NSTYPE &&
                          sizeof IAM_HAZE_SET == IAM_NSTYPE && sizeof IAM_PAD_SND == IAM_NSTYPE * 4 &&
                          sizeof IAM_PAD_CH == IAM_NSTYPE * 4 && sizeof IAM_PAD_MORE == IAM_NSTYPE * 8 &&
                          sizeof IAM_CH_MORE == 6 && sizeof IAM_BREATH == IAM_NSTYPE * 4 &&
                          sizeof IAM_BEAT == IAM_NSTYPE * 4 && sizeof IAM_HAZE == IAM_NSTYPE * 16 &&
                          sizeof IAM_VISIT_BASS == IAM_NSTYPE * IAM_NVISIT && sizeof IAM_VISIT_ACID == IAM_NSTYPE * IAM_NVISIT &&
                          sizeof IAM_VISIT_KIT == IAM_NSTYPE * IAM_NVISIT && sizeof IAM_VISIT_COL == IAM_NSTYPE * IAM_NVISIT &&
                          sizeof IAM_VISIT_APAT == IAM_NSTYPE * IAM_NVISIT &&
                          sizeof IAM_VISIT_BEAT == IAM_NSTYPE * IAM_NVISIT * 4 &&
                          sizeof IAM_DRESS == 8 * 13 && /* styles 4..11. Still and Sleep dress themselves */
                          sizeof IAM_STYLE_NAME / sizeof IAM_STYLE_NAME[0] == IAM_NSTYPE)
                             ? 1
                             : -1];
static const uint16_t IAM_SNARE[IAM_NPAT] = {0x1010, 0x0100, 0x1010, 0x1110, 0x1010, 0x0110, 0x1010, 0x0100};
static const uint16_t IAM_HAT[IAM_NPAT] = {0xAAAA, 0x4444, 0x5555, 0x2222, 0xAAAA, 0x1111, 0x4924, 0x4444};
static const uint16_t IAM_PERC[IAM_NPAT] = {0x0840, 0x2222, 0x0120, 0x4800, 0x0A0A, 0x0400, 0x2080, 0x1110};
static const uint16_t IAM_RIM[IAM_NPAT] = {0x0808, 0x0202, 0x0880, 0x0008, 0x2020, 0x0404, 0x0088, 0x1010};
static const uint16_t IAM_TOM[IAM_NPAT] = {0x4040, 0x0404, 0x4004, 0x0044, 0x4400, 0x0400, 0x4044, 0x0004};
static const uint16_t IAM_BELL[IAM_NPAT] = {0x0101, 0x0001, 0x0100, 0x0010, 0x1000, 0x0104, 0x0004, 0x1010};
static const uint16_t IAM_SHAKER[IAM_NPAT] = {0xAAAA, 0x5555, 0x2222, 0x4924, 0xAAAA, 0x1249, 0x4444, 0x2492};
static const uint16_t IAM_CRASH[IAM_NPAT] = {0x0001, 0x0100, 0x0010, 0x1000, 0x0001, 0x0400, 0x0080, 0x0004};
static const int IAM_ACID[IAM_NPAT][4] = {{45, 8, 1, 20}, {70, 16, 2, 35}, {25, 4, 1, 10}, {85, 16, 1, 50},
                                          {55, 12, 2, 30}, {15, 8, 1, 5},  {90, 16, 3, 60}, {40, 16, 1, 25}};

struct iam_snap {
    int16_t par[P_NPARAMS];
    uint16_t pats[EN_NPAT][EN_NLANE];
    uint16_t pool;
    uint8_t mask, style, sec, pat[IAM_NPART];
    uint8_t lvl[IAM_NPART];
    uint8_t own[3]; /* pad, bass, acid: the engine key's sound. Not one of the twelve named keys */
    uint8_t duck, swap; /* which parts a drop sat out; where the next break walks */
    uint8_t xown, xon; /* clap (bit 0) and open hat (bit 1): the player set it, and whether it is on */
    uint16_t don;      /* bit per black key, for the drums that are not arrangement parts */
    uint16_t pown;     /* this drum left the linked beat. A step edit or an FX mutate */
    uint16_t stepo;    /* the player wrote these steps. A later pattern write keeps them */
    uint16_t stepv[IAM_NDRUMKEY];
    uint16_t vstep[4]; /* pad, bass, acid, fourth. A bit is a sixteenth */
    uint16_t lay;      /* the twelve named keys. A click adds one. It does not replace the engine */
    uint32_t born_n;   /* the next age. 0 is unused, so a stamp of 0 means that key is off */
    uint32_t born[16]; /* engines 0..3, then Glass..Soft. Drums are not in this list */
    uint16_t sown;     /* a key or a wake has taken this drum. A section will not move it */
    uint8_t sw;        /* 1 once clap and open hat have been given their own level */
    uint8_t pseed;     /* 1 once each drum's pattern index has been taken from the song */
    uint8_t dpat[IAM_NDRUMKEY];
    uint8_t dkit[IAM_NDRUMKEY];
    uint8_t dlvl[IAM_NDRUMKEY];
    int8_t xtune[EN_XDRUM];
    int8_t haze_oct;       /* dust, chop, air, hush, wind. 0 is the part as written */
    uint8_t xdec[EN_XDRUM]; /* rim, tom, cowbell, shaker, crash. 50 is the length they had */
    uint8_t wob[4];        /* held knob 2 on pad, bass, acid, air. 0 is the song */
    uint8_t sweep[IAM_NDRUMKEY]; /* held knob 2 on a drum. 0 is the hit it already had */
};
struct iam_state {
    uint8_t on, auto_on, style, sec, form, mask, solo, dirty;
    uint8_t duck; /* parts a drop sat out. Ignored outside that section */
    uint8_t swap; /* walks forward, so the next break is a different pair */
    uint8_t hand; /* full gain until the next section. A click sets it. A silent change may set it back */
    uint8_t xown, xon; /* clap (bit 0) and open hat (bit 1): kept so an old undo still lines up */
    uint16_t don;      /* bit i: black key i is on, when that drum is not an arrangement part */
    uint16_t pown;     /* bit i: drum i left the linked beat. A step edit or an FX mutate */
    uint16_t stepo;    /* bit i: drum i's steps are the ones the player wrote */
    uint16_t stepv[IAM_NDRUMKEY];
    uint16_t vstep[4]; /* pad, bass, acid, fourth. Dust and chop open on their rhythm. The rest open full */
    uint16_t lay;      /* bit i: named key i is on. Glass through Soft. Beside the engines */
    uint32_t born_n;   /* next age for a synth key that just came on */
    uint32_t born[16]; /* when that engine or named key came on. 0 is off. Not saved in the song */
    uint8_t step_i;    /* which step OCT- will turn. 0 is step 1. It stays while the screen is latched */
    uint8_t step_undo; /* this visit already took one undo for a step. Moving the mark does not */
    uint8_t step_stay; /* 1 once SEQ was tapped while this key was held. The screen stays up */
    uint8_t seq_on;    /* SEQ: every track that is on, one row each */
    uint8_t seq_at;    /* which of those rows the white keys edit */
    uint8_t seq_n;     /* how many rows are on, refreshed each frame */
    uint8_t seq_undo;  /* this visit already took one undo for a step */
    uint8_t seq_isol;  /* SEL on this screen: only the selected row is heard */
    uint16_t sown;     /* bit i: the player owns drum i's on/off */
    uint8_t sw;        /* 1 once clap and the open hat keep the level they opened with */
    uint8_t pseed;
    uint8_t follow; /* this apply only. A section, a pump, or the drums OCT+ brings in. Not saved */
    uint8_t mut;   /* FX click: which change is next. Not saved */
    uint8_t dly;   /* free knob 1: ring, 0 off .. 100. Not saved */
    uint8_t hel;   /* free knob 2: wah, 0 off .. 100. Not saved. The helix stays off */
    uint8_t mos;   /* free knob 3: phaser, 0 off .. 100. Not saved */
    uint8_t kvol;  /* GLO white keys, 0 silent .. 100 the voice. Boots at 90. Not saved */
    uint8_t arp;   /* GLO: held white keys arpeggiate. Not saved, not an undo */
    uint8_t fx_twist; /* a knob moved while FX was down. The turn is ignored, and the release is not a mutate */
    uint8_t dpat[IAM_NDRUMKEY]; /* pattern index, one sequence per black key */
    uint8_t dkit[IAM_NDRUMKEY];
    uint8_t dlvl[IAM_NDRUMKEY]; /* clap, open hat, and the five above. 0..100 while on */
    int8_t xtune[EN_XDRUM];
    int8_t haze_oct;       /* dust, chop, air, hush, wind. 0 is the part as written. Not a project parameter */
    uint8_t xdec[EN_XDRUM]; /* rim, tom, cowbell, shaker, crash. 50 is the length they had */
    uint8_t wob[4];        /* held knob 2 on pad, bass, acid, air. 0 is the song. Not saved */
    uint8_t sweep[IAM_NDRUMKEY]; /* held knob 2 on a drum. 0 is the hit. Not saved */
    uint8_t card; /* 0: the song. odd: a tip, index card / 2 */
    uint8_t painted;
    uint8_t sel_arm; /* SEL went down alone, and SEQ is closed: the release opens the next tip */
    uint32_t picture; /* what the last paint showed */
    uint8_t pat[IAM_NPART], lvl[IAM_NPART];
    uint8_t own[3]; /* the pad, bass and acid keys. A named key is a different sound */
    uint8_t primed, turned, fx_down, sel_down; /* primed: the first frame is not a command */
    int8_t held;                               /* part under the finger, or -1 */
    int8_t held_key;                           /* the physical key, or -1. Not the part's white key */
    int8_t held_drum;                          /* black-key index under the finger, or -1 */
    int8_t held_pre;                           /* sound-key index under the finger, or -1 */
    uint8_t lock;                              /* bars a manual move owns the form */
    uint8_t dip;                               /* a pressed drop returns on the next bar */
    uint8_t dip_sec, dip_bar, dip_mask, dip_duck, dip_hand, dip_pump;
    int16_t dip_cut, dip_res;                  /* the acid filter, so an electro drop gives it back */
    uint8_t pad_own;                           /* bars the player owns the pad's sound and chord */
    uint8_t pad_laps;                          /* colour steps taken. The first stays on the style's row */
    uint8_t bar_n;                             /* bars into this section */
    uint8_t step_seen;                         /* last sixteenth the clock drew */
    uint8_t q_xor, q_find, q_scene;            /* waiting for the bar: part clicks, one place, one scene */
    uint8_t q_drums;                           /* OCT+ excite also brings in two silent drums. EDIT does not */
    uint8_t q_pump;                            /* further OCT+ presses, once excited: how many land this bar */
    uint8_t pump_at;                           /* which drum's steps the next pump thickens */
    uint8_t q_xtra;                            /* unused. The drums queue on q_don */
    uint8_t q_nowake;                          /* a kit edit from the clap or open hat must not wake the parent */
    uint8_t q_kit_set;                         /* bit per drum lane: a kit is waiting for the bar */
    uint8_t q_kit[4];                          /* the kit, when that bit is set */
    uint16_t q_don;                            /* a tap on clap, open hat, or one of the five, waiting for the bar */
    uint16_t q_lay;                            /* a tap on a named key, waiting for the bar. Two taps cancel */
    int8_t q_dpat[IAM_NDRUMKEY];               /* a pattern still waiting. Hold knob 2 does not write it */
    int8_t q_dkit[IAM_NDRUMKEY];               /* knob 1, per black key */
    int8_t q_oct, q_pst;                       /* octave steps, and preset detents, which net out */
    int8_t q_snd[IAM_NPART], q_pat[IAM_NPART]; /* sound and pattern detents, per part */
    uint32_t bar_seen, rng, home_t, knob_t;    /* knob_t: last detent, so a tap after a twist is not a tap */
    uint32_t fx_t;                             /* FX went down. A short release, with no twist, mutates */
    uint32_t hop_rng;                          /* which crossing a new song or the dial picks. Not the song's rng, and not saved */
    uint8_t hop_last;                          /* the crossing just used, so the next press is another */
    uint32_t find_rng;                         /* which place LFO picks. Not the song's rng, and not saved */
    uint8_t find_last;                         /* the place just used, so the next press is another */
    uint8_t find_kind;                         /* the place waiting: a chord, a part, the beat, the air */
    uint8_t find_room;                         /* Sleep: the room that place steps into */
    uint32_t key_t[NKEYS];
    uint32_t keys, btn;
    int undo_n;
    struct iam_snap undo[IAM_UNDO];
} iam;

static uint32_t iam_rng(void)
{
    iam.rng ^= iam.rng << 13;
    iam.rng ^= iam.rng >> 17;
    iam.rng ^= iam.rng << 5;
    return iam.rng;
}
static int iam_popcount(uint32_t m)
{
    int n = 0;
    while (m) {
        n += (int)(m & 1u);
        m >>= 1;
    }
    return n;
}
static void iam_set(int p, int v)
{
    const en_param_t *pi = en_param_info(p);
    if (v < pi->lo)
        v = pi->lo;
    if (v > pi->hi)
        v = pi->hi;
    if (proj.par[p] == (int16_t)v)
        return;
    proj.par[p] = (int16_t)v;
    en_set(p, v);
    edited();
    iam.dirty = 1;
}
/* What a key edits. The first four white keys are the engines. The rest pick a sound.
 * Every black key is one drum. Clap, the open hat, and the five above perc are not
 * arrangement parts, so holding one does not edit the snare or the hat. None edit the scale. */
static void iam_key_role(int key, int *part, int *drum, int *pre)
{
    int i;
    *part = -1;
    *drum = -1;
    *pre = -1;
    for (i = 0; i < 11; i++) {
        if (BLACK_KEY[i] != (uint8_t)key)
            continue;
        *drum = i;
        if (i == 0 || i == 1 || i == 3 || i == 5)
            *part = IAM_DRUM_PART[i];
        return;
    }
    for (i = 0; i < 16; i++) {
        if (WHITE_KEY[i] != (uint8_t)key)
            continue;
        if (i < 4) {
            *part = i;
            return;
        }
        *pre = i - 4;
        *part = IAM_PRE_PART[*pre];
        return;
    }
}
static int iam_gain(int part);
static int iam_drum_on(int i)
{
    if (i < 0 || i >= IAM_NDRUMKEY)
        return 0;
    if (i == 0)
        return (iam.mask >> 4) & 1;
    if (i == 1)
        return (iam.mask >> 5) & 1;
    if (i == 3)
        return (iam.mask >> 6) & 1;
    if (i == 5)
        return (iam.mask >> 7) & 1;
    return (iam.don >> i) & 1;
}
/* The fourth key is a different part on each vibe. */
static const char *iam_part_name(int part)
{
    static const char *const air[] = {"BED", "DUST", "CHOP", "AIR", "HUSH", "WIND"};
    int a;
    if (part == 3) {
        a = IAM_AIR[iam.style];
        if (a < 0 || a > 5)
            a = 0;
        return air[a];
    }
    if (iam.style == 12) {
        if (part == 1)
            return "DRONE";
        if (part == 2)
            return "FLUTE";
    }
    return IAM_PART_NAME[part];
}
static const char *iam_drum_row(int drum)
{
    static const char *const row[6] = {"KICK", "SNARE", "CLAP", "CHIME", "OPEN", "BELL"};
    if (drum < 0 || drum >= IAM_NDRUMKEY)
        return "";
    if (iam.style == 12 && drum < 6)
        return row[drum];
    return IAM_DRUM_ROW[drum];
}
static const char *iam_drum_name(int drum)
{
    static const char *const name[6] = {"KICK", "SNARE", "CLAP", "CHIMES", "OPEN HAT", "BELL"};
    if (drum < 0 || drum >= IAM_NDRUMKEY)
        return "";
    if (iam.style == 12 && drum < 6)
        return name[drum];
    return IAM_DRUM_NAME[drum];
}
static int iam_main_drum(int drum)
{
    return drum == 0 || drum == 1 || drum == 3 || drum == 5;
}
static uint16_t iam_drum_steps(int drum, int pat)
{
    pat &= 7;
    if (drum == 0)
        return IAM_KICK[pat];
    if (drum == 1)
        return IAM_SNARE[pat];
    if (drum == 2)
        return (uint16_t)(IAM_SNARE[pat] & 0x1010u);
    if (drum == 3)
        return IAM_HAT[pat];
    if (drum == 4) {
        /* Quarters of a shifted hat. Most hat rows are offbeats, so the quarters
         * were empty and the open hat never played, on every kit. */
        uint16_t h = IAM_HAT[(pat + 3) & 7];
        uint16_t q = (uint16_t)(h & 0x1111u);
        if (!q)
            q = (uint16_t)(h & 0x4444u);
        if (!q)
            q = 0x4444u;
        return q;
    }
    if (drum == 5)
        return IAM_PERC[pat];
    if (drum == 6)
        return IAM_RIM[pat];
    if (drum == 7)
        return IAM_TOM[pat];
    if (drum == 8)
        return IAM_BELL[pat];
    if (drum == 9)
        return IAM_SHAKER[pat];
    return IAM_CRASH[pat];
}
/* What plays when a drum is on and the song's row is blank. Off is the silence. */
static uint16_t iam_drum_floor(int drum)
{
    static const uint16_t fb[IAM_NDRUMKEY] = {
        0x1111, 0x1010, 0x1010, 0xAAAA, 0x4444, 0x0840, 0x0808, 0x4040, 0x0101, 0xAAAA, 0x0001,
    };
    if ((unsigned)drum >= IAM_NDRUMKEY)
        return 0x1010u;
    return fb[drum];
}
/* A written pattern plays instead of the preset. pat is the preset's index.
 * An on drum whose link is blank is already playing the floor. The row shows that.
 * A row the player cleared stays cleared. Off stays the silence. */
static uint16_t iam_steps_live(int drum, int pat)
{
    uint16_t st;
    if ((unsigned)drum < IAM_NDRUMKEY && (iam.stepo & (1u << drum)))
        return iam.stepv[drum];
    st = iam_drum_steps(drum, pat);
    if (!st && (unsigned)drum < IAM_NDRUMKEY && iam_drum_on(drum))
        return iam_drum_floor(drum);
    return st;
}
static void iam_seed_pats(void)
{
    int i;
    if (iam.pseed)
        return;
    iam.pseed = 1;
    iam.dpat[2] = iam.pat[5];
    iam.dpat[4] = iam.pat[6];
    iam.dkit[2] = (uint8_t)proj.par[P_KIT_SNARE];
    iam.dkit[4] = (uint8_t)proj.par[P_KIT_HAT];
    for (i = 6; i < IAM_NDRUMKEY; i++) {
        iam.dpat[i] = 0;
        iam.dkit[i] = (uint8_t)proj.par[P_KIT_KICK];
        iam.dlvl[i] = 0;
    }
}
static void iam_bind_main(void)
{
    iam.dpat[0] = iam.pat[4];
    iam.dpat[1] = iam.pat[5];
    iam.dpat[3] = iam.pat[6];
    iam.dpat[5] = iam.pat[7];
    iam.dkit[0] = (uint8_t)proj.par[P_KIT_KICK];
    iam.dkit[1] = (uint8_t)proj.par[P_KIT_SNARE];
    iam.dkit[3] = (uint8_t)proj.par[P_KIT_HAT];
    iam.dkit[5] = (uint8_t)proj.par[P_KIT_PERC];
}
/* The beat stays one groove until that drum is mutated, or its steps are written. */
static void iam_link_family(void)
{
    if (!(iam.pown & (1u << 2)))
        iam.dpat[2] = iam.dpat[1]; /* clap with the snare */
    if (!(iam.pown & (1u << 4)))
        iam.dpat[4] = iam.dpat[3]; /* open hat with the hat */
    if (!(iam.pown & (1u << 6)))
        iam.dpat[6] = iam.dpat[1]; /* rim with the snare */
    if (!(iam.pown & (1u << 7)))
        iam.dpat[7] = iam.dpat[5]; /* tom with the perc */
    if (!(iam.pown & (1u << 8)))
        iam.dpat[8] = iam.dpat[5]; /* cowbell with the perc */
    if (!(iam.pown & (1u << 9)))
        iam.dpat[9] = iam.dpat[3]; /* shaker with the hat */
    if (!(iam.pown & (1u << 10)))
        iam.dpat[10] = iam.dpat[0]; /* crash with the kick */
}
/* The pattern and the kit the knob is aiming at. They wrap the same way the bar applies them. */
static int iam_drum_pat_aim(int drum)
{
    int base = iam_main_drum(drum) ? iam.pat[IAM_DRUM_PART[drum]] : iam.dpat[drum];
    return (base + (int)iam.q_dpat[drum] + IAM_NPAT * 4) % IAM_NPAT;
}
static int iam_drum_kit_aim(int drum)
{
    int cur = iam_main_drum(drum) ? proj.par[IAM_SND[IAM_DRUM_PART[drum]]] : iam.dkit[drum];
    return (cur + (int)iam.q_dkit[drum] + EN_NKIT * 8) % EN_NKIT;
}
static void iam_isol_aim(int *part, int *drum);
static void iam_xpublish(void)
{
    int i, ip = -1, id = -1, isol = iam.seq_isol && iam.seq_on;
    if (isol)
        iam_isol_aim(&ip, &id);
    en_lane_kit(2, iam.dkit[2]);
    en_lane_kit(4, iam.dkit[4]);
    for (i = 0; i < EN_XDRUM; i++) {
        int d = 6 + i;
        int on = (iam.don >> d) & 1;
        if (isol && d != id)
            on = 0; /* SEQ solo is this row, not the drums beside it */
        uint16_t st = iam_steps_live(d, iam.dpat[d]);
        if (on && !st && !(iam.stepo & (1u << d)))
            st = iam_drum_floor(d); /* a blank row does not come on silent */
        en_xdrum(i, on, iam.dkit[d], st, on ? iam.dlvl[d] : 0, iam.xtune[i], iam.xdec[i]);
    }
}
/* Lanes already published. An on drum whose row is empty takes the floor.
 * A row the player cleared stays cleared. */
static void iam_drums_hear(void)
{
    int d, s;
    for (d = 0; d < EN_NLANE; d++) {
        uint16_t fb;
        if (!iam_drum_on(d) || (iam.stepo & (1u << d)))
            continue;
        fb = iam_drum_floor(d);
        for (s = 0; s < EN_NPAT; s++) {
            if (en_pats[s][d])
                continue;
            en_pats[s][d] = fb;
            proj.slot[proj.piece].pats[s][d] = fb;
        }
    }
}
/* Drum lanes only. The acid line is separate: density and range grow it again. */
static void iam_write_drum_pats(void)
{
    int s, lane;
    static const uint16_t sparse[EN_NLANE] = {0, 0, 0, 0x0001u, 0x0001u, 0x0001u};
    iam_seed_pats();
    iam_bind_main();
    iam_link_family();
    for (s = 0; s < EN_NPAT; s++) {
        for (lane = 0; lane < EN_NLANE; lane++) {
            /* Sleep's clock uses one hit, unless this drum was written by hand.
             * A written row is the same in every pattern slot, so a later write
             * does not put the preset back. */
            if (iam.stepo & (1u << lane))
                en_pats[s][lane] = iam.stepv[lane];
            else if (iam.style == 12)
                en_pats[s][lane] = sparse[lane];
            else
                en_pats[s][lane] = iam_drum_steps(lane, iam.dpat[lane] + s);
            /* On, and the row is blank, and the player did not clear it.
             * Sleep's empty kit stays empty while those drums are off. */
            if (!en_pats[s][lane] && iam_drum_on(lane) && !(iam.stepo & (1u << lane)))
                en_pats[s][lane] = iam_drum_floor(lane);
            proj.slot[proj.piece].pats[s][lane] = en_pats[s][lane];
        }
    }
    iam_xpublish();
}
static void iam_write_drums(void)
{
    iam_write_drum_pats();
    if (iam.style == 12) {
        /* A few long notes in one octave. The wide pattern would chatter. */
        iam_set(P_AC_DENS, 10);
        iam_set(P_AC_LEN, 12);
        iam_set(P_AC_RANGE, 1);
        iam_set(P_AC_MUT, 4);
    } else if (iam.style == 6 && (iam.pat[2] & 7) == IAM_ACID_PAT[6]) {
        /* one octave, a line you can follow. The wide pattern never settles into a tune */
        iam_set(P_AC_DENS, 90);
        iam_set(P_AC_LEN, 16);
        iam_set(P_AC_RANGE, 1);
        iam_set(P_AC_MUT, 20);
    } else {
        const int *a = IAM_ACID[iam.pat[2] & 7];
        iam_set(P_AC_DENS, a[0]);
        iam_set(P_AC_LEN, a[1]);
        iam_set(P_AC_RANGE, a[2]);
        iam_set(P_AC_MUT, a[3]);
    }
    iam_set(P_CHORD, iam.pat[0] % EN_NCHORD);
    iam_set(P_DE_TRIG, iam.pat[1] % 6);
    iam_set(P_NZ_TONE, IAM_HAZE[iam.style][iam.pat[3] & 7][0]);
    iam_set(P_NZ_PUMP, IAM_HAZE[iam.style][iam.pat[3] & 7][1]);
}
static int iam_gain(int part)
{
    /* Dnb's pad and hat match the opening the seed-42 piece is known by.
     * Each other vibe opens as its own music. */
    static const int INTRO[IAM_NSTYPE][IAM_NPART] = {
        {100, 70, 0, 55, 80, 70, 45, 20},
        {100, 88, 0, 72, 74, 0, 58, 36},
        {92, 60, 48, 20, 80, 0, 64, 0},
        {100, 82, 0, 88, 0, 0, 0, 0},
        {100, 72, 0, 40, 78, 48, 58, 0},
        {88, 86, 0, 62, 74, 70, 36, 0},
        {0, 80, 88, 48, 82, 64, 56, 0},
        {100, 74, 0, 36, 84, 42, 64, 0},
        {90, 82, 0, 40, 76, 74, 34, 0},
        {78, 88, 0, 46, 72, 58, 66, 28},
        {64, 92, 0, 52, 86, 70, 28, 0},
        {100, 76, 0, 58, 62, 24, 40, 0},
        {80, 100, 80, 100, 0, 0, 100, 100}, /* Sleep: the drone and the wind. The rest wait */
    };
    if ((iam.hand >> part) & 1)
        return 100; /* the player just asked for this part */
    if (iam.sec == IAM_DROP && ((iam.duck >> part) & 1))
        return 0; /* one or two parts sit out. The rest stay at full */
    if (!iam.auto_on && !iam.lock)
        return 100;
    if (iam.sec == IAM_INTRO)
        return INTRO[iam.style][part];
    if (iam.sec == IAM_EXCITE && part >= 4 && iam.style != 12)
        return 100; /* Sleep keeps the chime at its own level */
    return 100;
}
/* Stabs, dub, pings, hits and rave die on the off-beat. A singing pad at this
 * level is a swell. One of these is a slap, so the hit sits at half.
 * Glass sings, and the bell came in too strong. It sits at 32% of that
 * swell. The stored level stays, so the percent is still the knob. */
static int iam_pad_gated(int snd)
{
    return snd == 6 || snd == 8 || snd == 12 || snd == 13 || snd == 14;
}
static int iam_pad_lvl(int lvl)
{
    if (proj.par[P_PADTONE] == 3)
        lvl = lvl * 32 / 100;
    else if (iam_pad_gated(proj.par[P_PADTONE]))
        lvl /= 2;
    return lvl;
}
/* The named keys use this level even when the part itself is sitting out.
 * Solo still drops the keys of the parts that are not the one held. */
static int iam_layer_lin(int part)
{
    int lvl = (int)iam.lvl[part] * iam_gain(part) / 100;
    if (lvl < 0)
        lvl = 0;
    if (lvl > 100)
        lvl = 100;
    return lvl;
}
/* Four synth keys. Slot 0..3 are the engines, 4..15 the named keys.
 * A key that is off has no age. One that just came on is the newest.
 * Past four, the oldest goes off before this mix is published.
 * Five, with the song's own filters, missed the audio deadline. */
static int iam_slot_on(int s)
{
    if (s < 4)
        return (iam.mask >> s) & 1;
    return (iam.lay >> (s - 4)) & 1;
}
static void iam_slot_off(int s)
{
    if (s < 4) {
        iam.mask = (uint8_t)(iam.mask & ~(1u << s));
        iam.hand = (uint8_t)(iam.hand & ~(1u << s));
    } else
        iam.lay = (uint16_t)(iam.lay & ~(1u << (s - 4)));
    iam.born[s] = 0;
}
static void iam_cap(void)
{
    int s, n = 0, guard;
    for (s = 0; s < 16; s++) {
        if (!iam_slot_on(s)) {
            iam.born[s] = 0;
            continue;
        }
        if (!iam.born[s]) {
            if (++iam.born_n == 0)
                iam.born_n = 1; /* 0 stays the off stamp */
            iam.born[s] = iam.born_n;
        }
        n++;
    }
    for (guard = 0; n > IAM_CAP && guard < 16; guard++) {
        int oldest = -1;
        uint32_t best = 0xFFFFFFFFu;
        for (s = 0; s < 16; s++) {
            if (iam.born[s] && iam.born[s] < best) {
                best = iam.born[s];
                oldest = s;
            }
        }
        if (oldest < 0)
            break;
        iam_slot_off(oldest);
        n--;
    }
}
static void iam_layers_send(void)
{
    uint16_t m = iam.lay;
    int i, ip = -1, id = -1;
    if (iam.solo) {
        for (i = 0; i < 12; i++)
            if (!((iam.solo >> IAM_PRE_PART[i]) & 1))
                m = (uint16_t)(m & ~(1u << i));
    }
    if (iam.seq_isol && iam.seq_on) {
        iam_isol_aim(&ip, &id);
        (void)id; /* a named key follows its engine. The drum id is the row's own voice */
        for (i = 0; i < 12; i++)
            if ((int)IAM_PRE_PART[i] != ip)
                m = (uint16_t)(m & ~(1u << i));
    }
    en_layers(m);
    en_layer_house(iam.style == 7); /* House */
    for (i = 0; i < 3; i++)
        en_layer_level(i, iam_layer_lin(i));
}
static void iam_apply(void)
{
    int i, solo = iam.solo, hat_applied = 0, clap_lvl, open_lvl;
    int ip = -1, id = -1, isol = iam.seq_isol && iam.seq_on;
    if (isol)
        iam_isol_aim(&ip, &id); /* one row. A hat does not keep its open hat */
    iam_cap(); /* the fifth synth key is already in the mask. The oldest leaves first */
    for (i = 0; i < IAM_NPART; i++) {
        int on = (iam.mask >> i) & 1, g = iam_gain(i), lvl = iam.lvl[i] * g / 100;
        if (isol && i != ip)
            lvl = 0;
        else if (solo && !(solo >> i & 1))
            lvl = 0;
        else if (!on)
            lvl = 0;
        if (lvl > 100)
            lvl = 100;
        if (i == 0)
            lvl = iam_pad_lvl(lvl); /* a stabbed colour does not take the level of a singing pad */
        iam_set(IAM_LVL[i], lvl);
        if (i == 6)
            hat_applied = lvl;
    }
    if (!iam.sw) {
        /* The opening levels. Later sections do not rewrite them.
         * The clap keeps a share of the snare as stored. The intro can mute
         * the snare, and that must not freeze the clap at silence. */
        iam.dlvl[2] = (int)iam.lvl[5] * 70 / 100;
        iam.dlvl[4] = hat_applied * 55 / 100;
        iam.don = (uint16_t)(iam.don & ~((1u << 2) | (1u << 4)));
        if ((iam.mask >> 5) & 1) {
            iam.don = (uint16_t)(iam.don | (1u << 2));
            if (!iam.dlvl[2])
                iam.dlvl[2] = 76; /* on, and the share came out as nothing */
        }
        if ((iam.mask >> 6) & 1)
            iam.don = (uint16_t)(iam.don | (1u << 4));
        iam.sw = 1;
    }
    if (iam.follow) {
        /* A section, a pump, or the drums OCT+ brings in. An untouched clap
         * rides the snare, and an untouched open hat rides the hat. A key
         * already taken stays. The five above perc stay off until their key. */
        if (!(iam.sown & (1u << 2))) {
            int was = (iam.don >> 2) & 1;
            iam.don = (uint16_t)(iam.don & ~(1u << 2));
            if ((iam.mask >> 5) & 1) {
                iam.don = (uint16_t)(iam.don | (1u << 2));
                if (!was && !iam.dlvl[2])
                    iam.dlvl[2] = 76; /* the song just turned it on. Off stays quiet */
            }
        }
        if (!(iam.sown & (1u << 4))) {
            iam.don = (uint16_t)(iam.don & ~(1u << 4));
            if ((iam.mask >> 6) & 1)
                iam.don = (uint16_t)(iam.don | (1u << 4));
        }
        iam.follow = 0;
    }
    clap_lvl = (iam.don & (1u << 2)) ? iam.dlvl[2] : 0;
    open_lvl = (iam.don & (1u << 4)) ? iam.dlvl[4] : 0;
    if (isol && id != 2)
        clap_lvl = 0;
    else if (solo && !(solo >> 5 & 1))
        clap_lvl = 0;
    if (isol && id != 4)
        open_lvl = 0;
    else if (solo && !(solo >> 6 & 1))
        open_lvl = 0;
    if (clap_lvl > 100)
        clap_lvl = 100;
    if (open_lvl > 100)
        open_lvl = 100;
    iam_set(P_LANE_LVL(2), clap_lvl);
    iam_set(P_LANE_LVL(4), open_lvl);
    iam_set(P_PULSE, (iam.mask & 0xF0) ? 84 : 36);
    iam_xpublish();
    iam_drums_hear(); /* a drum that just came on still has the steps it will play */
    for (i = 0; i < 4; i++)
        en_vstep(i, iam.vstep[i]);
    iam_layers_send();
    iam.dirty = 1;
}
/* The pad sound changed and the rest of the mix did not. Glass takes 32%
 * and a stabbed colour takes half. Another singing sound takes the level
 * the song is using. */
static void iam_pad_sit(void)
{
    int on = (iam.mask & 1u) != 0;
    int lvl = (int)iam.lvl[0] * iam_gain(0) / 100;
    if (iam.seq_isol && iam.seq_on) {
        int ip = -1, id = -1;
        iam_isol_aim(&ip, &id); /* a colour step must not bring the pad back under a solo */
        (void)id;
        if (ip != 0 || !on)
            lvl = 0;
    } else if (iam.solo && !(iam.solo & 1))
        lvl = 0;
    else if (!on)
        lvl = 0;
    if (lvl > 100)
        lvl = 100;
    iam_set(P_PAD, iam_pad_lvl(lvl));
}
static void iam_open_filters(void)
{
    iam_set(P_BRIGHT, proj.par[P_BRIGHT] + 18);
    iam_set(P_DR_CUT, proj.par[P_DR_CUT] + 16);
    iam_set(P_AC_CUT, proj.par[P_AC_CUT] + 20);
    iam_set(P_AC_RES, proj.par[P_AC_RES] + 8);
}
/* the next value in a style's row, or the first row entry above cur when cur is outside it */
static int iam_next_in(const uint8_t *row, int n, int cur, int mod)
{
    int i, best = 0, bestd = mod;
    cur %= mod;
    if (cur < 0)
        cur += mod;
    for (i = 0; i < n; i++) {
        int d = ((int)row[i] - cur + mod) % mod;
        if (d == 0)
            return row[(i + 1) % n];
        if (d < bestd) {
            bestd = d;
            best = i;
        }
    }
    return row[best];
}
static void iam_pad_degree(void)
{
    if (iam.held == 0 || en_frozen)
        return; /* a finger on the pad, or freeze: the chord waits */
    en_next();
}
/* One drum pattern on, so a long song does not keep the beat it woke up with.
 * Sleep's lanes are the clock's. They stay a single hit. */
static void iam_walk_pattern(void)
{
    static const int part[4] = {6, 4, 5, 7}; /* hat, kick, snare, perc */
    int p;
    if (iam.style == 12)
        return;
    p = part[iam.pad_laps & 3];
    iam.pat[p] = (uint8_t)((iam.pat[p] + 1) & 7);
    iam_write_drum_pats();
}
/* sound: a new timbre. home: the style's own chord, unless it is already there.
 * The first step stays on the style's row. Later steps leave it. */
static void iam_pad_colour_go(int sound, int home)
{
    const uint8_t *row;
    int c, first;
    first = iam.pad_laps == 0;
    if (sound) {
        if (first) {
            iam_set(P_PADTONE, iam_next_in(IAM_PAD_SND[iam.style], 4, proj.par[P_PADTONE], EN_NSOUND));
        } else {
            int s = IAM_PAD_MORE[iam.style][(iam.pad_laps - 1) & 7];
            if (s == proj.par[P_PADTONE])
                s = IAM_PAD_MORE[iam.style][iam.pad_laps & 7];
            iam_set(P_PADTONE, s);
        }
    }
    row = IAM_PAD_CH[iam.style];
    if (first || home) {
        c = home && (int)iam.pat[0] != (int)row[0] ? (int)row[0] : iam_next_in(row, 4, iam.pat[0], EN_NCHORD);
    } else {
        c = IAM_CH_MORE[(iam.pad_laps + iam.style) % 6];
        if (c == (int)iam.pat[0])
            c = IAM_CH_MORE[(iam.pad_laps + iam.style + 1) % 6];
    }
    iam.pat[0] = (uint8_t)c;
    iam_set(P_CHORD, c);
    iam_set(P_VOICING, proj.par[P_VOICING] ? 0 : 1);
    if (!first)
        iam_walk_pattern();
    iam.pad_laps++;
    iam_pad_sit(); /* the new colour, at the level that colour can stand */
}
/* The conductor, and a song mutate with the pad free. A finger on the pad,
 * or eight bars of owning it, keeps that colour. FX on the key itself does not. */
static void iam_pad_colour(int sound, int home)
{
    if (iam.held == 0 || iam.pad_own)
        return;
    iam_pad_colour_go(sound, home);
}
static void iam_stamp_beat(void)
{
    int i;
    for (i = 0; i < 4; i++)
        iam.pat[4 + i] = IAM_BEAT[iam.style][i];
}
static int iam_roll(int n)
{
    return (int)(iam_rng() % (uint32_t)n);
}
/* Sounds, kits, the beat, the noise, the acid line. The pad waits: the groove's
 * colour step would pull a wider pad back into the four. Electro keeps the line
 * you can follow. Sleep writes its own notes, and its own lanes. */
static void iam_visit_band(void)
{
    int s = iam.style, b, i;
    iam_set(P_DR_INST, IAM_VISIT_BASS[s][iam_roll(IAM_NVISIT)]);
    iam_set(P_AC_WAVE, IAM_VISIT_ACID[s][iam_roll(IAM_NVISIT)]);
    iam_set(P_KIT_KICK, IAM_VISIT_KIT[s][iam_roll(IAM_NVISIT)]);
    iam_set(P_KIT_SNARE, IAM_VISIT_KIT[s][iam_roll(IAM_NVISIT)]);
    iam_set(P_KIT_HAT, IAM_VISIT_KIT[s][iam_roll(IAM_NVISIT)]);
    iam_set(P_KIT_PERC, IAM_VISIT_KIT[s][iam_roll(IAM_NVISIT)]);
    b = iam_roll(IAM_NVISIT);
    if (s != 12) {
        for (i = 0; i < 4; i++)
            iam.pat[4 + i] = IAM_VISIT_BEAT[s][b][i];
    }
    iam_set(P_NZ_COLOR, IAM_VISIT_COL[s][iam_roll(IAM_NVISIT)]);
    iam.pat[3] = (uint8_t)iam_roll(8);
    b = iam_roll(IAM_NVISIT);
    if (s != 6 && s != 12)
        iam.pat[2] = IAM_VISIT_APAT[s][b];
}
/* Stabs, dub, pings, hits and rave die on the off-beat. A vibe that does not
 * already open on one of those keeps a singing pad. iam_pad_gated is above. */
static void iam_visit_open(void)
{
    int s = iam.style;
    int n = iam_roll(12);
    int snd = n < 4 ? IAM_PAD_SND[s][n] : IAM_PAD_MORE[s][n - 4];
    int c, i, in = 0;
    if (n >= 4 && iam_pad_gated(snd)) {
        for (i = 0; i < 4; i++)
            if (IAM_PAD_SND[s][i] == (uint8_t)snd)
                in = 1;
        if (!in)
            snd = IAM_PAD_SND[s][n & 3];
    }
    iam_set(P_PADTONE, snd);
    c = IAM_PAD_CH[s][iam_roll(4)];
    iam.pat[0] = (uint8_t)c;
    iam_set(P_CHORD, c);
    iam_set(P_VOICING, iam_roll(2));
    iam_pad_sit(); /* enter already applied the colour step's level */
}
/* The engine's breakdown mutes every drum hit and shuts the drone's filter.
 * IAM never asks for it. A break is a thinner arrangement in the groove. */
static void iam_leave_break(void)
{
    if (en_sec != EN_SEC_GROOVE)
        en_section(EN_CMD_DROP);
}
/* Sit one part out, or two when the song is already full. The pad goes last. */
static void iam_quiet(void)
{
    static const int pref[IAM_NPART] = {5, 7, 2, 3, 6, 4, 1, 0};
    int i, on = 0, want, n = 0;
    iam.duck = 0;
    for (i = 0; i < IAM_NPART; i++)
        if ((iam.mask >> i) & 1)
            on++;
    want = on >= 4 ? 2 : (on >= 2 ? 1 : 0);
    for (i = 0; i < IAM_NPART && n < want; i++) {
        int p = pref[i];
        if ((iam.mask >> p) & 1) {
            iam.duck = (uint8_t)(iam.duck | (1u << p));
            n++;
        }
    }
}
/* Two that are sounding step out. Two that are quiet step in. */
static void iam_swap(void)
{
    int on[IAM_NPART], off[IAM_NPART];
    int n_on = 0, n_off = 0, i;
    for (i = 0; i < IAM_NPART; i++) {
        if ((iam.mask >> i) & 1)
            on[n_on++] = i;
        else
            off[n_off++] = i;
    }
    for (i = 0; i < 2 && n_on > 0; i++) {
        int k = (int)(iam.swap % (uint8_t)n_on);
        int part = on[k];
        on[k] = on[n_on - 1];
        n_on--;
        iam.mask = (uint8_t)(iam.mask & ~(1u << part));
        iam.swap++;
    }
    for (i = 0; i < 2 && n_off > 0; i++) {
        int k = (int)(iam.swap % (uint8_t)n_off);
        int part = off[k];
        off[k] = off[n_off - 1];
        n_off--;
        iam.mask = (uint8_t)(iam.mask | (1u << part));
        iam.swap++;
    }
}
/* Electro: bass and dust can step back. The keys the song drew stay on.
 * The acid is not forced back on if the draw left it dark. */
static void iam_electro_break(void)
{
    int n = 0;
    if (iam.mask & (1u << 1)) {
        iam.mask = (uint8_t)(iam.mask & ~(1u << 1));
        n++;
    }
    if (iam.mask & (1u << 3)) {
        iam.mask = (uint8_t)(iam.mask & ~(1u << 3));
        n++;
    }
    if (n < 2 && (iam.mask & (1u << 7)))
        iam.mask = (uint8_t)(iam.mask & ~(1u << 7));
}
/* The drop sits the backing out. The line keeps playing. */
static void iam_electro_drop(void)
{
    iam.duck = 0;
    if (iam.mask & (1u << 1))
        iam.duck = (uint8_t)(iam.duck | (1u << 1));
    if (iam.mask & (1u << 3))
        iam.duck = (uint8_t)(iam.duck | (1u << 3));
    if (!iam.duck && (iam.mask & (1u << 5)))
        iam.duck = (uint8_t)(1u << 5);
}
/* Each section opens the filter a step, so a line that is on arrives instead of jumping.
 * The draw chose the keys. A later section does not put the same ones back. */
static void iam_electro_song(int sec)
{
    static const uint8_t cut[IAM_NSEC] = {46, 64, 80, 58, 76, 68};
    static const uint8_t res[IAM_NSEC] = {72, 84, 92, 78, 88, 84};
    if (iam.held == 2)
        return; /* the acid key owns the filter while it is held */
    iam_set(P_AC_CUT, cut[sec]);
    iam_set(P_AC_RES, res[sec]);
}
/* Sleep moves by who is in the room. The engine stays in the groove.
 * Intro is the drone and the wind. The groove adds the flute and a chime.
 * Excite brings the chord. The break is wind, chimes and a bell. The drop is
 * wind alone. Glide is the whole slow band. A few drums may sit beside it. */
static void iam_sleep_song(int sec)
{
    static const uint8_t mask[IAM_NSEC] = {0x0A, 0x4E, 0x4B, 0xC8, 0x08, 0x4F};
    if (sec < 0 || sec >= IAM_NSEC)
        sec = IAM_GROOVE;
    iam.mask = mask[sec];
    iam.duck = 0;
    if (sec == IAM_BREAK)
        en_sleep_cue(3); /* a chime and a bell on the next downbeat */
}
/* What this part would publish if the arrangement left it on at full gain.
 * Glass and a stabbed pad still count, at the level the mix actually uses. */
static int iam_kept_lvl(int part)
{
    int lvl = iam.lvl[part];
    if (part == 0)
        lvl = iam_pad_lvl(lvl);
    if (lvl < 0)
        lvl = 0;
    if (lvl > 100)
        lvl = 100;
    return lvl;
}
static int iam_part_pub(int part)
{
    int g, lvl;
    if (part < 0 || part >= IAM_NPART)
        return 0;
    if (iam.solo && !((iam.solo >> part) & 1))
        return 0;
    if (!((iam.mask >> part) & 1))
        return 0;
    g = iam_gain(part);
    lvl = (int)iam.lvl[part] * g / 100;
    if (lvl < 0)
        lvl = 0;
    if (lvl > 100)
        lvl = 100;
    if (part == 0)
        lvl = iam_pad_lvl(lvl);
    return lvl;
}
static int iam_lay_pub(int i)
{
    int part;
    if (i < 0 || i >= 12 || !((iam.lay >> i) & 1))
        return 0;
    part = IAM_PRE_PART[i];
    if (iam.solo && !((iam.solo >> part) & 1))
        return 0;
    return iam_layer_lin(part);
}
/* Clap, open hat, and the five above the perc. A main drum is the part. */
static int iam_side_on(int d)
{
    if (d < 0 || d >= IAM_NDRUMKEY || iam_main_drum(d))
        return 0;
    if (!((iam.don >> d) & 1) || iam.dlvl[d] <= 0)
        return 0;
    if (d == 2 && iam.solo && !((iam.solo >> 5) & 1))
        return 0;
    if (d == 4 && iam.solo && !((iam.solo >> 6) & 1))
        return 0;
    return 1;
}
static int iam_audible(void)
{
    int i;
    for (i = 0; i < IAM_NPART; i++)
        if (iam_part_pub(i) > 0)
            return 1;
    for (i = 0; i < 12; i++)
        if (iam_lay_pub(i) > 0)
            return 1;
    for (i = 0; i < IAM_NDRUMKEY; i++)
        if (iam_side_on(i))
            return 1;
    return 0;
}
/* The intro table, a drop, or a swap can publish every lit part at zero.
 * Those parts stay at full gain. One part the change just removed comes
 * back when the new set has nothing of its own. A mix that already has a
 * voice is left as the section wrote it. */
static void iam_rescue(uint8_t was)
{
    int i, any = 0;
    if (iam.seq_isol && iam.seq_on)
        return; /* a solo is a quiet mix on purpose. The song underneath stays */
    if (iam_audible())
        return;
    for (i = 0; i < IAM_NPART; i++) {
        if (!((iam.mask >> i) & 1))
            continue;
        if (iam.solo && !((iam.solo >> i) & 1))
            continue;
        if (iam_kept_lvl(i) <= 0)
            continue;
        iam.hand = (uint8_t)(iam.hand | (1u << i));
        iam.duck = (uint8_t)(iam.duck & ~(1u << i));
        any = 1;
    }
    if (any && iam_audible())
        return;
    for (i = 0; i < 12; i++) {
        int p;
        if (!((iam.lay >> i) & 1))
            continue;
        p = IAM_PRE_PART[i];
        if (iam.solo && !((iam.solo >> p) & 1))
            continue;
        if (iam_kept_lvl(p) <= 0)
            continue;
        iam.hand = (uint8_t)(iam.hand | (1u << p));
        iam.duck = (uint8_t)(iam.duck & ~(1u << p));
        any = 1;
    }
    if (any && iam_audible())
        return;
    for (i = 0; i < IAM_NPART; i++) {
        if (!((was >> i) & 1) || ((iam.mask >> i) & 1))
            continue;
        if (iam.solo && !((iam.solo >> i) & 1))
            continue;
        if (iam_kept_lvl(i) <= 0)
            continue;
        iam.mask = (uint8_t)(iam.mask | (1u << i));
        iam.hand = (uint8_t)(iam.hand | (1u << i));
        iam.duck = (uint8_t)(iam.duck & ~(1u << i));
        return;
    }
}
static void iam_enter(int sec)
{
    uint8_t was = iam.mask;
    iam.hand = 0; /* the form takes the levels back; parts the player left on stay on */
    iam.pump_at = 0; /* a new section starts the step walk on the hat */
    iam.sec = (uint8_t)sec;
    iam.bar_n = 0;
    if (sec == IAM_BREAK) {
        iam_leave_break();
        if (iam.style == 6)
            iam_electro_break(); /* two out, and the lead stays */
        else if (iam.style != 12)
            iam_swap(); /* two out, two in. Sleep replaces the mask itself */
        iam_pad_degree(); /* one new degree, then the colour holds */
    } else if (sec == IAM_DROP) {
        iam_leave_break();
        if (iam.style == 6)
            iam_electro_drop(); /* the backing sits out. The line stays */
        else if (iam.style != 12)
            iam_quiet(); /* one or two sit out. The rest of the song stays */
    } else if (sec == IAM_EXCITE) {
        iam_leave_break();
        if (iam.style != 12)
            iam_open_filters(); /* Sleep stays dark. The chord is the lift */
        iam_pad_colour(1, 0);
        iam_pad_degree();
    } else if (sec == IAM_GLIDE) {
        int d = 4 + (int)(iam_rng() & 3u); /* one drum, read and written once */
        iam.pat[d] = (uint8_t)((iam.pat[d] + 1) & 7);
        iam_pad_colour(1, 0);
        iam_write_drums();
        iam_pad_degree();
        iam_leave_break();
    } else if (sec == IAM_GROOVE) {
        iam_pad_colour(1, 0);
        iam_pad_degree();
        iam_leave_break();
    } else {
        iam_leave_break();
    }
    if (iam.style == 6)
        iam_electro_song(sec);
    else if (iam.style == 12)
        iam_sleep_song(sec);
    iam_rescue(was); /* the intro, the swap, or the drop must not publish a silent mix */
    iam.follow = 1; /* the mask is final. An untouched clap and open hat join it */
    iam_apply();
    say(IAM_SEC_NAME[iam.sec], iam.auto_on ? "AUTO" : "MANUAL");
}
static void iam_push(void)
{
    struct iam_snap *s;
    int p, l;
    if (iam.undo_n >= IAM_UNDO) {
        int u;
        for (u = 0; u < IAM_UNDO - 1; u++)
            iam.undo[u] = iam.undo[u + 1]; /* the device libc has no memmove */
        iam.undo_n = IAM_UNDO - 1;
    }
    s = &iam.undo[iam.undo_n++];
    for (p = 0; p < P_NPARAMS; p++)
        s->par[p] = proj.par[p];
    for (p = 0; p < EN_NPAT; p++)
        for (l = 0; l < EN_NLANE; l++)
            s->pats[p][l] = en_pats[p][l];
    s->pool = proj.pool;
    s->mask = iam.mask;
    s->style = iam.style;
    s->sec = iam.sec;
    s->duck = iam.duck;
    s->swap = iam.swap;
    s->xown = iam.xown;
    s->xon = iam.xon;
    s->don = iam.don;
    s->pown = iam.pown;
    s->sown = iam.sown;
    s->sw = iam.sw;
    s->pseed = iam.pseed;
    s->stepo = iam.stepo;
    for (p = 0; p < IAM_NDRUMKEY; p++) {
        s->dpat[p] = iam.dpat[p];
        s->dkit[p] = iam.dkit[p];
        s->dlvl[p] = iam.dlvl[p];
        s->stepv[p] = iam.stepv[p];
    }
    for (p = 0; p < 4; p++)
        s->vstep[p] = iam.vstep[p];
    s->lay = iam.lay;
    s->born_n = iam.born_n;
    for (p = 0; p < 16; p++)
        s->born[p] = iam.born[p];
    for (p = 0; p < EN_XDRUM; p++) {
        s->xtune[p] = iam.xtune[p];
        s->xdec[p] = iam.xdec[p];
    }
    s->haze_oct = iam.haze_oct;
    for (p = 0; p < 4; p++)
        s->wob[p] = iam.wob[p];
    for (p = 0; p < IAM_NDRUMKEY; p++)
        s->sweep[p] = iam.sweep[p];
    for (p = 0; p < IAM_NPART; p++) {
        s->pat[p] = iam.pat[p];
        s->lvl[p] = iam.lvl[p];
    }
    for (p = 0; p < 3; p++)
        s->own[p] = iam.own[p];
}
static void iam_pop(void)
{
    struct iam_snap *s;
    int p, l;
    if (iam.undo_n <= 0) {
        say("NOTHING", "BEHIND");
        return;
    }
    s = &iam.undo[--iam.undo_n];
    for (p = 0; p < P_NPARAMS; p++)
        if (!en_par_global(p))
            iam_set(p, s->par[p]);
    for (p = 0; p < EN_NPAT; p++)
        for (l = 0; l < EN_NLANE; l++) {
            en_pats[p][l] = s->pats[p][l];
            proj.slot[proj.piece].pats[p][l] = s->pats[p][l];
        }
    proj.pool = s->pool;
    en_pool(s->pool);
    iam.mask = s->mask;
    iam.style = s->style;
    iam.sec = s->sec;
    iam.duck = s->duck;
    iam.swap = s->swap;
    iam.xown = s->xown;
    iam.xon = s->xon;
    iam.don = s->don;
    iam.pown = s->pown;
    iam.sown = s->sown;
    iam.sw = s->sw;
    iam.pseed = s->pseed;
    iam.stepo = s->stepo;
    for (p = 0; p < IAM_NDRUMKEY; p++) {
        iam.dpat[p] = s->dpat[p];
        iam.dkit[p] = s->dkit[p];
        iam.dlvl[p] = s->dlvl[p];
        iam.stepv[p] = s->stepv[p];
    }
    for (p = 0; p < 4; p++)
        iam.vstep[p] = s->vstep[p];
    iam.lay = s->lay;
    iam.born_n = s->born_n;
    for (p = 0; p < 16; p++)
        iam.born[p] = s->born[p];
    for (p = 0; p < EN_XDRUM; p++) {
        iam.xtune[p] = s->xtune[p];
        iam.xdec[p] = s->xdec[p];
    }
    iam.haze_oct = s->haze_oct;
    for (p = 0; p < 4; p++) {
        iam.wob[p] = s->wob[p];
        en_wobble(p, s->wob[p]);
    }
    for (p = 0; p < IAM_NDRUMKEY; p++) {
        iam.sweep[p] = s->sweep[p];
        en_snap(p, s->sweep[p]);
    }
    en_haze(IAM_AIR[iam.style]); /* the fourth key's part follows the song behind this one */
    en_haze_oct(iam.haze_oct);
    en_sleep(iam.style == 12);
    for (p = 0; p < IAM_NPART; p++) {
        iam.pat[p] = s->pat[p];
        iam.lvl[p] = s->lvl[p];
    }
    for (p = 0; p < 3; p++)
        iam.own[p] = s->own[p];
    iam_leave_break(); /* the snapshot's music, never the engine's closed breakdown */
    iam.hand = 0;
    iam.follow = 0; /* back returns the drums the player had, it does not link them again */
    iam_rescue(iam.mask); /* back into the intro must not publish the returned parts at zero */
    iam_apply();
    en_hop(0); /* back is the song behind this one, not the crossing still running */
    en_find(0);
    say("BACK", IAM_STYLE_NAME[iam.style]);
}
/* Dust is the offbeats. A chop is the & of each beat. Air, hush, wind, and the
 * other engines stay open, so an untouched song does not start chopping. */
static uint16_t iam_vstep_home(int part)
{
    int air;
    if (part != 3)
        return 0xFFFFu;
    air = iam.style < IAM_NSTYPE ? (int)IAM_AIR[iam.style] : 0;
    if (air == 1)
        return 0xAAAAu;
    if (air == 2)
        return 0x4444u;
    return 0xFFFFu;
}
/* The twelve named keys. -1 when this sound is the engine's own. */
static int iam_pre_match(int part, int snd)
{
    int i;
    for (i = 0; i < 12; i++)
        if ((int)IAM_PRE_PART[i] == part && (int)IAM_PRE_SND[i] == snd)
            return i;
    return -1;
}
/* The engine key's sound. A named key keeps its own, so the two are never the same voice. */
static int iam_sound_free(int part, int snd)
{
    int i;
    if (iam_pre_match(part, snd) < 0 && snd >= 0 && snd < EN_NSOUND)
        return snd;
    if (part == 0) {
        for (i = 0; i < 4; i++)
            if (iam_pre_match(0, IAM_PAD_SND[iam.style][i]) < 0)
                return IAM_PAD_SND[iam.style][i];
        for (i = 0; i < 8; i++)
            if (iam_pre_match(0, IAM_PAD_MORE[iam.style][i]) < 0)
                return IAM_PAD_MORE[iam.style][i];
    } else if (part == 1) {
        if (iam_pre_match(1, IAM_BASS_SND[iam.style]) < 0)
            return IAM_BASS_SND[iam.style];
        for (i = 0; i < IAM_NVISIT; i++)
            if (iam_pre_match(1, IAM_VISIT_BASS[iam.style][i]) < 0)
                return IAM_VISIT_BASS[iam.style][i];
    } else if (part == 2) {
        if (iam_pre_match(2, IAM_ACID_SND[iam.style]) < 0)
            return IAM_ACID_SND[iam.style];
        for (i = 0; i < IAM_NVISIT; i++)
            if (iam_pre_match(2, IAM_VISIT_ACID[iam.style][i]) < 0)
                return IAM_VISIT_ACID[iam.style][i];
    }
    for (i = 0; i < EN_NSOUND; i++)
        if (iam_pre_match(part, i) < 0)
            return i;
    return 0;
}
/* The cast's own rolls. iam_rng stays on the song's stream, so a dial's next
 * seed is the one it was before this draw. */
static uint32_t iam_cast_state;
static int iam_cast_roll(int n)
{
    uint32_t x = iam_cast_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    iam_cast_state = x ? x : 1u;
    if (n <= 1)
        return 0;
    return (int)(iam_cast_state % (uint32_t)n);
}
/* First k of dst are a random subset. The rest stay so the draw can be read back. */
static void iam_cast_pick(int *dst, int n, int k)
{
    int i;
    for (i = 0; i < n; i++)
        dst[i] = i;
    if (k > n)
        k = n;
    for (i = 0; i < k; i++) {
        int j = i + iam_cast_roll(n - i);
        int t = dst[i];
        dst[i] = dst[j];
        dst[j] = t;
    }
}
/* A named key that is on already plays this sound. The engine takes another. */
static int iam_cast_clash(int part, int snd)
{
    int i;
    for (i = 0; i < 12; i++) {
        if (!((iam.lay >> i) & 1))
            continue;
        if ((int)IAM_PRE_PART[i] == part && (int)IAM_PRE_SND[i] == snd)
            return 1;
    }
    return 0;
}
static void iam_cast_unstack(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        int snd = proj.par[IAM_SND[i]];
        if (iam_cast_clash(i, snd))
            iam_set(IAM_SND[i], iam_sound_free(i, snd));
    }
}
/* Drum and bass that is not the seed-42 piece never visits. Dress it from the
 * same pools the dial uses, so a power-on is not the same four pad sounds. */
static void iam_cast_dnb_sounds(void)
{
    int n = iam_cast_roll(12);
    int snd = n < 4 ? IAM_PAD_SND[0][n] : IAM_PAD_MORE[0][n - 4];
    int prev, col, i, in = 0;
    if (n >= 4 && iam_pad_gated(snd)) {
        for (i = 0; i < 4; i++)
            if (IAM_PAD_SND[0][i] == (uint8_t)snd)
                in = 1;
        if (!in)
            snd = IAM_PAD_SND[0][n & 3];
    }
    iam_set(P_PADTONE, snd);
    iam_set(P_DR_INST, IAM_VISIT_BASS[0][iam_cast_roll(IAM_NVISIT)]);
    iam_set(P_AC_WAVE, IAM_VISIT_ACID[0][iam_cast_roll(IAM_NVISIT)]);
    prev = proj.par[P_NZ_COLOR];
    col = IAM_VISIT_COL[0][iam_cast_roll(IAM_NVISIT)];
    iam_set(P_NZ_COLOR, col);
    if (col == 2)
        iam.lvl[3] = 28; /* wood, the knock, stays quiet */
    else if (prev == 2 && iam.lvl[3] == 28)
        iam.lvl[3] = 48;
}
static void iam_cast_side(int d, int lvl)
{
    iam.don = (uint16_t)(iam.don | (1u << d));
    iam.dpat[d] = (uint8_t)iam_cast_roll(IAM_NPAT);
    iam.dkit[d] = (uint8_t)iam_cast_roll(EN_NKIT);
    iam.dlvl[d] = (uint8_t)lvl;
    iam.pown = (uint16_t)(iam.pown | (1u << d)); /* its own pattern, not the drum beside it */
    iam.sown = (uint16_t)(iam.sown | (1u << d)); /* a later section leaves this key */
}
/* The intro can duck every key the draw lit. One voice with a real level comes back. */
static void iam_cast_ensure(void)
{
    int s, n = 0, best = -1, bg = -1;
    iam_cast_unstack();
    if (iam_audible())
        return;
    for (s = 0; s < 4; s++) {
        int g = iam_gain(s);
        int lvl = (int)iam.lvl[s] * g / 100;
        if (s == 0)
            lvl = iam_pad_lvl(lvl);
        if (lvl > bg) {
            bg = lvl;
            best = s;
        }
    }
    if (best < 0 || bg <= 0)
        return;
    for (s = 0; s < 16; s++)
        if (iam_slot_on(s))
            n++;
    if (n >= IAM_OPEN && !iam_slot_on(best)) { /* already three. Swap, do not grow */
        int youngest = -1;
        uint32_t yb = 0;
        for (s = 0; s < 16; s++) {
            if (!iam_slot_on(s) || !iam.born[s])
                continue;
            if (youngest < 0 || iam.born[s] >= yb) {
                yb = iam.born[s];
                youngest = s;
            }
        }
        if (youngest >= 0)
            iam_slot_off(youngest);
    }
    iam.mask = (uint8_t)(iam.mask | (1u << best));
    if (!iam.born[best]) {
        if (++iam.born_n == 0)
            iam.born_n = 1;
        iam.born[best] = iam.born_n;
    }
    iam_cast_unstack();
    iam_apply();
}
/* Who is on, then the kit and the pattern inside each drum that is on.
 * Sleep keeps the rooms. Still keeps the chord and adds a few quiet drums.
 * Seed 42's boot is the piece the host locks. The unit's clock is not 42. */
static void iam_cast(uint32_t seed, int picked)
{
    int s, i, age, nsyn, ndrum, drawn;
    int slot[16];
    if (!picked && seed == 42u)
        return;
    iam_cast_state = (seed ? seed : 1u) ^ 0xC457u;
    s = iam.style;
    if (!picked && s == 0)
        iam_cast_dnb_sounds();
    for (i = 0; i < 16; i++)
        iam.born[i] = 0;
    if (s == 12) {
        /* Named colours only. The room's engines stay, and they are newer,
         * so a fuller section drops a colour before it drops the room. */
        int room = 0, add;
        int named[12];
        static const int extra[6] = {2, 6, 7, 8, 9, 10}; /* clap and the five. Not the clock */
        for (i = 0; i < 4; i++)
            if ((iam.mask >> i) & 1)
                room++;
        add = 1;
        if (room < 4)
            add += iam_cast_roll(2); /* one or two. The roll stays, so the drums after it stay */
        if (add < 0)
            add = 0;
        /* Three engines already fill a slow room. A colour on top, with this
         * reverb, is the pile that broke a dance song at five keys. */
        drawn = add;
        if (room >= 3)
            drawn = 0;
        else if (room + drawn > IAM_OPEN)
            drawn = IAM_OPEN - room;
        if (drawn < 0)
            drawn = 0;
        iam_cast_pick(named, 12, add); /* the colours not lit still consume their rolls */
        age = 1;
        for (i = 0; i < drawn; i++) {
            int pre = named[i];
            iam.lay = (uint16_t)(iam.lay | (1u << pre));
            iam.born[4 + pre] = (uint32_t)age++;
        }
        for (i = 0; i < 4; i++)
            if ((iam.mask >> i) & 1)
                iam.born[i] = (uint32_t)age++;
        iam.born_n = age > 0 ? (uint32_t)(age - 1) : 0;
        ndrum = iam_cast_roll(4); /* none, or up to three beside the room */
        iam_cast_pick(slot, 6, ndrum);
        for (i = 0; i < ndrum; i++) {
            int d = extra[slot[i]];
            uint16_t st, one;
            iam_cast_side(d, 40 + iam_cast_roll(24));
            st = iam_drum_steps(d, iam.dpat[d]);
            if (!st)
                st = iam_drum_floor(d);
            one = (uint16_t)(st & (uint16_t)(-st)); /* one hit. The room stays sparse */
            iam.stepv[d] = one ? one : 0x0001u;
            iam.stepo = (uint16_t)(iam.stepo | (1u << d));
        }
    } else {
        static const int side[7] = {2, 4, 6, 7, 8, 9, 10};
        int drum[11];
        nsyn = 2 + iam_cast_roll(4); /* two to five drawn. The kits below stay on this stream */
        iam_cast_pick(slot, 16, nsyn);
        drawn = nsyn > IAM_OPEN ? IAM_OPEN : nsyn; /* two or three light. Four and five broke the mix */
        iam.mask = (uint8_t)(iam.mask & 0xF0u);
        iam.lay = 0;
        age = 1;
        for (i = 0; i < drawn; i++) {
            int sl = slot[i];
            if (sl < 4)
                iam.mask = (uint8_t)(iam.mask | (1u << sl));
            else
                iam.lay = (uint16_t)(iam.lay | (1u << (sl - 4)));
            iam.born[sl] = (uint32_t)age++;
        }
        iam.born_n = (uint32_t)(age - 1);
        iam_cast_unstack();
        /* Each main drum, on or waiting: its own cousin, its own kit. */
        for (i = 0; i < 4; i++) {
            int row = iam_cast_roll(IAM_NVISIT);
            iam.pat[4 + i] = IAM_VISIT_BEAT[s][row][i];
            iam_set(IAM_SND[4 + i], iam_cast_roll(EN_NKIT));
        }
        for (i = 0; i < 7; i++)
            iam.dkit[side[i]] = (uint8_t)iam_cast_roll(EN_NKIT);
        ndrum = s == 3 ? iam_cast_roll(5) : 3 + iam_cast_roll(5);
        iam_cast_pick(drum, 11, ndrum);
        iam.mask = (uint8_t)(iam.mask & 0x0Fu);
        iam.don = 0;
        /* Clap and the open hat stay where the draw put them. */
        iam.sown = (uint16_t)((1u << 2) | (1u << 4));
        for (i = 0; i < ndrum; i++) {
            int d = drum[i];
            if (iam_main_drum(d)) {
                int part = IAM_DRUM_PART[d];
                iam.mask = (uint8_t)(iam.mask | (1u << part));
                if (s == 3 && iam.lvl[part] > 48)
                    iam.lvl[part] = 48; /* the chord stays in front */
                else if (iam.lvl[part] < 64)
                    iam.lvl[part] = 76;
            } else {
                int lvl = s == 3 ? 36 + iam_cast_roll(20) : 70 + iam_cast_roll(20);
                iam_cast_side(d, lvl);
            }
        }
    }
    iam_write_drums();
    iam_apply();
    if (s != 12)
        iam_cast_ensure();
}
static void iam_bake(uint32_t seed, int style)
{
    /* Style 0's row is the seed-42 song: 170, dorian, a 9th, swing 8, echo 40. */
    static const int TEMPO[IAM_NSTYPE] = {170, 82, 124, 73, 118, 98, 114, 122, 90, 132, 140, 100, 56};
    static const int MODE[IAM_NSTYPE] = {MODE_DORIAN,     MODE_MIXOLYDIAN, MODE_AEOLIAN, MODE_PENT_MAJ,
                                        MODE_MIXOLYDIAN, MODE_DORIAN,     MODE_PHRYGIAN, MODE_IONIAN,
                                        MODE_AEOLIAN,    MODE_MIXOLYDIAN, MODE_PHRYGIAN, MODE_AEOLIAN,
                                        MODE_PENT_MAJ};
    static const int CHORD[IAM_NSTYPE] = {EN_CH_9TH, EN_CH_DUB, EN_CH_SUS, EN_CH_4THS, EN_CH_7TH, EN_CH_9TH,
                                         EN_CH_SUS, EN_CH_7TH, EN_CH_SUS, EN_CH_9TH, EN_CH_4THS, EN_CH_SUS,
                                         EN_CH_4THS};
    static const int VOICING[IAM_NSTYPE] = {0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0};
    static const int SWING[IAM_NSTYPE] = {8, 24, 0, 0, 16, 34, 0, 6, 30, 40, 0, 4, 0};
    static const int EVOLVE[IAM_NSTYPE] = {35, 25, 30, 18, 20, 26, 18, 16, 14, 22, 20, 14, 6};
    static const int MOTION[IAM_NSTYPE] = {40, 28, 36, 22, 24, 30, 24, 20, 16, 26, 32, 18, 8};
    static const int DRIFT[IAM_NSTYPE] = {45, 18, 20, 12, 12, 14, 10, 8, 12, 14, 18, 10, 6};
    static const int ECHO[IAM_NSTYPE] = {40, 66, 26, 48, 34, 26, 20, 30, 36, 44, 38, 64, 28};
    static const int ETIME[IAM_NSTYPE] = {2, 1, 2, 5, 2, 1, 2, 2, 3, 2, 3, 4, 5}; /* 5 is a bar. The parameter stops there */
    static const int REVERB[IAM_NSTYPE] = {48, 66, 34, 88, 42, 28, 22, 48, 52, 34, 46, 60, 92};
    static const int SIZE[IAM_NSTYPE] = {55, 74, 40, 94, 50, 34, 28, 56, 64, 40, 62, 74, 96};
    static const int DRIVE[IAM_NSTYPE] = {30, 16, 16, 6, 16, 24, 30, 12, 18, 14, 36, 8, 0};
    static const int LKICK[IAM_NSTYPE] = {40, 52, 68, 12, 50, 46, 34, 62, 28, 52, 72, 24, 0};
    static const int SND0[IAM_NSTYPE] = {1, 9, 4, 7, 1, 9, 6, 9, 1, 9, 4, 5, 9}; /* the roll's base. The vibe's own sound replaces it */
    int picked = style >= 0, i, m;
    iam.rng = seed ? seed : 1u;
    iam.style = (uint8_t)(style < 0 ? (int)(iam_rng() % IAM_NDANCE) : style % IAM_NSTYPE);
    (void)iam_rng();
    iam_set(P_TEMPO, TEMPO[iam.style] + (int)(iam_rng() % 7u) - 3);
    iam_set(P_ROOT, (int)(iam_rng() % 12u));
    m = MODE[iam.style];
    iam_set(P_MODE, m);
    proj.pool = EN_MODE_POOL[m];
    en_pool(proj.pool);
    iam_set(P_CHORD, CHORD[iam.style]);
    iam_set(P_VOICING, VOICING[iam.style]);
    iam_set(P_SWING, SWING[iam.style]);
    iam_set(P_EVOLVE, EVOLVE[iam.style]);
    iam_set(P_MOTION, MOTION[iam.style]);
    iam_set(P_DRIFT, DRIFT[iam.style]);
    iam_set(P_PE_ATK, IAM_BREATH[iam.style][0]);
    iam_set(P_PE_HOLD, IAM_BREATH[iam.style][1]);
    iam_set(P_PE_REL, IAM_BREATH[iam.style][2]);
    iam_set(P_PE_REST, IAM_BREATH[iam.style][3]);
    iam_set(P_PL_DEST, 0); /* the vowel wanders. Level would be the pad switching itself off */
    iam_set(P_PL_DEPTH, 22);
    iam_set(P_PL_RATE, 5);
    iam_set(P_PL_SHAPE, 5);
    iam_set(P_PAD_MAN, 0);
    iam_set(P_ECHO, ECHO[iam.style]);
    iam_set(P_ETIME, ETIME[iam.style]);
    iam_set(P_REVERB, REVERB[iam.style]);
    iam_set(P_SIZE, SIZE[iam.style]);
    iam_set(P_DRIVE, DRIVE[iam.style]);
    iam_set(P_LINKS, 1);
    iam_set(P_LK_KICK, LKICK[iam.style]);
    iam_set(P_LK_SNARE, 0); /* a snare opens the pad's filter. The pad is already in the song, so that flash is a spike */
    iam_set(P_DD_CHANGE, 0);
    iam_set(P_PATTERN, 0);
    iam_set(P_OCT, 0);
    for (i = 0; i < IAM_NPART; i++) {
        int snd = i == 0 ? IAM_PAD_SND[iam.style][(int)(iam_rng() % 4u)]
                         : i == 3 ? IAM_HAZE_COL[(int)(iam_rng() % 4u)]
                                  : SND0[iam.style] + (int)(iam_rng() % 4u);
        iam.pat[i] = (uint8_t)(iam_rng() % IAM_NPAT);
        iam.lvl[i] = (uint8_t)(i < 4 ? 62 + (int)(iam_rng() % 20u) : 70 + (int)(iam_rng() % 20u));
        iam_set(IAM_SND[i], snd);
    }
    iam.pat[0] = (uint8_t)CHORD[iam.style]; /* the pad's pattern is the style's chord */
    /* Seed 42's boot keeps this. Every other song draws again, after the visit. */
    iam.mask = IAM_SPINE[iam.style];
    if (iam_popcount(iam.mask) > 2 && (iam_rng() & 1u)) {
        int b = (int)(iam_rng() % IAM_NPART);
        if ((iam.mask >> b) & 1)
            iam.mask = (uint8_t)(iam.mask & ~(1u << b));
    }
    if (iam_popcount(iam.mask) < 4 && (iam_rng() & 2u)) {
        int b = (int)(iam_rng() % IAM_NPART);
        iam.mask = (uint8_t)(iam.mask | (1u << b));
    }
    /* The dial lands in the vibe. A boot, and ENV, still open on the spine. */
    if (picked && iam.style >= 4)
        iam.mask = IAM_ARRIVE[iam.style];
    else if (picked && iam.style != 3)
        iam.mask = (uint8_t)((iam.mask & 0x0Fu) | IAM_ARRIVE[iam.style]);
    if (iam.style == 3)
        iam.mask = IAM_SPINE[3];
    if (iam.style == 6 && !picked)
        iam.mask = IAM_SPINE[6]; /* acid, kick, hat. A boot starts on the line. The groove brings the band */
    iam.xown = 0;
    iam.xon = 0;
    /* A new song links the beat again, and an untouched clap and open hat follow
     * the section. A step edit, or FX on that drum, takes it off that. */
    iam.sw = 0;
    iam.pseed = 0;
    iam.don = 0;
    iam.pown = 0;
    iam.stepo = 0; /* a new song links the presets again */
    for (i = 0; i < 4; i++)
        iam.vstep[i] = iam_vstep_home(i); /* dust and chop keep their rhythm. The rest stay open */
    iam.sown = 0;
    iam.follow = 0;
    iam.mut = 0;
    iam.dly = 0;
    iam.hel = 0;
    iam.mos = 0;
    en_cloud(0, 0, 0); /* a new song is dry. The effects fade if they were open */
    en_phase(0);
    en_echo(0);
    en_verb(0);
    en_shimmer(0);
    en_ring(0); /* a new song is dry. The ring and the wah fade if they were open */
    en_wah(0);
    en_find(0); /* and it is not still searching */
    for (i = 0; i < EN_XDRUM; i++) {
        iam.xtune[i] = 0;
        iam.xdec[i] = 50; /* the length the hit already had */
    }
    for (i = 0; i < 4; i++) {
        iam.wob[i] = 0;
        en_wobble(i, 0); /* a new song is steady until the knob is turned */
    }
    for (i = 0; i < IAM_NDRUMKEY; i++) {
        iam.sweep[i] = 0;
        en_snap(i, 0);
    }
    /* Style 0's boot is the seed-42 song: those rolls stay, so its chords stay.
     * The dial, and every other vibe, dress the parts. The dial stamps the beat,
     * then draws one of the eight songs. A new song does too, except drum and
     * bass, which keeps the rolls. */
    if (picked || iam.style != 0) {
        iam_set(P_DR_INST, IAM_BASS_SND[iam.style]);
        iam_set(P_AC_WAVE, IAM_ACID_SND[iam.style]);
        iam_set(P_KIT_KICK, IAM_KIT[iam.style]);
        iam_set(P_KIT_SNARE, IAM_KIT[iam.style]);
        iam_set(P_KIT_HAT, IAM_KIT[iam.style]);
        iam_set(P_KIT_PERC, IAM_KIT[iam.style]);
        iam.pat[1] = 0; /* the bass follows the chord */
        iam.pat[2] = IAM_ACID_PAT[iam.style];
        if (iam.style != 0) {
            iam.pat[3] = IAM_HAZE_PAT[iam.style];
            iam_set(P_NZ_COLOR, IAM_HAZE_SET[iam.style]);
        }
        if (iam.style == 1 && iam.lvl[1] < 80)
            iam.lvl[1] = 80;
        if (iam.style == 1)
            iam.lvl[3] = 50; /* the chop sits with the organ */
        if (iam.style == 2 && iam.lvl[4] < 82)
            iam.lvl[4] = 82;
        if (iam.style == 2)
            iam.lvl[3] = 52; /* air, for when that key comes on */
        if (iam.style == 3) {
            iam.lvl[0] = 76;
            iam.lvl[1] = 66;
            iam.lvl[3] = 36; /* a dark tone under the chord */
            iam_set(P_BASSOCT, -1);
            iam_set(P_DL_DEPTH, 8);
            iam_set(P_DE_ATK, 48);
            iam_set(P_DE_DEC, 82);
            iam_set(P_DR_CUT, 28);
            iam_set(P_BRIGHT, 30);
        } else if (iam.style == 12) {
            /* Present, not a dance mix. The drone sits where a voice can sing over it. */
            iam.lvl[0] = 52;
            iam.lvl[1] = 84;
            iam.lvl[3] = 50;
            iam.lvl[4] = 0;
            iam.lvl[5] = 0;
            iam.lvl[6] = 58; /* chimes */
            iam.lvl[7] = 74; /* the bowl */
            iam_set(P_BASSOCT, 0);
            iam_set(P_DE_ATK, 90);
            iam_set(P_DE_AMT, 12);
            iam_set(P_DR_CUT, 68);
            iam_set(P_DR_RES, 14);
            iam_set(P_BRIGHT, 24);
            iam_set(P_AC_OCT, 2); /* the line sits up, where a flute speaks */
            iam_set(P_AC_CUT, 48);
            iam_set(P_AC_RES, 8);
            iam_set(P_AC_ENV, 12);
            iam_set(P_AC_DEC, 100);
            iam_set(P_AC_ACC, 0);
            iam_set(P_AC_SLIDE, 18);
            iam_set(P_DL_DEPTH, 4);
            iam_set(P_DL_RATE, 1);
            iam_set(P_PL_DEPTH, 10);
            iam_set(P_LK_HAT, 0);
        } else if (iam.style == 1) {
            iam_set(P_BASSOCT, -1);
            iam_set(P_DL_DEPTH, 34);
        } else if (iam.style == 2) {
            iam_set(P_BASSOCT, -1);
            iam_set(P_DL_DEPTH, 16);
            iam_set(P_AC_CUT, 52);
        } else if (iam.style >= 4) {
            const uint8_t *d = IAM_DRESS[iam.style - 4];
            if (iam.lvl[1] < d[0])
                iam.lvl[1] = d[0];
            if (iam.lvl[4] < d[1])
                iam.lvl[4] = d[1];
            if (d[2] && iam.lvl[2] < d[2])
                iam.lvl[2] = d[2];
            iam.lvl[3] = d[3];
            if (d[4] && iam.lvl[0] < d[4])
                iam.lvl[0] = d[4];
            iam_set(P_BASSOCT, -1);
            iam_set(P_DL_DEST, 0); /* cutoff, so a deep depth reads as a wobble */
            iam_set(P_DL_DEPTH, d[5]);
            iam_set(P_DL_RATE, d[10]);
            iam_set(P_DL_SHAPE, d[12]);
            iam_set(P_AC_CUT, d[6]);
            iam_set(P_AC_RES, d[11]);
            iam_set(P_BRIGHT, d[7]);
            iam_set(P_DR_CUT, d[8]);
            iam_set(P_DE_ATK, d[9]);
        }
        if (iam.style == 6) {
            /* The bleep is the line. The FM bass and the 808 sit with it. */
            iam.lvl[1] = 66;
            iam.lvl[4] = 86;
            iam.lvl[5] = 72;
            iam.lvl[6] = 60;
            iam_set(P_AC_OCT, 1);
            iam_set(P_AC_SLIDE, 70);
            iam_set(P_AC_ACC, 64);
            iam_set(P_AC_ENV, 76);
            iam_set(P_AC_DEC, 44);
        }
        if (picked)
            iam_stamp_beat(); /* the recipe's beat. The visit may pick a cousin */
    }
    if (picked || iam.style != 0)
        iam_visit_band(); /* eight songs: sounds, kits, a beat in the family, the noise. Not the mask */
    /* Wave's bass was floored at 74. It came in too strong. Twenty down sits it
     * under the strings. The acid is set with the other songs, below. */
    if (iam.style == 11) {
        int b = (int)iam.lvl[1] - 20;
        iam.lvl[1] = (uint8_t)(b < 0 ? 0 : b);
    }
    /* House's pad was arriving on the dress floor of 72, or the roll above it.
     * That chord sat a bit loud. Twelve down is the trim. */
    if (iam.style == 7) {
        int p = (int)iam.lvl[0] - 12;
        iam.lvl[0] = (uint8_t)(p < 0 ? 0 : p);
    }
    /* Hip hop's pad was arriving on the 62..81 roll. It sat over the song.
     * It comes on at 30. */
    if (iam.style == 8)
        iam.lvl[0] = 30;
    /* Wood, the knock that replaced brown, was arriving at the rolled level (63 on
     * the boot song) and at 48 on dubstep. That entrance was too loud. It comes in
     * at 28. Pink and vinyl stay. */
    if (IAM_AIR[iam.style] == 1 && proj.par[P_NZ_COLOR] == 2)
        iam.lvl[3] = 28;
    /* Acid was arriving on the 62..81 roll, and higher where a line leads.
     * It sat over the song. It comes on at 48. */
    iam.lvl[2] = 48;
    /* The rolled pattern, and a dressed song's chord trigger, hold the bass under
     * the whole chord. One bar (two on Still). A later pattern write reads this. */
    iam.pat[1] = IAM_DR_TRIG[iam.style];
    iam_write_drums();
    iam_set(P_DE_DEC, IAM_DR_DEC[iam.style]); /* Still's dress asked for several seconds */
    en_haze(IAM_AIR[iam.style]); /* dust, chop, air, hush, wind. A static: the seed-42 rolls stay where they are */
    iam.haze_oct = 0;
    en_haze_oct(0);
    en_sleep(iam.style == 12);
    iam.lay = 0; /* a new song. The named keys do not carry over */
    iam.q_lay = 0;
    iam.form = (uint8_t)(picked ? IAM_GROOVE : IAM_INTRO);
    iam.duck = 0;
    iam.swap = 0;
    iam.lock = 0;
    iam.pad_own = 0;
    iam.pad_laps = 0;
    iam.solo = 0;
    iam.held = -1;
    iam.held_key = -1;
    iam.held_drum = -1;
    iam.held_pre = -1;
    iam.step_i = 0;
    iam.step_stay = 0; /* a new song is not still on a step screen */
    iam.seq_undo = 0;
    iam_enter(iam.form);
    if (picked || iam.style != 0)
        iam_visit_open(); /* after the section's colour step, so the wider pad stays */
    iam_cast(seed, picked); /* keys, kits, and drum patterns. The song's levels stay */
    for (i = 0; i < 3; i++)
        iam.own[i] = (uint8_t)iam_sound_free(i, proj.par[IAM_SND[i]]);
    en_seed(seed);
    en_regrow();
    en_pad_lock(1); /* one phrase for the four voices. The private clock, and its random, stay */
}
static void iam_boot(uint32_t seed)
{
    memset(&iam, 0, sizeof iam);
    iam.held = -1;
    iam.held_key = -1; /* memset would leave 0, and 0 is a real key */
    iam.held_drum = -1;
    iam.held_pre = -1;
    iam.auto_on = 1; /* before the bake, so the opening line says the conductor is on */
    iam.kvol = 90;   /* the white keys, a tenth under the voice. A new song keeps a turn */
    en_keys(90);
    iam_bake(seed, -1);
    iam.hop_rng = plat_ms() | 1u; /* zero would stay zero. Boot itself is not a crossing */
    iam.hop_last = 0xFF;
    iam.find_rng = (plat_ms() ^ 0xF1ADu) | 1u; /* not the same first roll as a hop */
    iam.find_last = 0xFF;
    iam.on = 1;
    en_fx(0);
    en_dj(0);
    ui.dj_mask = 0;
    ui.dj_lock = 0;
    en_find(0);
}
static uint32_t iam_hop_roll(void)
{
    iam.hop_rng += 0x9E3779B9u;
    iam.hop_rng ^= iam.hop_rng << 13;
    iam.hop_rng ^= iam.hop_rng >> 17;
    iam.hop_rng ^= iam.hop_rng << 5;
    return iam.hop_rng;
}
static void iam_say_hop(int kind)
{
    static const char *const name[4] = {"CUE", "THROW", "STUTTER", "REVERSE"};
    char b[24];
    const char *p = name[kind & 3];
    const char *s = IAM_STYLE_NAME[iam.style < IAM_NSTYPE ? iam.style : 0];
    char *d = b;
    while (*p && d - b < 10)
        *d++ = *p++;
    *d++ = ' ';
    while (*s && d - b < 23)
        *d++ = *s++;
    *d = 0;
    say(b, 0);
}
static int iam_hop_pick(void)
{
    int k = (int)((iam_hop_roll() >> 27) & 3u); /* cue, throw, stutter, reverse */
    if (k == (int)iam.hop_last)
        k = (k + 1) & 3; /* the next one is a different crossing */
    iam.hop_last = (uint8_t)k;
    en_hop(k + 1);
    return k;
}
static void iam_cross(void)
{
    iam_say_hop(iam_hop_pick());
}
/* SAVE only. The form's own break still arrives as the arrangement. */
static void iam_break_cross(void)
{
    static const char *const name[4] = {"CUE", "THROW", "STUTTER", "REVERSE"};
    say(name[iam_hop_pick() & 3], "BREAK");
}
static void iam_change(int style)
{
    uint32_t seed = iam_rng() ^ plat_ms() ^ 0xA11Au;
    en_find(0); /* a new song is not still searching */
    iam_push();
    iam.auto_on = 1;
    iam_bake(seed, style);
    iam.on = 1;
    iam.card = 0; /* the new song, not the tip that happened to be up */
    iam_cross(); /* ENV and the dial: the song you are leaving plays across */
}
/* The bit only. The bar publishes once, after every click of that bar, so the
 * cap sees the set that will actually sound. */
static void iam_toggle(int part)
{
    iam.mask ^= (uint8_t)(1u << part);
    if ((iam.mask >> part) & 1) {
        iam.hand |= (uint8_t)(1u << part);
        /* Kick, snare, hat, perc. Sleep stores the kit at 0. A tap should be heard. */
        if (part >= 4 && iam.lvl[part] < 64)
            iam.lvl[part] = 76;
    } else
        iam.hand &= (uint8_t)~(1u << part);
    say(iam_part_name(part), (iam.mask >> part) & 1 ? "ON" : "OFF");
}
/* Pink, wood, vinyl. Knob 1 walks those three, not the eight noise colours.
 * Wood is colour 2, and only Dust plays it as a knock. Chop still reads it as brown. */
static int iam_haze_pick(int color, int step)
{
    static const int col[3] = {1, 2, 6};
    int at = 0, i;
    for (i = 0; i < 3; i++)
        if (col[i] == color)
            at = i;
    if (step >= 0)
        at = (at + step) % 3;
    else
        at = (at - ((-step) % 3) + 3) % 3;
    return col[at];
}
static uint8_t iam_turn_nowake; /* kit edits from the clap or open hat, applied on the bar */
/* Knob 3 while a key is held: the stored level, as a percent. A dark part
 * that this turn woke keeps FROM SILENT on the second line. */
static void iam_say_lvl(int part, int v, int woke)
{
    char b[24], n[8], *d = b;
    const char *p;
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    if (iam.held_drum >= 0)
        p = iam_drum_name(iam.held_drum);
    else if (iam.held_pre >= 0 && iam.held_pre < 12)
        p = IAM_PRE_NAME[iam.held_pre];
    else if (part >= 0 && part < IAM_NPART)
        p = iam_part_name(part);
    else
        p = "LEVEL";
    while (*p && d - b < 12)
        *d++ = *p++;
    *d++ = ' ';
    itoa_u((uint32_t)v, n);
    p = n;
    while (*p && d - b < 20)
        *d++ = *p++;
    if (d - b < 22)
        *d++ = '%';
    *d = 0;
    say(b, woke ? "FROM SILENT" : 0);
}
static void iam_turn_part(int part, int knob, int step)
{
    int dark = !(iam.mask >> part & 1);
    int quiet = iam.held_drum == 2 || iam.held_drum == 4 || ((iam_turn_nowake >> part) & 1);
    int woke = 0;
    /* The clap and the open hat share a parent's kit. Their knob must not switch that parent on. */
    if (quiet && knob == 2) {
        iam.turned = 1;
        return;
    }
    /* Volume down on a dark key leaves it dark, and the twist is not also a click. */
    if (dark && knob == 2 && step <= 0) {
        iam.turned = 1;
        return;
    }
    if (!iam.turned)
        iam_push();
    iam.turned = 1;
    if (dark && knob == 2) {
        iam.lvl[part] = 0; /* the turn itself brings it in, rather than a jump to the stored level */
        iam.mask |= (uint8_t)(1u << part);
        iam.hand |= (uint8_t)(1u << part);
        woke = 1;
    } else if (dark && knob != 1 && !quiet) {
        iam.mask |= (uint8_t)(1u << part); /* sound or cutoff wakes a sleeping part at its level */
        iam.hand |= (uint8_t)(1u << part);
    }
    if (part == 0 && knob != 2)
        iam.pad_own = 8; /* sound, pattern, cutoff: the conductor leaves them for eight bars */
    if (knob == 0 && part == 3) {
        iam_set(P_NZ_COLOR, iam_haze_pick(proj.par[P_NZ_COLOR], step));
    } else if (knob == 0) {
        iam_set(IAM_SND[part], proj.par[IAM_SND[part]] + step);
    } else if (knob == 1) {
        iam.pat[part] = (uint8_t)((iam.pat[part] + step + IAM_NPAT * 4) % IAM_NPAT);
        iam_write_drums();
    } else if (knob == 2) {
        int v = (int)iam.lvl[part] + step * 4;
        iam.lvl[part] = (uint8_t)(v < 0 ? 0 : v > 100 ? 100 : v);
        iam_say_lvl(part, iam.lvl[part], woke);
    } else if (knob == 3) {
        int d = part >= 4 ? step * 3 : step * 4; /* drums: semitones. synths: the filter, 0..100 */
        iam_set(IAM_CUT[part], proj.par[IAM_CUT[part]] + d); /* this drum only */
    }
    iam_apply();
}
/* Knob 3 on one black key. Kick, snare, hat and perc keep the part's level.
 * Clap, the open hat, and the five above change only their own, and a turn up
 * wakes only that drum. */
static void iam_drum_level(int drum, int step)
{
    int on, v, woke = 0;
    if (drum < 0 || drum >= IAM_NDRUMKEY)
        return;
    if (iam_main_drum(drum)) {
        iam_turn_part(iam.held, 2, step);
        return;
    }
    on = (iam.don >> drum) & 1;
    if (!on && step <= 0) {
        iam.turned = 1; /* down on a dark drum is not a click, and it stays dark */
        return;
    }
    if (!iam.turned)
        iam_push();
    iam.turned = 1;
    if (!on) {
        iam.dlvl[drum] = 0; /* the turn brings it in, rather than a jump to the stored level */
        iam.don = (uint16_t)(iam.don | (1u << drum));
        iam.sown = (uint16_t)(iam.sown | (1u << drum)); /* this wake is the player's, not the section's */
        woke = 1;
    }
    v = (int)iam.dlvl[drum] + step * 4;
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    iam.dlvl[drum] = (uint8_t)v;
    iam_say_lvl(iam.held, v, woke);
    iam_apply();
}
static void iam_q_clear(void)
{
    int i;
    iam.q_xor = 0;
    iam.q_find = 0;
    iam.q_scene = IAM_Q_NONE;
    iam.q_drums = 0;
    iam.q_pump = 0;
    iam.q_xtra = 0;
    iam.q_nowake = 0;
    iam.q_oct = 0;
    iam.q_pst = 0;
    iam.q_kit_set = 0;
    iam.q_don = 0;
    iam.q_lay = 0;
    for (i = 0; i < IAM_NPART; i++) {
        iam.q_snd[i] = 0;
        iam.q_pat[i] = 0;
    }
    for (i = 0; i < IAM_NDRUMKEY; i++) {
        iam.q_dpat[i] = 0;
        iam.q_dkit[i] = 0;
    }
}
static int8_t iam_sat(int v)
{
    if (v > 24)
        return 24;
    if (v < -24)
        return -24;
    return (int8_t)v;
}
/* The footer shows the headline only, so the wait has to be the headline. */
static void iam_due(const char *what)
{
    char b[16], *d = b;
    const char *p = "BAR: ";
    if (!en_playing)
        return; /* it lands in this frame, and says what it did */
    while (*p && d - b < 14)
        *d++ = *p++;
    while (what && *what && d - b < 14)
        *d++ = *what++;
    *d = 0;
    say(b, 0);
    iam.dirty = 1;
}
static int iam_q_pending(void)
{
    int i;
    if (iam.q_scene || iam.q_xor || iam.q_find || iam.q_oct || iam.q_pst || iam.q_kit_set || iam.q_xtra ||
        iam.q_pump || iam.q_don || iam.q_lay)
        return 1;
    for (i = 0; i < IAM_NPART; i++)
        if (iam.q_snd[i] || iam.q_pat[i])
            return 1;
    for (i = 0; i < IAM_NDRUMKEY; i++)
        if (iam.q_dpat[i] || iam.q_dkit[i])
            return 1;
    return 0;
}
/* Apply what was asked for during this bar. A scene owns the bar: the caller
 * does not also advance the form. Undo is taken now, of the state this replaces. */
/* Two drum lanes that are sitting out: kick, then snare, then hat, then perc.
 * EDIT's excite, and the form's, do not call this. OCT+ does. */
static void iam_bring_drums(void)
{
    static const int order[4] = {4, 5, 6, 7};
    int n = 0, i;
    for (i = 0; i < 4 && n < 2; i++) {
        int part = order[i];
        if ((iam.mask >> part) & 1)
            continue;
        iam.mask = (uint8_t)(iam.mask | (1u << part));
        iam.hand = (uint8_t)(iam.hand | (1u << part));
        n++;
    }
    if (n) {
        iam.follow = 1; /* the snare or the hat just joined. An untouched partner joins too */
        iam_apply();
    }
}
/* Sixteen hits in a drum pattern. The pump walks toward a busier one. */
static uint16_t iam_lane_bits(int part, int pat)
{
    pat &= 7;
    if (part == 4)
        return IAM_KICK[pat];
    if (part == 5)
        return IAM_SNARE[pat];
    if (part == 6)
        return IAM_HAT[pat];
    return IAM_PERC[pat];
}
/* Kick, snare, hat, perc, then bass, acid, the fourth part, and the pad.
 * Clap, the open hat, and the five above them stay on their own keys.
 * One part. Nothing, when the song is full. */
static const char *iam_pump_track(void)
{
    static const int drums[4] = {4, 5, 6, 7};
    static const int rest[4] = {1, 2, 3, 0};
    int i;
    for (i = 0; i < 4; i++) {
        int part = drums[i];
        if ((iam.mask >> part) & 1)
            continue;
        iam.mask = (uint8_t)(iam.mask | (1u << part));
        iam.hand = (uint8_t)(iam.hand | (1u << part));
        return iam_part_name(part);
    }
    for (i = 0; i < 4; i++) {
        int part = rest[i];
        if ((iam.mask >> part) & 1)
            continue;
        iam.mask = (uint8_t)(iam.mask | (1u << part));
        iam.hand = (uint8_t)(iam.hand | (1u << part));
        return iam_part_name(part);
    }
    return 0;
}
/* Hat, snare, perc, kick. The nearest busier pattern, or the next one when
 * this groove is already the fullest. Not the acid: that would regrow the line. */
static void iam_pump_steps(void)
{
    static const int lane[4] = {6, 5, 7, 4};
    int part = lane[iam.pump_at & 3];
    int cur = iam.pat[part] & 7;
    int bits = (int)iam_popcount(iam_lane_bits(part, cur));
    int best = (cur + 1) & 7;
    int bestd = -99;
    int i;
    iam.pump_at = (uint8_t)((iam.pump_at + 1) & 3);
    for (i = 1; i < IAM_NPAT; i++) {
        int p = (cur + i) & 7;
        int d = (int)iam_popcount(iam_lane_bits(part, p)) - bits;
        if (d > bestd) {
            bestd = d;
            best = p;
            if (d > 0)
                break;
        }
    }
    iam.pat[part] = (uint8_t)best;
    iam_write_drum_pats();
}
/* One push covers every pump that landed on this bar. Levels stay as stored.
 * Tempo stops at 186, short of the engine's ceiling. */
static void iam_pump(int n)
{
    const char *who = 0;
    int i, added = 0, t;
    if (iam.style == 12) {
        say("SLEEP", "QUIET"); /* no extra parts, no tempo, no brighter filters */
        return;
    }
    for (i = 0; i < n; i++) {
        const char *w = iam_pump_track();
        if (w) {
            who = w;
            added++;
        }
        iam_pump_steps();
        t = proj.par[P_TEMPO] + 3;
        if (t > 186)
            t = 186;
        iam_set(P_TEMPO, t);
        iam_set(P_EVOLVE, proj.par[P_EVOLVE] + 8);
        iam_set(P_AC_MUT, proj.par[P_AC_MUT] + 10);
        iam_set(P_BRIGHT, proj.par[P_BRIGHT] + 6);
        iam_set(P_DR_CUT, proj.par[P_DR_CUT] + 6);
        if (iam.held != 2) /* the acid key owns that filter while it is held */
            iam_set(P_AC_CUT, proj.par[P_AC_CUT] + 8);
    }
    iam.follow = 1;
    iam_apply();
    if (added > 1)
        say("PUMP", "MORE");
    else if (who)
        say(who, "IN");
    else
        say("PUMP", "FASTER");
}
/* Chord, one part, a busier beat, or a new air. Sleep names the next room instead.
 * The roll is not the song's rng, and not the engine's, so a press does not move
 * the next bake or the peak. */
static int iam_find_roll(void)
{
    int k;
    iam.find_rng += 0x9E3779B9u;
    iam.find_rng ^= iam.find_rng << 13;
    iam.find_rng ^= iam.find_rng >> 17;
    iam.find_rng ^= iam.find_rng << 5;
    k = (int)((iam.find_rng >> 27) & 3u);
    if (k == (int)iam.find_last)
        k = (k + 1) & 3; /* the next press is a different place */
    iam.find_last = (uint8_t)k;
    return k;
}
static const char *iam_find_place(int kind)
{
    static const char *const name[4] = {"CHORD", "ENTER", "BEAT", "AIR"};
    static const char *const room[IAM_NSEC] = {"DRONE", "GROOVE", "CHORD", "BELLS", "WIND", "BAND"};
    if (iam.style == 12)
        return room[iam.find_room < IAM_NSEC ? iam.find_room : 0];
    return name[kind & 3];
}
static void iam_find_cancel(void)
{
    iam.q_find = 0;
    en_find(0); /* already letting go: the audio thread keeps the fade it started */
}
/* A drum lane takes the fullest other pattern, so the beat is obviously busier. */
static void iam_thicken(int part)
{
    int cur, bits, best, bestd, i;
    if (part < 4 || part > 7 || iam.style == 12)
        return;
    cur = iam.pat[part] & 7;
    bits = (int)iam_popcount(iam_lane_bits(part, cur));
    best = (cur + 3) & 7;
    bestd = -99;
    for (i = 1; i < IAM_NPAT; i++) {
        int p = (cur + i) & 7;
        int d = (int)iam_popcount(iam_lane_bits(part, p)) - bits;
        if (d > bestd) {
            bestd = d;
            best = p;
        }
    }
    iam.pat[part] = (uint8_t)best;
    iam_write_drum_pats();
}
static void iam_find_steps(void)
{
    static const int lane[4] = {6, 5, 7, 4}; /* hat, snare, perc, kick */
    int part = lane[iam.pump_at & 3];
    iam.pump_at = (uint8_t)((iam.pump_at + 1) & 3);
    iam_thicken(part);
}
static void iam_find_chord(void)
{
    if (iam.held == 0 || iam.pad_own) { /* the pad is taken: the beat still moves */
        iam_find_steps();
        iam_find_steps();
        say("BEAT", 0);
        return;
    }
    if (!((iam.mask >> 0) & 1)) {
        iam.mask = (uint8_t)(iam.mask | 1u);
        iam.hand = (uint8_t)(iam.hand | 1u);
        if (iam.lvl[0] < 64)
            iam.lvl[0] = 64;
    }
    iam_pad_colour(1, 0);
    if (iam.held != 0 && !en_frozen)
        en_next_now(); /* this bar, not the one after the landing */
    iam_set(P_BRIGHT, proj.par[P_BRIGHT] + 16);
    iam_set(P_DETUNE, proj.par[P_DETUNE] + 10);
    iam_apply();
    say("CHORD", 0);
}
static void iam_find_enter(void)
{
    static const int order[8] = {4, 5, 6, 7, 1, 2, 3, 0};
    int i, part = -1;
    for (i = 0; i < 8; i++) {
        if ((iam.mask >> order[i]) & 1)
            continue;
        part = order[i];
        break;
    }
    if (part < 0) { /* the song is full: both lines move, and the filters open */
        iam_set(P_DR_INST, (proj.par[P_DR_INST] + 8) % EN_NSOUND);
        iam_set(P_AC_WAVE, (proj.par[P_AC_WAVE] + 5) % EN_NSOUND);
        iam_set(P_DR_CUT, proj.par[P_DR_CUT] + 22);
        if (iam.held != 2)
            iam_set(P_AC_CUT, proj.par[P_AC_CUT] + 18);
        iam_set(P_AC_RES, proj.par[P_AC_RES] + 10);
        iam_set(P_AC_MUT, proj.par[P_AC_MUT] + 18);
        say("LINE", "NEW");
        return;
    }
    iam.mask = (uint8_t)(iam.mask | (1u << part));
    iam.hand = (uint8_t)(iam.hand | (1u << part));
    if (part != 2 && iam.lvl[part] < 64)
        iam.lvl[part] = 64; /* a shy arrival does not read as a new part. Acid stays at 48 */
    if (part >= 4)
        iam_thicken(part);
    iam.follow = 1; /* an untouched clap follows the snare, an untouched open hat the hat */
    iam_apply();
    say(iam_part_name(part), "IN");
}
static void iam_find_air(void)
{
    static const char *const name[3] = {"PINK", "BROWN", "VINYL"};
    static const int col[3] = {1, 2, 6};
    int c = iam_haze_pick(proj.par[P_NZ_COLOR], 1), i, which = 0;
    const char *who;
    iam_set(P_NZ_COLOR, c);
    for (i = 0; i < 3; i++)
        if (col[i] == c)
            which = i;
    if (!((iam.mask >> 3) & 1)) {
        iam.mask = (uint8_t)(iam.mask | (1u << 3));
        iam.hand = (uint8_t)(iam.hand | (1u << 3));
    }
    if (iam.lvl[3] < 52)
        iam.lvl[3] = 52; /* asked for. The baked arrival can stay quieter */
    iam.pat[3] = (uint8_t)((iam.pat[3] + 3) & 7);
    iam_write_drums();
    iam.follow = 1;
    iam_apply();
    iam_set(P_NZ_TONE, proj.par[P_NZ_TONE] + 24);
    iam_set(P_NZ_PUMP, proj.par[P_NZ_PUMP] + 18);
    who = name[which];
    if (which == 1 && iam.style < IAM_NSTYPE && IAM_AIR[iam.style] == 1)
        who = "WOOD"; /* Dust plays colour 2 as a knock. Chop keeps the lower brown */
    say(who, "AIR");
}
static void iam_find_sleep(void)
{
    int dest = iam.find_room < IAM_NSEC ? (int)iam.find_room : IAM_GROOVE;
    uint8_t was = iam.mask;
    iam.sec = (uint8_t)dest;
    iam.bar_n = 0;
    iam.hand = 0;
    iam_sleep_song(dest); /* the room's mask. The break also rings the chime and the bell */
    if (dest == IAM_DROP)
        en_sleep_cue(0); /* wind alone */
    else if (dest != IAM_BREAK)
        en_sleep_cue(1 + (iam.find_kind & 1)); /* a chime, or a bell */
    iam_rescue(was); /* a room of silent parts keeps one voice from the room just left */
    iam.follow = 1;
    iam_apply();
    say(iam_find_place(0), 0);
}
static void iam_find_land(int kind)
{
    en_find(0); /* the loop lets go as the new place comes in */
    iam_push();
    iam.lock = 8;
    if (iam.style == 12) {
        iam_find_sleep();
        return;
    }
    if (kind == 0)
        iam_find_chord();
    else if (kind == 1)
        iam_find_enter();
    else if (kind == 2) {
        iam_find_steps();
        iam_find_steps();
        say("BEAT", 0);
    } else
        iam_find_air();
}
static int iam_fire(void)
{
    int part, scene, pst, oct, find, drums, pumps, snd_any = 0, pat_any = 0, edit = 0, kit_edit = 0, lane;
    int turned_was, d;
    uint8_t xor, kit_set, nowake;
    uint16_t qdon, qlay;
    uint8_t kit[4];
    int8_t snd[IAM_NPART], pat[IAM_NPART], qdpat[IAM_NDRUMKEY], qdkit[IAM_NDRUMKEY];
    const char *who = 0, *kit_who = 0;
    xor = iam.q_xor;
    scene = iam.q_scene;
    drums = iam.q_drums;
    pumps = iam.q_pump;
    qdon = iam.q_don;
    qlay = iam.q_lay;
    nowake = iam.q_nowake;
    pst = iam.q_pst;
    oct = iam.q_oct;
    find = iam.q_find;
    kit_set = iam.q_kit_set;
    for (lane = 0; lane < 4; lane++)
        kit[lane] = iam.q_kit[lane];
    for (part = 0; part < IAM_NPART; part++) {
        snd[part] = iam.q_snd[part];
        pat[part] = iam.q_pat[part];
        if (snd[part] || pat[part])
            edit = 1;
    }
    for (d = 0; d < IAM_NDRUMKEY; d++) {
        qdpat[d] = iam.q_dpat[d];
        qdkit[d] = iam.q_dkit[d];
        if (qdpat[d] || qdkit[d])
            edit = 1;
    }
    iam_q_clear();
    if (scene == IAM_Q_VIBE && pst == 0)
        scene = IAM_Q_NONE; /* forward and back in the same bar: the dial stays */
    if (scene != IAM_Q_NONE && scene != IAM_Q_DROP)
        iam.dip = 0; /* back, a break, a new song: the drop does not also return on its own */
    if (scene == IAM_Q_BACK)
        iam_pop(); /* the move that was already committed, before this bar's edits */
    /* One undo for the kit and for a sound or pattern, taken before the mask moves,
     * so back returns both. A plain on or off does not take one. A held key's twist
     * flag is put back, so letting go can still be a click. */
    turned_was = iam.turned;
    if (kit_set || edit) {
        iam_push();
        iam.turned = 1;
    }
    for (lane = 0; lane < 4; lane++) {
        if (!((kit_set >> lane) & 1))
            continue;
        iam_set(IAM_SND[4 + lane], kit[lane]);
        kit_who = EN_KIT_NAME[(unsigned)kit[lane] < EN_NKIT ? kit[lane] : 0];
    }
    if (qdon) {
        int said = 0;
        for (d = 0; d < IAM_NDRUMKEY; d++) {
            int on;
            if (iam_main_drum(d) || !((qdon >> d) & 1))
                continue;
            iam.don = (uint16_t)(iam.don ^ (1u << d));
            iam.sown = (uint16_t)(iam.sown | (1u << d)); /* this key, not the drum beside it */
            on = (iam.don >> d) & 1;
            /* Under 64 is the quiet opening, or the old first tap. A level
             * already set above that stays. Off is how the drum goes silent. */
            if (on && iam.dlvl[d] < 64)
                iam.dlvl[d] = 76;
            if (!said) {
                say(iam_drum_name(d), on ? "ON" : "OFF");
                said = 1;
            }
        }
        iam_apply();
    }
    if (qlay) {
        int i, said = 0;
        iam.lay = (uint16_t)(iam.lay ^ qlay); /* the same as a drum: on or off, and it does not take an undo */
        for (i = 0; i < 12 && !said; i++) {
            if (!((qlay >> i) & 1))
                continue;
            say(IAM_PRE_NAME[i], ((iam.lay >> i) & 1) ? "ON" : "OFF");
            said = 1;
        }
    }
    for (d = 0; d < IAM_NDRUMKEY; d++) {
        if (qdkit[d]) {
            int cur = iam_main_drum(d) ? proj.par[IAM_SND[IAM_DRUM_PART[d]]] : iam.dkit[d];
            int nk = (cur + qdkit[d] + EN_NKIT * 8) % EN_NKIT;
            if (iam_main_drum(d))
                iam_set(IAM_SND[IAM_DRUM_PART[d]], nk);
            else
                iam.dkit[d] = (uint8_t)nk;
            kit_who = EN_KIT_NAME[nk];
            kit_edit = 1;
        }
        if (qdpat[d]) {
            iam.stepo = (uint16_t)(iam.stepo & ~(1u << d)); /* a preset replaces the writing */
            iam.pown = (uint16_t)(iam.pown | (1u << d)); /* before the write, so the link leaves this drum */
            if (iam_main_drum(d)) {
                int p = IAM_DRUM_PART[d];
                iam.pat[p] = (uint8_t)((iam.pat[p] + qdpat[d] + IAM_NPAT * 4) % IAM_NPAT);
            } else {
                iam.dpat[d] = (uint8_t)((iam.dpat[d] + qdpat[d] + IAM_NPAT * 4) % IAM_NPAT);
            }
            pat_any = 1;
            who = iam_drum_name(d);
        }
    }
    if (kit_edit || pat_any)
        iam_write_drum_pats();
    for (part = 0; part < IAM_NPART; part++)
        if ((xor >> part) & 1)
            iam_toggle(part);
    if (qlay || xor)
        iam_apply(); /* one publish: a new key and the oldest it replaced land together */
    iam_turn_nowake = nowake;
    for (part = 0; part < IAM_NPART; part++) {
        if (!snd[part] && !pat[part])
            continue;
        if (snd[part]) {
            iam_turn_part(part, 0, snd[part]);
            snd_any = 1;
            if (part >= 4)
                kit_edit = 1; /* a drum's knob 1 is the kit, including the clap's and the open hat's */
            who = iam_part_name(part);
        }
        if (pat[part]) {
            iam_turn_part(part, 1, pat[part]);
            pat_any = 1;
            who = iam_part_name(part);
        }
    }
    iam.turned = (uint8_t)turned_was;
    iam_turn_nowake = 0;
    if (oct) {
        iam_set(P_OCT, proj.par[P_OCT] + oct);
        say("OCTAVE", 0);
    }
    if (find && !scene)
        iam_find_land(iam.find_kind); /* one undo, and the effect is already letting go */
    else if (find)
        en_find(0); /* a scene owns this bar. The search does not land on top of it */
    if (!find && !scene && (snd_any || pat_any))
        say(who, snd_any ? (kit_edit ? "KIT" : "SOUND") : "PATTERN");
    else if (!find && !scene && kit_who && !xor)
        say(kit_who, "KIT");
    if (scene == IAM_Q_BREAK) {
        iam_push();
        iam.lock = 8;
        iam_enter(IAM_BREAK);
        iam_break_cross(); /* the swap is already underneath. The last bar crosses */
    } else if (scene == IAM_Q_EXCITE) {
        iam_push();
        iam.lock = 8;
        iam_enter(IAM_EXCITE);
        if (drums && iam.style != 12)
            iam_bring_drums(); /* after the snapshot, so back returns the drums too. Sleep stays quiet */
    } else if (scene == IAM_Q_DROP) {
        iam_push();
        iam.dip_sec = iam.sec;
        iam.dip_bar = iam.bar_n;
        iam.dip_mask = iam.mask;
        iam.dip_duck = iam.duck;
        iam.dip_hand = iam.hand;
        iam.dip_pump = iam.pump_at;
        iam.dip_cut = proj.par[P_AC_CUT];
        iam.dip_res = proj.par[P_AC_RES];
        iam.dip = 1; /* this bar only. The next bar puts those parts back */
        iam_enter(IAM_DROP);
    } else if (scene == IAM_Q_CHANGE) {
        iam_change(-1);
    } else if (scene == IAM_Q_VIBE) {
        int s = ((int)iam.style + pst) % IAM_NSTYPE;
        if (s < 0)
            s += IAM_NSTYPE;
        iam_change(s);
    }
    if (pumps) {
        iam_push(); /* back undoes the pump and leaves the excite, or whatever was under it */
        iam.lock = 8;
        iam_pump(pumps);
    }
    iam.dirty = 1;
    return scene != IAM_Q_NONE || pumps > 0 || (find && !scene);
}
/* A pressed drop borrowed the section for one bar. Put the song back.
 * The undo stays, so back still returns the music from before the drop. */
static void iam_dip_restore(void)
{
    iam.dip = 0;
    iam.sec = iam.dip_sec;
    iam.bar_n = iam.dip_bar;
    iam.mask = iam.dip_mask;
    iam.duck = iam.dip_duck;
    iam.hand = iam.dip_hand;
    iam.pump_at = iam.dip_pump;
    iam_set(P_AC_CUT, iam.dip_cut);
    iam_set(P_AC_RES, iam.dip_res);
    iam.follow = 0;
    iam_apply();
}
static void iam_bars(void)
{
    uint32_t bar;
    int had_lock, scene = 0, restored = 0;
    if (!en_playing) {
        if (iam_q_pending())
            iam_fire(); /* a stopped transport has no bar to wait for */
        return;
    }
    bar = en_bars / 16u;
    if (bar == iam.bar_seen)
        return;
    iam.bar_seen = bar;
    /* count a hold down before a landing sets a fresh one, so the new eight are whole */
    if (iam.pad_own)
        iam.pad_own--;
    had_lock = iam.lock != 0;
    if (iam.lock)
        iam.lock--;
    /* Before a queued scene, so back or a new song owns the bar and the drop does not
     * also put the old one back on top of it. A part click still lands after the return. */
    if (iam.dip && iam.q_scene == IAM_Q_NONE) {
        iam_dip_restore();
        restored = 1;
    }
    if (iam_q_pending())
        scene = iam_fire();
    if (scene || had_lock)
        return; /* the player's bar, or a lock that was already running */
    if (restored)
        return; /* the parts came back on this bar. The form counts from the next one */
    if (!iam.auto_on)
        return;
    if (++iam.bar_n < IAM_FORM_LEN[iam.form]) {
        if (iam.bar_n % 4 == 0 && iam.sec != IAM_BREAK) {
            iam_pad_degree(); /* a new note. The break holds the one it arrived on */
            if (iam.sec != IAM_INTRO)
                iam_pad_colour(1, 0); /* a new sound and chord. The intro holds the one it opened on */
        }
        return;
    }
    iam.form = (uint8_t)((iam.form + 1) % IAM_NSEC);
    iam_enter(IAM_FORM_SEC[iam.form]);
}
static void iam_tip(void)
{
    int tip;
    if (!(iam.card & 1u)) {
        iam.card = 1; /* the first tip */
        return;
    }
    tip = (int)(iam.card / 2u) + 1;
    iam.card = (uint8_t)(tip >= IAM_NTIP ? 0 : tip * 2 + 1); /* the next tip, then the song */
}
/* A drum, or one of the four engines. A sound key edits the engine it plays. */
static int iam_step_screen(void)
{
    if (iam.held_drum >= 0)
        return 1;
    return iam.held >= 0 && iam.held < 4;
}
static const char *iam_step_who(void)
{
    if (iam.held_drum >= 0)
        return iam_drum_name(iam.held_drum);
    if (iam.held_pre >= 0 && iam.held_pre < 12)
        return IAM_PRE_NAME[iam.held_pre];
    return iam_part_name(iam.held);
}
static void iam_vstep_set(int part, uint16_t m)
{
    if (part < 0 || part > 3)
        return;
    iam.vstep[part] = m;
    en_vstep(part, m);
    iam.dirty = 1;
}
/* The steps on the strip become this drum's pattern, heard now. */
static void iam_step_grab(int d)
{
    int aim;
    if ((iam.stepo & (1u << d)) && !iam.q_dpat[d])
        return; /* already the bits on the strip */
    aim = iam_drum_pat_aim(d);
    iam.stepv[d] = iam_drum_steps(d, aim);
    /* The first edit keeps the floor an on, blank row is already playing. */
    if (!iam.stepv[d] && !(iam.stepo & (1u << d)) && iam_drum_on(d))
        iam.stepv[d] = iam_drum_floor(d);
    iam.q_dpat[d] = 0;
    if (iam_main_drum(d))
        iam.pat[IAM_DRUM_PART[d]] = (uint8_t)aim;
    else
        iam.dpat[d] = (uint8_t)aim;
    iam.stepo = (uint16_t)(iam.stepo | (1u << d));
    iam.pown = (uint16_t)(iam.pown | (1u << d)); /* the link stops copying a preset over this */
}
static void iam_step_toggle(void)
{
    int d = iam.held_drum;
    uint16_t bit;
    if (d < 0 || d >= IAM_NDRUMKEY)
        return;
    if (!iam.step_undo)
        iam_push(); /* one undo for the steps. Moving the mark is not that undo */
    iam.step_undo = 1;
    iam.turned = 1; /* letting go is not also a click */
    iam_step_grab(d);
    bit = (uint16_t)(1u << (iam.step_i & 15));
    iam.stepv[d] = (uint16_t)(iam.stepv[d] ^ bit);
    iam_write_drum_pats();
    say(iam_drum_name(d), (iam.stepv[d] & bit) ? "STEP" : "REST");
}
static void iam_step_next(void)
{
    if (iam.held_drum < 0)
        return;
    iam.turned = 1; /* moving along the row is not a click */
    iam.step_i = (uint8_t)((iam.step_i + 1) & 15);
    iam.dirty = 1;
}
/* The latched screen. OCT+ plants the marked step, OCT- clears it. A step
 * that is already that way is left, and it is not another undo. */
static void iam_step_set(int on)
{
    int d = iam.held_drum;
    uint16_t bit, cur;
    iam.turned = 1;
    bit = (uint16_t)(1u << (iam.step_i & 15));
    if (d >= 0 && d < IAM_NDRUMKEY) {
        cur = iam_steps_live(d, iam_drum_pat_aim(d));
        if (on == ((cur & bit) != 0)) {
            say(iam_drum_name(d), on ? "STEP" : "REST");
            return;
        }
        if (!iam.step_undo)
            iam_push(); /* one undo for the steps. Moving the mark is not that undo */
        iam.step_undo = 1;
        iam_step_grab(d);
        if (on)
            iam.stepv[d] = (uint16_t)(iam.stepv[d] | bit);
        else
            iam.stepv[d] = (uint16_t)(iam.stepv[d] & (uint16_t)~bit);
        iam_write_drum_pats();
        say(iam_drum_name(d), on ? "STEP" : "REST");
        return;
    }
    if (iam.held < 0 || iam.held >= 4)
        return;
    cur = iam.vstep[iam.held];
    if (on == ((cur & bit) != 0)) {
        say(iam_step_who(), on ? "STEP" : "REST");
        return;
    }
    if (!iam.step_undo)
        iam_push();
    iam.step_undo = 1;
    if (on)
        cur = (uint16_t)(cur | bit);
    else
        cur = (uint16_t)(cur & (uint16_t)~bit);
    iam_vstep_set(iam.held, cur);
    say(iam_step_who(), on ? "STEP" : "REST");
}
/* SELECT on the latched screen. The tempo stays. */
static void iam_step_move(int step)
{
    char b[12], n[8], *p;
    const char *s;
    int i;
    if (!iam_step_screen() || !step)
        return;
    iam.turned = 1;
    i = ((int)iam.step_i + step) % 16;
    if (i < 0)
        i += 16;
    iam.step_i = (uint8_t)i;
    iam.dirty = 1;
    p = b;
    s = "STEP ";
    while (*s && p - b < 6)
        *p++ = *s++;
    itoa_u((uint32_t)(iam.step_i + 1), n);
    s = n;
    while (*s && p - b < 10)
        *p++ = *s++;
    *p = 0;
    say(iam_step_who(), b);
}
/* The finger has left. The part, its name, and its mark stay, so OCT and the knobs keep editing. */
static void iam_step_keep(void)
{
    iam.held_key = -1;
    iam.solo = 0;
}
static void iam_step_drop(void)
{
    iam.held = -1;
    iam.held_key = -1;
    iam.held_drum = -1;
    iam.held_pre = -1;
    iam.step_i = 0;
    iam.step_stay = 0;
    iam.solo = 0;
}
/* One row per track that is on. Engines first, then the drums. A named key
 * shares its engine's row, so Glass is the pad's sixteen. */
struct iam_seq_tr {
    int8_t drum;
    int8_t part;
};
static int iam_engine_live(int part)
{
    int i;
    if ((iam.mask >> part) & 1)
        return 1;
    for (i = 0; i < 12; i++)
        if (((iam.lay >> i) & 1) && (int)IAM_PRE_PART[i] == part)
            return 1;
    return 0;
}
static int iam_seq_list(struct iam_seq_tr *t, int cap)
{
    int n = 0, p, d;
    for (p = 0; p < 4 && n < cap; p++) {
        if (!iam_engine_live(p))
            continue;
        t[n].drum = -1;
        t[n].part = (int8_t)p;
        n++;
    }
    for (d = 0; d < IAM_NDRUMKEY && n < cap; d++) {
        if (!iam_drum_on(d))
            continue;
        t[n].drum = (int8_t)d;
        t[n].part = -1;
        n++;
    }
    return n;
}
static const char *iam_seq_name(const struct iam_seq_tr *t)
{
    int i;
    if (t->drum >= 0)
        return iam_drum_row(t->drum);
    if ((iam.mask >> t->part) & 1)
        return iam_part_name(t->part);
    for (i = 0; i < 12; i++)
        if (((iam.lay >> i) & 1) && (int)IAM_PRE_PART[i] == t->part)
            return IAM_PRE_NAME[i];
    return iam_part_name(t->part);
}
static int iam_seq_clamp(struct iam_seq_tr *t)
{
    int n = iam_seq_list(t, 15);
    iam.seq_n = (uint8_t)n;
    if (n <= 0)
        iam.seq_at = 0;
    else if (iam.seq_at >= n)
        iam.seq_at = (uint8_t)(n - 1);
    return n;
}
/* The row SEL is holding aside. An engine keeps that part and drops every drum.
 * Kick, snare, hat and perc keep that part and that drum, and drop the sibling.
 * Clap, the open hat, and the drums above perc keep only that drum. */
static void iam_isol_aim(int *part, int *drum)
{
    struct iam_seq_tr t[15];
    int n = iam_seq_clamp(t);
    int d;
    *part = -1;
    *drum = -1;
    if (!iam.seq_isol || !iam.seq_on || n <= 0)
        return;
    d = t[iam.seq_at].drum;
    if (d >= 0) {
        *drum = d;
        if (iam_main_drum(d))
            *part = IAM_DRUM_PART[d];
        return;
    }
    *part = t[iam.seq_at].part;
}
static uint16_t iam_seq_bits(const struct iam_seq_tr *t)
{
    if (t->drum >= 0)
        return iam_steps_live(t->drum, iam_drum_pat_aim(t->drum));
    if (t->part >= 0 && t->part < 4)
        return iam.vstep[t->part];
    return 0;
}
/* The percent stored for this row, before the section's gain. Kick, snare,
 * hat and perc keep the part. Clap, the open hat, and the five above keep
 * their own. A named key uses its engine. */
static int iam_seq_stored(const struct iam_seq_tr *t)
{
    int v = -1;
    if (t->drum >= 0 && t->drum < IAM_NDRUMKEY)
        v = iam_main_drum(t->drum) ? (int)iam.lvl[IAM_DRUM_PART[t->drum]] : (int)iam.dlvl[t->drum];
    else if (t->part >= 0 && t->part < IAM_NPART)
        v = (int)iam.lvl[t->part];
    if (v < 0)
        return -1;
    if (v > 100)
        v = 100;
    return v;
}
static void iam_seq_toggle(int step)
{
    struct iam_seq_tr t[15];
    int n, d, p;
    uint16_t bit, cur;
    const char *who;
    if (step < 0 || step > 15)
        return;
    n = iam_seq_clamp(t);
    if (n <= 0)
        return;
    if (!iam.seq_undo)
        iam_push();
    iam.seq_undo = 1;
    bit = (uint16_t)(1u << step);
    who = iam_seq_name(&t[iam.seq_at]);
    d = t[iam.seq_at].drum;
    if (d >= 0) {
        iam_step_grab(d);
        iam.stepv[d] = (uint16_t)(iam.stepv[d] ^ bit);
        iam_write_drum_pats();
        say(who, (iam.stepv[d] & bit) ? "STEP" : "REST");
        iam.dirty = 1;
        return;
    }
    p = t[iam.seq_at].part;
    cur = (uint16_t)(iam.vstep[p] ^ bit);
    iam_vstep_set(p, cur);
    say(who, (cur & bit) ? "STEP" : "REST");
}
static void iam_oct_text(char *b, int v)
{
    if (v < 0) {
        b[0] = '-';
        v = -v;
    } else if (v > 0)
        b[0] = '+';
    else {
        b[0] = '0';
        b[1] = 0;
        return;
    }
    if (v > 9)
        v = 9;
    b[1] = (char)('0' + v);
    b[2] = 0;
}
/* The row's octave, as a count. A lane's tune is one octave at 50.
 * An extra drum's tune is one octave at 12. */
static int iam_seq_oct_of(const struct iam_seq_tr *t, int *v)
{
    if (t->drum >= 0 && t->drum < 6) {
        *v = proj.par[P_LANE_TUNE(t->drum)] / 50;
        return 1;
    }
    if (t->drum >= 6 && t->drum < IAM_NDRUMKEY) {
        *v = (int)iam.xtune[t->drum - 6] / 12;
        return 1;
    }
    if (t->drum < 0 && t->part == 0) {
        *v = proj.par[P_PAD_OCT];
        return 1;
    }
    if (t->drum < 0 && t->part == 1) {
        *v = proj.par[P_BASSOCT];
        return 1;
    }
    if (t->drum < 0 && t->part == 2) {
        *v = proj.par[P_AC_OCT];
        return 1;
    }
    if (t->drum < 0 && t->part == 3) {
        *v = iam.haze_oct;
        return 1;
    }
    return 0;
}
static void iam_seq_say_oct(const char *who, int oct, int dir, int changed)
{
    char line[4];
    if (!changed) {
        say(who, dir > 0 ? "TOP" : "BOTTOM");
        return;
    }
    iam_oct_text(line, oct);
    say(who, line);
}
/* One octave on the row SELECT is on. The song's own octave stays.
 * A press that cannot move does not take an undo. */
static void iam_seq_oct(int dir)
{
    struct iam_seq_tr t[15];
    int n = iam_seq_clamp(t), d, cur, v, p;
    const char *who;
    if (n <= 0 || !dir)
        return;
    who = iam_seq_name(&t[iam.seq_at]);
    d = t[iam.seq_at].drum;
    if (d >= 0 && d < 6) {
        p = P_LANE_TUNE(d);
        cur = proj.par[p];
        v = cur + dir * 50;
        if (v > 50)
            v = 50;
        if (v < -50)
            v = -50;
        if (v == cur) {
            iam_seq_say_oct(who, 0, dir, 0);
            return;
        }
        iam_push();
        iam_set(p, v);
        iam_seq_say_oct(who, v / 50, dir, 1);
        return;
    }
    if (d >= 6 && d < IAM_NDRUMKEY) {
        int x = d - 6;
        cur = (int)iam.xtune[x];
        v = cur + dir * 12;
        if (v > 24)
            v = 24;
        if (v < -24)
            v = -24;
        if (v == cur) {
            iam_seq_say_oct(who, 0, dir, 0);
            return;
        }
        iam_push();
        iam.xtune[x] = (int8_t)v;
        iam_xpublish();
        iam_seq_say_oct(who, v / 12, dir, 1);
        return;
    }
    if (d < 0 && t[iam.seq_at].part == 3) {
        cur = (int)iam.haze_oct;
        v = cur + dir;
        if (v > 3)
            v = 3;
        if (v < -3)
            v = -3;
        if (v == cur) {
            iam_seq_say_oct(who, 0, dir, 0);
            return;
        }
        iam_push();
        iam.haze_oct = (int8_t)v;
        en_haze_oct(v);
        iam_seq_say_oct(who, v, dir, 1);
        return;
    }
    if (d < 0 && t[iam.seq_at].part >= 0 && t[iam.seq_at].part <= 2) {
        p = t[iam.seq_at].part == 0 ? P_PAD_OCT : t[iam.seq_at].part == 1 ? P_BASSOCT : P_AC_OCT;
        cur = proj.par[p];
        v = cur + dir;
        if (v > 3)
            v = 3;
        if (v < -3)
            v = -3;
        if (v == cur) {
            iam_seq_say_oct(who, 0, dir, 0);
            return;
        }
        iam_push();
        iam_set(p, v);
        iam_seq_say_oct(who, v, dir, 1);
    }
}
static void iam_seq_move(int dir)
{
    struct iam_seq_tr t[15];
    int n = iam_seq_clamp(t), i;
    if (n <= 0 || !dir)
        return;
    i = ((int)iam.seq_at + dir) % n; /* several detents in one turn still wrap */
    if (i < 0)
        i += n;
    iam.seq_at = (uint8_t)i;
    iam.turned = 0; /* the next level twist undoes itself, not the row before */
    say(iam_seq_name(&t[i]), iam.seq_isol ? "SOLO" : 0);
    if (iam.seq_isol)
        iam_apply(); /* the solo follows the row. A plain move leaves the mix */
    iam.dirty = 1;
}
static void iam_seq_open(void)
{
    struct iam_seq_tr t[15];
    int n, i, at = 0;
    n = iam_seq_clamp(t);
    if (iam.held_drum >= 0) {
        for (i = 0; i < n; i++)
            if (t[i].drum == iam.held_drum)
                at = i;
    } else if (iam.held >= 0 && iam.held < 4) {
        for (i = 0; i < n; i++)
            if (t[i].drum < 0 && t[i].part == iam.held)
                at = i;
    }
    iam.seq_on = 1;
    iam.seq_isol = 0; /* opening hears the song. SEL is what solos */
    iam.seq_at = (uint8_t)(n > 0 ? at : 0);
    iam.seq_undo = 0;
    iam.card = 0;
    if (iam.held_key >= 0)
        iam.turned = 1; /* letting go is not also a click */
    else
        iam.turned = 0; /* knob 3 undoes this level, not a twist from before the screen */
    if (n <= 0)
        say("SEQ", "NO TRACKS");
    else
        say("SEQ", iam_seq_name(&t[iam.seq_at]));
    iam.dirty = 1;
}
/* SEQ, GLO, or HOME. The solo leaves with the screen, and the song is heard again. */
static void iam_seq_shut(void)
{
    int was = iam.seq_isol && iam.seq_on;
    iam.seq_on = 0;
    iam.seq_isol = 0;
    if (was)
        iam_apply();
}
static void iam_seq_isol_flip(void)
{
    struct iam_seq_tr t[15];
    int n;
    if (!iam.seq_on)
        return;
    n = iam_seq_clamp(t);
    if (n <= 0) {
        iam.seq_isol = 0;
        iam_apply();
        say("SEQ", "NO TRACKS");
        return;
    }
    iam.seq_isol ^= 1;
    say(iam_seq_name(&t[iam.seq_at]), iam.seq_isol ? "SOLO" : "SONG");
    iam_apply();
}
static void iam_seq_keys(uint32_t keys)
{
    uint32_t down = keys & ~iam.keys, up = iam.keys & ~keys;
    int i;
    iam.keys = keys;
    for (i = 0; i < NKEYS; i++) {
        int w, k;
        if (!((down >> i) & 1u))
            continue;
        w = -1;
        for (k = 0; k < 16; k++)
            if (WHITE_KEY[k] == (uint8_t)i) {
                w = k;
                break;
            }
        if (w >= 0)
            iam_seq_toggle(w);
    }
    if (iam.held_key >= 0 && (up >> iam.held_key & 1u)) {
        iam.held = -1;
        iam.held_key = -1;
        iam.held_drum = -1;
        iam.held_pre = -1;
        iam.solo = 0;
        iam.turned = 1;
        iam_apply();
    }
}
/* SEQ while a drum or a white key is held. Said once. Letting go is not also a click. */
static void iam_step_enter(void)
{
    if (iam.step_stay || !iam.on || !iam_step_screen())
        return;
    iam.step_stay = 1;
    iam.turned = 1;
    say(iam_step_who(), "STEPS");
    iam.dirty = 1;
}
static int iam_step_latched(void)
{
    return iam.step_stay && iam_step_screen();
}
static void iam_press(int b)
{
    if (b == B_SEL || b == B_FX)
        return; /* SEL is decided on the way up. A short FX release mutates; a hold casts and leaves a tip */
    if (iam.card & 1u)
        iam.card = 0; /* any other command comes back to the song */
    if (b == B_PLAY) {
        iam.auto_on ^= 1;
        if (iam.auto_on && iam.sec == IAM_INTRO)
            iam.form = 0;
        say(iam.auto_on ? "AUTO" : "MANUAL", iam.auto_on ? "CONDUCTING" : "YOURS");
        iam_rescue(iam.mask); /* auto, back on in the intro, must not mute every part that is on */
        iam_apply();
    } else if (b == B_SAVE) {
        iam_find_cancel();
        iam.q_scene = IAM_Q_BREAK;
        iam.q_pst = 0;
        iam.q_pump = 0;
        iam_due("BREAK");
    } else if (b == B_EDIT) {
        iam_find_cancel();
        iam.q_scene = IAM_Q_EXCITE;
        iam.q_pst = 0;
        iam.q_drums = 0;
        iam.q_pump = 0; /* EDIT is the colour and the filters, not the pump */
        iam_due("EXCITE");
    } else if (b == B_SEQ) {
        if (iam.seq_on) {
            iam_seq_shut();
            say("SONG", 0);
        } else
            iam_seq_open();
    } else if (b == B_ARP) {
        iam_find_cancel();
        iam.q_scene = IAM_Q_BACK;
        iam.q_pst = 0;
        iam.q_pump = 0;
        iam_due("BACK");
    } else if (b == B_ENV) {
        iam_find_cancel();
        iam.q_scene = IAM_Q_CHANGE;
        iam.q_pst = 0;
        iam.q_pump = 0;
        iam_due("CHANGE");
    } else if (b == B_REC) {
        if (iam_loop_playing()) {
            iam_loop_stop(); /* a loop brought out of GLO. Freeze waits until it is gone */
        } else {
            en_freeze(!en_frozen);
            say(en_frozen ? "HELD" : "MOVING", 0);
        }
    } else if (b == B_LFO) {
        int place = iam_find_roll();
        iam.q_scene = IAM_Q_NONE; /* one intention for this bar */
        iam.q_pst = 0;
        iam.q_pump = 0;
        iam.q_drums = 0;
        iam.find_kind = (uint8_t)place;
        if (iam.style == 12)
            iam.find_room = (uint8_t)((iam.sec + 1) % IAM_NSEC);
        iam.q_find = 1;
        en_find(iam.style == 12 ? 2 : 1); /* the effect now. The place waits for the bar */
        say("FIND", iam_find_place(place));
    } else if (b == B_HOME) {
        if (iam.seq_on) {
            iam_seq_shut(); /* the chord stays where it is */
            say("SONG", 0);
        } else if (iam_step_latched()) {
            iam_step_drop(); /* the step screen. The chord stays where it is */
            iam_apply();
            say("SONG", 0);
        } else {
            en_next();
            say("NEXT", "CHORD");
        }
    } else if ((b == B_OCTUP || b == B_OCTDN) && iam.seq_on) {
        iam_seq_oct(b == B_OCTUP ? 1 : -1); /* this track. Not excite, and not back */
    } else if (b == B_OCTUP && iam_step_latched()) {
        iam_step_set(1); /* the marked step on. Not excite, and not a pump */
    } else if (b == B_OCTDN && iam_step_latched()) {
        iam_step_set(0); /* the marked step off. Not back */
    } else if (b == B_OCTUP && iam.held_drum >= 0) {
        iam_step_next(); /* still holding: the next step. Not excite */
    } else if (b == B_OCTDN && iam.held_drum >= 0) {
        iam_step_toggle();
    } else if (b == B_OCTUP) {
        iam_find_cancel();
        if (iam.sec == IAM_EXCITE || iam.q_scene == IAM_Q_EXCITE) {
            if (iam.q_pump < 6)
                iam.q_pump++; /* already going: keep going. Several in one bar are one undo */
            if (iam.q_scene != IAM_Q_EXCITE) {
                iam.q_scene = IAM_Q_NONE; /* forward. It does not also break, drop, or go back */
                iam.q_pst = 0;
            }
            iam_due("PUMP");
        } else {
            iam.q_scene = IAM_Q_EXCITE;
            iam.q_pst = 0;
            iam.q_drums = 1; /* two silent drums, with the filters and the chord */
            iam.q_pump = 0;
            iam_due("EXCITE");
        }
    } else if (b == B_OCTDN) {
        iam_find_cancel();
        iam.q_scene = IAM_Q_BACK;
        iam.q_pst = 0;
        iam.q_drums = 0;
        iam.q_pump = 0;
        iam_due("BACK");
    } else if (b == B_GLO) {
        int role;
        iam_find_cancel(); /* the search lets go. The editor is not a place */
        iam_seq_shut();
        iam_step_drop(); /* the step screen does not follow the editor back */
        iam_q_clear(); /* a queued bar does not fire underneath the editor */
        for (role = 0; role < NE; role++)
            (void)plat_enc(role); /* the deep editor must not inherit a pile of detents */
        iam.on = 0;
        iam.dirty = 1;
        ui.btn = plat_buttons();
        ui.btn_used = ui.btn;
        ui.keys = plat_keys();
        ui.shift_keys = ui.keys;
        ui.sig[0] = ui.sig[1] = ui.sig[2] = 0xFFFFFFFFu; /* a matching hash would skip the paint and leave this clear */
        lcd_fill(0, 0, 240, 240, K_BG);
        say("DEEP EDIT", "GLO: BACK");
    }
    iam.dirty = 1;
}
static int iam_pending_on(int part)
{
    return ((iam.mask >> part) & 1) ^ ((iam.q_xor >> part) & 1);
}
static void iam_arm_part(int part, int on)
{
    if (on == ((iam.mask >> part) & 1))
        iam.q_xor = (uint8_t)(iam.q_xor & ~(1u << part));
    else
        iam.q_xor = (uint8_t)(iam.q_xor | (1u << part));
}
static void iam_queue_part(int part)
{
    iam.q_xor ^= (uint8_t)(1u << part); /* two clicks in one bar cancel */
    if ((iam.q_xor >> part) & 1)
        iam_due(iam_part_name(part));
    else
        say("STAYS", 0);
}
static void iam_queue_side(int di)
{
    uint16_t bit = (uint16_t)(1u << di);
    iam.q_don = (uint16_t)(iam.q_don ^ bit); /* two clicks in one bar cancel */
    if (iam.q_don & bit)
        iam_due(iam_drum_name(di));
    else
        say("STAYS", 0);
}
static void iam_queue_drum(int di)
{
    if (iam_main_drum(di))
        iam_queue_part(IAM_DRUM_PART[di]);
    else
        iam_queue_side(di); /* clap, open hat, and the five: not the part beside them */
}
/* The engine key's sound. The same key again turns that part off. A different
 * sound, from knob 1, replaces it. A named key does not come through here. */
static void iam_queue_sound(int part, int snd, const char *name)
{
    int pend = proj.par[IAM_SND[part]] + (int)iam.q_snd[part];
    int queued = ((iam.q_xor >> part) & 1) || iam.q_snd[part];
    if (queued && iam_pending_on(part) && pend == snd) {
        iam.q_xor = (uint8_t)(iam.q_xor & ~(1u << part));
        iam.q_snd[part] = 0;
        say("STAYS", 0);
        return;
    }
    if (!queued && ((iam.mask >> part) & 1) && proj.par[IAM_SND[part]] == snd) {
        iam_arm_part(part, 0);
        iam_due(name);
        return;
    }
    /* replaces a queued twist, rather than adding to it. Zero means no sound edit */
    if (proj.par[IAM_SND[part]] == snd)
        iam.q_snd[part] = 0;
    else
        iam.q_snd[part] = iam_sat(snd - proj.par[IAM_SND[part]]);
    iam_arm_part(part, 1);
    if (!((iam.q_xor >> part) & 1) && !iam.q_snd[part])
        say("STAYS", 0);
    else
        iam_due(name);
}
/* A named key joins the song on the next bar. The same key turns that voice off.
 * Two clicks in one bar cancel. The engine's own sound stays where it is. */
static void iam_queue_preset(int pi)
{
    uint16_t bit;
    if (pi < 0 || pi > 11)
        return;
    bit = (uint16_t)(1u << pi);
    iam.q_lay = (uint16_t)(iam.q_lay ^ bit);
    if (iam.q_lay & bit)
        iam_due(IAM_PRE_NAME[pi]);
    else
        say("STAYS", 0);
}
static void iam_queue_engine(int part)
{
    iam_queue_sound(part, iam.own[part], iam_part_name(part));
}
static void iam_keys(uint32_t keys, uint32_t btn)
{
    uint32_t down = keys & ~iam.keys, up = iam.keys & ~keys;
    int i, fx = (btn >> B_FX) & 1, sel = (btn >> B_SEL) & 1;
    if (iam.seq_on) {
        iam_seq_keys(keys);
        return;
    }
    iam.keys = keys;
    for (i = 0; i < NKEYS; i++) {
        int part, drum, pre;
        if (!((down >> i) & 1u))
            continue;
        iam.key_t[i] = plat_ms();
        if (sel)
            iam.sel_arm = 0; /* SEL is soloing, not asking for a tip */
        iam_key_role(i, &part, &drum, &pre);
        if (part < 0 && drum < 0)
            continue;
        iam.held = (int8_t)part;
        iam.held_key = (int8_t)i;
        iam.held_drum = (int8_t)drum;
        iam.held_pre = (int8_t)pre;
        iam.step_i = 0; /* each hold starts on step 1 */
        iam.step_undo = 0;
        iam.step_stay = 0; /* SEQ on this key latches it. A new key leaves the last screen */
        iam.turned = 0;
        iam.fx_down = (uint8_t)fx;
        iam.sel_down = (uint8_t)sel;
        if (sel && part >= 0)
            iam.solo = (uint8_t)(1u << part);
        iam_apply();
    }
    if (!sel)
        iam.solo = 0;
    for (i = 0; i < NKEYS; i++) {
        if (!((up >> i) & 1u) || i != iam.held_key)
            continue;
        if (!iam.turned && !iam.fx_down && !iam.sel_down && iam.key_t[i] &&
            plat_ms() - iam.key_t[i] < IAM_CLICK_MS && iam.knob_t < iam.key_t[i] &&
            iam.fx_t < iam.key_t[i]) { /* an FX click during this hold is the mutate, not also a tap */
            if (iam.held_drum >= 0)
                iam_queue_drum(iam.held_drum);
            else if (iam.held_pre >= 0)
                iam_queue_preset(iam.held_pre);
            else if (iam.held >= 0 && iam.held < 3)
                iam_queue_engine(iam.held); /* the song's own sound. A named key is the other one */
            else if (iam.held >= 0)
                iam_queue_part(iam.held);
        }
        if (iam.step_stay && iam_step_screen())
            iam_step_keep();
        else
            iam_step_drop();
        iam_apply();
    }
    if (iam.held_key >= 0 && !(keys >> iam.held_key & 1u)) {
        /* The up edge already decided a latch. This only catches a key that is already gone. */
        if (iam.step_stay && iam_step_screen())
            iam_step_keep();
        else
            iam_step_drop();
    }
}
/* The headline is the only line the song screen shows. */
static void iam_say_amt(const char *what, int v)
{
    char b[20], n[8], *d = b;
    const char *p = what;
    while (*p && d - b < 12)
        *d++ = *p++;
    *d++ = ' ';
    if (v <= 0)
        p = "OFF";
    else if (v >= 100)
        p = "FULL";
    else {
        itoa_u((uint32_t)v, n);
        p = n;
    }
    while (*p && d - b < 18)
        *d++ = *p++;
    *d = 0;
    say(b, 0);
}
/* The four synth filters, in the one line the song screen shows. */
static void iam_say_cut(void)
{
    char b[24], n[8], *d = b;
    const char *p = "CUTOFF";
    int i;
    while (*p && d - b < 8)
        *d++ = *p++;
    for (i = 0; i < 4; i++) {
        if (d - b >= 22)
            break;
        *d++ = ' ';
        itoa_u((uint32_t)proj.par[IAM_CUT[i]], n);
        p = n;
        while (*p && d - b < 23)
            *d++ = *p++;
    }
    *d = 0;
    say(b, 0);
}
static int iam_amt(int cur, int e)
{
    int v = cur + e * 8;
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    return v;
}
/* The next rhythm in a fixed row. A hand-written mask moves one hit instead.
 * No roll: mutate must not call the song's rng. */
static uint16_t iam_vstep_turn(uint16_t m, int dir)
{
    static const uint16_t row[] = {
        0xFFFFu, 0x4444u, 0xAAAAu, 0x5555u, 0x1111u, 0x8888u, 0x2222u, 0x4924u,
        0x2492u, 0x0F0Fu, 0xF00Fu, 0xCCCCu, 0x3333u, 0x8421u, 0x1248u, 0x0A0Au};
    int n = 16, i, at = -1;
    for (i = 0; i < n; i++) {
        if (row[i] == m) {
            at = i;
            break;
        }
    }
    if (at >= 0) {
        at += dir > 0 ? 1 : n - 1;
        if (at >= n)
            at = 0;
        return row[at];
    }
    if (dir > 0) {
        for (i = 0; i < 16; i++)
            if (((m >> i) & 1) == 0)
                return (uint16_t)(m | (uint16_t)(1u << i));
        return 0x4444u;
    }
    for (i = 15; i >= 0; i--)
        if ((m >> i) & 1)
            return (uint16_t)(m & (uint16_t)~(1u << i));
    return 0x1111u;
}
/* One detent, either way, moves the song. Down is not the undo of up.
 * Tempo stays. A part's on or off stays. Nothing here waits for the bar.
 * Pad, bass, acid and the fourth key also step their sixteen. */
static void iam_mutate(int e)
{
    static const int LEAD[4] = {4, 5, 6, 7}; /* kick, snare, hat, perc */
    int dir = e >= 0 ? 1 : -1;
    int n = e >= 0 ? e : -e;
    int i;
    const char *tag = "MUTATE";
    if (n > 8)
        n = 8;
    if (iam.style == 12) {
        for (i = 0; i < n; i++) {
            int k = (int)(iam.mut++ & 3u);
            if (k == 0) {
                iam_set(P_SIZE, proj.par[P_SIZE] + dir * 16);
                tag = "MUTATE SPACE";
            } else if (k == 1) {
                iam_set(P_REVERB, proj.par[P_REVERB] + dir * 14);
                tag = "MUTATE ROOM";
            } else if (k == 2) {
                iam_set(P_DR_CUT, proj.par[P_DR_CUT] + dir * 18);
                tag = "MUTATE DRONE";
            } else {
                iam_set(P_BRIGHT, proj.par[P_BRIGHT] + dir * 14);
                iam_set(P_NZ_TONE, proj.par[P_NZ_TONE] + dir * 8);
                iam_vstep_set(3, iam_vstep_turn(iam.vstep[3], dir));
                tag = "MUTATE WIND";
            }
        }
        say(tag, 0);
        return;
    }
    for (i = 0; i < n; i++) {
        int k = (int)(iam.mut++ & 7u);
        if (dir < 0)
            k ^= 4;
        if (k == 0) {
            int p = LEAD[iam.mut & 3];
            iam.pat[p] = (uint8_t)((iam.pat[p] + dir + 32) & 7);
            iam_write_drum_pats();
            tag = "MUTATE BEAT";
        } else if (k == 1) {
            iam.pat[2] = (uint8_t)((iam.pat[2] + (dir > 0 ? 3 : 1) + 32) & 7);
            iam_write_drums();
            iam_vstep_set(2, iam_vstep_turn(iam.vstep[2], dir));
            tag = "MUTATE ACID";
        } else if (k == 2) {
            iam_pad_colour(1, 0);
            iam_vstep_set(0, iam_vstep_turn(iam.vstep[0], dir));
            tag = "MUTATE PAD";
        } else if (k == 3) {
            int v = (proj.par[P_DR_INST] + (dir > 0 ? 3 : 5) + EN_NSOUND * 4) % EN_NSOUND;
            iam_set(P_DR_INST, v);
            iam_vstep_set(1, iam_vstep_turn(iam.vstep[1], dir));
            tag = "MUTATE BASS";
        } else if (k == 4) {
            iam.pat[3] = (uint8_t)((iam.pat[3] + (dir > 0 ? 1 : 3) + 32) & 7);
            iam_write_drums();
            iam_vstep_set(3, iam_vstep_turn(iam.vstep[3], dir));
            tag = "MUTATE AIR";
        } else if (k == 5) {
            iam_set(P_BRIGHT, proj.par[P_BRIGHT] + dir * 18);
            iam_set(P_DR_CUT, proj.par[P_DR_CUT] + dir * 16);
            iam_set(P_AC_CUT, proj.par[P_AC_CUT] + dir * 20);
            iam_set(P_EVOLVE, proj.par[P_EVOLVE] + dir * 12);
            iam_set(P_MOTION, proj.par[P_MOTION] + dir * 10);
            tag = "MUTATE FILTER";
        } else if (k == 6) {
            int part = LEAD[iam.mut & 3];
            int v = (proj.par[IAM_SND[part]] + dir + EN_NKIT * 8) % EN_NKIT;
            iam_set(IAM_SND[part], v);
            tag = "MUTATE KIT";
        } else {
            iam_set(P_ECHO, proj.par[P_ECHO] + dir * 16);
            iam_set(P_REVERB, proj.par[P_REVERB] + dir * 14);
            iam_set(P_DRIVE, proj.par[P_DRIVE] + dir * 10);
            tag = "MUTATE ROOM";
        }
    }
    say(tag, 0);
}
static int iam_key_settling(uint32_t keys)
{
    int i;
    uint32_t now = plat_ms();
    for (i = 0; i < NKEYS; i++)
        if ((keys >> i) & 1u && iam.key_t[i] && now - iam.key_t[i] < IAM_SETTLE_MS)
            return 1;
    return 0;
}
/* Knob 2 while a key is held. Heard now. It does not switch the key on.
 * One undo for the gesture. A synth wobbles on the eighth. A drum's pitch
 * sweeps, and the next hit is the one that shows it. Zero is the song. */
static void iam_turn_shape(int e)
{
    int v;
    if (!iam.turned)
        iam_push();
    iam.turned = 1; /* letting go is the edit, not also a click */
    if (iam.held_drum >= 0) {
        int d = iam.held_drum;
        v = (int)iam.sweep[d] + e * 16;
        if (v < 0)
            v = 0;
        if (v > 100)
            v = 100;
        iam.sweep[d] = (uint8_t)v;
        en_snap(d, v);
        iam_say_amt("SWEEP", v);
        return;
    }
    if (iam.held < 0 || iam.held > 3)
        return;
    v = (int)iam.wob[iam.held] + e * 16;
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    iam.wob[iam.held] = (uint8_t)v;
    en_wobble(iam.held, v);
    iam_say_amt("WOBBLE", v);
}
/* FX while a key is down. That key only, now, one way. It does not take an undo,
 * and it does not switch the key on. A drum steps its own pattern and leaves the
 * linked beat. A named key mutates the engine it plays. */
static void iam_mutate_key(void)
{
    int part = iam.held;
    const char *who;
    if (iam.held_drum >= 0) {
        int d = iam.held_drum;
        iam.stepo = (uint16_t)(iam.stepo & ~(1u << d));
        iam.pown = (uint16_t)(iam.pown | (1u << d));
        if (iam_main_drum(d)) {
            int p = IAM_DRUM_PART[d];
            iam.pat[p] = (uint8_t)((iam.pat[p] + 1) & 7);
        } else
            iam.dpat[d] = (uint8_t)((iam.dpat[d] + 1) & 7);
        iam_write_drum_pats();
        say(iam_drum_name(d), "MUTATE");
        return;
    }
    if (part < 0 || part > 3)
        return;
    if (iam.held_pre >= 0 && iam.held_pre < 12)
        who = IAM_PRE_NAME[iam.held_pre];
    else
        who = iam_part_name(part);
    if (part == 0) {
        iam_pad_colour_go(1, 0);
        iam_vstep_set(0, iam_vstep_turn(iam.vstep[0], 1));
    } else if (part == 1) {
        int v = (proj.par[P_DR_INST] + 3 + EN_NSOUND * 4) % EN_NSOUND;
        iam_set(P_DR_INST, v);
        iam_vstep_set(1, iam_vstep_turn(iam.vstep[1], 1));
    } else if (part == 2) {
        iam.pat[2] = (uint8_t)((iam.pat[2] + 3) & 7);
        iam_write_drums();
        iam_vstep_set(2, iam_vstep_turn(iam.vstep[2], 1));
    } else {
        iam.pat[3] = (uint8_t)((iam.pat[3] + 1) & 7);
        iam_write_drums();
        iam_vstep_set(3, iam_vstep_turn(iam.vstep[3], 1));
    }
    say(who, "MUTATE");
}
/* Knob 3 on the SEQ screen. The selected row's stored level, four percent
 * a detent, one undo for the twist. The phaser waits until the screen closes. */
static void iam_seq_say_lvl(const char *name, int v)
{
    char b[24], n[8], *d = b;
    const char *p = name && name[0] ? name : "LEVEL";
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    while (*p && d - b < 12)
        *d++ = *p++;
    *d++ = ' ';
    itoa_u((uint32_t)v, n);
    p = n;
    while (*p && d - b < 20)
        *d++ = *p++;
    if (d - b < 22)
        *d++ = '%';
    *d = 0;
    say(b, 0);
}
static void iam_seq_level(int step)
{
    struct iam_seq_tr t[15];
    int n, v, d, part;
    if (!step)
        return;
    n = iam_seq_clamp(t);
    if (n <= 0)
        return;
    d = t[iam.seq_at].drum;
    if (d >= 0 && d < IAM_NDRUMKEY && !iam_main_drum(d)) {
        part = -1;
        v = (int)iam.dlvl[d];
    } else {
        part = d >= 0 ? IAM_DRUM_PART[d] : (int)t[iam.seq_at].part;
        if (part < 0 || part >= IAM_NPART)
            return;
        v = (int)iam.lvl[part];
    }
    v += step * 4;
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    if (!iam.turned)
        iam_push();
    iam.turned = 1;
    if (part < 0)
        iam.dlvl[d] = (uint8_t)v;
    else
        iam.lvl[part] = (uint8_t)v;
    iam_seq_say_lvl(iam_seq_name(&t[iam.seq_at]), v);
    iam_apply();
}
static void iam_knobs(uint32_t btn)
{
    int role, fx = (btn >> B_FX) & 1, settle = iam_key_settling(plat_keys());
    for (role = 0; role < NE; role++) {
        int32_t e = plat_enc(role);
        if (!e)
            continue;
        if (settle)
            continue; /* a detent this close to the press does not eat the click */
        iam.dirty = 1;
        iam.knob_t = plat_ms();
        if (fx) {
            iam.fx_twist = 1; /* the detent is spent. It does not move a knob, and the release is not a mutate */
            continue;
        }
        if (role == EN_SELECT && iam.seq_on) {
            iam_seq_move(e); /* the tracks. The tempo waits until the screen closes */
            continue;
        }
        if (role == EN_K3 && iam.seq_on) {
            iam_seq_level(e); /* this row's level. The phaser waits until SEQ closes */
            continue;
        }
        if (iam.held_drum >= 0 && role >= EN_K1 && role <= EN_K4) {
            int d = iam.held_drum;
            if (role == EN_K1) {
                iam.turned = 1; /* letting go is the edit, not also a click */
                iam.q_dkit[d] = iam_sat((int)iam.q_dkit[d] + e);
                /* the kit name is already the line under the drum */
            } else if (role == EN_K2) {
                iam_turn_shape(e); /* a pitch sweep, heard on the next hit. It does not take the pattern */
            } else if (role == EN_K3) {
                iam_drum_level(d, e); /* this drum's level. Not the one beside it */
            } else if (iam_main_drum(d)) {
                iam_turn_part(iam.held, 3, e); /* tune, and the undo, live */
            } else if (d == 2) {
                if (!iam.turned)
                    iam_push();
                iam.turned = 1;
                iam_set(P_LANE_TUNE(2), proj.par[P_LANE_TUNE(2)] + e * 3);
            } else if (d == 4) {
                if (!iam.turned)
                    iam_push();
                iam.turned = 1;
                iam_set(P_LANE_TUNE(4), proj.par[P_LANE_TUNE(4)] + e * 3);
            } else {
                int x = d - 6;
                int t;
                if (!iam.turned)
                    iam_push();
                iam.turned = 1;
                t = (int)iam.xtune[x] + e * 3;
                if (t > 24)
                    t = 24;
                if (t < -24)
                    t = -24;
                iam.xtune[x] = (int8_t)t;
                iam_xpublish();
            }
            continue;
        }
        if (iam.held >= 0 && role >= EN_K1 && role <= EN_K4) {
            if (role == EN_K1) {
                iam.turned = 1; /* letting go is the edit, not also a click */
                iam.q_snd[iam.held] = iam_sat((int)iam.q_snd[iam.held] + e);
            } else if (role == EN_K2) {
                iam_turn_shape(e); /* an eighth-note wobble. Heard now */
            } else {
                iam_turn_part(iam.held, role - EN_K1, e); /* level and cutoff stay live */
            }
            continue;
        }
        if (role == EN_SELECT && iam_step_latched()) {
            iam_step_move(e); /* the steps. The tempo waits until HOME */
            continue;
        }
        if (iam.held >= 0 || iam.held_drum >= 0)
            continue; /* a held part owns the knobs: tempo, scale and style wait */
        if (role == EN_K1) {
            int v = iam_amt((int)iam.dly, e);
            iam.dly = (uint8_t)v;
            en_ring(v);
            iam_say_amt("RING", v);
        } else if (role == EN_K2) {
            int v = iam_amt((int)iam.hel, e);
            iam.hel = (uint8_t)v;
            en_echo(0); /* the tape stays the one the song baked */
            en_wah(v);
            iam_say_amt("WAH", v);
        } else if (role == EN_K3) {
            int v = iam_amt((int)iam.mos, e);
            iam.mos = (uint8_t)v;
            en_phase(v);
            iam_say_amt("PHASER", v);
        } else if (role == EN_K4) {
            int p;
            for (p = 0; p < 4; p++)
                iam_set(IAM_CUT[p], proj.par[IAM_CUT[p]] + e * 4); /* the synth filters, together */
            iam_say_cut();
        } else if (role == EN_SELECT)
            iam_set(P_TEMPO, proj.par[P_TEMPO] + e);
        else if (role == EN_ALGO) {
            int m = (proj.par[P_MODE] + e) % EN_NMODES;
            if (m < 0)
                m += EN_NMODES;
            iam_set(P_MODE, m);
            proj.pool = EN_MODE_POOL[m];
            en_pool(proj.pool);
            say("SCALE", EN_MODE_NAME[m]);
        } else if (role == EN_PRESET) {
            iam_find_cancel(); /* the dial is a new song, not a place in this one */
            iam.q_scene = IAM_Q_VIBE;
            iam.q_pst = iam_sat((int)iam.q_pst + e); /* detents in one bar net out, and wrap at the landing */
            iam.q_pump = 0;
            iam_due("VIBE");
        }
    }
}
static int iam_sounding(float v) { return v > 0.35f; }
/* The engine's own key. A named sound has its own key, and both can be lit. */
static int iam_sound_key(int part)
{
    if (part < 0 || part > 3 || !((iam.mask >> part) & 1))
        return -1;
    return part;
}
/* Full light while the sound is in the air. The key stays dim the rest of the time,
 * so a flash is that noise and that key. */
static uint32_t iam_flash(void)
{
    uint32_t f = 0;
    int i, w;
    if (en_playing) {
        if (((iam.mask >> 0) & 1) && iam_sounding(en_pad_level) && (w = iam_sound_key(0)) >= 0)
            f |= 1u << WHITE_KEY[w];
        if (((iam.mask >> 1) & 1) && iam_sounding(en_bass_env) && (w = iam_sound_key(1)) >= 0)
            f |= 1u << WHITE_KEY[w];
        if (((iam.mask >> 2) & 1) && (iam_sounding(en_acid_glow) || iam_sounding(en_acid_level)) &&
            (w = iam_sound_key(2)) >= 0)
            f |= 1u << WHITE_KEY[w];
        if (((iam.mask >> 3) & 1) && iam_sounding(en_noise_level))
            f |= 1u << WHITE_KEY[3];
        {
            uint16_t hot = en_layer_hot;
            for (i = 0; i < 12; i++)
                if ((hot >> i) & 1)
                    f |= 1u << WHITE_KEY[4 + i];
        }
        for (i = 0; i < IAM_NDRUMKEY; i++) {
            float g = i < EN_NDRUM ? en_drum_glow[i] : en_xdrum_glow[i - EN_NDRUM];
            if (iam_drum_on(i) && iam_sounding(g))
                f |= 1u << BLACK_KEY[i];
        }
    }
    if (iam.held_key >= 0)
        f |= 1u << iam.held_key;
    return f;
}
static void iam_leds(uint32_t btn)
{
    uint32_t b = 0, k = 0;
    int i;
    if (iam.auto_on)
        b |= 1u << B_PLAY;
    if (iam_loop_button())
        b |= 1u << B_REC; /* a loop from GLO: REC blinks until it is pressed */
    if ((btn >> B_FX) & 1)
        b |= 1u << B_FX;
    if ((btn >> B_SEL) & 1u || (iam.card & 1u) || (iam.seq_on && iam.seq_isol))
        b |= 1u << B_SEL; /* stays lit after the release, while the row is alone */
    if (iam.q_find)
        b |= 1u << B_LFO; /* the place is waiting */
    if (iam.sec == IAM_BREAK)
        b |= 1u << B_SAVE;
    if (iam.sec == IAM_EXCITE)
        b |= 1u << B_EDIT;
    if (iam.sec == IAM_DROP || iam.seq_on)
        b |= 1u << B_SEQ;
    if (iam.seq_on) {
        /* White keys are this track's steps. Black keys stay the drums that are on.
         * A voice flash would blink a step that is meant to stay lit. */
        struct iam_seq_tr t[15];
        int n = iam_seq_clamp(t);
        uint16_t bits = n > 0 ? iam_seq_bits(&t[iam.seq_at]) : 0;
        uint32_t f = 0;
        for (i = 0; i < 16; i++)
            if ((bits >> i) & 1)
                k |= 1u << WHITE_KEY[i];
        for (i = 0; i < IAM_NDRUMKEY; i++)
            if (iam_drum_on(i))
                k |= 1u << BLACK_KEY[i];
        if (en_playing) {
            for (i = 0; i < IAM_NDRUMKEY; i++) {
                float g = i < EN_NDRUM ? en_drum_glow[i] : en_xdrum_glow[i - EN_NDRUM];
                if (iam_drum_on(i) && iam_sounding(g))
                    f |= 1u << BLACK_KEY[i];
            }
        }
        plat_glow(1);
        plat_key_flash(f);
        plat_leds(b, k);
        return;
    }
    for (i = 0; i < 4; i++) {
        int w = iam_sound_key(i);
        if (w >= 0)
            k |= 1u << WHITE_KEY[w];
    }
    for (i = 0; i < 12; i++)
        if ((iam.lay >> i) & 1)
            k |= 1u << WHITE_KEY[4 + i];
    for (i = 0; i < IAM_NDRUMKEY; i++)
        if (iam_drum_on(i))
            k |= 1u << BLACK_KEY[i];
    if (iam.held_key >= 0)
        k |= 1u << iam.held_key;
    plat_glow(1);
    plat_key_flash(iam_flash());
    plat_leds(b, k);
}
static void iam_line(int x, int y, int w, const char *s, uint16_t c)
{
    /* the device blit does not clip, so a line leaving the panel is skipped whole */
    if (y < 0 || w <= 0 || x >= 240 || y + (int)FONT_S.h > 240)
        return;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (w <= 0)
        return;
    if (x + w > 240)
        w = 240 - x;
    draw_text_box((uint32_t)x, (uint32_t)y, (uint32_t)w, &FONT_S, s, c, 1);
}
static uint32_t iam_mix(uint32_t s, uint32_t v) { return s * 16777619u ^ v; }
static uint32_t iam_picture(void)
{
    uint32_t s = 2166136261u;
    const char *p;
    int i, toast = plat_ms() < ui.msg_until;
    s = iam_mix(s, iam.card);
    s = iam_mix(s, (uint32_t)(iam.held + 2));
    s = iam_mix(s, (uint32_t)(iam.held_drum + 2));
    s = iam_mix(s, (uint32_t)(iam.held_pre + 2));
    s = iam_mix(s, iam.mask);
    s = iam_mix(s, iam.lay);
    s = iam_mix(s, iam.q_lay);
    s = iam_mix(s, iam.xown);
    s = iam_mix(s, iam.xon);
    s = iam_mix(s, iam.don);
    s = iam_mix(s, (uint32_t)proj.par[P_LANE_TUNE(2)]);
    s = iam_mix(s, (uint32_t)proj.par[P_LANE_TUNE(4)]);
    for (i = 0; i < IAM_NDRUMKEY; i++) {
        s = iam_mix(s, iam.dkit[i]);
        s = iam_mix(s, iam.dpat[i]);
        s = iam_mix(s, iam.dlvl[i]);
        s = iam_mix(s, iam.stepv[i]); /* a written step has to repaint the strip */
        s = iam_mix(s, (uint32_t)(int)iam.q_dpat[i]); /* the steps follow the knob, before the bar */
        s = iam_mix(s, (uint32_t)(int)iam.q_dkit[i]);
    }
    for (i = 0; i < EN_XDRUM; i++) {
        s = iam_mix(s, (uint32_t)(int)iam.xtune[i]);
        s = iam_mix(s, iam.xdec[i]);
    }
    for (i = 0; i < 4; i++)
        s = iam_mix(s, iam.wob[i]);
    for (i = 0; i < IAM_NDRUMKEY; i++)
        s = iam_mix(s, iam.sweep[i]);
    s = iam_mix(s, (uint32_t)proj.par[P_KIT_KICK]);
    s = iam_mix(s, (uint32_t)proj.par[P_KIT_SNARE]);
    s = iam_mix(s, (uint32_t)proj.par[P_KIT_HAT]);
    s = iam_mix(s, (uint32_t)proj.par[P_KIT_PERC]);
    s = iam_mix(s, (uint32_t)proj.par[P_PADTONE]);
    s = iam_mix(s, (uint32_t)proj.par[P_DR_INST]);
    s = iam_mix(s, (uint32_t)proj.par[P_AC_WAVE]);
    s = iam_mix(s, (uint32_t)proj.par[P_NZ_COLOR]);
    for (i = 0; i < IAM_NPART; i++)
        s = iam_mix(s, (uint32_t)(int)iam.q_snd[i]); /* the list follows a turn, before the bar */
    s = iam_mix(s, iam.stepo);
    s = iam_mix(s, iam.step_i); /* the mark moves while the same key stays down */
    s = iam_mix(s, iam.auto_on);
    s = iam_mix(s, iam.sec);
    s = iam_mix(s, iam.style);
    s = iam_mix(s, (uint32_t)proj.par[P_TEMPO]);
    s = iam_mix(s, (uint32_t)proj.par[P_ROOT]);
    s = iam_mix(s, (uint32_t)en_playing);
    s = iam_mix(s, (uint32_t)en_frozen);
    s = iam_mix(s, iam.q_find); /* the place stays on the action line after the toast */
    s = iam_mix(s, iam.find_kind);
    s = iam_mix(s, iam.find_room);
    s = iam_mix(s, (uint32_t)toast);
    if (toast) {
        for (p = ui.msg[0]; *p; p++)
            s = iam_mix(s, (uint8_t)*p);
        for (p = ui.msg[1]; *p; p++)
            s = iam_mix(s, (uint8_t)*p);
    }
    if (iam.held >= 0 && iam.held < IAM_NPART) {
        s = iam_mix(s, iam.pat[iam.held]);
        s = iam_mix(s, (uint32_t)(int)iam.q_pat[iam.held]); /* the drawing follows the knob, before the bar */
        s = iam_mix(s, iam.lvl[iam.held]);
        s = iam_mix(s, (uint32_t)proj.par[IAM_CUT[iam.held]]);
        if (iam.held < 4)
            s = iam_mix(s, iam.vstep[iam.held]);
    }
    s = iam_mix(s, iam.seq_on);
    s = iam_mix(s, iam.seq_at);
    s = iam_mix(s, iam.seq_isol); /* the title gains SOLO, and loses it */
    if (iam.seq_on) {
        for (i = 0; i < 4; i++)
            s = iam_mix(s, iam.vstep[i]); /* every row, not only the key under the finger */
        s = iam_mix(s, (uint32_t)proj.par[P_PAD_OCT]);
        s = iam_mix(s, (uint32_t)proj.par[P_BASSOCT]);
        s = iam_mix(s, (uint32_t)proj.par[P_AC_OCT]);
        for (i = 0; i < 6; i++)
            s = iam_mix(s, (uint32_t)proj.par[P_LANE_TUNE(i)]);
        s = iam_mix(s, (uint32_t)(int)iam.haze_oct); /* the title names a non-zero octave */
        {
            struct iam_seq_tr tr[15];
            int n = iam_seq_clamp(tr);
            int lv = n > 0 ? iam_seq_stored(&tr[iam.seq_at]) : -1;
            s = iam_mix(s, (uint32_t)(lv + 2)); /* the percent stays after the toast */
        }
    }
    return s;
}
/* Sixteen steps, grouped as four beats, then four bars of the phrase.
 * Pips only: putting the step into the picture would clear the whole panel. */
/* The strip under the status is the bar while nothing is held, and the pattern while a key is. */
static int iam_strip_held(void)
{
    return iam.held_drum >= 0 || (iam.held >= 0 && iam.held < 4);
}
static void iam_clock(void)
{
    uint32_t bars = en_bars, step = bars & 15u, beat = step >> 2, phrase = (bars / 16u) & 3u;
    int tip = iam.card & 1u;
    int y = tip ? 11 : 58;
    int x0 = tip ? 54 : 8;
    int px = tip ? 194 : 168;
    int i, x;
    int32_t tw, tx;
    if (!tip) {
        char b[5];
        b[0] = (char)('1' + phrase); /* which bar of the four-bar phrase */
        b[1] = ':';
        b[2] = (char)('0' + (step + 1u) / 10u);
        b[3] = (char)('0' + (step + 1u) % 10u);
        b[4] = 0;
        lcd_fill(196, 0, 44, 20, K_BG);
        cv_begin(44, FONT_S.h, K_BG);
        tw = text_w(&FONT_S, b);
        tx = 42 - tw; /* the last glyph's ink sits a little past its advance */
        if (tx < 0)
            tx = 0;
        cv_text(tx, 0, &FONT_S, b, K_ACC);
        cv_blit(196, 1);
    }
    if (!tip && iam_strip_held()) {
        iam.step_seen = (uint8_t)step;
        return; /* the sixteen steps of the key under the finger own this row */
    }
    for (i = 0; i < 16; i++) {
        uint16_t c = (uint32_t)i == step ? K_ACC : ((uint32_t)i >> 2) == beat ? K_DIM : K_LINE;
        x = x0 + i * 8 + (i >> 2) * 3;
        lcd_fill((uint32_t)x, (uint32_t)y, 6, 7, c);
    }
    for (i = 0; i < 4; i++) {
        x = px + i * 11;
        lcd_fill((uint32_t)x, (uint32_t)y, 8, 7, (uint32_t)i == phrase ? K_PAD : K_LINE);
    }
    iam.step_seen = (uint8_t)step;
}
static void iam_text(int x, int y, int w, const felucca_font_t *f, const char *s, uint16_t c, int align)
{
    if (y < 0 || w <= 0 || x >= 240 || y + (int)f->h > 240)
        return;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (w <= 0 || x >= 240)
        return;
    if (x + w > 240)
        w = 240 - x;
    draw_text_box((uint32_t)x, (uint32_t)y, (uint32_t)w, f, s, c, align);
}
static void iam_steps_draw(int y, uint16_t mask, int pending, int cursor)
{
    int i;
    uint16_t on_c = pending ? K_PAD : K_ACC;
    for (i = 0; i < 16; i++) {
        int x = 6 + i * 13 + (i >> 2) * 4;
        int on = (mask >> i) & 1;
        int cur = i == cursor;
        lcd_fill((uint32_t)x, (uint32_t)y, 10, 14, on ? on_c : ((cur || (i & 3) == 0) ? K_DIM : K_LINE));
        if (cur) { /* bars above and below the step OCT- will turn */
            lcd_fill((uint32_t)x, (uint32_t)y, 10, 2, K_TEXT);
            lcd_fill((uint32_t)x, (uint32_t)(y + 12), 10, 2, K_TEXT);
        }
    }
}
/* Graphics for the strip only. The key's name and its sound sit above it. */
static void iam_pattern_draw(int part, int drum)
{
    int pending;
    if (drum >= 0) {
        uint16_t bits;
        pending = iam.q_dpat[drum] != 0;
        /* A queued preset shows before the bar. Knob 2 does not queue one. */
        if (pending)
            bits = iam_drum_steps(drum, iam_drum_pat_aim(drum));
        else
            bits = iam_steps_live(drum, iam_drum_pat_aim(drum));
        iam_steps_draw(58, bits, pending, iam.step_i);
        return;
    }
    if (part >= 0 && part < 4)
        iam_steps_draw(58, iam.vstep[part], 0, iam.step_i);
}
static int iam_snd_aim(int part)
{
    const en_param_t *pi;
    int v;
    if (part == 3)
        return iam_haze_pick(proj.par[P_NZ_COLOR], iam.q_snd[3]);
    pi = en_param_info(IAM_SND[part]);
    v = (int)proj.par[IAM_SND[part]] + (int)iam.q_snd[part];
    if (v < pi->lo)
        v = pi->lo;
    if (v > pi->hi)
        v = pi->hi;
    return v;
}
static const char *iam_snd_name(int part)
{
    static const char *const haze[8] = {"White", "Pink", "Brown", "Blue", "Violet", "Grey", "Vinyl", "Yellow"};
    int v = iam_snd_aim(part);
    if (part == 0)
        return EN_PAD_SOUND[(unsigned)v < EN_NSOUND ? v : 0];
    if (part == 1)
        return EN_DRONE_SOUND[(unsigned)v < EN_NSOUND ? v : 0];
    if (part == 2)
        return EN_ACID_SOUND[(unsigned)v < EN_NSOUND ? v : 0];
    if (part == 3) {
        if ((v & 7) == 2 && iam.style < IAM_NSTYPE && IAM_AIR[iam.style] == 1)
            return "Wood"; /* the knock. The same colour on a chop stays Brown */
        return haze[v & 7];
    }
    return EN_KIT_NAME[(unsigned)v < EN_NKIT ? v : 0];
}
static void iam_put(char *b, int n, const char *s)
{
    int i = 0;
    if (n < 2)
        return;
    while (b[i])
        i++;
    while (s && *s && i < n - 1)
        b[i++] = *s++;
    b[i] = 0;
}
/* The percent knob 3 stores for the key under the finger, before the section's
 * gain. A named key uses its engine. Clap, the open hat, and the five above
 * perc keep their own. Nothing held is -1. */
static int iam_hold_lvl(void)
{
    int v;
    if (iam.held_drum >= 0 && !iam_main_drum(iam.held_drum))
        v = iam.held_drum < IAM_NDRUMKEY ? (int)iam.dlvl[iam.held_drum] : 0;
    else if (iam.held >= 0 && iam.held < IAM_NPART)
        v = (int)iam.lvl[iam.held];
    else
        return -1;
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    return v;
}
/* The key under the finger. A named sound uses its own name. A drum uses the drum. */
static const char *iam_hold_title(void)
{
    if (iam.held_drum >= 0)
        return iam_drum_name(iam.held_drum);
    if (iam.held_pre >= 0 && iam.held_pre < 12)
        return IAM_PRE_NAME[iam.held_pre];
    if (iam.held >= 0 && iam.held < IAM_NPART)
        return iam_part_name(iam.held);
    return "";
}
/* The sound or the kit knob 1 is aiming at, before the bar. Capitals, with the key name. */
static const char *iam_hold_sub(void)
{
    static char b[16];
    const char *s = "";
    if (iam.held_drum >= 0) {
        int kit = iam_drum_kit_aim(iam.held_drum);
        s = EN_KIT_NAME[(unsigned)kit < EN_NKIT ? kit : 0];
    } else if (iam.held >= 0 && iam.held < 4)
        s = iam_snd_name(iam.held);
    caps(b, s, (int)sizeof b);
    return b;
}
/* One slot. On is a filled cell. Waiting, or the key under the finger, uses the held colour. */
static void iam_cell(int x, int y, int w, int h, const char *s, uint16_t fg)
{
    uint16_t bg = fg == K_DIM ? K_BG : K_PANEL;
    int32_t tw, tx;
    int ty;
    if (w < 8 || h < (int)FONT_XS.h)
        return;
    lcd_fill((uint32_t)x, (uint32_t)y, (uint32_t)w, (uint32_t)h, K_LINE);
    lcd_fill((uint32_t)(x + 1), (uint32_t)(y + 1), (uint32_t)(w - 2), (uint32_t)(h - 2), bg);
    tw = text_w(&FONT_XS, s);
    tx = ((int32_t)(w - 4) - tw) / 2;
    if (tx < 0)
        tx = 0;
    ty = (h - (int)FONT_XS.h) / 2;
    cv_begin((uint32_t)(w - 4), FONT_XS.h, bg);
    cv_text(tx, 0, &FONT_XS, s, fg);
    cv_blit((uint32_t)(x + 2), (uint32_t)(y + ty));
    if (fg != K_DIM)
        lcd_fill((uint32_t)(x + 1), (uint32_t)(y + 1), 2, (uint32_t)(h - 2), fg);
}
static uint16_t iam_synth_col(int i)
{
    if (i < 4) {
        if (iam.held == i && iam.held_pre < 0)
            return K_PAD;
        if (iam.q_snd[i] || iam.q_pat[i])
            return K_PAD;
        return (iam.mask >> i & 1) ? K_ACC : K_DIM;
    }
    {
        int pi = i - 4;
        uint16_t bit = (uint16_t)(1u << pi);
        if (iam.held_pre == pi || (iam.q_lay & bit))
            return K_PAD;
        if (iam.lay & bit)
            return K_ACC;
        return K_DIM;
    }
}
static const char *iam_synth_label(int i)
{
    if (i < 4)
        return iam_part_name(i);
    return IAM_PRE_NAME[i - 4];
}
static uint16_t iam_drum_col(int d)
{
    if (iam.held_drum == d || iam.q_dpat[d] || iam.q_dkit[d])
        return K_PAD;
    return iam_drum_on(d) ? K_ACC : K_DIM;
}
/* One row of slots. Widths already add up to the panel, and the long names get the wider cells. */
static void iam_cells(int y, int n, int base, const int *w, int drum)
{
    int i, x = 0;
    for (i = 0; i < n; i++) {
        const char *s = drum ? iam_drum_row(base + i) : iam_synth_label(base + i);
        uint16_t c = drum ? iam_drum_col(base + i) : iam_synth_col(base + i);
        iam_cell(x, y, w[i], 22, s, c);
        x += w[i];
    }
}
/* A bar after keys 3, 8 and 10, counted from 1 across the eleven drums. */
static int iam_drum_bar(int key)
{
    return key == 3 || key == 8 || key == 10;
}
static void iam_drum_cells(int y, int n, int base, const int *w)
{
    int i, x = 0;
    for (i = 0; i < n; i++) {
        int key = base + i + 1;
        iam_cell(x, y, w[i], 22, iam_drum_row(base + i), iam_drum_col(base + i));
        x += w[i];
        if (iam_drum_bar(key)) {
            lcd_fill((uint32_t)x, (uint32_t)y, IAM_DRUM_BAR, 22, K_TEXT);
            x += IAM_DRUM_BAR;
        }
    }
}
/* Eleven black keys, then the sixteen white keys. Drums sit above the synths
 * as five, then six. The two blocks keep the same height they had, so the
 * clock above and the bottom edge stay where they were. */
static void iam_grid(void)
{
    static const int synth_w[4] = {60, 60, 60, 60};
    int row;
    iam_drum_cells(76, 5, 0, IAM_DROW1_W);
    iam_drum_cells(102, 6, 5, IAM_DROW2_W);
    for (row = 0; row < 4; row++)
        iam_cells(134 + row * 24, 4, row * 4, synth_w, 0);
}
static void iam_status(int toast)
{
    char line[28], tmp[8], act[28];
    const char *st;
    char *d = line;
    int naming;
    st = IAM_STYLE_NAME[iam.style < IAM_NSTYPE ? iam.style : 0];
    while (*st && d - line < 16)
        *d++ = *st++;
    *d++ = ' ';
    st = EN_NOTE_NAME[proj.par[P_ROOT] & 11];
    while (*st && d - line < 22)
        *d++ = *st++;
    *d = 0;
    cv_begin(240, 56, K_BG);
    cv_text(4, 1, &FONT_S, line, K_TEXT);
    itoa_u((uint32_t)proj.par[P_TEMPO], tmp);
    {
        int32_t tw = text_w(&FONT_S, tmp);
        int32_t tx = 188 - tw; /* the bar counter owns the right edge, from 196 */
        if (tx < 96)
            tx = 96;
        cv_text(tx, 1, &FONT_S, tmp, K_TEXT);
    }
    /* A held key takes the section row and the action line: the key, then knob 1.
     * The stored level stays on the right of that row. A toast still owns the
     * action line, and the section comes back with it. */
    naming = (iam.held >= 0 || iam.held_drum >= 0) && !toast && !iam.q_find;
    if (naming)
        cv_text(4, 20, &FONT_S, iam_hold_title(), K_TEXT);
    else {
        d = line;
        st = !en_playing ? "STOPPED" : iam.auto_on ? "AUTO" : "MANUAL";
        while (*st && d - line < 12)
            *d++ = *st++;
        *d++ = ' ';
        st = IAM_SEC_NAME[iam.sec < IAM_NSEC ? iam.sec : 0];
        while (*st && d - line < 24)
            *d++ = *st++;
        *d = 0;
        cv_text(4, 20, &FONT_S, line, iam.auto_on ? K_PAD : K_TEXT);
    }
    if (naming) {
        char pct[8], num[8];
        const char *p;
        int i = 0, v = iam_hold_lvl();
        int32_t tw, tx;
        if (v < 0)
            v = 0;
        itoa_u((uint32_t)v, num);
        p = num;
        while (*p && i < 5)
            pct[i++] = *p++;
        pct[i++] = '%';
        pct[i] = 0;
        tw = text_w(&FONT_S, pct);
        tx = 236 - tw;
        if (tx < 128)
            tx = 128;
        cv_text(tx, 20, &FONT_S, pct, K_ACC);
    }
    act[0] = 0;
    if (toast && ui.msg[0][0]) {
        iam_put(act, 28, ui.msg[0]);
        if (ui.msg[1][0]) {
            iam_put(act, 28, " ");
            iam_put(act, 28, ui.msg[1]);
        }
    } else if (iam.q_find) {
        iam_put(act, 28, "FIND ");
        iam_put(act, 28, iam_find_place(iam.find_kind));
    } else if (naming)
        iam_put(act, 28, iam_hold_sub());
    else if (en_frozen)
        iam_put(act, 28, "FREEZE");
    if (act[0])
        cv_text(4, 40, &FONT_XS, act, (toast || iam.q_find) ? K_ACC : K_TEXT);
    cv_blit(0, 0);
}
/* Every track that is on, one row. The step is not in the picture: a full
 * clear on each sixteenth is the flicker. The cells and the number move alone. */
static int iam_seq_cx(int step)
{
    return 68 + step * 10 + (step >> 2) * 3; /* 10px cells, a gap of 3 between beats */
}
static int iam_seq_top(int n)
{
    int top;
    if (n <= 12)
        return 0;
    top = (int)iam.seq_at - 4;
    if (top < 0)
        top = 0;
    if (top > n - 12)
        top = n - 12;
    return top;
}
static void iam_seq_title(const char *name)
{
    struct iam_seq_tr t[15];
    int n, v = 0, lv = -1;
    int32_t x;
    char oct[4], pct[8], num[8];
    cv_begin(190, FONT_S.h, K_BG);
    x = cv_text(2, 0, &FONT_S, "SEQ", K_ACC);
    if (name && name[0])
        x = cv_text(x + 6, 0, &FONT_S, name, K_TEXT);
    n = iam_seq_clamp(t);
    if (n > 0)
        lv = iam_seq_stored(&t[iam.seq_at]);
    if (lv >= 0) {
        int i = 0;
        const char *p;
        itoa_u((uint32_t)lv, num);
        p = num;
        while (*p && i < 5)
            pct[i++] = *p++;
        pct[i++] = '%';
        pct[i] = 0;
        if (x + 6 + text_w(&FONT_S, pct) < 188)
            x = cv_text(x + 6, 0, &FONT_S, pct, K_ACC); /* the level wins over the octave */
    }
    if (n > 0 && iam_seq_oct_of(&t[iam.seq_at], &v) && v) {
        iam_oct_text(oct, v); /* zero stays the title the screen already had */
        if (x + 6 + text_w(&FONT_S, oct) < 188)
            x = cv_text(x + 6, 0, &FONT_S, oct, K_ACC);
    }
    if (iam.seq_isol && x + 6 + text_w(&FONT_S, "SOLO") < 188)
        cv_text(x + 6, 0, &FONT_S, "SOLO", K_ACC);
    cv_blit(0, 1);
}
static void iam_seq_num(int n)
{
    char b[3];
    int32_t tw, tx;
    b[0] = (char)('0' + n / 10);
    b[1] = (char)('0' + n % 10);
    b[2] = 0;
    lcd_fill(196, 0, 44, 20, K_BG);
    cv_begin(44, FONT_S.h, K_BG);
    tw = text_w(&FONT_S, b);
    tx = 42 - tw;
    if (tx < 0)
        tx = 0;
    cv_text(tx, 0, &FONT_S, b, K_ACC);
    cv_blit(196, 1);
}
static void iam_seq_name_at(int y, const char *nm, int sel)
{
    int ny = y + (18 - (int)FONT_XS.h) / 2;
    if (ny < y)
        ny = y;
    if (sel)
        lcd_fill(0, (uint32_t)y, 3, 18, K_ACC);
    cv_begin(58, FONT_XS.h, K_BG);
    cv_text(0, 0, &FONT_XS, nm, sel ? K_ACC : K_TEXT);
    cv_blit(6, (uint32_t)ny);
}
static void iam_seq_cells_at(int y, uint16_t bits, int sel, int step)
{
    int i, cy = y + 3;
    for (i = 0; i < 16; i++) {
        int x = iam_seq_cx(i);
        int on = (bits >> i) & 1;
        uint16_t c = on ? (sel ? K_ACC : K_PAD) : K_LINE;
        lcd_fill((uint32_t)x, (uint32_t)cy, 10, 12, c);
        if (i == step)
            lcd_fill((uint32_t)x, (uint32_t)cy, 10, 2, K_TEXT);
    }
}
static void iam_seq_paint(int cells_only)
{
    struct iam_seq_tr t[15];
    int n = iam_seq_clamp(t);
    int step = (int)(en_bars & 15u);
    int top, vis, i;
    if (!cells_only)
        iam_seq_title(n > 0 ? iam_seq_name(&t[iam.seq_at]) : "NO TRACKS");
    if (n <= 0) {
        iam.step_seen = (uint8_t)step;
        return;
    }
    iam_seq_num(step + 1);
    top = iam_seq_top(n);
    vis = n - top;
    if (vis > 12)
        vis = 12;
    for (i = 0; i < vis; i++) {
        int y = 22 + i * 18;
        int sel = top + i == (int)iam.seq_at;
        if (!cells_only)
            iam_seq_name_at(y, iam_seq_name(&t[top + i]), sel);
        iam_seq_cells_at(y, iam_seq_bits(&t[top + i]), sel, step);
    }
    iam.step_seen = (uint8_t)step;
}
static void iam_draw(void)
{
    int i, toast = plat_ms() < ui.msg_until;
    lcd_fill(0, 0, 240, 240, K_BG);
    if (iam.held < 0 && (iam.card & 1u)) {
        int tip = (int)(iam.card / 2u);
        iam_line(0, 6, 48, "IAMM-1", K_ACC); /* the clock sits on the rest of this row */
        iam_clock();
        if (tip < 0 || tip >= IAM_NTIP)
            tip = 0;
        for (i = 0; i < IAM_TIP_LINES; i++) {
            uint16_t c = i == 0 ? K_PAD : i == 1 ? K_DIM : K_TEXT;
            if (!IAM_TIP[tip][i][0])
                continue;
            iam_line(0, 32 + i * 24, 240, IAM_TIP[tip][i], c);
        }
        iam_line(0, 214, 240, tip + 1 >= IAM_NTIP ? "SEL: THE SONG" : "SEL: NEXT TIP", K_DIM);
        iam.dirty = 0;
        return;
    }
    if (iam.seq_on) {
        iam_seq_paint(0);
        iam.dirty = 0;
        return;
    }
    iam_status(toast);
    iam_grid();
    if (iam.held >= 0 || iam.held_drum >= 0)
        iam_pattern_draw(iam.held, iam.held_drum);
    iam_clock();
    iam.dirty = 0;
}
static void iam_frame(void)
{
    uint32_t btn = plat_buttons(), prev, down, up, i;
    if (!iam.primed) {
        int role;
        iam.primed = 1;
        iam.btn = btn;
        iam.keys = plat_keys();
        for (i = 0; i < NKEYS; i++)
            iam.key_t[i] = plat_ms() - 1000u; /* a key held across boot is not a tap */
        for (role = 0; role < NE; role++)
            (void)plat_enc(role);
    }
    prev = iam.btn;
    down = btn & ~prev;
    up = prev & ~btn;
    iam.btn = btn;
    if ((down >> B_SEL) & 1u) {
        if (iam.seq_on)
            iam.sel_arm = 0; /* the release solos this row. It does not open a tip */
        else
            iam.sel_arm = iam.held < 0; /* a key already down means this SEL is a solo */
    }
    for (i = 0; i < NB; i++)
        if ((down >> i) & 1u)
            iam_press((int)i);
    if (!iam.on)
        return; /* GLO handed the panel to the deep editor */
    iam_keys(plat_keys(), btn); /* a key under SEL cancels the tip */
    if ((up >> B_SEL) & 1u) {
        if (iam.seq_on)
            iam_seq_isol_flip(); /* one release solos. The next brings the song back */
        else if (iam.sel_arm)
            iam_tip();
        iam.sel_arm = 0;
    }
    if ((down >> B_FX) & 1u) {
        iam.fx_t = plat_ms();
        iam.fx_twist = 0;
    }
    iam_knobs(btn); /* a twist while FX is down is ignored, and the release is not a mutate */
    if ((up >> B_FX) & 1u && !iam.fx_twist && iam.fx_t && plat_ms() - iam.fx_t < IAM_CLICK_MS) {
        if (iam.card & 1u)
            iam.card = 0; /* the click comes back to the song, then mutates */
        if (iam.held_key >= 0 && (iam.held >= 0 || iam.held_drum >= 0))
            iam_mutate_key(); /* the key under the finger. Nothing held still walks the song */
        else
            iam_mutate(1);
        iam.dirty = 1;
    }
    iam_bars();
    iam_loop_tick(); /* a loop brought out of GLO keeps its sixteenths here */
    { /* the count the host reads, including a frame that does not repaint */
        struct iam_seq_tr tr[15];
        (void)iam_seq_clamp(tr);
    }
    iam_leds(btn);
    { /* one paint when the picture changes. A full clear every frame is the flicker */
        uint32_t pic = iam_picture();
        if (!iam.painted || pic != iam.picture) {
            iam.painted = 1;
            iam.picture = pic;
            iam_draw();
        }
    }
    if (iam.painted && (uint8_t)(en_bars & 15u) != iam.step_seen) {
        if (iam.seq_on && !(iam.held < 0 && (iam.card & 1u)))
            iam_seq_paint(1); /* the cells and the step number. The rows stay put */
        else
            iam_clock(); /* the step moved. The rest of the panel stays put */
    }
}
/* A key on a silent voice is raised while it is held, then put back. Not a saved edit. */
static const int IAM_VOICE_LVL[3] = {P_PAD, P_BASS, P_ACID};
static int16_t iam_play_was[3];
static uint8_t iam_play_lift;
static void iam_editor_lift_off(void)
{
    int vc, i;
    for (vc = 0; vc < 3; vc++) {
        int p, still;
        if (!(iam_play_lift & (1u << vc)))
            continue;
        still = 0;
        for (i = 0; i < NKEYS; i++)
            if (ui.key_voice[i] == (uint8_t)(vc + 1))
                still = 1;
        if (still || iam_loop_voice(vc))
            continue; /* a loop still playing this voice keeps the lift */
        p = IAM_VOICE_LVL[vc];
        iam_play_lift = (uint8_t)(iam_play_lift & ~(1u << vc));
        if (proj.par[p] != 52) /* a knob moved it while the key was down: that value stays */
            continue;
        proj.par[p] = iam_play_was[vc];
        en_set(p, iam_play_was[vc]);
    }
}
/* A voice sitting out is brought up while a key holds it, then put back.
 * One already in the song is left alone: raising it makes the part jump. */
static void iam_editor_lift_on(int vc)
{
    int p, v;
    if (vc < 0 || vc > 2 || (iam_play_lift & (1u << vc)))
        return;
    p = IAM_VOICE_LVL[vc];
    v = proj.par[p];
    if (v >= 20)
        return;
    iam_play_was[vc] = (int16_t)v;
    iam_play_lift = (uint8_t)(iam_play_lift | (1u << vc));
    proj.par[p] = 52;
    en_set(p, 52);
}
/* GLO jam: white keys play the scale on the track's lead voice. The pad plays
 * the chord. The drone and the acid play its root. Black keys hold an effect.
 * A shared pad pitch stays until the last key lets it go. No frand on this path. */
static uint8_t iam_jam_note[NKEYS][4];
static uint8_t iam_jam_n[NKEYS];
static uint8_t iam_jam_ref[128];
static uint8_t iam_jam_on[NKEYS];
static uint8_t iam_jam_fxkey[NKEYS]; /* the effect's FX_* + 1, while that black key is down */
static uint16_t iam_jam_ord[NKEYS];    /* press order, so the newest key is the one just taken */
static uint16_t iam_jam_seq;
static const uint8_t IAM_JAM_FX[11] = {FX_RPT4, FX_RPT8, FX_RPT16, FX_REV, FX_GATE16, FX_THROW,
                                       FX_FLANGER, FX_FLANGE, FX_GATE8, FX_LP, FX_HP};
static int iam_jam_still(int key, int midi, int vc)
{
    int i, t;
    for (i = 0; i < NKEYS; i++) {
        if (i == key || !iam_jam_on[i] || ui.key_voice[i] != (uint8_t)(vc + 1))
            continue;
        for (t = 0; t < iam_jam_n[i]; t++)
            if (iam_jam_note[i][t] == (uint8_t)midi)
                return 1;
    }
    return 0;
}
/* GLO's REC. White keys only: a press lands on the sixteenth it was played.
 * The buffer keeps four bars. Older playing falls out, and a new pass does
 * not sit on top of the old one. The next press of REC loops what is left.
 * One press more lets it go. Leaving GLO keeps that loop until REC again.
 * An open take does not come out. Black keys stay live effects. */
#define IAM_LOOP_EV 128
#define IAM_LOOP_KEEP 64u
struct iam_loop_note {
    uint32_t bar;
    uint8_t note, voice, len; /* len 0: still held. 255: held for the whole window */
};
static struct iam_loop_note iam_loop_ev[IAM_LOOP_EV];
static int iam_loop_n;
static uint8_t iam_loop_rec, iam_loop_play, iam_loop_len;
static int iam_loop_playing(void) { return iam_loop_play; }
static int iam_loop_recording(void) { return iam_loop_rec; }
static uint32_t iam_loop_org, iam_loop_last;
static uint8_t iam_loop_hold[3][16];
static int iam_loop_held(int vc, int note)
{
    if ((unsigned)vc > 2u || (unsigned)note > 127u)
        return 0;
    return (iam_loop_hold[vc][note >> 3] >> (note & 7)) & 1;
}
static int iam_loop_voice(int vc)
{
    int i;
    if (!iam_loop_play || vc < 0 || vc > 2)
        return 0;
    for (i = 0; i < iam_loop_n; i++)
        if ((int)iam_loop_ev[i].voice == vc)
            return 1;
    return 0;
}
static int iam_loop_busy(void)
{
    int vc, n;
    if (!iam_loop_play)
        return 0;
    for (vc = 0; vc < 3; vc++)
        for (n = 0; n < 16; n++)
            if (iam_loop_hold[vc][n])
                return 1;
    return 0;
}
static void iam_loop_mark(int vc, int note, int on)
{
    uint8_t *p = &iam_loop_hold[vc][note >> 3], b = (uint8_t)(1u << (note & 7));
    *p = on ? (uint8_t)(*p | b) : (uint8_t)(*p & (uint8_t)~b);
}
static int iam_loop_finger(int vc, int note)
{
    int i, t;
    if (vc < 0 || vc > 2 || note < 0 || note > 127)
        return 0;
    if (vc == EN_V_PAD)
        return iam_jam_ref[note] > 0;
    for (i = 0; i < NKEYS; i++) {
        if (!iam_jam_on[i] || (int)ui.key_voice[i] != vc + 1)
            continue;
        for (t = 0; t < iam_jam_n[i]; t++)
            if (iam_jam_note[i][t] == (uint8_t)note)
                return 1;
    }
    return 0;
}
static void iam_loop_sound(int vc, int note, int on)
{
    int was;
    if (vc < 0 || vc > 2 || note < 0 || note > 127)
        return;
    was = iam_loop_held(vc, note);
    if (on) {
        iam_loop_mark(vc, note, 1);
        if (iam_loop_finger(vc, note))
            return; /* the key already sounds. The loop joins it */
        if (was)
            en_note(vc, note, 0); /* the same pitch again: a new attack, not a second voice */
        en_note(vc, note, 1);
    } else if (was) {
        iam_loop_mark(vc, note, 0);
        if (!iam_loop_finger(vc, note))
            en_note(vc, note, 0);
    }
}
static void iam_loop_silence(void)
{
    int vc, n;
    for (vc = 0; vc < 3; vc++)
        for (n = 0; n < 128; n++)
            if (iam_loop_held(vc, n))
                iam_loop_sound(vc, n, 0);
}
static void iam_loop_drop_oldest(void)
{
    int i;
    if (iam_loop_n < 1)
        return;
    for (i = 1; i < iam_loop_n; i++)
        iam_loop_ev[i - 1] = iam_loop_ev[i];
    iam_loop_n--;
}
static void iam_loop_add(uint32_t bar, int voice, int note, int down)
{
    int i;
    if (!iam_loop_rec || voice < 0 || voice > 2 || note < 0 || note > 127)
        return;
    if (!down) {
        for (i = iam_loop_n - 1; i >= 0; i--)
            if (iam_loop_ev[i].voice == (uint8_t)voice && iam_loop_ev[i].note == (uint8_t)note &&
                !iam_loop_ev[i].len) {
                uint32_t d = bar > iam_loop_ev[i].bar ? bar - iam_loop_ev[i].bar : 1u;
                if (d > IAM_LOOP_KEEP)
                    d = IAM_LOOP_KEEP;
                iam_loop_ev[i].len = (uint8_t)d;
                return;
            }
        return; /* a key that was already down when REC began has no start */
    }
    while (iam_loop_n >= IAM_LOOP_EV)
        iam_loop_drop_oldest();
    iam_loop_ev[iam_loop_n].bar = bar;
    iam_loop_ev[iam_loop_n].note = (uint8_t)note;
    iam_loop_ev[iam_loop_n].voice = (uint8_t)voice;
    iam_loop_ev[iam_loop_n].len = 0;
    iam_loop_n++;
}
static void iam_loop_forget(uint32_t now)
{
    int i, w = 0;
    for (i = 0; i < iam_loop_n; i++) {
        uint32_t b = iam_loop_ev[i].bar;
        uint32_t e = iam_loop_ev[i].len ? b + iam_loop_ev[i].len : now + 1u;
        if (e <= now && now - e >= IAM_LOOP_KEEP)
            continue;
        iam_loop_ev[w++] = iam_loop_ev[i];
    }
    iam_loop_n = w;
}
static void iam_loop_close(uint32_t now)
{
    int i;
    for (i = 0; i < iam_loop_n; i++) {
        uint32_t d;
        if (iam_loop_ev[i].len)
            continue;
        if (now > iam_loop_ev[i].bar && now - iam_loop_ev[i].bar >= IAM_LOOP_KEEP)
            iam_loop_ev[i].len = 255; /* still down, and older than the window */
        else {
            d = now > iam_loop_ev[i].bar ? now - iam_loop_ev[i].bar : 1u;
            if (d < 1u)
                d = 1u;
            iam_loop_ev[i].len = (uint8_t)d;
        }
    }
}
static int iam_loop_covers(uint32_t pos, uint32_t start, uint32_t dur, uint32_t n)
{
    uint32_t d;
    if (!n || !dur)
        return 0;
    if (dur >= n)
        return 1;
    d = (pos + n - (start % n)) % n;
    return d < dur;
}
static void iam_loop_fire(uint32_t absbar)
{
    uint32_t pos, i, n = iam_loop_len;
    if (!n || absbar < iam_loop_org)
        return;
    pos = (absbar - iam_loop_org) % n;
    for (i = 0; i < (uint32_t)iam_loop_n; i++) {
        uint32_t dur = iam_loop_ev[i].len, off;
        if (dur >= n)
            continue;
        off = (iam_loop_ev[i].bar + dur) % n;
        if (off == pos)
            iam_loop_sound(iam_loop_ev[i].voice, iam_loop_ev[i].note, 0);
    }
    for (i = 0; i < (uint32_t)iam_loop_n; i++)
        if (iam_loop_ev[i].bar == pos)
            iam_loop_sound(iam_loop_ev[i].voice, iam_loop_ev[i].note, 1);
}
static void iam_loop_catch(uint32_t now)
{
    uint32_t pos, i, n = iam_loop_len;
    if (!n || now < iam_loop_org)
        return;
    pos = (now - iam_loop_org) % n;
    for (i = 0; i < (uint32_t)iam_loop_n; i++)
        if (iam_loop_covers(pos, iam_loop_ev[i].bar, iam_loop_ev[i].len, n))
            iam_loop_sound(iam_loop_ev[i].voice, iam_loop_ev[i].note, 1);
}
static const char *iam_loop_name(uint32_t len)
{
    if (len <= 16u)
        return "ONE BAR";
    if (len <= 32u)
        return "TWO BARS";
    if (len <= 48u)
        return "THREE BARS";
    return "FOUR BARS";
}
static void iam_loop_commit(void)
{
    uint32_t now = en_bars, take, len, start, i, w = 0;
    if (now < iam_loop_org)
        now = iam_loop_org;
    take = now - iam_loop_org;
    if (take < 1u)
        take = 1u;
    iam_loop_close(now);
    len = (take + 15u) & ~15u;
    if (len > IAM_LOOP_KEEP)
        len = IAM_LOOP_KEEP;
    start = take >= len ? now - len + 1u : iam_loop_org;
    for (i = 0; i < (uint32_t)iam_loop_n; i++) {
        uint32_t b = iam_loop_ev[i].bar, dur = iam_loop_ev[i].len, e, pos;
        uint8_t note = iam_loop_ev[i].note, voice = iam_loop_ev[i].voice;
        if (dur == 255u) {
            pos = 0;
            dur = len;
        } else {
            if (!dur)
                dur = 1u;
            e = b + dur;
            if (e <= start || b > now)
                continue;
            if (b < start) {
                dur -= start - b;
                b = start;
            }
            pos = b - start;
            if (pos >= len)
                continue;
            if (dur > len)
                dur = len;
            if (dur < 1u)
                dur = 1u;
        }
        iam_loop_ev[w].bar = pos;
        iam_loop_ev[w].note = note;
        iam_loop_ev[w].voice = voice;
        iam_loop_ev[w].len = (uint8_t)dur;
        w++;
    }
    iam_loop_n = (int)w;
    iam_loop_len = (uint8_t)len;
    iam_loop_org = start;
    iam_loop_last = now;
    iam_loop_play = w ? 1 : 0;
    if (!w) {
        iam_loop_len = 0;
        say("REC", "EMPTY");
        return;
    }
    iam_loop_catch(now);
    say("LOOP", iam_loop_name(len));
}
static void iam_loop_halt(void)
{
    iam_loop_rec = 0;
    iam_loop_play = 0;
    iam_loop_silence();
    iam_loop_n = 0;
    iam_loop_len = 0;
}
static void iam_loop_stop(void)
{
    iam_loop_halt();
    iam_editor_lift_off(); /* a voice raised only for the jam sits back down */
    say("LOOP", "OFF");
}
static void iam_loop_toggle(void)
{
    int i, t, vc;
    if (iam_loop_rec) {
        iam_loop_rec = 0;
        iam_loop_commit();
        return;
    }
    if (iam_loop_play) {
        iam_loop_stop();
        return;
    }
    iam_loop_silence();
    iam_loop_n = 0;
    iam_loop_len = 0;
    iam_loop_rec = 1;
    iam_loop_org = en_bars & ~15u;
    iam_loop_last = en_bars;
    for (i = 0; i < NKEYS; i++) { /* a chord already under the fingers is the start of the take */
        if (!iam_jam_on[i])
            continue;
        vc = (int)ui.key_voice[i] - 1;
        if (vc < 0 || vc > 2)
            continue;
        for (t = 0; t < iam_jam_n[i]; t++)
            iam_loop_add(en_bars, vc, iam_jam_note[i][t], 1);
    }
    say("REC", "FOUR BARS");
}
static void iam_loop_tick(void)
{
    uint32_t now = en_bars, b;
    if (iam_loop_rec)
        iam_loop_forget(now);
    if (!iam_loop_play || !iam_loop_len)
        return;
    if (now == iam_loop_last)
        return;
    if (now < iam_loop_last || now - iam_loop_last > 4u)
        iam_loop_last = now - 1u;
    for (b = iam_loop_last + 1u; b <= now; b++)
        iam_loop_fire(b);
    iam_loop_last = now;
}
static int iam_loop_button(void)
{
    if (iam_loop_rec)
        return 1;
    return iam_loop_play && ((ui.frame / 8u) & 1u);
}
static int iam_loop_sounding(void)
{
    uint32_t now = en_bars, pos, i, n = iam_loop_len;
    uint8_t seen[3][16];
    int c = 0, vc, note;
    if (!iam_loop_play || !n || now < iam_loop_org)
        return 0;
    pos = (now - iam_loop_org) % n;
    for (vc = 0; vc < 3; vc++)
        for (note = 0; note < 16; note++)
            seen[vc][note] = 0;
    for (i = 0; i < (uint32_t)iam_loop_n; i++) {
        vc = iam_loop_ev[i].voice;
        note = iam_loop_ev[i].note;
        if ((unsigned)vc > 2u || (unsigned)note > 127u)
            continue;
        if (!iam_loop_covers(pos, iam_loop_ev[i].bar, iam_loop_ev[i].len, n))
            continue;
        if ((seen[vc][note >> 3] >> (note & 7)) & 1)
            continue;
        seen[vc][note >> 3] |= (uint8_t)(1u << (note & 7));
        c++;
    }
    return c;
}
static int iam_editor_key_off(int key)
{
    int t, n, vc;
    if (key < 0 || key >= NKEYS || !iam_jam_on[key])
        return 0;
    vc = (int)ui.key_voice[key] - 1;
    iam_jam_on[key] = 0;
    iam_jam_fxkey[key] = 0;
    n = iam_jam_n[key];
    iam_jam_n[key] = 0;
    if (vc >= 0 && vc <= 2) {
        for (t = 0; t < n; t++) {
            int midi = iam_jam_note[key][t];
            iam_jam_note[key][t] = 0;
            if ((unsigned)midi >= 128u)
                continue;
            if (vc == EN_V_PAD) {
                if (iam_jam_ref[midi]) {
                    iam_jam_ref[midi]--;
                    iam_loop_add(en_bars, EN_V_PAD, midi, 0);
                    if (!iam_jam_ref[midi] && !iam_loop_held(EN_V_PAD, midi))
                        en_note(EN_V_PAD, midi, 0);
                }
            } else {
                iam_loop_add(en_bars, vc, midi, 0);
                if (!iam_jam_still(key, midi, vc) && !iam_loop_held(vc, midi))
                    en_note(vc, midi, 0);
            }
        }
    }
    ui.key_voice[key] = 0;
    return 1;
}
static uint32_t iam_jam_mask(void)
{
    uint32_t m = 0;
    int i;
    for (i = 0; i < NKEYS; i++)
        if (iam_jam_fxkey[i])
            m |= 1u << (iam_jam_fxkey[i] - 1);
    return m;
}
static void iam_editor_notes_off(void)
{
    int i;
    for (i = 0; i < NKEYS; i++) {
        if (iam_editor_key_off(i))
            continue;
        if (!ui.key_voice[i])
            continue;
        en_note((int)ui.key_voice[i] - 1, 53 + i, 0);
        ui.key_voice[i] = 0;
    }
    for (i = 0; i < 128; i++) /* a pitch still counted: let it go with the editor */
        if (iam_jam_ref[i]) {
            iam_jam_ref[i] = 0;
            if (!iam_loop_held(EN_V_PAD, i))
                en_note(EN_V_PAD, i, 0); /* a loop note stays while the finger leaves */
        }
    iam_editor_lift_off();
}
/* A key that reads up cannot keep a chord, an effect, or the line under it.
 * The loop is the one case that sounds with no finger down. */
static void iam_editor_unstick(uint32_t keys)
{
    int i, live = 0;
    if (!iam_song())
        return;
    for (i = 0; i < NKEYS; i++) {
        int vc;
        if ((keys >> i) & 1u)
            continue;
        if (iam_jam_on[i]) {
            iam_editor_key_off(i);
            continue;
        }
        if (!ui.key_voice[i])
            continue;
        vc = (int)ui.key_voice[i] - 1;
        if (vc >= 0 && vc <= 2)
            en_note(vc, 53 + i, 0);
        ui.key_voice[i] = 0;
    }
    for (i = 0; i < NKEYS; i++) {
        uint8_t v = ui.key_voice[i];
        if (v >= 1 && v <= 3)
            live = 1;
    }
    if (!live && en_held && !iam_loop_play && !iam_loop_rec)
        en_hands_off();
}
/* The page of a voice plays that voice. Any other page plays whichever of the
 * pad, the drone and the acid the track has loudest. The pad wins a tie, so a
 * balanced song stays a chord. */
static int iam_jam_voice(void)
{
    int v = voice_of((int)ui.view);
    int pad, bass, acid;
    if (v >= 0)
        return v;
    pad = proj.par[P_PAD];
    bass = proj.par[P_BASS];
    acid = proj.par[P_ACID];
    if (pad < 0)
        pad = 0;
    if (bass < 0)
        bass = 0;
    if (acid < 0)
        acid = 0;
    if (acid > pad && acid >= bass)
        return EN_V_ACID;
    if (bass > pad)
        return EN_V_DRONE;
    return EN_V_PAD;
}
static void iam_jam_announce(int vc)
{
    static const char *const WHO[3] = {"PAD", "DRONE", "ACID"};
    static const int SND[3] = {P_PADTONE, P_DR_INST, P_AC_WAVE};
    static const char *const *const BANK[3] = {EN_PAD_SOUND, EN_DRONE_SOUND, EN_ACID_SOUND};
    static int said_vc = -1, said_snd = -1;
    char b[16];
    int s;
    if (vc < 0 || vc > 2)
        return;
    s = proj.par[SND[vc]];
    if ((unsigned)s >= EN_NSOUND)
        s = 0;
    if (vc == said_vc && s == said_snd)
        return;
    said_vc = vc;
    said_snd = s;
    caps(b, BANK[vc][s], (int)sizeof b);
    say(WHO[vc], b);
}
/* Degree w of the scale, then the next octave. The pitch class stays in the
 * octave that starts at C3, as the keys always have. */
static int iam_scale_note(int white, int root, const int *steps, int nsteps)
{
    int deg = white % nsteps;
    int oct = white / nsteps;
    int pc = (root + steps[deg]) % 12;
    if (pc < 0)
        pc += 12;
    return 48 + pc + 12 * oct;
}
/* The shift the voice applies before it folds a hand note into its window. */
static int iam_jam_reg(int vc)
{
    int song = proj.par[P_OCT];
    if (vc == EN_V_DRONE)
        return -24 + 12 * (proj.par[P_BASSOCT] + song);
    if (vc == EN_V_ACID)
        return -12 + 12 * (proj.par[P_AC_OCT] + song);
    return 12 * (proj.par[P_PAD_OCT] + song);
}
/* One octave for every white key. Each note is folded on its own, so a low
 * drone used to put key 8 back on key 1. The pad's chord reaches eleven
 * above the root, and that has to fit too. Wider than the window: the
 * bottom stays in. */
static int iam_jam_lift(int vc, int root, const int *steps, int nsteps)
{
    int lo, hi, extra, reg, low, high, k;
    if (vc == EN_V_DRONE) {
        lo = 24;
        hi = 83;
        extra = 0;
    } else if (vc == EN_V_ACID) {
        lo = 28;
        hi = 96;
        extra = 0;
    } else {
        lo = 24;
        hi = 108;
        extra = 11;
    }
    reg = iam_jam_reg(vc);
    low = iam_scale_note(0, root, steps, nsteps) + reg;
    high = iam_scale_note(15, root, steps, nsteps) + reg + extra;
    k = 0;
    while (low + 12 * k < lo && k < 6)
        k++;
    while (high + 12 * k > hi && k > -6)
        k--;
    if (low + 12 * k < lo)
        while (low + 12 * k < lo && k < 6)
            k++;
    return k;
}
static int iam_jam_midi(int note)
{
    if (note < 0)
        return 0;
    if (note > 127)
        return 127;
    return note;
}
/* White key w is degree w of the scale, then the next octave.
 * The pad sings the chord. The drone and the acid sing its root, on the
 * voice the track is already moving, so the filter and the LFO come with it.
 * Every key of this press shares one octave, so the run reaches the last key. */
static void iam_editor_chord(int key, int white)
{
    int steps[12], nsteps, deg, root, kind, pcs[8], n, t, pc0, base, vc;
    root = proj.par[P_ROOT];
    if (root < 0)
        root = 0;
    nsteps = en_scale(proj.pool, steps);
    if (nsteps < 1)
        nsteps = 1;
    deg = white % nsteps;
    kind = proj.par[P_CHORD];
    if (kind < 0 || kind >= EN_NCHORD)
        kind = EN_CH_AUTO;
    n = en_chord_kind(proj.pool, root, deg, kind, pcs);
    if (n > 4)
        n = 4;
    if (n < 1)
        return;
    pc0 = (root + steps[deg]) % 12;
    if (pc0 < 0)
        pc0 += 12;
    vc = iam_jam_voice();
    base = iam_jam_midi(iam_scale_note(white, root, steps, nsteps) + 12 * iam_jam_lift(vc, root, steps, nsteps));
    iam_editor_lift_on(vc);
    iam_jam_announce(vc);
    iam_jam_ord[key] = ++iam_jam_seq; /* the arp hears this key before the ones already down */
    if (vc != EN_V_PAD) {
        iam_jam_note[key][0] = (uint8_t)base;
        iam_jam_n[key] = 1;
        en_note(vc, base, 1);
        iam_loop_add(en_bars, vc, base, 1);
        iam_jam_on[key] = 1;
        ui.key_voice[key] = (uint8_t)(vc + 1);
        return;
    }
    for (t = 0; t < n; t++) {
        int note = iam_jam_midi(base + ((pcs[t] - pc0 + 12) % 12)), seen = 0, u;
        for (u = 0; u < iam_jam_n[key]; u++)
            seen |= iam_jam_note[key][u] == (uint8_t)note;
        if (seen || iam_jam_n[key] >= 4)
            continue;
        iam_jam_note[key][iam_jam_n[key]++] = (uint8_t)note;
        if (iam_jam_ref[note] < 255)
            iam_jam_ref[note]++;
        if (iam_jam_ref[note] == 1)
            en_note(EN_V_PAD, note, 1);
        iam_loop_add(en_bars, EN_V_PAD, note, 1);
    }
    iam_jam_on[key] = 1;
    ui.key_voice[key] = (uint8_t)(EN_V_PAD + 1); /* this key only. The scale stays dark */
}
/* The song keeps playing. White keys: chords. Black keys: an effect while held. */
static void iam_editor_note(int key)
{
    int i, b = -1, w = -1;
    if (key < 0 || key >= NKEYS || iam_jam_on[key])
        return;
    for (i = 0; i < 11; i++)
        if (BLACK_KEY[i] == (uint8_t)key)
            b = i;
    if (b >= 0) {
        iam_jam_fxkey[key] = (uint8_t)(IAM_JAM_FX[b] + 1);
        iam_jam_on[key] = 1;
        ui.key_voice[key] = 0x7F; /* lit, and not a voice the hand-mode path owns */
        return;
    }
    if (ui.view == V_SEQ) /* white keys there are drum steps */
        return;
    for (i = 0; i < 16; i++)
        if (WHITE_KEY[i] == (uint8_t)key)
            w = i;
    if (w >= 0)
        iam_editor_chord(key, w);
}
/* A GLO piece turns its engines on through their levels. Named keys stay,
 * and they render on top. Five synth keys, with the song's filters, broke
 * up on the unit. The oldest named key goes off until the piece sits under
 * four. An engine the piece will sound is not an extra key. */
static void iam_piece_room(void)
{
    int eng = 0, lay = 0, s, guard, dropped = 0;
    if (project_seed < IAM_SEED_MIN)
        return;
    if (proj.par[P_PAD] > 0)
        eng++;
    if (proj.par[P_BASS] > 0)
        eng++;
    if (proj.par[P_ACID] > 0)
        eng++;
    if (proj.par[P_NOISE] > 0)
        eng++;
    for (s = 4; s < 16; s++)
        if (iam_slot_on(s))
            lay++;
    for (guard = 0; eng + lay >= IAM_CAP && lay > 0 && guard < 12; guard++) {
        int oldest = -1;
        uint32_t best = 0xFFFFFFFFu;
        for (s = 4; s < 16; s++) {
            uint32_t age;
            if (!iam_slot_on(s))
                continue;
            age = iam.born[s] ? iam.born[s] : 0u; /* no stamp is the oldest */
            if (age < best) {
                best = age;
                oldest = s;
            }
        }
        if (oldest < 0)
            break;
        iam_slot_off(oldest);
        lay--;
        dropped = 1;
    }
    if (dropped)
        iam_layers_send();
}
static int iam_song(void) { return project_seed >= IAM_SEED_MIN; }
static int iam_kvol_get(void) { return iam.kvol; }
static void iam_kvol_set(int v)
{
    if (v < 0)
        v = 0;
    if (v > 100)
        v = 100;
    iam.kvol = (uint8_t)v;
    en_keys(v);
}
/* GLO arpeggiator. The latch is iam.arp. The pattern is the white keys already
 * down: one note at a time, low to high, and the key just taken is next.
 * Off then on, because a drone or acid note that stays held only slides. */
#define IAM_ARP_N 64
struct iam_arp_note {
    uint8_t vc, note, key;
    uint16_t ord;
};
static struct iam_arp_note arp_pool[IAM_ARP_N], arp_prev[IAM_ARP_N];
static int arp_prev_n;
static uint8_t arp_was, arp_have, arp_vc, arp_note, arp_idx;
static uint32_t arp_bar;

static void iam_arp_reset(void)
{
    arp_was = 0;
    arp_have = 0;
    arp_idx = 0;
    arp_prev_n = 0;
    arp_bar = 0;
}
static int iam_arp_on(void) { return iam_song() && iam.arp; }
static void iam_arp_sort(struct iam_arp_note *p, int n)
{
    int i;
    for (i = 1; i < n; i++) {
        struct iam_arp_note k = p[i];
        int j = i;
        while (j > 0 && (p[j - 1].note > k.note || (p[j - 1].note == k.note && p[j - 1].vc > k.vc))) {
            p[j] = p[j - 1];
            j--;
        }
        p[j] = k;
    }
}
static int iam_arp_gather(struct iam_arp_note *out, int cap)
{
    int i, t, n = 0;
    for (i = 0; i < NKEYS; i++) {
        int vc, u;
        if (!iam_jam_on[i])
            continue;
        vc = (int)ui.key_voice[i] - 1;
        if (vc < EN_V_PAD || vc > EN_V_ACID)
            continue;
        for (t = 0; t < iam_jam_n[i] && t < 4; t++) {
            int note = iam_jam_note[i][t], dup = 0;
            if ((unsigned)note >= 128u)
                continue;
            for (u = 0; u < n; u++)
                dup |= out[u].vc == (uint8_t)vc && out[u].note == (uint8_t)note;
            if (dup || n >= cap)
                continue;
            out[n].vc = (uint8_t)vc;
            out[n].note = (uint8_t)note;
            out[n].key = (uint8_t)i;
            out[n].ord = iam_jam_ord[i];
            n++;
        }
    }
    iam_arp_sort(out, n);
    return n;
}
static void iam_arp_keep(const struct iam_arp_note *p, int n)
{
    int i;
    arp_prev_n = n;
    for (i = 0; i < n; i++)
        arp_prev[i] = p[i];
}
static int iam_arp_same(const struct iam_arp_note *p, int n)
{
    int i;
    if (n != arp_prev_n)
        return 0;
    for (i = 0; i < n; i++)
        if (p[i].vc != arp_prev[i].vc || p[i].note != arp_prev[i].note)
            return 0;
    return 1;
}
static int iam_arp_at(const struct iam_arp_note *p, int n, int vc, int note)
{
    int i;
    for (i = 0; i < n; i++)
        if (p[i].vc == (uint8_t)vc && p[i].note == (uint8_t)note)
            return i;
    return -1;
}
/* Lowest new pitch of the newest key. The pool is already low to high. */
static int iam_arp_fresh(const struct iam_arp_note *p, int n)
{
    int i, bi = -1;
    uint16_t top = 0;
    for (i = 0; i < n; i++) {
        int u, seen = 0;
        for (u = 0; u < arp_prev_n; u++)
            if (arp_prev[u].vc == p[i].vc && arp_prev[u].note == p[i].note)
                seen = 1;
        if (seen)
            continue;
        if (bi < 0 || p[i].ord > top) {
            top = p[i].ord;
            bi = i;
        }
    }
    return bi;
}
static void iam_arp_sound(const struct iam_arp_note *p, int n, int idx)
{
    int i;
    if (n < 1)
        return;
    if (idx < 0 || idx >= n)
        idx = 0;
    for (i = 0; i < n; i++)
        en_note(p[i].vc, p[i].note, 0);
    en_note(p[idx].vc, p[idx].note, 1);
    arp_vc = p[idx].vc;
    arp_note = p[idx].note;
    arp_idx = (uint8_t)idx;
    arp_have = 1;
}
/* The chord again. Off first: a pad note-on of a note already playing takes another voice. */
static void iam_arp_restore(const struct iam_arp_note *p, int n)
{
    int keys[NKEYS], nk = 0, i, t, u;
    uint8_t sent[IAM_ARP_N];
    for (i = 0; i < n; i++)
        en_note(p[i].vc, p[i].note, 0);
    for (i = 0; i < NKEYS; i++) {
        int vc = (int)ui.key_voice[i] - 1;
        if (iam_jam_on[i] && vc >= EN_V_PAD && vc <= EN_V_ACID)
            keys[nk++] = i;
    }
    for (i = 1; i < nk; i++) {
        int k = keys[i], j = i;
        while (j > 0 && iam_jam_ord[keys[j - 1]] > iam_jam_ord[k]) {
            keys[j] = keys[j - 1];
            j--;
        }
        keys[j] = k;
    }
    for (i = 0; i < n; i++)
        sent[i] = 0;
    for (i = 0; i < nk; i++) {
        int key = keys[i];
        int vc = (int)ui.key_voice[key] - 1;
        for (t = 0; t < iam_jam_n[key] && t < 4; t++) {
            int note = iam_jam_note[key][t];
            for (u = 0; u < n; u++) {
                if (sent[u] || p[u].vc != (uint8_t)vc || p[u].note != (uint8_t)note)
                    continue;
                en_note(vc, note, 1);
                sent[u] = 1;
                break;
            }
        }
    }
}
static void iam_arp_tick(void)
{
    int n, bar_new;
    if (!iam_song() || iam.on)
        return;
    n = iam_arp_gather(arp_pool, IAM_ARP_N);
    if (!iam.arp) {
        if (arp_was) {
            iam_arp_restore(arp_pool, n); /* keys still down: the chord, not one note */
            iam_arp_reset();
        }
        return;
    }
    if (ui.fx_menu || ui.view == V_SEQ) { /* those white keys are effects, or drum steps */
        if (!arp_was)
            arp_was = 1;
        if (!n)
            arp_have = 0;
        arp_bar = en_bars;
        iam_arp_keep(arp_pool, n);
        return;
    }
    if (!arp_was) {
        arp_was = 1;
        arp_bar = en_bars;
        arp_prev_n = 0;
        if (n)
            iam_arp_sound(arp_pool, n, 0); /* the low note, now, not on the next sixteenth */
        iam_arp_keep(arp_pool, n);
        return;
    }
    if (!n) {
        arp_have = 0;
        arp_idx = 0;
        arp_bar = en_bars;
        iam_arp_keep(arp_pool, 0);
        return;
    }
    bar_new = arp_bar != en_bars;
    if (!iam_arp_same(arp_pool, n)) {
        int neu = iam_arp_fresh(arp_pool, n);
        if (neu >= 0) {
            iam_arp_sound(arp_pool, n, neu);
        } else {
            int cur = iam_arp_at(arp_pool, n, arp_vc, arp_note);
            if (cur >= 0 && !bar_new) {
                arp_idx = (uint8_t)cur; /* a key let go. The one sounding stays */
            } else if (cur >= 0) {
                int idx = cur + 1;
                if (idx >= n)
                    idx = 0;
                iam_arp_sound(arp_pool, n, idx);
            } else {
                int idx = arp_idx;
                if (idx >= n)
                    idx = 0;
                iam_arp_sound(arp_pool, n, idx);
            }
        }
        arp_bar = en_bars;
        iam_arp_keep(arp_pool, n);
        return;
    }
    if (bar_new) {
        int idx = arp_have ? (int)arp_idx + 1 : 0;
        if (idx >= n)
            idx = 0;
        iam_arp_sound(arp_pool, n, idx); /* the same pitch still restarts */
        arp_bar = en_bars;
    }
    iam_arp_keep(arp_pool, n);
}
/* PLAY conducts, SAVE is the break, EDIT mutates, ARP arpeggiates.
 * FX is the effects page. 1: Felucca does not also run. */
static int iam_glo_perf(int b)
{
    if (b == B_PLAY) {
        iam.auto_on ^= 1;
        if (iam.auto_on && iam.sec == IAM_INTRO)
            iam.form = 0;
        say(iam.auto_on ? "AUTO" : "MANUAL", iam.auto_on ? "CONDUCTING" : "YOURS");
        iam_rescue(iam.mask); /* auto, back on in the intro, must not mute every part that is on */
        iam_apply();
        if (!en_playing)
            en_play(1); /* a song that had stopped starts. The seed stays */
        return 1;
    }
    if (b == B_SAVE) {
        iam_find_cancel();
        iam.q_scene = IAM_Q_BREAK;
        iam.q_pst = 0;
        iam.q_pump = 0;
        iam_due("BREAK");
        return 1;
    }
    if (b == B_EDIT) {
        iam_mutate(1); /* the song, now. A held white key here is a pitch, not a part */
        return 1;
    }
    if (b == B_ARP) { /* the acid stays on SELECT. A hold still opens the card */
        iam.arp ^= 1;
        say("ARP", iam.arp ? "ON" : "OFF");
        return 1;
    }
    return 0;
}
static void iam_resume(void)
{
    iam.arp = 0;
    iam_arp_reset(); /* notes_off lets the keys go. The latch does not follow the song back */
    iam_seq_shut();
    iam_step_drop();        /* HOME from the editor returns the song, not a latched step row */
    if (!iam_loop_play)
        iam_loop_halt();    /* an open take stops. A finished loop stays until REC */
    iam_editor_notes_off(); /* a key still down does not hold its note into the song */
    if (!iam_loop_busy())
        en_hands_off();     /* a note the loop is not holding lets go with the editor */
    iam_leave_break();      /* an engine breakdown ends on the next bar */
    iam.home_t = 0;
    iam.on = 1;
    iam.btn = plat_buttons(); /* a button still down is not a new press */
    iam.keys = plat_keys();
    iam.painted = 0; /* the deep editor had the panel */
    iam.dirty = 1;
    ui.help = 0;
    en_fx(0);
    en_dj(0);
    ui.dj_mask = 0;                 /* the lock stays, and the next GLO frame sends it again */
    say("IAMM-1", "THE MACHINE");
    lcd_fill(0, 0, 240, 240, K_BG);
}
/* GLO again, or HOME, while the editor has an IAM song. HYP seeds keep both buttons. */
static int iam_editor_back(int b)
{
    if (project_seed < IAM_SEED_MIN || (b != B_GLO && b != B_HOME))
        return 0;
    iam_resume();
    return 1;
}
static void iam_home_watch(void)
{
    uint32_t b;
    iam_bars(); /* the form keeps its bars under the editor */
    b = plat_buttons();
    if (!(b >> B_HOME & 1u)) {
        iam.home_t = 0;
        return;
    }
    if (!iam.home_t)
        iam.home_t = plat_ms();
    else if (plat_ms() - iam.home_t > 800u && !(ui.btn_used & (1u << B_HOME)))
        iam_resume(); /* HOME+SELECT is the tempo, and that hold stays in the editor */
}
