// -*- tabs-indent-mode: t */
/* See LICENSE file for copyright and license details. */
#include <X11/Xft/Xft.h>
#include <X11/XF86keysym.h>


/* appearance */
static const char *fonts[] = {
	"Knack:size=10",
	"Icons:size=10"
};
static const char dmenufont[]       = "Knack:size=10";
static const char normbordercolor[] = "#263238";
static const char normbgcolor[]     = "#263238";
static const char normfgcolor[]     = "#ffffff";
static const char selbordercolor[]  = "#B8BB26";
static const char selbgcolor[]      = "#B8BB26";
static const char selfgcolor[]      = "#263238";
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int gappx              = 8;


/* tagging */
static const char *tags[] = { "1", "2", "3", "4" };

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
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const int resizehints  = 0;    /* 1 means respect size hints in tiled resizals */
static const char clock_fmt[] = "%m/%d/%y @ %I:%M %p";

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
  { "[B]",      htile },
  { "[G]",      grid },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-fn", dmenufont, "-nb", normbgcolor,
    "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };

static const char *ztream[] = { "/home/bryan/.bin/ztream", "-fn", dmenufont, "-nb", normbgcolor,
    "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *term[]    = { "/home/bryan/.bin/st", NULL };

static const char *vol_up[]  = { "amixer", "-q", "sset", "Master", "5%+", "unmute", NULL };
static const char *vol_dwn[] = { "amixer", "-q", "sset", "Master", "5%-", "unmute", NULL };
static const char *vol_mut[] = { "amixer", "-q", "sset", "Master", "toggle", NULL };

static const char *media_tog[]  = { "/home/bryan/.bin/mediactl", "toggle", NULL };
static const char *media_stp[]  = { "/home/bryan/.bin/mediactl", "stop", NULL };
static const char *media_prv[]  = { "/home/bryan/.bin/mediactl", "prev", NULL };
static const char *media_nxt[]  = { "/home/bryan/.bin/mediactl", "next", NULL };

static const char *lock_cmd[]   = { "sxlock", NULL };

static const char *emacs[] = {"emacsclient", "-c", NULL, NULL, "Emacs"};

static Key keys[] = {
	/* modifier            key                      function        argument */
	{ MODKEY,              XK_p,                    spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,    XK_Return,               spawn,          {.v = term } },
	{ MODKEY|ControlMask,  XK_b,                    togglebar,      {0} },
	{ MODKEY,              XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,              XK_k,                    focusstack,     {.i = -1 } },
	{ MODKEY,              XK_i,                    incnmaster,     {.i = +1 } },
	{ MODKEY,              XK_d,                    incnmaster,     {.i = -1 } },
	{ MODKEY,              XK_h,                    setmfact,       {.f = -0.05} },
	{ MODKEY,              XK_l,                    setmfact,       {.f = +0.05} },
  { MODKEY|ShiftMask,    XK_h,                    setcfact,       {.f = +0.05} },
  { MODKEY|ShiftMask,    XK_l,                    setcfact,       {.f = -0.05} },
  { MODKEY|ShiftMask,    XK_o,                    setcfact,       {.f = 0} },
	{ MODKEY,              XK_Return,               zoom,           {0} },
	{ MODKEY,              XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask,    XK_c,                    killclient,     {0} },
	{ MODKEY,              XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,              XK_f,                    setlayout,      {.v = &layouts[1]} },
	{ MODKEY,              XK_m,                    setlayout,      {.v = &layouts[2]} },
  { MODKEY,              XK_b,                    setlayout,      {.v = &layouts[3]} },
  { MODKEY,              XK_g,                    setlayout,      {.v = &layouts[4]} },
	{ MODKEY,              XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,    XK_space,                togglefloating, {0} },
	{ MODKEY,              XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,              XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,              XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(               XK_1,                                    0)
	TAGKEYS(               XK_2,                                    1)
	TAGKEYS(               XK_3,                                    2)
	TAGKEYS(               XK_4,                                    3)
	{ MODKEY|ShiftMask,    XK_q,                    quit,           {0} },
  { MODKEY|ShiftMask,    XK_b,                    banishpointer,  {0} },

  { 0,                   XF86XK_AudioLowerVolume, spawn,          { .v = vol_dwn } },
  { 0,                   XF86XK_AudioRaiseVolume, spawn,          { .v = vol_up  } },
  { 0,                   XF86XK_AudioMute,        spawn,          { .v = vol_mut } },

  { 0,                   XF86XK_AudioPlay,        spawn,          { .v = media_tog } },
  { 0,                   XF86XK_AudioNext,        spawn,          { .v = media_nxt } },
  { 0,                   XF86XK_AudioPrev,        spawn,          { .v = media_prv } },
  { 0,                   XF86XK_AudioStop,        spawn,          { .v = ztream } },

  { MODKEY,              XK_e,                    runorraise,     { .v = emacs } },

  { ControlMask|MODKEY,  XK_space,                spawn,          { .v = lock_cmd } },

};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
