#pragma once

#define MATRIX_ROWS 8
#define MATRIX_COLS 5

#define DIRECT_PINS { \
    { GP13, GP28, GP12, GP29, GP0 }, \
    { GP22, GP14, GP26, GP4,  GP27}, \
    { GP21, GP23, GP7, GP20, GP6 }, \
    { GP16, GP9, GP8, NO_PIN, NO_PIN } \
}

#define DIRECT_PINS_RIGHT {  \
    { GP0, GP29, GP12, GP28, GP13 }, \
    { GP27, GP4, GP26, GP14, GP22}, \
    { GP6, GP20, GP7, GP23, GP21 }, \
    { GP8, GP9, GP16, NO_PIN, NO_PIN } \
}

/* Debounce reduces chatter (unintended double-presses) */
#define DEBOUNCE 5

/* Serial Config */
#define USE_SERIAL
#define SERIAL_USART_TX_PIN GP1
#define MASTER_LEFT

/* Tapping terms */
#define TAPPING_TERM 200
#define QUICK_TAP_TERM 150

/* Hold only accepts taps from opposite hands */
#define CHORDAL_HOLD

/* Avoid accidental holds by registering a tap even when holding, then tapping another key within tapping term */
#define PERMISSIVE_HOLD

/* One shot modifiers */
#define ONESHOT_TAP_TOGGLE 2
#define ONESHOT_TIMEOUT 5000

#define USB_VBUS_PIN 19
