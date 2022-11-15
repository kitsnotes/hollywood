#ifndef HOLLYWOOD_H
#define HOLLYWOOD_H

#define HOLLYWOOD_OS_VERSION        "1.0"
#define HOLLYWOOD_OS_DOMAIN         "originull.org"
#define HOLLYWOOD_OS_ORGNAME        "Originull Software"

#define HOLLYWOOD_SESSION_DBUS      "org.originull.hollywood"
#define HOLLYWOOD_SESSION_DBUSOBJ   "/Session"
#define HOLLYWOOD_SESSION_DBUSPATH  "org.originull.hollywood.Session"

#define HOLYWOOD_SETTINGS_APP       "org.originull.hwsettings.desktop"
#define HOLYWOOD_SYSMON_APP         "org.originull.hwsysmon.desktop"
#define HOLYWOOD_TERMINULL_APP      "org.originull.terminull.desktop"

#define HOLLYWOOD_WP_APPLET         "org.originull.wallpaper"

#define HOLLYWOOD_SERVICE_SVC       "/usr/sbin/rc-service"
#define HOLLYWOOD_SERVICE_UPDATE    "/usr/sbin/rc-update"
#define HOLLYWOOD_SERVICE_NTPD      "openntpd"

// Default Stage Settings
#define HOLLYWOOD_STAGE_LAYOUT      0   // 0 northern, 1 southern
#define HOLLYWOOD_STAGE_SIZE        0   // 0 small, 1 large
#define HOLLYWOOD_STAGE_POSITION    0x00008 // bottom match Qt::Edge

// Stage Clock Settings
#define HOLLYWOOD_STCLK_SHOW        true
#define HOLLYWOOD_STCLK_USEDATE     false
#define HOLLYWOOD_STCLK_USESECONDS  false
#define HOLLYWOOD_STCLK_24HOUR      true
#define HOLLYWOOD_STCLK_USEAMPM     true

// Default Managed Style Aspects
// (Used by Platform Theme)
#define HOLLYWOOD_DEF_STYLE         "Hollywood"
#define HOLLYWOOD_DEF_ICONTHEME     "breeze"
#define HOLLYWOOD_DARK_ICONTHEME    "breeze-dark"
#define HOLLYWOOD_DEF_STDFONT       "Open Sans"
#define HOLLYWOOD_DEF_FIXEDSYS      "Hack"

// This matches to "Periwinkle" in applet-wallpaper
// colors.txt definition file
#define HOLLYWOOD_DEF_DESKTOP_BG    "#be93d4"

#define HOLLYWOOD_PT_SMALL          8
#define HOLLYWOOD_PT_NORMAL         10
#define HOLLYWOOD_PT_LARGE          12
#define HOLLYWOOD_PT_XLARGE         14

// Default Managed Accent Colors
// (Used by Platform Theme & System Settings)
#define HOLLYWOOD_ACCENT_BLUE       "#3c8ce6"
#define HOLLYWOOD_ACCENT_RED        "#ff0b0b"
#define HOLLYWOOD_ACCENT_YELLOW     "#ffe207"
#define HOLLYWOOD_ACCENT_ORANGE     "#ff9d00"
#define HOLLYWOOD_ACCENT_PURPLE     "#a80de6"
#define HOLLYWOOD_ACCENT_PINK       "#f789ff"
#define HOLLYWOOD_ACCENT_GREEN      "#0eaa13"
#define HOLLYWOOD_ACCENT_GRAY       "#8b8e9d"

#define HOLLYWOOD_WNDCLR_LIGHT      "#efefef"
#define HOLLYWOOD_VIEWCLR_LIGHT     "#ffffff"
#define HOLLYWOOD_TEXTCLR_LIGHT     "#000000"
#define HOLLYWOOD_SELTXT_LIGHT      "#ffffff"

#define HOLLYWOOD_WNDCLR_DARK       "#1c1b22"
#define HOLLYWOOD_VIEWCLR_DARK      "#36393f"
#define HOLLYWOOD_TEXTCLR_DARK      "#ffffff"
#define HOLLYWOOD_SELTXT_DARK       "#000000"

// default mouse settings
#define HOLLYWOOD_MAX_SCROLL_SPEED  40
#define HOLLYWOOD_DEF_SCROLL_SPEED  6
#define HOLLYWOOD_MIN_DBLCLK_TIME   100
#define HOLLYWOOD_MAX_DBLCLK_TIME   1200
#define HOLLYWOOD_DEF_DBLCLK_TIME   40

// Keyboard Things
#define HOLLYWOOD_MIN_KBD_DELAY     100
#define HOLLYWOOD_MAX_KBD_DELAY     1100
#define HOLLYWOOD_DEF_KBD_DELAY     400

#define HOLLYWOOD_MIN_KBD_INT       0
#define HOLLYWOOD_MAX_KBD_INT       210
#define HOLLYWOOD_DEF_KBD_INT       45
#define HOLLYWOOD_KEYBOARD_BL       "/sys/class/leds/tpacpi::kbd_backlight"
#define HOLLYWOOD_XKBDBL_PATH       "/usr/share/X11/xkb/rules/base.lst"
#define HOLLYWOOD_DEFAULT_KBD       "pc104"
#define HOLLYWOOD_DEFAULT_KBDL      "us"
#define HOLLYWOOD_DEFAULT_KBDV      "null"

#endif // HOLLYWOOD_H
