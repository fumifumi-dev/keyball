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

#include "modifiers.h"
#include QMK_KEYBOARD_H

#include "quantum.h"
#include "keymap_japanese.h"

#ifdef COMBO_ENABLE
const uint16_t PROGMEM CM_LPRN[] = {JP_7, JP_8, COMBO_END};
const uint16_t PROGMEM CM_RPRN[] = {JP_8, JP_9, COMBO_END};
const uint16_t PROGMEM CM_LBRC[] = {JP_4, JP_5, COMBO_END};
const uint16_t PROGMEM CM_RBRC[] = {JP_5, JP_6, COMBO_END};
const uint16_t PROGMEM CM_LCBR[] = {JP_1, JP_2, COMBO_END};
const uint16_t PROGMEM CM_RCBR[] = {JP_2, JP_3, COMBO_END};
const uint16_t PROGMEM TO_PROP[] = {CW_TOGG, KC_LGUI, COMBO_END};

combo_t key_combos[] = {
    COMBO(CM_LPRN, JP_LPRN),
    COMBO(CM_RPRN, JP_RPRN),
    COMBO(CM_LBRC, JP_LBRC),
    COMBO(CM_RBRC, JP_RBRC),
    COMBO(CM_LCBR, JP_LCBR),
    COMBO(CM_RCBR, JP_RCBR),
    COMBO(TO_PROP, TO(5)),
};
#endif

#ifdef KEY_OVERRIDE_ENABLE
const key_override_t sclm_coln_ko = ko_make_basic(MOD_MASK_SHIFT, JP_SCLN, JP_COLN);
const key_override_t minus_unds_ko = ko_make_basic(MOD_MASK_SHIFT, JP_MINS, JP_UNDS);

const key_override_t **key_overrides = (const key_override_t *[]){
    &sclm_coln_ko,
    &minus_unds_ko,
    NULL
};
#endif

#ifdef TAP_DANCE_ENABLE
enum {
    TD_L1_BTN3 = 0,
};
void l1btn3_finished(tap_dance_state_t *state, void *user_data);
void l1btn3_reset(tap_dance_state_t *state, void *user_data);
tap_dance_action_t tap_dance_actions[] = {
    [TD_L1_BTN3] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, l1btn3_finished, l1btn3_reset),
};
#endif

#define L1_BTN1 LCTL_T(KC_TAB)
#define L1_BTN2 LT(1,KC_SPC)
#define L1_BTN3 TD(TD_L1_BTN3)

#define R1_BTN1 RCTL_T(KC_BSPC)
#define R1_BTN2 LSFT_T(KC_ENT)

#define JP_F_TS LSFT_T(JP_F)
#define JP_J_TS RSFT_T(JP_J)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [0] = LAYOUT_universal(
    JP_Q     , JP_W     , JP_E     , JP_R     , JP_T     ,                            JP_Y     , JP_U     , JP_I     , JP_O     , JP_P     ,
    JP_A     , JP_S     , JP_D     , JP_F     , JP_G     ,                            JP_H     , JP_J     , JP_K     , JP_L     , JP_MINS  ,
    JP_Z     , JP_X     , JP_C     , JP_V     , JP_B     ,                            JP_N     , JP_M     , JP_COMM  , JP_DOT   , JP_SCLN  ,
    CW_TOGG  , KC_LGUI  , KC_LALT  , L1_BTN3  , L1_BTN2  , L1_BTN1  ,      R1_BTN1  , R1_BTN2  , _______  , _______  , _______  , LT(2,KC_DEL)
  ),

  [1] = LAYOUT_universal(
    JP_EXLM  , JP_QUES  , JP_EQL   , JP_QUOT  , JP_TILD  ,                            JP_YEN   , JP_7     , JP_8     , JP_9     , JP_ASTR  ,
    JP_AMPR  , JP_HASH  , JP_DLR   , JP_DQUO  , JP_GRV   ,                            LAG(JP_K), JP_4     , JP_5     , JP_6     , JP_PLUS  ,
    JP_AT    , JP_PERC  , JP_CIRC  , JP_PIPE  , JP_BSLS  ,                            KC_PSCR  , JP_1     , JP_2     , JP_3     , JP_SLSH  ,
    KC_ESC   , _______  , _______  , _______  , _______  , _______  ,      _______  ,  JP_0    , _______  , _______  , _______  , TO(2)
  ),

  [2] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                            KC_F6    , KC_F7    , KC_F8    , KC_F9    , KC_F10   ,
    TO(0)    , JP_ZKHK  , KC_CAPS  , XXXXXXX  , XXXXXXX  ,                            XXXXXXX  , KC_PSCR  , XXXXXXX  , KC_F11   , KC_F12   ,
    KC_HOME  , KC_PGUP  , KC_PGDN  , KC_END   , XXXXXXX  ,                            XXXXXXX  , KC_SCRL  , KC_PAUS  , KC_INS   , KC_RALT   ,
    KC_ESC   , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , _______
  ),

  [3] = LAYOUT_universal(
    KC_F1    , KC_F2    , KC_F3    , KC_F4    , KC_F5    ,                            KC_F6    , KC_HOME  , KC_UP    , KC_END   , KC_F10   ,
    _______  , _______  ,KC_MS_WH_UP,KC_MS_WH_DOWN,KC_APP,                            C(JP_V)  , KC_LEFT  , KC_DOWN  , KC_RGHT  , C(JP_X)  ,
    _______  , _______  , KC_PGUP  , KC_PGDN  , _______  ,                            C(JP_C)  , KC_F7    , KC_F8    , KC_F9    , G(JP_V)  ,
    KC_ESC   , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , KC_DEL
  ),

  [4] = LAYOUT_universal(
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , KC_BTN1  , KC_BTN3  , KC_BTN2  , _______  ,
    _______  , _______  , _______  , _______  , _______  ,                            _______  , _______  , _______  , _______  , _______  ,
    KC_LSFT  , _______  , _______  , _______  , _______  , _______  ,      _______  , _______  , _______  , _______  , _______  , KC_DEL
  ),

  [5] = LAYOUT_universal(
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , QK_BOOT  ,                            QK_BOOT  , AML_TO   , XXXXXXX  , XXXXXXX  , OLED_TO  ,
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  ,                            CPI_I100 , AML_I1S  , AML_KI50 , XXXXXXX  , SCRL_DVD ,
    XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  , XXXXXXX  ,                            CPI_D100 , AML_D1S  , AML_KI50 , XXXXXXX  , SCRL_DVI ,
    TO(0)    , XXXXXXX  , XXXXXXX  , XXXXXXX  , KBC_RST  , KBC_SAVE ,      KBC_SAVE , KBC_RST  , _______  , _______  , _______  , TO(0)
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(layer == 3);
    return state;
}

#ifdef TAP_DANCE_ENABLE
typedef struct {
    bool is_press_action;
    int state;
} tap;

enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD = 2,
    DOUBLE_TAP = 3,
};

int cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || state->pressed) return SINGLE_HOLD;
        else return SINGLE_TAP;
    } else if (state->count == 2) {
        return DOUBLE_TAP;
    } else return 8;
}

static tap l1btn3_state;

void l1btn3_finished(tap_dance_state_t *state, void *user_data) {
    l1btn3_state.state = cur_dance(state);
    switch (l1btn3_state.state) {
        case SINGLE_TAP: register_code(JP_MHEN); break;
        case SINGLE_HOLD: layer_on(3); break;
        case DOUBLE_TAP: register_code(JP_HENK); break;
    }
}

void l1btn3_reset(tap_dance_state_t *state, void *user_data) {
    switch (l1btn3_state.state) {
        case SINGLE_TAP: unregister_code(JP_MHEN); break;
        case SINGLE_HOLD: layer_off(3); break;
        case DOUBLE_TAP: unregister_code(JP_HENK); break;
    }
    l1btn3_state.state = 0;
}
#endif

#ifdef CAPS_WORD_ENABLE
bool caps_word_press_user(uint16_t keycode) {
    bool shift = (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
    switch (keycode) {
        case JP_A ... JP_Z:
            add_weak_mods(MOD_BIT(KC_LSFT));
        case JP_1 ... JP_0:
        case JP_MINS:
        case JP_UNDS:
        case KC_BSPC:
        case KC_DEL:
        case KC_LSFT:
            return true;
        case JP_SCLN:
            if (shift) return true;
    }
    return false;
}
#endif
