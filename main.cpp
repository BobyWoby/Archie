#include <cctype>
#include <ggml-backend.h>
#include "AudioProcessor.h"
#include <iostream>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "archie/CommandProcessor.h"
#include "whisper.h"
#include "SDL3/SDL_audio.h"



int main(){
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer("Window", 640, 640, SDL_WINDOW_TRANSPARENT, &window, &renderer);


    bool running = true;
    const int lenMS = 30 * 1000; // Record for 30 seconds maximum
    
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    AP::Audio audio;

    // Initialize Whisper
    ggml_backend_load_all();
    whisper_context_params cparams = whisper_context_default_params();
    cparams.flash_attn = false;

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH);


    AP::whisper_params params;
    whisper_context *ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);
    wparams.print_progress   = true;
    wparams.print_special    = params.print_special;
    wparams.print_realtime   = false;
    wparams.print_timestamps = !params.no_timestamps;
    wparams.translate        = params.translate;
    wparams.no_context       = true;
    wparams.no_timestamps    = params.no_timestamps;
    wparams.single_segment   = true;
    wparams.max_tokens       = params.max_tokens;
    wparams.language         = params.language.c_str();
    wparams.n_threads        = params.n_threads;
    wparams.duration_ms = 0;

    wparams.audio_ctx = params.audio_ctx;

    wparams.temperature     = 0.4f;
    wparams.temperature_inc = 1.0f;
    wparams.greedy.best_of  = 5;

    wparams.beam_search.beam_size = 5;

    wparams.initial_prompt = "";

    wparams.suppress_regex = params.suppress_regex.c_str();



    std::vector<float> audioBuffer;
    std::vector<std::string> tokens;
    while(running){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_EVENT_QUIT:
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if(event.key.scancode == SDL_SCANCODE_ESCAPE){
                        running = false;
                        whisper_free(ctx);
                        return 0;
                    }
                    else if(event.key.scancode == SDL_SCANCODE_C){
                        // capture audio
                        SDL_PauseAudioStreamDevice(audio.outStream);
                        SDL_FlushAudioStream(audio.outStream);
                        SDL_ResumeAudioStreamDevice(audio.inStream);
                    }
                    else if(event.key.scancode == SDL_SCANCODE_P){
                        // play audio
                        SDL_PauseAudioStreamDevice(audio.inStream);
                        SDL_FlushAudioStream(audio.inStream);
                        audio.get(20000, audioBuffer);
                        whisper_full(ctx, wparams, audioBuffer.data(), audioBuffer.size());
                        std::string result;

                        const int n_segments = whisper_full_n_segments(ctx);
                        for (int i = 0; i < n_segments; ++i) {
                            const char * text = whisper_full_get_segment_text(ctx, i);
                            result += text;
                       }
                        std::cout << result << std::endl;
                        //split the result into words here, then process them into commands
                        tokens = AP::split(result, " ");
                        tokens.erase(tokens.begin());
                        for(int i = 0; i < tokens.size(); i++){
                            std::string cleanedStr;
                            for(int j = 0; j < tokens.at(i).length(); j++){
                                auto c = tokens.at(i)[j];
                                if(std::isalnum(c) || (c == '.' && j < tokens.at(i).length() - 1)) cleanedStr += std::tolower(c);

                            }
                            //makes removes all non-alphanumeric characters from the string
                            tokens[i] = cleanedStr;
                            //Do something with the cleaned string, like process the command or smt
                        }
                        cmd::Command::parseCommand(tokens);
                        running = false;
                        whisper_free(ctx);
                        return 0;
                        // SDL_ResumeAudioStreamDevice(audio.outStream);
                    }
                    break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
        //do smt with the buffer that was read to
        
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    whisper_free(ctx);
    return 0;
}
