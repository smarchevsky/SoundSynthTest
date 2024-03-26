#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "synth.h"

#include <iostream>
#include <math.h>
#include <ostream>
#include <vector>

SDL_Window* gWindow = NULL;
const int SAMPLE_RATE = 44100;

void audio_callback(void* user_data, Uint8* raw_buffer, int bytes)
{
    Sint16* buffer = (Sint16*)raw_buffer;
    int length = bytes / 2; // 2 bytes per sample for AUDIO_S16SYS
    auto* player = reinterpret_cast<SimpleSynthPlayer*>(user_data);

    for (int sampleIndex = 0; sampleIndex < length; ++sampleIndex) {
        float val = player->update();
        buffer[sampleIndex] = (Sint16)(32767 * glm::clamp(val, -1.f, 1.f));
    }
}

static const std::string music2 = R"(
a3a3 __ __ __    c3c3 __ __ a3_     __ a3_ d3_ __    a3_ __ g2_ __
a3a3 __ __ __    e3e3 __ __ a3_     __ a3_ f3_ __    e3_ __ c3_ __
a3_  __ e3_ __   a4_ __ a3_ g2_     __ g2_ f2_ __    b3_ __ a3a3 a3a3
a3a3a3a3____     ________           ________         ________

)";

int main(int argc, char* args[])
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gWindow);
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    SimpleSynthPlayer player(SAMPLE_RATE);
    player.parse(music2);

    SDL_AudioSpec audioSpec;
    audioSpec.freq = SAMPLE_RATE;
    audioSpec.format = AUDIO_S16SYS; // sample type (signed short 16 bit)
    audioSpec.channels = 1;
    audioSpec.samples = 2048;
    audioSpec.callback = audio_callback;
    audioSpec.userdata = &player;

    SDL_AudioSpec have;
    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(nullptr, 0, &audioSpec, &have, 0);
    SDL_PauseAudioDevice(audio_device, 0);

    // SDL_PauseAudioDevice(audio_device, 1);

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.key.repeat == 0) {
            }
            if (e.key.keysym.sym == SDLK_q && e.key.keysym.mod & KMOD_CTRL)
                quit = true;
        }
        SDL_RenderPresent(renderer);
    }

    SDL_CloseAudioDevice(audio_device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gWindow);

    Mix_Quit();
    SDL_Quit();
    return 0;
}
