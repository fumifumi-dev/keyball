/*
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdbool.h>
#include <stdint.h>
#include "quantum.h"
#include "send_string_keycodes.h"
#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif

#include "keyball.h"
#include "drivers/pmw3360/pmw3360.h"

#include <string.h>

const uint8_t CPI_DEFAULT    = KEYBALL_CPI_DEFAULT / 100;
const uint8_t CPI_MAX        = pmw3360_MAXCPI + 1;
const uint8_t SCROLL_DIV_MAX = 7;

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
const uint16_t AML_TIMEOUT_MIN = 100;
const uint16_t AML_TIMEOUT_MAX = 1000;
const uint16_t AML_TIMEOUT_QU = 50;

const uint16_t AML_ACTIVE_TIMEOUT_MIN = 1000;
const uint16_t AML_ACTIVE_TIMEOUT_MAX = 30000;
const uint16_t AML_ACTIVE_TIMEOUT_QU = 1000;
#endif

const uint8_t AML_ACTIVATE_THRESHOLD = 10;

keyball_t keyball = {
    .this_have_ball = false,
    .that_enable    = false,
    .that_have_ball = false,

    .this_motion = {0},
    .that_motion = {0},

    .cpi_value   = 0,
    .cpi_changed = false,

    .scroll_mode = false,
    .scroll_div  = 0,

#ifdef OLED_ENABLE
    .oled_disable = false,
    .oled_state = false,
#endif

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    .aml_timeout = AUTO_MOUSE_TIME,
    .aml_active_timeout = AUTO_MOUSE_ACTIVE_TIME,
#endif
};

//////////////////////////////////////////////////////////////////////////////
// Hook points

__attribute__((weak)) void keyball_on_adjust_layout(keyball_adjust_t v) {}

//////////////////////////////////////////////////////////////////////////////
// Static utilities

// add16 adds two int16_t with clipping.
static int16_t add16(int16_t a, int16_t b) {
    int16_t r = a + b;
    if (a >= 0 && b >= 0 && r < 0) {
        r = 32767;
    } else if (a < 0 && b < 0 && r >= 0) {
        r = -32768;
    }
    return r;
}

// divmod16 divides *v by div, returns the quotient, and assigns the remainder
// to *v.
static int16_t divmod16(int16_t *v, int16_t div) {
    int16_t r = *v / div;
    *v -= r * div;
    return r;
}

// clip2int8 clips an integer fit into int8_t.
static inline int8_t clip2int8(int16_t v) {
    return (v) < -127 ? -127 : (v) > 127 ? 127 : (int8_t)v;
}

static void add_cpi(int8_t delta) {
    int16_t v = keyball_get_cpi() + delta;
    keyball_set_cpi(v < 1 ? 1 : v);
}

static void add_scroll_div(int8_t delta) {
    int8_t v = keyball_get_scroll_div() + delta;
    keyball_set_scroll_div(v < 1 ? 1 : v);
}

//////////////////////////////////////////////////////////////////////////////
// Pointing device driver

#if KEYBALL_MODEL == 46
void keyboard_pre_init_kb(void) {
    keyball.this_have_ball = pmw3360_init();
    keyboard_pre_init_user();
}
#endif

void pointing_device_driver_init(void) {
#if KEYBALL_MODEL != 46
    keyball.this_have_ball = pmw3360_init();
#endif
    if (keyball.this_have_ball) {
#if defined(KEYBALL_PMW3360_UPLOAD_SROM_ID)
#    if KEYBALL_PMW3360_UPLOAD_SROM_ID == 0x04
        pmw3360_srom_upload(pmw3360_srom_0x04);
#    elif KEYBALL_PMW3360_UPLOAD_SROM_ID == 0x81
        pmw3360_srom_upload(pmw3360_srom_0x81);
#    else
#        error Invalid value for KEYBALL_PMW3360_UPLOAD_SROM_ID. Please choose 0x04 or 0x81 or disable it.
#    endif
#endif
        pmw3360_cpi_set(CPI_DEFAULT - 1);
    }
}

uint16_t pointing_device_driver_get_cpi(void) {
    return keyball_get_cpi();
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    keyball_set_cpi(cpi);
}

__attribute__((weak)) void keyball_on_apply_motion_to_mouse_move(keyball_motion_t *m, report_mouse_t *r, bool is_left) {
#if KEYBALL_MODEL == 61 || KEYBALL_MODEL == 39 || KEYBALL_MODEL == 147 || KEYBALL_MODEL == 44
    r->x = clip2int8(m->y);
    r->y = clip2int8(m->x);
    if (is_left) {
        r->x = -r->x;
        r->y = -r->y;
    }
#elif KEYBALL_MODEL == 46
    r->x = clip2int8(m->x);
    r->y = -clip2int8(m->y);
#else
#    error("unknown Keyball model")
#endif
    // clear motion
    m->x = 0;
    m->y = 0;
}

__attribute__((weak)) void keyball_on_apply_motion_to_mouse_scroll(keyball_motion_t *m, report_mouse_t *r, bool is_left) {
    // consume motion of trackball.
    int16_t div = 1 << (keyball_get_scroll_div() - 1);
    int16_t x = divmod16(&m->x, div);
    int16_t y = divmod16(&m->y, div);

    // apply to mouse report.
#if KEYBALL_MODEL == 61 || KEYBALL_MODEL == 39 || KEYBALL_MODEL == 147 || KEYBALL_MODEL == 44
    r->h = clip2int8(y);
    r->v = -clip2int8(x);
    if (is_left) {
        r->h = -r->h;
        r->v = -r->v;
    }
#elif KEYBALL_MODEL == 46
    r->h = clip2int8(x);
    r->v = clip2int8(y);
#else
#    error("unknown Keyball model")
#endif

    // Scroll snapping
#if KEYBALL_SCROLLSNAP_ENABLE == 1
    // Old behavior up to 1.3.2)
    uint32_t now = timer_read32();
    if (r->h != 0 || r->v != 0) {
        keyball.scroll_snap_last = now;
    } else if (TIMER_DIFF_32(now, keyball.scroll_snap_last) >= KEYBALL_SCROLLSNAP_RESET_TIMER) {
        keyball.scroll_snap_tension_h = 0;
    }
    if (abs(keyball.scroll_snap_tension_h) < KEYBALL_SCROLLSNAP_TENSION_THRESHOLD) {
        keyball.scroll_snap_tension_h += y;
        r->h = 0;
    }
#elif KEYBALL_SCROLLSNAP_ENABLE == 2
    // New behavior
    switch (keyball_get_scrollsnap_mode()) {
        case KEYBALL_SCROLLSNAP_MODE_VERTICAL:
            r->h = 0;
            break;
        case KEYBALL_SCROLLSNAP_MODE_HORIZONTAL:
            r->v = 0;
            break;
        default:
            // pass by without doing anything
            break;
    }
#endif
}

static void motion_to_mouse(keyball_motion_t *m, report_mouse_t *r, bool is_left, bool as_scroll) {
    if (as_scroll) {
        keyball_on_apply_motion_to_mouse_scroll(m, r, is_left);
    } else {
        keyball_on_apply_motion_to_mouse_move(m, r, is_left);
    }
}

static inline bool should_report(void) {
    uint32_t now = timer_read32();
#if defined(KEYBALL_REPORTMOUSE_INTERVAL) && KEYBALL_REPORTMOUSE_INTERVAL > 0
    // throttling mouse report rate.
    static uint32_t last = 0;
    if (TIMER_DIFF_32(now, last) < KEYBALL_REPORTMOUSE_INTERVAL) {
        return false;
    }
    last = now;
#endif
#if defined(KEYBALL_SCROLLBALL_INHIVITOR) && KEYBALL_SCROLLBALL_INHIVITOR > 0
    if (TIMER_DIFF_32(now, keyball.scroll_mode_changed) < KEYBALL_SCROLLBALL_INHIVITOR) {
        keyball.this_motion.x = 0;
        keyball.this_motion.y = 0;
        keyball.that_motion.x = 0;
        keyball.that_motion.y = 0;
    }
#endif
    return true;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t rep) {
    // fetch from optical sensor.
    if (keyball.this_have_ball) {
        pmw3360_motion_t d = {0};
        if (pmw3360_motion_burst(&d)) {
            ATOMIC_BLOCK_FORCEON {
                keyball.this_motion.x = add16(keyball.this_motion.x, d.x);
                keyball.this_motion.y = add16(keyball.this_motion.y, d.y);
            }
        }
    }
    // report mouse event, if keyboard is primary.
    if (is_keyboard_master() && should_report()) {
        // modify mouse report by PMW3360 motion.
        motion_to_mouse(&keyball.this_motion, &rep, is_keyboard_left(), keyball.scroll_mode);
        motion_to_mouse(&keyball.that_motion, &rep, !is_keyboard_left(), keyball.scroll_mode ^ keyball.this_have_ball);
        // store mouse report for OLED.
        keyball.last_mouse = rep;
    }
    return rep;
}

//////////////////////////////////////////////////////////////////////////////
// Split RPC

#ifdef SPLIT_KEYBOARD

static void rpc_get_info_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    keyball_info_t info = {
        .ballcnt = keyball.this_have_ball ? 1 : 0,
    };
    *(keyball_info_t *)out_data = info;
    keyball_on_adjust_layout(KEYBALL_ADJUST_SECONDARY);
}

static void rpc_get_info_invoke(void) {
    static bool     negotiated = false;
    static uint32_t last_sync  = 0;
    static int      round      = 0;
    uint32_t        now        = timer_read32();
    if (negotiated || TIMER_DIFF_32(now, last_sync) < KEYBALL_TX_GETINFO_INTERVAL) {
        return;
    }
    last_sync = now;
    round++;
    keyball_info_t recv = {0};
    if (!transaction_rpc_exec(KEYBALL_GET_INFO, 0, NULL, sizeof(recv), &recv)) {
        if (round < KEYBALL_TX_GETINFO_MAXTRY) {
            dprintf("keyball:rpc_get_info_invoke: missed #%d\n", round);
            return;
        }
    }
    negotiated             = true;
    keyball.that_enable    = true;
    keyball.that_have_ball = recv.ballcnt > 0;
    dprintf("keyball:rpc_get_info_invoke: negotiated #%d %d\n", round, keyball.that_have_ball);

    // split keyboard negotiation completed.

#    ifdef VIA_ENABLE
    // adjust VIA layout options according to current combination.
    uint8_t  layouts = (keyball.this_have_ball ? (is_keyboard_left() ? 0x02 : 0x01) : 0x00) | (keyball.that_have_ball ? (is_keyboard_left() ? 0x01 : 0x02) : 0x00);
    uint32_t curr    = via_get_layout_options();
    uint32_t next    = (curr & ~0x3) | layouts;
    if (next != curr) {
        via_set_layout_options(next);
    }
#    endif

    keyball_on_adjust_layout(KEYBALL_ADJUST_PRIMARY);
}

static void rpc_get_motion_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    *(keyball_motion_t *)out_data = keyball.this_motion;
    // clear motion
    keyball.this_motion.x = 0;
    keyball.this_motion.y = 0;
}

static void rpc_get_motion_invoke(void) {
    static uint32_t last_sync = 0;
    uint32_t        now       = timer_read32();
    if (TIMER_DIFF_32(now, last_sync) < KEYBALL_TX_GETMOTION_INTERVAL) {
        return;
    }
    keyball_motion_t recv = {0};
    if (transaction_rpc_exec(KEYBALL_GET_MOTION, 0, NULL, sizeof(recv), &recv)) {
        keyball.that_motion.x = add16(keyball.that_motion.x, recv.x);
        keyball.that_motion.y = add16(keyball.that_motion.y, recv.y);
    }
    last_sync = now;
    return;
}

static void rpc_set_cpi_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    keyball_set_cpi(*(keyball_cpi_t *)in_data);
}

static void rpc_set_cpi_invoke(void) {
    if (!keyball.cpi_changed) {
        return;
    }
    keyball_cpi_t req = keyball.cpi_value;
    if (!transaction_rpc_send(KEYBALL_SET_CPI, sizeof(req), &req)) {
        return;
    }
    keyball.cpi_changed = false;
}

#endif

//////////////////////////////////////////////////////////////////////////////
// OLED utility

#ifdef OLED_ENABLE

#include "lib/oledkit/oledkit.h"

static char chmat[6] = {0};

static const char BL = '\x3D';
static const char BC = ' ';
static const char ZERO = '0';
static const char SLF = '\x4F';
static const char SDW = '\x51';
static const char LOGO[] PROGMEM = "\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A";
static const char DIV_METER[][6] PROGMEM = {
    "\x2B\x2C\x2F\x2F\x2F", // d=1
    "\x2B\x2C\x2F\x2F\x2E", // d=2
    "\x2B\x2C\x2F\x2F\x2D", // d=3
    "\x2B\x2C\x2F\x2E\x2D", // d=4
    "\x2B\x2C\x2F\x2D\x2D", // d=5
    "\x2B\x2C\x2E\x2D\x2D", // d=6
    "\x2B\x2C\x2D\x2D\x2D"  // d=7
};
static const char CPI[] PROGMEM = "\x53\x54";
static const char AML[] PROGMEM = "\x58\x59   ";
static const char AML_T[] PROGMEM = "\x5A\x5B";

static void formt_num(int8_t num) {
    if (num < 0) {
        num = -num;
        chmat[1] ++;
    }
    char ten = (num >= 10)? ZERO : BC;
    while (num >= 10) {
        ten ++;
        num -= 10;
    }
    chmat[2] = ten;
    chmat[3] = ZERO + num;
}

static void formt_num_oled(int8_t ch1, int8_t ch2){
    chmat[1] = ch1 ? SLF : BC;
    formt_num(ch1);
    oled_write(chmat, false);
    chmat[0] ++;
    chmat[1] = ch2 ? SDW : BC;
    formt_num(ch2);
    oled_write(chmat, false);
}

void keyball_oledkit_logo(void) {
    oled_write_P(LOGO, false);
}

void keyball_oledkit_blankchar(uint8_t ch) {
    while (ch > 0) {
        oled_write_char(BC, false);
        ch --;
    }
}

void keyball_oledkit_layer(bool master) {
    if (master) {
        for (uint8_t i = 1; i < 6; i++) {
            oled_write_char((layer_state & (1 << i)) ? '0' + i : BL, false);
        }
    } else {
        keyball_oledkit_blankchar(5);
    }
}

void keyball_oledkit_keypress(bool left) {
#if KEYBALL_MODEL == 39
    uint8_t i = 0;
    while (i < 4) {
        matrix_row_t r = matrix_get_row( left ? i : (i + 4) );
        i++;

        if (left) {
            chmat[0] = 0x45 + (r & 1);
            chmat[1] = 0x3F + ((r >> 1) & 3);
            chmat[2] = 0x3F + ((r >> 3) & 3);
            chmat[3] = 0x3E + ((r >> 5) & 1) + ((i >> 2) * 5);
            chmat[4] = BC;
        } else {
            chmat[0] = BC;
            chmat[4] = 0x43 + (r & 1);
            if (i < 4) {
                chmat[1] = BC;
                chmat[2] = 0x3F + ((r >> 2) & 2) + ((r >> 4) & 1);
                chmat[3] = 0x3F + (r & 2) + ((r >> 2) & 1);
            } else {
                chmat[1] = 0x45 + ((r >> 5) & 1);
                chmat[2] = 0x43 + ((r >> 4) & 1);
                chmat[3] = BC;
            }
        }
        oled_write(chmat, false);
    }
#endif
}

void keyball_oledkit_state(void) {
    bool cw = is_caps_word_on();
    led_t led_state = host_keyboard_led_state();
    chmat[0] = cw ? 0x47 : BC;
    chmat[1] = cw ? 0x48 : BC;
    chmat[2] = BC;
    chmat[3] = led_state.caps_lock ? 0x49 : BC;
    chmat[4] = led_state.num_lock  ? 0x4A : BC;
    oled_write(chmat, false);
    chmat[0] = BC;
    chmat[1] = BC;
    chmat[3] = led_state.scroll_lock ? 0x5D : BC;
    chmat[4] = led_state.kana  ? 0x5E : BC;
    oled_write(chmat, false);
}

void keyball_oledkit_mouse(void) {
    chmat[1] = BC;
    chmat[4] = 0;
    formt_num(keyball_get_cpi());
    oled_write(chmat + 1, false);
    oled_write_P(CPI, false);
    chmat[0] = 0x4B;
    chmat[4] = BC;
    formt_num_oled(keyball.last_mouse.x, keyball.last_mouse.y);
}

void keyball_oledkit_scroll(void) {
    chmat[0] = 0x4D;
    chmat[4] = BC;
    oled_write_P(DIV_METER[keyball_get_scroll_div() - 1], false);
    formt_num_oled(keyball.last_mouse.h, - keyball.last_mouse.v);
}

void keyball_oledkit_eeprom(void) {
    bool oled_d = keyball_get_oled_disable();
    led_t led_state = host_keyboard_led_state();
    chmat[0] = oled_d ? BC : 0x55;
    chmat[1] = oled_d ? BC : 0x56;
    chmat[2] = oled_d ? BC : 0x57;
    chmat[3] = led_state.caps_lock ? 0x49 : BC;
    chmat[4] = led_state.num_lock  ? 0x4A : BC;
    oled_write(chmat, false);
    chmat[0] = BC;
    chmat[1] = BC;
    chmat[2] = BC;
    chmat[3] = led_state.scroll_lock ? 0x5D : BC;
    chmat[4] = led_state.kana  ? 0x5E : BC;
    oled_write(chmat, false);

    chmat[0] = 0x4E;
    chmat[4] = 0;
    formt_num(keyball_get_cpi());
    oled_write(chmat + 1, false);
    oled_write_P(CPI, false);

    oled_write_P(DIV_METER[keyball_get_scroll_div() - 1], false);

    bool aml = get_auto_mouse_enable();
    if (aml) {
        oled_write_P(AML, false);
        chmat[0] = BC;
        chmat[1] = BC;
        chmat[4] = 0;
        formt_num(keyball_get_auto_mouse_timeout() / 10);
        oled_write(chmat + 1, false);
        oled_write_P(AML_T, false);

        chmat[4] = 0x5C;
        formt_num(keyball_get_auto_mouse_active_timeout() / 1000);
        oled_write(chmat, false);
    } else keyball_oledkit_blankchar(15);

}


__attribute__((weak)) void keyball_oledkit_render(bool master, bool left) {
}

bool oled_task_kb(void) {
    bool master = is_keyboard_master();
    if (master) {
        bool on = is_oled_on();
        if (keyball.oled_state) {
            if (!on) oled_on();
        } else {
            if (on) oled_off();
            return true;
        }
    }

    keyball_oledkit_render(master, is_keyboard_left());
    return oled_task_user();
}
#endif


//////////////////////////////////////////////////////////////////////////////
// Public API functions

bool keyball_get_scroll_mode(void) {
    return keyball.scroll_mode;
}

void keyball_set_scroll_mode(bool mode) {
    if (mode != keyball.scroll_mode) {
        keyball.scroll_mode_changed = timer_read32();
    }
    keyball.scroll_mode = mode;
}

keyball_scrollsnap_mode_t keyball_get_scrollsnap_mode(void) {
#if KEYBALL_SCROLLSNAP_ENABLE == 2
    return keyball.scrollsnap_mode;
#else
    return 0;
#endif
}

void keyball_set_scrollsnap_mode(keyball_scrollsnap_mode_t mode) {
#if KEYBALL_SCROLLSNAP_ENABLE == 2
    keyball.scrollsnap_mode = mode;
#endif
}

uint8_t keyball_get_scroll_div(void) {
    return keyball.scroll_div;
}

void keyball_set_scroll_div(uint8_t div) {
    keyball.scroll_div = div > SCROLL_DIV_MAX ? SCROLL_DIV_MAX : (div == 0 ? KEYBALL_SCROLL_DIV_DEFAULT : div);
}

uint8_t keyball_get_cpi(void) {
    return keyball.cpi_value;
}

void keyball_set_cpi(uint8_t cpi) {
    if (cpi > CPI_MAX) {
        cpi = CPI_MAX;
    } else if (cpi == 0) {
        cpi = CPI_DEFAULT;
    }
    keyball.cpi_value   = cpi;
    keyball.cpi_changed = true;
    if (keyball.this_have_ball) {
        pmw3360_cpi_set(cpi - 1);
    }
}

#ifdef OLED_ENABLE
bool keyball_get_oled_disable(void) {
    return keyball.oled_disable;
}
void keyball_set_oled_disable(bool flg) {
    keyball.oled_disable = flg;
}
#endif

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
static uint16_t inc_timeout(uint16_t value, uint16_t qu, uint16_t max) {
    value += qu;
    return (value > max) ? max : value;
}

static uint16_t dec_timeout(uint16_t value, uint16_t qu, uint16_t min) {
    return (value > qu) ? value - qu : min;
}

static uint16_t get_timeout(uint16_t value, uint16_t min, uint16_t qu) {
    return ((value - min) / qu) + 1;
}

static uint16_t set_timeout(uint16_t time, uint16_t def, uint16_t min, uint16_t qu, uint16_t max) {
    if (time == 0) return def;
    uint16_t value = min + qu * (time - 1);
    return (value > max) ? max : value;
}

uint16_t keyball_get_auto_mouse_timeout(void) {
    return keyball.aml_timeout;
}

uint16_t keyball_get_auto_mouse_active_timeout(void) {
    return keyball.aml_active_timeout;
}

void keyball_inc_auto_mouse_timeout(void) {
    keyball.aml_timeout = inc_timeout(keyball.aml_timeout, AML_TIMEOUT_QU, AML_TIMEOUT_MAX);
}

void keyball_inc_auto_mouse_active_timeout(void) {
    keyball.aml_active_timeout = inc_timeout(keyball.aml_active_timeout, AML_ACTIVE_TIMEOUT_QU, AML_ACTIVE_TIMEOUT_MAX);
}

void keyball_dec_auto_mouse_timeout(void) {
    keyball.aml_timeout = dec_timeout(keyball.aml_timeout, AML_TIMEOUT_QU, AML_TIMEOUT_MIN);
}

void keyball_dec_auto_mouse_active_timeout(void) {
    keyball.aml_active_timeout = dec_timeout(keyball.aml_active_timeout, AML_ACTIVE_TIMEOUT_QU, AML_ACTIVE_TIMEOUT_MIN);
}

uint16_t keyball_get_aml_to_eeprom(void) {
    return get_timeout(keyball.aml_timeout, AML_TIMEOUT_MIN, AML_TIMEOUT_QU);
}

uint16_t keyball_get_aml_ato_eeprom(void) {
    return get_timeout(keyball.aml_active_timeout, AML_ACTIVE_TIMEOUT_MIN, AML_ACTIVE_TIMEOUT_QU);
}

void keyball_set_aml_to_eeprom(uint16_t time) {
    keyball.aml_timeout = set_timeout(time, AUTO_MOUSE_TIME, AML_TIMEOUT_MIN, AML_TIMEOUT_QU, AML_TIMEOUT_MAX);
}

void keyball_set_aml_ato_eeprom(uint16_t time) {
    keyball.aml_active_timeout = set_timeout(time, AUTO_MOUSE_ACTIVE_TIME, AML_ACTIVE_TIMEOUT_MIN, AML_ACTIVE_TIMEOUT_QU, AML_ACTIVE_TIMEOUT_MAX);
}
#endif

//////////////////////////////////////////////////////////////////////////////
// Keyboard hooks

void keyboard_post_init_kb(void) {
#ifdef SPLIT_KEYBOARD
    // register transaction handlers on secondary.
    if (!is_keyboard_master()) {
        transaction_register_rpc(KEYBALL_GET_INFO, rpc_get_info_handler);
        transaction_register_rpc(KEYBALL_GET_MOTION, rpc_get_motion_handler);
        transaction_register_rpc(KEYBALL_SET_CPI, rpc_set_cpi_handler);
    }
#endif

    // read keyball configuration from EEPROM
    if (is_keyboard_master()) {
        if (eeconfig_is_enabled()) {
            keyball_config_t c = {.raw = eeconfig_read_kb()};
            keyball_set_cpi(c.cpi);
            keyball_set_scroll_div(c.sdiv);
#ifdef OLED_ENABLE
            keyball_set_oled_disable(c.oledd);
#endif
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
            set_auto_mouse_enable(c.amle == 0);
            keyball_set_aml_to_eeprom(c.amlto);
            keyball_set_aml_ato_eeprom(c.amlato);
            set_auto_mouse_timeout(keyball_get_auto_mouse_active_timeout());
#endif
#if KEYBALL_SCROLLSNAP_ENABLE == 2
            keyball_set_scrollsnap_mode(c.ssnap);
#endif
        }
#ifdef LY_EEPROM
        layer_on(LY_EEPROM);
#endif
    }

    keyball_on_adjust_layout(KEYBALL_ADJUST_PENDING);
    keyboard_post_init_user();
}

#if SPLIT_KEYBOARD
void housekeeping_task_kb(void) {
    if (is_keyboard_master()) {
        rpc_get_info_invoke();
        if (keyball.that_have_ball) {
            rpc_get_motion_invoke();
            rpc_set_cpi_invoke();
        }
    }
}
#endif

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
#ifdef LY_EEPROM
    if (get_highest_layer(layer_state) == LY_EEPROM) {
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
#endif
    return pointing_device_task_user(mouse_report);
}

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
bool is_mouse_record_kb(uint16_t keycode, keyrecord_t* record) {
    set_auto_mouse_timeout(
        IS_MOUSEKEY_BUTTON(keycode) ?
        keyball_get_auto_mouse_timeout() : keyball_get_auto_mouse_active_timeout()
    );
    switch (keycode) {
        case SCRL_MO: return true;
    }
    return is_mouse_record_user(keycode, record);
}

bool auto_mouse_activation(report_mouse_t mouse_report) {
    if (get_highest_layer(layer_state) != 0) return false;
    return mouse_report.x != 0 || mouse_report.y != 0 || mouse_report.h != 0 || mouse_report.v != 0 || mouse_report.buttons;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    set_auto_mouse_timeout(keyball_get_auto_mouse_active_timeout());
    return layer_state_set_user(state);
}
#endif

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) {
        return false;
    }

    // strip QK_MODS part.
    if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) {
        keycode &= 0xff;
    }

    switch (keycode) {
#ifndef MOUSEKEY_ENABLE
        // process KC_MS_BTN1~8 by myself
        // See process_action() in quantum/action.c for details.
        case KC_MS_BTN1 ... KC_MS_BTN8: {
            extern void register_mouse(uint8_t mouse_keycode, bool pressed);
            register_mouse(keycode, record->event.pressed);
            // to apply QK_MODS actions, allow to process others.
            return true;
        }
#endif

        case SCRL_MO:
            keyball_set_scroll_mode(record->event.pressed);
            // process_auto_mouse may use this in future, if changed order of
            // processes.
            return true;
    }

    // process events which works on pressed only.
    if (record->event.pressed) {
        switch (keycode) {
            case KBC_RST:
                keyball_set_cpi(0);
                keyball_set_scroll_div(0);
#ifdef OLED_ENABLE
                keyball_set_oled_disable(false);
#endif
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
                set_auto_mouse_enable(true);
                keyball_set_aml_to_eeprom(0);
                keyball_set_aml_ato_eeprom(0);
                set_auto_mouse_timeout(keyball_get_auto_mouse_active_timeout());
#endif
                break;
            case KBC_SAVE: {
                keyball_config_t c = {
                    .cpi   = keyball.cpi_value,
                    .sdiv  = keyball.scroll_div,
#ifdef OLED_ENABLE
                    .oledd = keyball.oled_disable,
#endif
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
                    .amle  = !get_auto_mouse_enable(),
                    .amlto = keyball_get_aml_to_eeprom(),
                    .amlato = keyball_get_aml_ato_eeprom(),
#endif
#if KEYBALL_SCROLLSNAP_ENABLE == 2
                    .ssnap = keyball_get_scrollsnap_mode(),
#endif
                };
                eeconfig_update_kb(c.raw);
            } break;

            case CPI_I100:
                add_cpi(1);
                break;
            case CPI_D100:
                add_cpi(-1);
                break;
            case CPI_I1K:
                add_cpi(10);
                break;
            case CPI_D1K:
                add_cpi(-10);
                break;

            case SCRL_TO:
                keyball_set_scroll_mode(!keyball.scroll_mode);
                break;
            case SCRL_DVI:
                add_scroll_div(1);
                break;
            case SCRL_DVD:
                add_scroll_div(-1);
                break;

#ifdef OLED_ENABLE
            case OLED_TO:
                keyball_set_oled_disable(!keyball_get_oled_disable());
                break;
#endif

#if KEYBALL_SCROLLSNAP_ENABLE == 2
            case SSNP_HOR:
                keyball_set_scrollsnap_mode(KEYBALL_SCROLLSNAP_MODE_HORIZONTAL);
                break;
            case SSNP_VRT:
                keyball_set_scrollsnap_mode(KEYBALL_SCROLLSNAP_MODE_VERTICAL);
                break;
            case SSNP_FRE:
                keyball_set_scrollsnap_mode(KEYBALL_SCROLLSNAP_MODE_FREE);
                break;
#endif

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
            case AML_TO:
                set_auto_mouse_enable(!get_auto_mouse_enable());
                break;
            case AML_I1S:
                {
                    keyball_inc_auto_mouse_active_timeout();
                    set_auto_mouse_timeout(keyball_get_auto_mouse_active_timeout());
                }
                break;
            case AML_D1S:
                {
                    keyball_dec_auto_mouse_active_timeout();
                    set_auto_mouse_timeout(keyball_get_auto_mouse_active_timeout());
                }
                break;
            case AML_KI50:
                {
                    keyball_inc_auto_mouse_timeout();
                }
                break;
            case AML_KD50:
                {
                    keyball_dec_auto_mouse_timeout();
                }
                break;
#endif

            default:
                return true;
        }
        return false;
    }

    return true;
}

void matrix_scan_kb(void) {
#ifdef OLED_ENABLE
    if (is_keyboard_master()) {
        uint16_t now = (uint16_t)(timer_read32() >> 8);
        uint16_t last = (uint16_t)(last_input_activity_time() >> 8);
#  ifdef LY_EEPROM
        bool state = get_highest_layer(layer_state) == LY_EEPROM;
        if (state) {
            if (
#    if KEYBALL_OLED_TIMEOUT > 0
                !timer_expired(last + ((KEYBALL_OLED_TIMEOUT / 6) >> 8), now)
#    else
                !timer_expired(last + 10000, now)
 #    endif
            ) {
                layer_off(LY_EEPROM);
                state = false;
            }
        }
#  endif
        keyball.oled_state =
#  ifdef LY_EEPROM
            state ||
#  endif
            (
                !keyball_get_oled_disable()
#  if KEYBALL_OLED_TIMEOUT > 0
                && timer_expired(last + (KEYBALL_OLED_TIMEOUT >> 8), now)
#  endif
            );
    }
#endif
    return matrix_scan_user();
}

// Disable functions keycode_config() and mod_config() in keycode_config.c to
// reduce size.  These functions are provided for customizing magic keycode.
// These two functions are mostly unnecessary if `MAGIC_KEYCODE_ENABLE = no` is
// set.
//
// If `MAGIC_KEYCODE_ENABLE = no` and you want to keep these two functions as
// they are, define the macro KEYBALL_KEEP_MAGIC_FUNCTIONS.
//
// See: https://docs.qmk.fm/#/squeezing_avr?id=magic-functions
//
#if !defined(MAGIC_KEYCODE_ENABLE) && !defined(KEYBALL_KEEP_MAGIC_FUNCTIONS)

uint16_t keycode_config(uint16_t keycode) {
    return keycode;
}

uint8_t mod_config(uint8_t mod) {
    return mod;
}

#endif
