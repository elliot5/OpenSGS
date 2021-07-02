#include "osgs_config.h"
#include "osgs_logging.h"
#include "osgs_audiogen.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#define __USE_MISC
#include <math.h>
#include <limits.h>
#include <time.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

/* nuklear - 1.32.0 - public domain */
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include <nuklear.h>
#include "nuklear_sdl_gl3.h"

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024


waveprp_t wave_properties;
int play = 0;

void osgs_audio_callback(void* userdata, Uint8* stream, int streamLength) {
    #if SAMPLE_DEPTH == 8
        Sint8* buffer = (Sint8*)stream; /* Signed, 8-Bit */
        Sint8 samples = streamLength / sizeof(Sint8);
    #elif SAMPLE_DEPTH == 16
        Sint16* buffer = (Sint16*)stream; /* Signed, 16-Bit, Least Significant Bit */
        Sint16 samples = streamLength / sizeof(Sint16);
    #elif SAMPLE_DEPTH == 32
        Sint32* buffer = (Sint32*)stream; /* Signed, 32-Bit, Least Significant Bit */
        Sint32 samples = streamLength / sizeof(Sint32);
    #endif

    SAMPLE_EXT_T i;
    for(i = 0; i < samples; i++) {
        if(play) {
            buffer[i] = get_wave(wave_properties);
        } else {
            buffer[i] = 0.0f;
        }
    }
}

SDL_AudioSpec init_sdl_audio()
{
    SDL_AudioDeviceID   default_audio_device = 0;
    int                 sdlaudio_driver_cnt = 0;
    int                 sdlaudio_device_cnt = 0;
    int                 sdlaudio_mode = 0; /* 0 = playback */
    int                 i = 0;

    /* audio specification pre-setup */
    SDL_AudioSpec requested_spec, supplied_spec;
    SDL_zero(requested_spec); /* SDL_Memset 0 */
    #if SAMPLE_DEPTH == 8
        requested_spec.format = AUDIO_S8; /* Signed, 8-Bit */
    #elif SAMPLE_DEPTH == 16
        requested_spec.format = AUDIO_S16LSB; /* Signed, 16-Bit, Least Significant Bit */
    #elif SAMPLE_DEPTH == 32
        requested_spec.format = AUDIO_S32LSB; /* Signed, 32-Bit, Least Significant Bit */
    #endif
    requested_spec.freq = 44100;
    requested_spec.channels = 1;
    requested_spec.samples = 2048;
    requested_spec.callback = osgs_audio_callback;

    /* determine audio drivers */
    sdlaudio_driver_cnt = SDL_GetNumAudioDrivers();
    OSGS_LOGINFO(("[sdl] detected %d sdl audio drivers", sdlaudio_driver_cnt));

    for(i = 0; i < sdlaudio_driver_cnt; i++)
    {
        const char* sdlaudio_driver = SDL_GetAudioDriver(i);
        if(!SDL_AudioInit(sdlaudio_driver))
        {
            OSGS_LOGINFO(("  > success \'%s\'", sdlaudio_driver));
            break;
        } else {
            OSGS_LOGINFO(("  > skipping \'%s\'", sdlaudio_driver));
            continue;
        }
    }
    if(i == sdlaudio_driver_cnt)
    {
        OSGS_LOGERROR(("failed on setup of sdlaudio: %s", SDL_GetError()));
        SDL_Quit();
        return requested_spec;
    }
    OSGS_LOGINFO(("[sdl] chosen audio driver: %s", SDL_GetCurrentAudioDriver()));

    /* determine audio devices */
    sdlaudio_device_cnt = SDL_GetNumAudioDevices(sdlaudio_mode);
    OSGS_LOGINFO(("[sdl] detected %d sdl audio devices", sdlaudio_device_cnt));
    for(i = 0; i < sdlaudio_device_cnt; i++)
    {
        const char* sdlaudio_device = SDL_GetAudioDeviceName(i, sdlaudio_mode);
        OSGS_LOGINFO(("  > detected \'%s\'", sdlaudio_device));
    }

    /* output */
    OSGS_LOGINFO(("[sdl] requested - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d",\
    requested_spec.freq, SDL_AUDIO_ISFLOAT(requested_spec.format), SDL_AUDIO_ISSIGNED(requested_spec.format),\
    SDL_AUDIO_ISBIGENDIAN(requested_spec.format), SDL_AUDIO_BITSIZE(requested_spec.format), requested_spec.channels, requested_spec.samples));

    /* open audio device, allowing changes to the specification, NULL = default device */
    default_audio_device = SDL_OpenAudioDevice(NULL, 0, &requested_spec, &supplied_spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if(!default_audio_device) {
        OSGS_LOGERROR(("[sdl] failed to open audio device: %s", SDL_GetError()));
        SDL_Quit();
        return supplied_spec;
    }

    /* output supplied specification */
    OSGS_LOGINFO(("[sdl] supplied - frequency: %d format: f %d s %d be %d sz %d channels: %d samples: %d",\
    supplied_spec.freq, SDL_AUDIO_ISFLOAT(supplied_spec.format), SDL_AUDIO_ISSIGNED(supplied_spec.format),\
    SDL_AUDIO_ISBIGENDIAN(supplied_spec.format), SDL_AUDIO_BITSIZE(supplied_spec.format), supplied_spec.channels, supplied_spec.samples));

    /* unpause audio device  */
    SDL_PauseAudioDevice(default_audio_device, 0);
    play = 1;

    return supplied_spec;
}

int main(int charc, char* argv[])
{
    /* platform */
    SDL_Window *win;
    SDL_GLContext glContext;
    int win_width, win_height;
    int running = 1;

    /* gui */
    struct nk_context *ctx;
    struct nk_font_atlas *atlas;
    struct nk_font *roboto;

    /* spec */
    SDL_AudioSpec supplied_spec;

    /* sdl hints */
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"); /* ensure compositor is enabled */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0"); /* ensure high-dpi compatability */

    /* init */
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS|SDL_INIT_AUDIO);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    win = SDL_CreateWindow("OpenSGS",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    glContext = SDL_GL_CreateContext(win);
    SDL_GetWindowSize(win, &win_width, &win_height);

    /* opengl setup */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    /* sdl init */
    ctx = nk_sdl_init(win);
    nk_sdl_font_stash_begin(&atlas);
    roboto = nk_font_atlas_add_from_file(atlas, "./Nuklear/extra_font/Roboto-Regular.ttf", 16, 0);
    nk_sdl_font_stash_end();
    nk_style_set_font(ctx, &roboto->handle);

    /* sdl audio */
    supplied_spec = init_sdl_audio();
    if(supplied_spec.callback == NULL)
    {
        OSGS_LOGERROR(("NULL Callback not allowed, exiting"));
        exit(1);
    }

    while (running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) goto cleanup;
            nk_sdl_handle_event(&evt);
        } nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "OpenSGS Wave Generator", nk_rect(140, 50, 345, 285),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            char* playbtn_label = "Play";
            nk_layout_row_static(ctx, 30, 300, 1);
            if(play)
            {
                playbtn_label = "Pause";
            }
            if (nk_button_label(ctx, playbtn_label)) {
                if(play)
                {
                    play = 0;
                } else {
                    play = 1;
                }
            }
            nk_label(ctx, "Frequency:", NK_TEXT_LEFT);
            nk_slider_float(ctx, 0, &wave_properties.wave_frequency, 1.0f, 0.01f);
            nk_label(ctx, "Amplitude: ", NK_TEXT_LEFT);
            nk_slider_float(ctx, 0, &wave_properties.wave_amplitude, 2.0f, 0.01f);   

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "Sine", wave_properties.wave_mode == SINE)) { wave_properties.wave_mode = SINE; }
            if (nk_option_label(ctx, "Square", wave_properties.wave_mode == SQUARE)) { wave_properties.wave_mode = SQUARE; }
            if (nk_option_label(ctx, "Triangle", wave_properties.wave_mode == TRIANGLE)) { wave_properties.wave_mode = TRIANGLE; }
            nk_layout_row_dynamic(ctx, 22, 1);

        }
        nk_end(ctx);

        /* draw */
        SDL_GetWindowSize(win, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
        SDL_GL_SwapWindow(win);
    }

cleanup:
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
