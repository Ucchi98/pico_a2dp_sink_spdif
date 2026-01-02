/*
 * Copyright (C) 2022 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL BLUEKITCHEN
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

#define BTSTACK_FILE__ "btstack_audio_pico.c"

/*
 *  btstack_audio_pico.c
 *
 *  Implementation of btstack_audio.h using pico_i2s
 *
 */

#include "btstack_config.h"

#include "btstack_debug.h"
#include "btstack_audio.h"
#include "btstack_run_loop.h"

#include <stddef.h>
#include <hardware/dma.h>

#include "spdif.h"

#define DRIVER_POLL_INTERVAL_MS          5
#define SAMPLES_PER_BUFFER 512

// client
static void (*playback_callback)(int16_t * buffer, uint16_t num_samples);

// timer to fill output ring buffer
static btstack_timer_source_t  driver_timer_sink;
static bool btstack_audio_pico_audio_buffer_pool = false;

static bool btstack_audio_pico_sink_active;

// from pico-playground/audio/sine_wave/sine_wave.c

static uint8_t btstack_audio_pico_channel_count;

static int16_t audio_buffer[SAMPLES_PER_BUFFER*2];
static void btstack_audio_pico_sink_fill_buffers(void){

    (*playback_callback)(audio_buffer, SAMPLES_PER_BUFFER);

    // duplicate samples for mono
    if (btstack_audio_pico_channel_count == 1){
        int16_t i;
        for (i = SAMPLES_PER_BUFFER - 1 ; i >= 0; i--){
            audio_buffer[2*i  ] = audio_buffer[i];
            audio_buffer[2*i+1] = audio_buffer[i];
        }
    }
    spdif_write(audio_buffer, SAMPLES_PER_BUFFER * 2 * 2);
}

static void driver_timer_handler_sink(btstack_timer_source_t * ts){

    // refill
    btstack_audio_pico_sink_fill_buffers();

    // re-set timer
    btstack_run_loop_set_timer(ts, DRIVER_POLL_INTERVAL_MS);
    btstack_run_loop_add_timer(ts);
}

static int btstack_audio_pico_sink_init(
    uint8_t channels,
    uint32_t samplerate, 
    void (*playback)(int16_t * buffer, uint16_t num_samples)
){
    btstack_assert(playback != NULL);
    btstack_assert(channels != 0);

    playback_callback  = playback;

    if (!btstack_audio_pico_audio_buffer_pool) {
        spdif_init(samplerate);
        btstack_audio_pico_channel_count = channels;
        btstack_audio_pico_audio_buffer_pool = true;
    }
    return 0;
}

static void btstack_audio_pico_sink_set_volume(uint8_t volume){
    UNUSED(volume);
}

static void btstack_audio_pico_sink_start_stream(void){

    // start timer
    btstack_run_loop_set_timer_handler(&driver_timer_sink, &driver_timer_handler_sink);
    btstack_run_loop_set_timer(&driver_timer_sink, DRIVER_POLL_INTERVAL_MS);
    btstack_run_loop_add_timer(&driver_timer_sink);

    // state
    btstack_audio_pico_sink_active = true;
}

static void btstack_audio_pico_sink_stop_stream(void){

    // stop timer
    btstack_run_loop_remove_timer(&driver_timer_sink);
    // state
    btstack_audio_pico_sink_active = false;
}

static void btstack_audio_pico_sink_close(void){
    // stop stream if needed
    if (btstack_audio_pico_sink_active){
        btstack_audio_pico_sink_stop_stream();
    }
}

static const btstack_audio_sink_t btstack_audio_pico_sink = {
    .init = &btstack_audio_pico_sink_init,
    .set_volume = &btstack_audio_pico_sink_set_volume,
    .start_stream = &btstack_audio_pico_sink_start_stream,
    .stop_stream = &btstack_audio_pico_sink_stop_stream,
    .close = &btstack_audio_pico_sink_close,
};

const btstack_audio_sink_t * btstack_audio_pico_sink_get_instance(void){
    return &btstack_audio_pico_sink;
}
