/* SPDX-License-Identifier: GPL-3.0-only */
/* ENO project: one flash object (OBJ_PROJ). A project of another format, or none, gives the
 * defaults. Part of the unity build, after app.h.
 *
 * The pieces: eight recipes for the whole garden (layers, timbre, tempo, key and mode, how busy and
 * how far it moves). Each keeps its own settings as you leave it; Tune and MIDI out are global. */
project_t proj;

typedef struct {
    const char *name;
    int8_t v[P_NPARAMS];                 /* P_* order; Tune and MIDI out are not taken from here */
} piece_t;

/*                     Pad Bel Bas Pul  Den Evo Dri Hit  Ech ET Rev Siz  Bri Swe Ton Mot  Tun MI Tem Rt Md Oc */
static const piece_t PIECES[NPIECES] = {
    {"Garden",       {60, 65, 45, 40,  40, 35, 45, 35,  45, 3, 65, 80,  40, 60, 50, 50,  0, 1,  72, 2, 1, 0}},
    {"Departures",   {50, 70,  0,  0,  35, 30, 65,  0,  30, 3, 70, 85,  25, 55, 10, 40,  0, 1,  60, 1, 0, 0}},
    {"Voices",       {85,  0, 25,  0,  30, 30, 75,  0,  20, 5, 80, 90,  30, 85, 30, 55,  0, 1,  56, 5, 5, 0}},
    {"Afternoon",    {50, 65, 35, 25,  55, 45, 40, 30,  50, 2, 60, 75,  60, 50, 60, 55,  0, 1,  84, 7, 3, 0}},
    {"Night Bus",    {45, 55, 55, 60,  45, 50, 35, 55,  55, 1, 55, 70,  35, 40, 75, 70,  0, 1,  96, 9, 5, 0}},
    {"Shoreline",    {55, 45, 30,  0,  25, 40, 60,  0,  55, 4, 75, 95,  45, 80, 40, 85,  0, 1,  64, 4, 6, 0}},
    {"Glass",        {35, 80, 30, 20,  60, 55, 45, 25,  45, 2, 65, 80,  55, 45, 90, 50,  0, 1,  76, 3, 7, 1}},
    {"Tides",        {55, 60, 40, 35,  45, 75, 30, 40,  50, 3, 65, 85,  45, 60, 55, 95,  0, 1,  70, 10, 4, 0}},
};

const char *piece_name(int i) { return PIECES[(unsigned)i < NPIECES ? i : 0].name; }

static int global_par(int p) { return p == P_TUNE || p == P_MIDI; }

static void recipe(int i, slot_t *s)
{
    int p;
    for (p = 0; p < P_NPARAMS; p++)
        s->par[p] = PIECES[i].v[p];
    s->pool = EN_MODE_POOL[PIECES[i].v[P_MODE]];
}

static void take(const slot_t *s)                /* a piece's settings into play, the globals kept */
{
    int p;
    for (p = 0; p < P_NPARAMS; p++)
        if (!global_par(p))
            proj.par[p] = s->par[p];
    proj.pool = s->pool;
}

static void keep(void)                           /* the piece playing, into its slot */
{
    int p;
    for (p = 0; p < P_NPARAMS; p++)
        proj.slot[proj.piece].par[p] = proj.par[p];
    proj.slot[proj.piece].pool = proj.pool;
}

void project_defaults(void)
{
    int i;
    memset(&proj, 0, sizeof proj);
    proj.magic = PROJ_MAGIC;
    proj.format = PROJ_FORMAT;
    for (i = 0; i < NPIECES; i++)
        recipe(i, &proj.slot[i]);
    for (i = 0; i < P_NPARAMS; i++)
        proj.par[i] = en_param_info(i)->def;
    take(&proj.slot[0]);
    proj.leds = 1;
}

static int pool_ok(uint32_t p)                   /* the root and at least two more notes */
{
    int n = 0, i;
    for (i = 0; i < 12; i++)
        n += (int)(p >> i & 1u);
    return (p & 1u) && !(p >> 12) && n >= 3;
}

static int pars_ok(const int16_t *par)
{
    int i;
    for (i = 0; i < P_NPARAMS; i++) {
        const en_param_t *p = en_param_info(i);
        if (par[i] < p->lo || par[i] > p->hi)
            return 0;
    }
    return 1;
}

static int project_valid(void)
{
    int i;
    if (proj.magic != PROJ_MAGIC || !pool_ok(proj.pool) || !pars_ok(proj.par))
        return 0;
    if (proj.format == 1u) {                     /* before pieces: what was saved becomes Garden */
        proj.piece = 0;
        for (i = 0; i < NPIECES; i++)
            recipe(i, &proj.slot[i]);
        keep();
        proj.format = PROJ_FORMAT;
    }
    if (proj.format != PROJ_FORMAT || proj.piece >= NPIECES)
        return 0;
    for (i = 0; i < NPIECES; i++)
        if (!pool_ok(proj.slot[i].pool) || !pars_ok(proj.slot[i].par))
            return 0;
    return 1;
}

int project_load(void)
{
    int n;
    memset(&proj, 0, sizeof proj);
    n = plat_store_load(OBJ_PROJ, &proj, sizeof proj);
    if (n >= 8 && project_valid())
        return 0;
    project_defaults();
    return -1;
}

int project_save(void)
{
    keep();
    return plat_store_save(OBJ_PROJ, &proj, sizeof proj);
}

void project_piece(int i)
{
    keep();
    proj.piece = (uint8_t)((i % NPIECES + NPIECES) % NPIECES);
    take(&proj.slot[proj.piece]);
}

void project_piece_reset(void)
{
    slot_t s;
    recipe(proj.piece, &s);
    take(&s);
}

void project_apply(void)
{
    int i;
    for (i = 0; i < P_NPARAMS; i++)
        en_set(i, proj.par[i]);
    en_pool(proj.pool);
}
