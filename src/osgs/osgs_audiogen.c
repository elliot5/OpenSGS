#include "osgs_audiogen.h"
#include "osgs_logging.h"

#include <stdlib.h>
#define __USE_MISC
#include <math.h>

double wave_phase = 0.0;
double wave_phase_speed = 0.0;

float get_sine_wave_signed()
{
    const double TAO = 2.0 * M_PI;
    return sin(wave_phase * (TAO / 44100.0));
}

float get_square_wave()
{
    if(get_sine_wave_signed() > 0)
    {
        return 1.0;
    } else {
        return -1.0;
    }
}

SAMPLE_T get_triangle_wave()
{
    int m = 2048;
    return (m - abs(((int)(wave_phase)) % (2*m) - m));
}

SAMPLE_T get_wave(waveprp_t wave_properties)
{
    const double MAX_PHASE_SPEED = 1024.0;
    SAMPLE_EXT_T base_wave = 0.0;
    switch (wave_properties.wave_mode) {
        case SINE:
            base_wave = get_sine_wave_signed() * get_sample_amplitude(wave_properties.wave_amplitude);
            break;
        case SQUARE:
            base_wave = get_square_wave() * get_sample_amplitude(wave_properties.wave_amplitude);
            break;
        case TRIANGLE:
            base_wave = get_triangle_wave() * get_sample_amplitude(wave_properties.wave_amplitude);
            break;
    }
    // wave speed
    wave_phase_speed = (wave_properties.wave_frequency * MAX_PHASE_SPEED); // 0 - 1000 phase speed calculation
    wave_phase += wave_phase_speed; // increase phase
    // declip
    int* wave_clipped = 0;
    SAMPLE_T final_wave = declip_wave(base_wave, &wave_clipped);
    // logging and return
    #ifdef OSGS_LOGWAVE
        if(wave_clipped == 0) { OSGS_LOGINFO("%d", final_wave); }
        else { OSGS_LOGINFO("[!] %d", final_wave); }
    #endif
    return final_wave;
}

SAMPLE_T declip_wave(SAMPLE_EXT_T wave, int* clip_result)
{
    SAMPLE_T declipped_wave = 0;
    if(wave > SAMPLE_CLIPMAX)
    {
        declipped_wave = SAMPLE_CLIPMAX;
        *clip_result = 1;
    }
    else if(wave < (SAMPLE_CLIPMAX * -1))
    {
        declipped_wave = (SAMPLE_CLIPMAX * -1);
        *clip_result = -1;
    }
    else
    {
        declipped_wave = (SAMPLE_T)wave;
        *clip_result = 0;
    }
    return declipped_wave;
}



SAMPLE_EXT_T get_sample_amplitude(float wave_amplitude)
{
    return (SAMPLE_MAX * wave_amplitude);
}

