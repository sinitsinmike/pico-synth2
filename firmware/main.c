#include <pico/multicore.h>
#include "synth.h"

static synth_t synth = {
    .stdio_uart = {
        .uart = uart0,
        .baud_rate = 115200,
        .tx = 16,
        .rx = 17,
    },

    .engine.dac = {
        .pio = pio0,
        .state_machine = 0,
        .basepin = 10,
        .with_ldac = true,
    },

    .midi = {
        .uart = uart1,
        .uart_rx = 5,
        .with_usb = true,
    },

    .tui = {
        .i2c = {
            .i2c = i2c1,
            .baudrate = 400000,
            .basepin = 2,
        },

        .eeprom = {
            .ic = PS_TUI_EEPROM_24LC512,
        },

        .encoder = {
            .button = 6,
            .a = 8,
            .b = 7,
            .button_debounce_us = 250000,
            .rotate_debounce_us = 2000,
        },

        .oled = {
            .controller = PS_TUI_OLED_SH1106,
        },
    },

    .channels = {
        {
            .midi_channel = 0,
        },
        {
            .midi_channel = 0,
        },
    },
};


void
ps_midi_channel_cb(ps_midi_command_type_t cmd, uint8_t channel, uint8_t *data, uint8_t data_len)
{
    switch (cmd) {
    case PS_MIDI_COMMAND_NOTE_ON:
        if (data[1] != 0) {
            channel_set_note(&synth, channel, data[0], data[1]);
            break;
        }

    case PS_MIDI_COMMAND_NOTE_OFF:
        channel_unset_note(&synth, channel , data[0]);
        break;
    }
}


static void
main1(void)
{
    synth_core1(&synth);
}


int
main(void)
{
    multicore_launch_core1(main1);
    synth_core0(&synth);
    return 0;
}
