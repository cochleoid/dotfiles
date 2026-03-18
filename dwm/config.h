/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=10" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */

/* ──────────────────────────────── */
/* website commands                */
/* ──────────────────────────────── */
static const char *lichess[]        = { "/usr/bin/librewolf", "--new-window", "lichess.org", NULL };
static const char *desmos[]         = { "/usr/bin/librewolf", "--new-window", "desmos.com/calculator", NULL };
static const char *chatgpt[]        = { "/usr/bin/librewolf", "--new-window", "chatgpt.com", NULL };
static const char *classlink[]      = { "/usr/bin/librewolf", "--new-window", "launchpad.classlink.com/mypolk", NULL };
static const char *wolframalpha[]   = { "/usr/bin/librewolf", "--new-window", "wolframalpha.com", NULL };
static const char *knowt[]          = { "/usr/bin/librewolf", "--new-window", "knowt.com", NULL };
static const char *focusgrades[]    = { "/usr/bin/librewolf", "--new-window", "polkk12.focusschoolsoftware.com/focus/", NULL };

/* ──────────────────────────────── */
/* system / utility commands        */
/* ──────────────────────────────── */
static char dmenumon[2]             = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]       = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont,
                                        "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan,
                                        "-sf", col_gray4, NULL };
static const char *scrot[]          = { "scrot", "-s", "Screenshot.png", NULL };
static const char *ocrcapture[]     = { "/usr/bin/ocrcapture", NULL };
static const char *telegramdesktop[]= { "telegram-desktop", NULL };
static const char *librewolf[]      = { "librewolf", NULL };
static const char *connectyamaha[]  = { "/usr/bin/bluetoothctl", "connect", "34:81:F4:34:D1:96", NULL };
static const char *connectaudio[]   = { "/usr/bin/bluetoothctl", "connect", "88:C9:E8:7D:B8:70", NULL };
static const char *xtrlock[]        = { "/usr/bin/xtrlock", NULL };
static const char *btop[]           = { "/usr/bin/urxvt", "-e", "btop", NULL };
static const char *cmus[]           = { "/usr/bin/urxvt", "-e", "cmus", NULL };
static const char *termcmd[]        = { "urxvt", NULL };

/* ──────────────────────────────── */
/* volume and brightness controls   */
/* ──────────────────────────────── */
static const char *upvol[]          = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%", NULL };
static const char *downvol[]        = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%", NULL };
static const char *mutevol[]        = { "/usr/bin/pactl", "set-sink-mute", "0", "toggle", NULL };
static const char *light_up[]       = { "/usr/bin/light", "-A", "5", NULL };
static const char *light_down[]     = { "/usr/bin/light", "-U", "5", NULL };
static const char *keylight_up[]    = { "/usr/bin/light", "-s", "sysfs/leds/smc::kbd_backlight", "-A", "5", NULL };
static const char *keylight_down[]  = { "/usr/bin/light", "-s", "sysfs/leds/smc::kbd_backlight", "-U", "5", NULL };

/* ──────────────────────────────── */
/* media controls                   */
/* ──────────────────────────────── */
static const char *pausemusic[]     = { "/usr/bin/cmus-remote", "-u", NULL };
static const char *nextmusic[]      = { "/usr/bin/cmus-remote", "-n", NULL };
static const char *prevmusic[]      = { "/usr/bin/cmus-remote", "-r", NULL };

/* ──────────────────────────────── */
/* key bindings                     */
/* ──────────────────────────────── */
static const Key keys[] = {
	/* modifier                     key                       function        argument */
	{ MODKEY,                       XK_p,                     spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return,                spawn,          {.v = termcmd } },

	/* hardware keys */
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,          {.v = downvol } },
	{ 0,                            XF86XK_MonBrightnessUp,   spawn,          {.v = light_up } },
	{ 0,                            XF86XK_MonBrightnessDown, spawn,          {.v = light_down } },
	{ 0,                            XF86XK_AudioMute,         spawn,          {.v = mutevol } },
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,          {.v = upvol } },
	{ 0,                            XF86XK_KbdBrightnessUp,   spawn,          {.v = keylight_up } },
	{ 0,                            XF86XK_KbdBrightnessDown, spawn,          {.v = keylight_down } },
 	{ 0,                            XF86XK_AudioPlay,         spawn,          {.v = pausemusic } },
	{ 0,                            XF86XK_AudioNext,         spawn,          {.v = nextmusic } },
	{ 0,                            XF86XK_AudioPrev,         spawn,          {.v = prevmusic } },

	/* application shortcuts */
	{ MODKEY|ShiftMask,             XK_t,                     spawn,          {.v = telegramdesktop } },
	{ MODKEY|ShiftMask,             XK_s,                     spawn,          {.v = scrot } },
	{ MODKEY|ShiftMask,             XK_p,                     spawn,          {.v = connectaudio } },
	{ MODKEY|ShiftMask,             XK_y,                     spawn,          {.v = connectyamaha } },
	{ MODKEY|ShiftMask,             XK_b,                     spawn,          {.v = librewolf } },
 	{ MODKEY|ShiftMask,             XK_l,                     spawn,          {.v = lichess } },
    { MODKEY|ShiftMask,             XK_d,                     spawn,          {.v = desmos } },
    { MODKEY|ShiftMask,             XK_w,                     spawn,          {.v = wolframalpha } },
    { MODKEY|ShiftMask,             XK_g,                     spawn,          {.v = chatgpt } },
    { MODKEY|ShiftMask,             XK_e,                     spawn,          {.v = classlink } },
 	{ MODKEY|ShiftMask,             XK_h,                     spawn,          {.v = btop } },
	{ MODKEY|ShiftMask,             XK_y,                     spawn,          {.v = ocrcapture } },
 	{ MODKEY|ShiftMask,             XK_m,                     spawn,          {.v = cmus } },
	{ MODKEY|ShiftMask,             XK_x,                     spawn,          {.v = xtrlock } },
	{ MODKEY|ShiftMask,             XK_k,                     spawn,          {.v = knowt } },
	{ MODKEY|ShiftMask,             XK_f,                     spawn,          {.v = focusgrades } },

	/* dwm controls */
	{ MODKEY,                       XK_b,                     togglebar,      {0} },
	{ MODKEY,                       XK_j,                     focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,                     focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,                     incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,                     incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,                     setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,                     setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return,                zoom,           {0} },
	{ MODKEY,                       XK_Tab,                   view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,                     killclient,     {0} },
	{ MODKEY,                       XK_t,                     setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,                     setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,                     setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,                 setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,                 togglefloating, {0} },
	{ MODKEY,                       XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,                     tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,                 tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,                tagmon,         {.i = +1 } },

	/* tags */
	TAGKEYS(                        XK_1, 0)
	TAGKEYS(                        XK_2, 1)
	TAGKEYS(                        XK_3, 2)
	TAGKEYS(                        XK_4, 3)
	TAGKEYS(                        XK_5, 4)
	TAGKEYS(                        XK_6, 5)
	TAGKEYS(                        XK_7, 6)
	TAGKEYS(                        XK_8, 7)
	TAGKEYS(                        XK_9, 8)

	/* quit */
	{ MODKEY|ShiftMask,             XK_q,                     quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

