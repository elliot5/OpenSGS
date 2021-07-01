#include <stdint.h>

// Sometimes audio will break due to SDL request != supplied
// Later this will be automatically tuned yet for now
// You can manually set the flag SAMPLE_DEPTH e.g
// for something compatible for your system (8,16,32)
// #define SAMPLE_DEPTH 16

// SAMPLE_EXT_T is a large number used to represent waves
// a order of magnitude higher then the original smaple
// If SAMPLE_DEPTH not set, assume 32-bit
#ifndef SAMPLE_DEPTH
    #define SAMPLE_DEPTH 32
#endif
// Set SAMPLE_T to correct SAMPLE_DEPTH
#if SAMPLE_DEPTH == 8
    #define SAMPLE_T int8_t
    #define SAMPLE_MAX 127 // 127
    #define SAMPLE_CLIPMAX 120
    #define SAMPLE_EXT_T int16_t
#elif SAMPLE_DEPTH == 16
    #define SAMPLE_T int16_t
    #define SAMPLE_MAX 32767 // 32,767
    #define SAMPLE_CLIPMAX 32000
    #define SAMPLE_EXT_T int32_t
#elif SAMPLE_DEPTH == 32
    #define SAMPLE_T int32_t
    #define SAMPLE_MAX 2147483647 // 2,147,483,647
    #define SAMPLE_CLIPMAX 2000000000
    #define SAMPLE_EXT_T int64_t
#else
    #error "INVALID SAMPLE_DEPTH [8, 16, 32]"
#endif

typedef enum {
    SINE,
    SQUARE,
    TRIANGLE
} wave_t;

typedef struct waveprp_s {
    wave_t wave_mode;
    float wave_frequency;
    float wave_amplitude;
} waveprp_t;

float get_sine_wave_signed();

SAMPLE_T get_sine_wave();

float get_square_wave();

SAMPLE_T get_triangle_wave();

SAMPLE_T get_wave(waveprp_t wave_properties);

SAMPLE_EXT_T get_sample_amplitude(float wave_amplitude);

SAMPLE_T declip_wave(SAMPLE_EXT_T wave, int* clip_result);
