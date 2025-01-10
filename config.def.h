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
static const char *tags[] = { "1", "2", "3" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "mpv",      NULL,       NULL,       0,            1,           -1 },
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
#define MODALT Mod1Mask
#define MODWIN Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODWIN,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODWIN|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODWIN|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODWIN|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "x-terminal-emulator", NULL };
static const char *filemanager[]  = { "x-file-manager", NULL };
static const char *browsercmd[]  = { "x-www-browser", NULL };
static const char *cmd_vol_down[]  = { "pamixer", "-i", "5", NULL };
static const char *cmd_vol_up[]    = { "pamixer", "-d", "5", NULL };
static const char *cmd_vol_mute[]  = { "pamixer", "-t", NULL };


static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODWIN,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODWIN,                       XK_t,      spawn,          {.v = termcmd } },
	{ MODWIN,                       XK_e,      spawn,          {.v = browsercmd } },
	{ MODWIN,                       XK_e,      spawn,          {.v = filemanager } },
	{ 0,                            XF86XK_AudioLowerVolume,  spawn,          {.v = cmd_vol_down } },
	{ 0,                            XF86XK_AudioRaiseVolume,  spawn,          {.v = cmd_vol_up } },
	{ 0,                            XF86XK_AudioMute,         spawn,          {.v = cmd_vol_mute } },
	{ MODWIN,                       XK_b,      togglebar,      {0} },
	{ MODALT,                       XK_Tab,    focusstack,     {.i = +1 } },
	{ MODALT|ShiftMask,             XK_Tab,    focusstack,     {.i = -1 } },
	{ MODWIN,                       XK_Up,     incnmaster,     {.i = +1 } },
	{ MODWIN,                       XK_Down,   incnmaster,     {.i = -1 } },
	{ MODWIN,                       XK_Left,   setmfact,       {.f = -0.05} },
	{ MODWIN,                       XK_Right,  setmfact,       {.f = +0.05} },
	{ MODWIN,                       XK_Return, zoom,           {0} },
	{ MODWIN,                       XK_Tab,    view,           {0} },
	{ MODALT,                       XK_F4,     killclient,     {0} },
	{ MODWIN,                       XK_n,      setlayout,      {.v = &layouts[0]} },
	{ MODWIN,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODWIN,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODWIN,                       XK_space,  togglefloating, {0} },
	{ MODWIN,                       XK_0,      view,           {.ui = ~0 } },
	{ MODWIN|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODWIN,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODWIN,                       XK_period, focusmon,       {.i = +1 } },
	{ MODWIN|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODWIN|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	{ MODWIN|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkClientWin,         MODWIN,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODWIN,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODWIN,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODWIN,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODWIN,         Button3,        toggletag,      {0} },
};

static size_t statusblock_time(char *destination, const Arg *a)
{
	time_t tim = time(NULL);
	struct tm *lt = localtime(&tim);
	return lt == NULL ? 0 : strftime(destination, 32, "%F %R", lt);
}

static size_t statusblock_meminfo(char *destination, const Arg *a)
{
	FILE *f = fopen("/proc/meminfo", "r");
	if (f == NULL)
		return 0;
	unsigned long mem_total = 0, mem_avail = 0;
	for (char line[128]; fgets(line, sizeof(line), f);)
		if ((mem_total || sscanf(line, "MemTotal: %lu kB", &mem_total)) &&
		    (mem_avail || sscanf(line, "MemAvailable: %lu kB", &mem_avail)))
			break;
	const size_t len = sprintf(destination, "💻%lu/%luMB", (mem_total - mem_avail) / 1024, mem_total / 1024);
	fclose(f);
	return len;
}

static size_t statusblock_wifi(char *destination, const Arg *a)
{
	FILE *f = fopen("/proc/net/wireless", "r");
	if (f == NULL)
		return 0;
	int link = 0;
	for (char line[128]; fgets(line, sizeof(line), f);)
		if(sscanf(line, " wlan0: %*s %d. %*d. %*d ", &link))
			break;
	const size_t len = sprintf(destination, "📡%d%%", MIN(99, link * 100 / 70));
	fclose(f);
	return len;
}

static size_t statusblock_temperature(char *destination, const Arg *a)
{
	FILE *f = fopen(a->v, "r");
	if (f == NULL)
		return 0;
	long temp = 0;
	const size_t len = fscanf(f, "%ld", &temp) ? sprintf(destination, "🌡%ld℃", temp / 1000 ) : 0;
	fclose(f);
	return len;
}

static size_t statusblock_static(char *destination, const Arg *a)
{
	return sprintf(destination, "%s", (const char*)a->v);
}

static size_t statusblock_popen(char *destination, const Arg *a)
{
	static time_t prev_time = 0;
	time_t cur_time = time(NULL);
	size_t len = 0;
	if (cur_time - prev_time > 60*60) {
		prev_time = cur_time;
		FILE *f = popen(a->v, "r");
		if (f) {
			len = fread(destination, 1, 64, f);
			pclose(f);
		}
	}
	return len;
}

static const char *sunclock_cmd[]  = { "sunclock", NULL };
static const char *htop_cmd[]  = { "x-terminal-emulator", "-e", "htop", NULL };
static const char *xosview_cmd[]  = { "xosview", NULL };
static const char *xcalc_cmd[]  = { "xcalc", NULL };
static const char *wifi_cmd[]  = { "sh", "-c", "wpa_cli scan_results | tail -n +3 | sed 's/.*]\t//' | dmenu", NULL }; //TODO: this doesn't actually do anything, just shows list of networks from previous scan
static const char *weather_cmd[]  = { "x-terminal-emulator", "-e", "sh", "-c", "curl https://wttr.in/ ; read foo", NULL };

static void statusblock_spawn(const unsigned int button, const Arg *a)
{
	if(button == Button1 && a->v)
		spawn(a);
}

static const StatusBlock statusblocks[] = {
	//{statusblock_popen, {.v = "curl https://wttr.in/?format=%c%t%20%w"}, statusblock_spawn, {.v = weather_cmd}},
	{statusblock_static, {.v = "🖩"}, statusblock_spawn, {.v = xcalc_cmd}},
	{statusblock_wifi, {0}, statusblock_spawn, {.v = wifi_cmd}},
	{statusblock_temperature, {.v = "/sys/devices/virtual/thermal/thermal_zone0/temp"}, NULL, {0}},
	{statusblock_meminfo, {0}, statusblock_spawn, {.v = xosview_cmd}},
	{statusblock_time, {0}, statusblock_spawn, {.v = sunclock_cmd}}
};

static const struct timespec status_interval = {
	.tv_sec = 2,
	.tv_nsec = 0
};
