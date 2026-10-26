/* SPDX-License-Identifier: GPL-3.0-only */
/* One sound, dry, from the same engine the FM-1 runs (firmware/src/dsp/amb.c).
 *   hear --list                 the names, as JSON
 *   hear pad|bass|acid N        that preset, held
 *   hear air|dust N             Pink, Wood, Vinyl (the three the knob walks)
 *   hear bed N                  a noise colour the knob does not walk
 *   hear lane KIT LANE          kick, snare, clap, hat, open hat, perc
 *   hear xdrum KIT WHICH        rim, tom, cowbell, shaker, crash
 *   hear key N                  one of the twelve named keys
 * A wav (16-bit stereo, 44100) goes to stdout. A peak line goes to stderr.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amb.h"

void en_midi_out(uint32_t st, uint32_t d1, uint32_t d2)
{
    (void)st;
    (void)d1;
    (void)d2;
}

/* Pink, wood, vinyl. The same three iam_haze_pick walks. Wood is colour 2. */
static const char *const AIR_NAME[3] = {"Pink", "Wood", "Vinyl"};
static const int AIR_COLOR[3] = {1, 2, 6};
/* The other five. The song screen does not offer them. */
static const char *const BED_NAME[5] = {"White", "Blue", "Violet", "Grey", "Yellow"};
static const int BED_COLOR[5] = {0, 3, 4, 5, 7};
static const char *const XNAME[EN_XDRUM] = {"Rim", "Tom", "Cowbell", "Shaker", "Crash"};
static const char *const KEY_NAME[12] = {"Glass", "Dark", "Dub", "Stabs", "Sub", "Reese",
                                         "Hoover", "FM", "Pluck", "Trance", "Bleep", "Soft"};
static const char *const KEY_ENGINE[12] = {"Pad", "Pad", "Pad", "Pad", "Bass", "Bass",
                                           "Bass", "Bass", "Acid", "Acid", "Acid", "Acid"};
static const int KEY_PART[12] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2};

static int clamp(int v, int lo, int hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

static int argn(int argc, char **argv, int i, int def)
{
    if (i >= argc)
        return def;
    return atoi(argv[i]);
}

static void jstr(const char *s)
{
    fputc('"', stdout);
    while (*s) {
        if (*s == '"' || *s == '\\')
            fputc('\\', stdout);
        fputc(*s++, stdout);
    }
    fputc('"', stdout);
}

static void list_names(const char *key, const char *const *n, int count)
{
    int i;
    printf("  \"%s\": [", key);
    for (i = 0; i < count; i++) {
        if (i)
            fputc(',', stdout);
        jstr(n[i]);
    }
    fputs("]", stdout);
}

static void list_out(void)
{
    int i;
    fputs("{\n", stdout);
    list_names("pad", EN_PAD_SOUND, EN_NPAD_SND);
    fputs(",\n", stdout);
    list_names("bass", EN_DRONE_SOUND, EN_NBASS_SND);
    fputs(",\n", stdout);
    list_names("acid", EN_ACID_SOUND, EN_NACID_SND);
    fputs(",\n", stdout);
    fputs("  \"air\": [", stdout);
    for (i = 0; i < 3; i++) {
        if (i)
            fputc(',', stdout);
        fputs("{\"kind\":\"air\",\"a\":", stdout);
        printf("%d,\"name\":", i);
        jstr(AIR_NAME[i]);
        fputs(",\"note\":\"On the knob. The air tone.\"}", stdout);
    }
    fputs("],\n  \"dust\": [", stdout);
    for (i = 0; i < 3; i++) {
        if (i)
            fputc(',', stdout);
        fputs("{\"kind\":\"dust\",\"a\":", stdout);
        printf("%d,\"name\":", i);
        jstr(AIR_NAME[i]);
        fputs(",\"note\":\"On the knob. Dust. Wood is a knock here.\"}", stdout);
    }
    fputs("],\n  \"bed\": [", stdout);
    for (i = 0; i < 5; i++) {
        if (i)
            fputc(',', stdout);
        fputs("{\"kind\":\"bed\",\"a\":", stdout);
        printf("%d,\"name\":", i);
        jstr(BED_NAME[i]);
        fputs(",\"note\":\"In the engine. The knob does not walk this colour.\"}", stdout);
    }
    fputs("],\n", stdout);
    list_names("kits", EN_KIT_NAME, EN_NKIT);
    fputs(",\n", stdout);
    list_names("lanes", EN_LANE_NAME, EN_NLANE);
    fputs(",\n", stdout);
    list_names("extra", XNAME, EN_XDRUM);
    fputs(",\n  \"keys\": [", stdout);
    for (i = 0; i < 12; i++) {
        if (i)
            fputc(',', stdout);
        fputs("{\"name\":", stdout);
        jstr(KEY_NAME[i]);
        fputs(",\"engine\":", stdout);
        jstr(KEY_ENGINE[i]);
        fputc('}', stdout);
    }
    fputs("]\n}\n", stdout);
}

/* Everything else off, and dry. The one voice we arm comes after this. */
static void silence(void)
{
    int i, l;
    en_layers(0);
    en_layer_level(0, 0);
    en_layer_level(1, 0);
    en_layer_level(2, 0);
    en_fx(0);
    en_haze(0);
    en_echo(0);
    en_verb(0);
    for (i = 0; i < EN_XDRUM; i++)
        en_xdrum(i, 0, 0, 0, 0, 0, 50);
    for (i = 0; i < EN_NPAT; i++)
        for (l = 0; l < EN_NLANE; l++)
            en_pats[i][l] = 0;
    en_set(P_ECHO, 0);
    en_set(P_REVERB, 0);
    en_set(P_MOTION, 0);
    en_set(P_EVOLVE, 0);
    en_set(P_LINKS, 0);
    en_set(P_SWING, 0);
    en_set(P_PAD, 0);
    en_set(P_BASS, 0);
    en_set(P_ACID, 0);
    en_set(P_NOISE, 0);
    en_set(P_PULSE, 0);
    en_set(P_PAD_MAN, 1);
    en_set(P_DR_MAN, 1);
    en_set(P_AC_MAN, 1);
    for (l = 0; l < EN_NLANE; l++)
        en_set(P_LANE_LVL(l), 0);
}

static void kit_all(int kit)
{
    kit = clamp(kit, 0, EN_NKIT - 1);
    en_set(P_KIT_KICK, kit);
    en_set(P_KIT_SNARE, kit);
    en_set(P_KIT_HAT, kit);
    en_set(P_KIT_PERC, kit);
    en_lane_kit(2, kit);                /* clap */
    en_lane_kit(4, kit);                /* open hat */
}

/* Returns the acid note to retrigger, or 0. Sets *secs. */
static int arm(const char *kind, int a, int b, double *secs)
{
    int acid = 0;
    *secs = 3.0;
    silence();
    if (!strcmp(kind, "pad")) {
        a = clamp(a, 0, EN_NPAD_SND - 1);
        en_set(P_PAD, 75);
        en_set(P_PADTONE, a);
        en_set(P_PE_ATK, 18);
        en_set(P_PE_HOLD, 80);
        en_note(EN_V_PAD, 60, 1);
        en_note(EN_V_PAD, 64, 1);
        en_note(EN_V_PAD, 67, 1);
    } else if (!strcmp(kind, "bass")) {
        a = clamp(a, 0, EN_NBASS_SND - 1);
        en_set(P_BASS, 78);
        en_set(P_DR_INST, a);
        en_note(EN_V_DRONE, 48, 1);     /* C3: a laptop can still speak a sub */
    } else if (!strcmp(kind, "acid")) {
        a = clamp(a, 0, EN_NACID_SND - 1);
        en_set(P_ACID, 75);
        en_set(P_AC_WAVE, a);
        en_note(EN_V_ACID, 67, 1);
        acid = 67;
        *secs = 2.4;
    } else if (!strcmp(kind, "air") || !strcmp(kind, "dust") || !strcmp(kind, "bed")) {
        int color, shape;
        if (!strcmp(kind, "bed")) {
            a = clamp(a, 0, 4);
            color = BED_COLOR[a];
            shape = 0;                  /* the bed: the hiss itself */
        } else {
            a = clamp(a, 0, 2);
            color = AIR_COLOR[a];
            shape = kind[0] == 'd' ? 1 : 3;
        }
        en_set(P_NOISE, 72);
        en_set(P_NZ_COLOR, color);
        en_set(P_NZ_TONE, 50);
        en_set(P_NZ_PUMP, 0);
        en_haze(shape);
        en_set(P_TEMPO, 96);
        en_play(1);
    } else if (!strcmp(kind, "lane") || !strcmp(kind, "xdrum")) {
        int kit = clamp(a, 0, EN_NKIT - 1);
        kit_all(kit);
        en_set(P_PULSE, 78);
        en_set(P_TEMPO, 96);
        en_set(P_DRIVE, 40);
        if (!strcmp(kind, "lane")) {
            b = clamp(b, 0, EN_NLANE - 1);
            en_set(P_LANE_LVL(b), 80);
            en_pats[0][b] = 0x1111;     /* the downbeat of each beat */
        } else {
            b = clamp(b, 0, EN_XDRUM - 1);
            en_xdrum(b, 1, kit, 0x1111, 80, 0, 50);
        }
        en_play(1);
        *secs = 2.4;
    } else if (!strcmp(kind, "key")) {
        int part;
        a = clamp(a, 0, 11);
        part = KEY_PART[a];
        en_set(P_TEMPO, 100);
        en_layer_level(part, 88);
        en_layers(1u << a);
        en_play(1);
        if (part == 0) {
            /* Glass and Dark follow the pad's breath. The pad itself stays quiet. */
            en_set(P_PAD_MAN, 0);
            en_set(P_PAD, 12);
            en_set(P_PADTONE, 15);
            en_set(P_PE_ATK, 12);
            en_set(P_PE_HOLD, 80);
        } else if (part == 1) {
            en_note(EN_V_DRONE, 48, 1);
        } else {
            en_note(EN_V_ACID, 67, 1);
            acid = 67;
        }
    } else {
        fprintf(stderr, "unknown kind %s\n", kind);
        exit(2);
    }
    return acid;
}

static void le16(unsigned char *p, unsigned v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
}

static void le32(unsigned char *p, unsigned v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

static void write_wav(double secs, int acid)
{
    int32_t buf[256];
    int16_t pcm[256];
    uint32_t frames = (uint32_t)(secs * 44100.0);
    uint32_t done = 0, next = acid ? 16000u : 0xFFFFFFFFu;
    uint32_t data_bytes, riff;
    float peak = 0.0f;
    unsigned char hdr[44];
    if (frames < 1)
        frames = 1;
    data_bytes = frames * 4u;
    riff = 36u + data_bytes;
    memset(hdr, 0, sizeof hdr);
    memcpy(hdr, "RIFF", 4);
    le32(hdr + 4, riff);
    memcpy(hdr + 8, "WAVEfmt ", 8);
    le32(hdr + 16, 16);
    le16(hdr + 20, 1);                  /* PCM */
    le16(hdr + 22, 2);                  /* stereo */
    le32(hdr + 24, 44100);
    le32(hdr + 28, 44100 * 4);          /* byte rate */
    le16(hdr + 32, 4);                  /* block align */
    le16(hdr + 34, 16);
    memcpy(hdr + 36, "data", 4);
    le32(hdr + 40, data_bytes);
    fwrite(hdr, 1, 44, stdout);
    while (done < frames) {
        uint32_t n = frames - done;
        uint32_t i;
        if (n > 128)
            n = 128;
        if (done >= next) {
            en_note(EN_V_ACID, acid, 0);
            en_note(EN_V_ACID, acid, 1);
            next += 16000u;
        }
        en_render(buf, n, 2400);
        for (i = 0; i < n * 2u; i++) {
            float s = (float)buf[i] * (1.0f / 8388608.0f);
            int v;
            if (s < 0.0f) {
                if (-s > peak)
                    peak = -s;
            } else if (s > peak) {
                peak = s;
            }
            v = buf[i] / 256;
            if (v > 32767)
                v = 32767;
            if (v < -32768)
                v = -32768;
            pcm[i] = (int16_t)v;
        }
        fwrite(pcm, 2, n * 2u, stdout);
        done += n;
    }
    fflush(stdout);
    fprintf(stderr, "peak %.4f\n", peak);
}

int main(int argc, char **argv)
{
    double secs;
    int acid;
    if (argc >= 2 && !strcmp(argv[1], "--list")) {
        list_out();
        return 0;
    }
    if (argc < 3) {
        fprintf(stderr, "usage: hear --list | hear KIND A [B]\n");
        return 2;
    }
    en_init();
    acid = arm(argv[1], argn(argc, argv, 2, 0), argn(argc, argv, 3, 0), &secs);
    write_wav(secs, acid);
    return 0;
}
