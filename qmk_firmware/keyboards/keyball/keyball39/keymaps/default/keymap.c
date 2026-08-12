/*
Copyright 2022 @Yowkees
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

#include QMK_KEYBOARD_H

#include "quantum.h"
#include "keymap_japanese.h"

#ifdef COMBO_ENABLE
enum {
    CM_MSST,
    CM_EEPR,
    CM_LPRN,
    CM_RPRN,
    CM_LBRC,
    CM_RBRC,
    CM_LCBR,
    CM_RCBR,
    CM_MB3,
    CM_F11,
    CM_F12,
    CM_GRAV,
    CM_CAPS,
    CM_INSE,
    CM_NUML,
    CM_SYSR,
    CM_PUBR,
    CM_SLCK,
    CM_KANA,
};

const uint16_t PROGMEM P_CM_MSST[] = {KC_ESC, KC_LGUI, COMBO_END};
const uint16_t PROGMEM P_CM_EEPR[] = {KC_ESC, KC_LALT, COMBO_END};
const uint16_t PROGMEM P_CM_LPRN[] = {JP_7, JP_8, COMBO_END};
const uint16_t PROGMEM P_CM_RPRN[] = {JP_8, JP_9, COMBO_END};
const uint16_t PROGMEM P_CM_LBRC[] = {JP_4, JP_5, COMBO_END};
const uint16_t PROGMEM P_CM_RBRC[] = {JP_5, JP_6, COMBO_END};
const uint16_t PROGMEM P_CM_LCBR[] = {JP_1, JP_2, COMBO_END};
const uint16_t PROGMEM P_CM_RCBR[] = {JP_2, JP_3, COMBO_END};
const uint16_t PROGMEM P_CM_MB3[]  = {KC_BTN1, KC_BTN2, COMBO_END};
const uint16_t PROGMEM P_CM_F11[]  = {KC_F8, KC_F9, COMBO_END};
const uint16_t PROGMEM P_CM_F12[]  = {KC_F9, KC_F10, COMBO_END};
const uint16_t PROGMEM P_CM_GRAV[] = {KC_F1, KC_F2, COMBO_END};
const uint16_t PROGMEM P_CM_CAPS[] = {KC_F2, KC_F3, COMBO_END};
const uint16_t PROGMEM P_CM_INSE[] = {KC_F3, KC_F4, COMBO_END};
const uint16_t PROGMEM P_CM_NUML[] = {KC_F4, KC_F5, COMBO_END};
const uint16_t PROGMEM P_CM_PUBR[] = {KC_F1, KC_F2, KC_F3, COMBO_END};
const uint16_t PROGMEM P_CM_SLCK[] = {KC_F2, KC_F3, KC_F4, COMBO_END};
const uint16_t PROGMEM P_CM_KANA[] = {KC_F3, KC_F4, KC_F5, COMBO_END};

combo_t key_combos[] = {
    [CM_MSST] = COMBO(P_CM_MSST, TG(LY_MOUSE_ST)),
    [CM_EEPR] = COMBO(P_CM_EEPR, TG(LY_EEPROM)),
    [CM_LPRN] = COMBO(P_CM_LPRN, JP_LPRN),
    [CM_RPRN] = COMBO(P_CM_RPRN, JP_RPRN),
    [CM_LBRC] = COMBO(P_CM_LBRC, JP_LBRC),
    [CM_RBRC] = COMBO(P_CM_RBRC, JP_RBRC),
    [CM_LCBR] = COMBO(P_CM_LCBR, JP_LCBR),
    [CM_RCBR] = COMBO(P_CM_RCBR, JP_RCBR),
    [CM_MB3]  = COMBO(P_CM_MB3 , KC_BTN3),
    [CM_F11]  = COMBO(P_CM_F11 , KC_F11),
    [CM_F12]  = COMBO(P_CM_F12 , KC_F12),
    [CM_GRAV] = COMBO(P_CM_GRAV, JP_ZKHK),
    [CM_CAPS] = COMBO(P_CM_CAPS, KC_CAPS),
    [CM_INSE] = COMBO(P_CM_INSE, KC_INS),
    [CM_NUML] = COMBO(P_CM_NUML, KC_NUM_LOCK),
    [CM_PUBR] = COMBO(P_CM_PUBR, KC_PAUSE),
    [CM_SLCK] = COMBO(P_CM_SLCK, KC_SCROLL_LOCK),
    [CM_KANA] = COMBO(P_CM_KANA, JP_KANA),
};
#endif

#ifdef KEY_OVERRIDE_ENABLE
const key_override_t sclm_coln_ko  = ko_make_basic(MOD_MASK_SHIFT, JP_SCLN, JP_COLN);
const key_override_t minus_unds_ko = ko_make_basic(MOD_MASK_SHIFT, JP_MINS, JP_UNDS);
const key_override_t prtsc_winp_ko = ko_make_basic(MOD_MASK_SHIFT, KC_PSCR, S(G(KC_S)));

const key_override_t **key_overrides = (const key_override_t *[]){
    &sclm_coln_ko,
    &minus_unds_ko,
    &prtsc_winp_ko,
    NULL
};
#endif

#ifdef TAP_DANCE_ENABLE
enum {
    TD_L1_BTN1 = 0,
    TD_L1_BTN1_MS
};
void l1btn1_finished(tap_dance_state_t *state, void *user_data);
void l1btn1_reset(tap_dance_state_t *state, void *user_data);
void l1btn1_ms_finished(tap_dance_state_t *state, void *user_data);
void l1btn1_ms_reset(tap_dance_state_t *state, void *user_data);
tap_dance_action_t tap_dance_actions[] = {
    [TD_L1_BTN1]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, l1btn1_finished, l1btn1_reset),
    [TD_L1_BTN1_MS] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, l1btn1_ms_finished, l1btn1_ms_reset),
};
#endif

#define L1_BTN1 TD(TD_L1_BTN1)
#define L1_BTN1_MS TD(TD_L1_BTN1_MS)
#define L1_BTN2 LT(LY_SYMBOL,KC_SPC)
#define L1_BTN3 LCTL_T(KC_TAB)

#define R1_BTN1 RCTL_T(KC_BSPC)
#define R1_BTN2 LSFT_T(KC_ENT)
#define R1_BTN6 RSFT_T(KC_DEL)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LY_MAIN] = LAYOUT_universal(
    JP_Q     , JP_W     , JP_E     , JP_R     , JP_T     ,                            JP_Y     , JP_U     , JP_I     , JP_O     , JP_P     ,
    JP_A     , JP_S     , JP_D     , JP_F     , JP_G     ,                            JP_H     , JP_J     , JP_K     , JP_L     , JP_MINS  ,
    JP_Z     , JP_X     , JP_C     , JP_V     , JP_B     ,                            JP_N     , JP_M     , JP_COMM  , JP_DOT   , JP_SCLN  ,
    KC_ESC   , KC_LGUI  , KC_LALT  , L1_BTN3  , L1_BTN2  , L1_BTN1  ,      R1_BTN1  , R1_BTN2  , _______  , _______  , _______  , R1_BTN6
  ),

  [LY_SYMBOL] = LAYOUT_universal(
    JP_EXLM  , JP_QUES  , JP_EQL   , JP_QUOT  , JP_TILD  ,                            KC_PSCR  , JP_7     , JP_8     , JP_9     , JP_PLUS  ,
    JP_AMPR  , JP_HASH  , JP_DLR   , JP_DQUO  , JP_GRV   ,                            JP_YEN   , JP_4     , JP_5     , JP_6     , JP_ASTR  ,
    JP_AT    , JP_PERC  , JP_CIRC  , JP_PIPE  , JP_BSLS  ,                            XXXXXXX  , JP_1     , JP_2     , JP_3     , JP_SLSH  ,
    CW_TOGG  , _______  , _______  , _______  , _______  , _______  ,      _______  ,  JP_0    , _______  , _______  , _______  , _______
  ),

  [LY_MOUSE] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , KC_BTN1  , _______ , KC_BTN2  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),

  [LY_MOUSE_ST] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                            KC_PSCR  , KC_PGUP  , KC_WH_U  , KC_PGDN  , KC_RALT  ,
    C(JP_A)  , C(JP_S)  , C(JP_D)  , C(JP_F)  , C(JP_G)  ,                            KC_HOME  , KC_BTN1  , KC_WH_D  , KC_BTN2  , KC_END   ,
    C(JP_Z)  , C(JP_X)  , C(JP_C)  , C(JP_V)  , KC_APP   ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    _______  , _______  , _______  , _______  , KC_SPC   , L1_BTN1_MS,     _______ ,  _______  , _______  , _______  , _______  , _______
  ),

  [LY_FN] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                            KC_PSCR  , KC_PGUP  , KC_UP    , KC_PGDN  , KC_RALT  ,
    C(JP_A)  , C(JP_S)  , C(JP_D)  , C(JP_F)  , C(JP_G)  ,                            KC_HOME  , KC_LEFT  , KC_DOWN  , KC_RGHT  , KC_END   ,
    C(JP_Z)  , C(JP_X)  , C(JP_C)  , C(JP_V)  , KC_APP   ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    _______  , _______  , _______  , _______  , _______  , XXXXXXX  ,     _______  ,  _______  , _______  , _______  , _______  , _______
  ),

  [LY_EEPROM] = LAYOUT_universal(
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , QK_BOOT  ,                            QK_BOOT  , AML_TO   , XXXXXXX  , XXXXXXX  , OLED_TO  ,
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  ,                            CPI_I100 , AML_I1S  , AML_KI50 , XXXXXXX  , SCRL_DVD ,
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  ,                            CPI_D100 , AML_D1S  , AML_KD50 , XXXXXXX  , SCRL_DVI ,
    TO(0)    , XXXXXXX  , XXXXXXX  , XXXXXXX  , KBC_RST  , KBC_SAVE ,      KBC_SAVE , KBC_RST  , _______  , _______  , _______  , TO(0)
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    if(layer_state_cmp(state, LY_SYMBOL) && layer_state_cmp(state, LY_MOUSE)) {
        state = remove_auto_mouse_layer(state, false);
    }
#endif
    uint8_t layer = get_highest_layer(state);
    // Auto enable scroll mode when the highest layer is LY_FN
    keyball_set_scroll_mode(layer == LY_FN);
    return state;
}

#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
bool auto_mouse_activation_ly(report_mouse_t mouse_report) {
    uint8_t layer = get_highest_layer(layer_state);
    return layer == LY_MAIN || layer == LY_MOUSE;
}
#endif

#ifdef COMBO_SHOULD_TRIGGER
bool combo_should_trigger(uint16_t combo_index, combo_t *combo, uint16_t keycode, keyrecord_t *record) {
    uint8_t layer = get_highest_layer(layer_state);
    switch (combo_index) {
        case CM_MSST ... CM_EEPR: return layer == LY_MAIN;
        case CM_LPRN ... CM_RCBR: return layer == LY_SYMBOL;
        case CM_MB3:              return layer == LY_MOUSE || layer == LY_MOUSE_ST;
        case CM_F11 ... CM_KANA:  return layer == LY_FN;
    }
    return false;
}
#endif

#ifdef OLED_ENABLE
    void keyball_oledkit_render(bool master, bool left) {
    keyball_oledkit_logo();
    keyball_oledkit_layer(master);
    keyball_oledkit_keypress(left);

    uint8_t bl = 45;
    if (master) {
        switch (get_highest_layer(layer_state)) {
            case LY_MAIN:
            case LY_SYMBOL:
                keyball_oledkit_state();
                bl = 35;
                break;
            case LY_MOUSE:
            case LY_MOUSE_ST:
                keyball_oledkit_mouse();
                bl = 30;
                break;
            case LY_FN:
                keyball_oledkit_scroll();
                bl = 30;
                break;
            case LY_EEPROM:
                keyball_oledkit_eeprom();
                bl = 10;
                break;
        }
    }
    keyball_oledkit_blankchar(bl);
}
#endif

#ifdef TAP_DANCE_ENABLE
typedef struct {
    bool is_press_action;
    int state;
} tap;

enum {
    TD_NONE,
    SINGLE_TAP,
    SINGLE_HOLD,
    DOUBLE_TAP,
};

int cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || state->pressed) return SINGLE_HOLD;
        else return SINGLE_TAP;
    } else if (state->count == 2) {
        return DOUBLE_TAP;
    } else return TD_NONE;
}

static tap l1btn1_state;

void l1btn1_finished(tap_dance_state_t *state, void *user_data) {
    l1btn1_state.state = cur_dance(state);
    switch (l1btn1_state.state) {
        case SINGLE_TAP: register_code16(JP_MHEN); break;
        case SINGLE_HOLD: layer_on(LY_FN); break;
        case DOUBLE_TAP: register_code16(JP_HENK); break;
    }
}

void l1btn1_reset(tap_dance_state_t *state, void *user_data) {
    switch (l1btn1_state.state) {
        case SINGLE_TAP: unregister_code16(JP_MHEN); break;
        case SINGLE_HOLD: layer_off(LY_FN); break;
        case DOUBLE_TAP: unregister_code16(JP_HENK); break;
    }
    l1btn1_state.state = 0;
}

static tap l1btn1_ms_state;

void l1btn1_ms_finished(tap_dance_state_t *state, void *user_data) {
    l1btn1_ms_state.state = cur_dance(state);
    switch (l1btn1_ms_state.state) {
        case SINGLE_TAP: layer_invert(LY_MOUSE_ST);  break;
        case SINGLE_HOLD: layer_on(LY_FN); break;
        case DOUBLE_TAP: layer_invert(LY_MOUSE_ST); break;
    }
}

void l1btn1_ms_reset(tap_dance_state_t *state, void *user_data) {
    switch (l1btn1_ms_state.state) {
        case SINGLE_HOLD: layer_off(LY_FN); break;
    }
    l1btn1_ms_state.state = 0;
}
#endif

#ifdef CAPS_WORD_ENABLE
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case JP_A ... JP_Z:
            add_weak_mods(MOD_BIT(KC_LSFT));
        case JP_1 ... JP_0:
        case JP_MINS:
        case JP_UNDS:
        case KC_BSPC:
        case KC_DEL:
        case KC_LSFT:
        case KC_RSFT:
            return true;
        case JP_SCLN:
            return (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    }
    return false;
}
#endif
