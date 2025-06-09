#pragma once
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "grammar-parser.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <whisper.h>

namespace AP {
    
    struct whisper_params {
        int32_t n_threads  = std::min(4, (int32_t) std::thread::hardware_concurrency());
        int32_t prompt_ms  = 5000;
        int32_t command_ms = 8000;
        int32_t capture_id = -1;
        int32_t max_tokens = 32;
        int32_t audio_ctx  = 0;

        float vad_thold  = 0.6f;
        float freq_thold = 100.0f;



        bool translate     = false;
        bool print_special = false;
        bool print_energy  = false;
        bool no_timestamps = true;
        bool use_gpu       = true;
        bool flash_attn    = false;

        std::string language = "en";
        std::string model = "../vendored/whisper/models/ggml-base.en.bin";
        std::string fname_out;
        std::string commands;
        std::string prompt;
        std::string context;
        std::string grammar;

    // A regular expression that matches tokens to suppress
        std::string suppress_regex;
    };
    void initialize_whisper();
    

    inline std::vector<std::string> split(std::string input, const std::string delim){
        std::string s = input;
        std::vector<std::string> result;
        size_t splitPos = 0;
        std::string tok;
        while((splitPos = input.find(" ")) != std::string::npos){
            result.push_back(input.substr(0, splitPos));
            input.erase(0, splitPos + delim.length());
        }
        result.push_back(input);

        return result;
    }

    class Audio{
        public:
        SDL_AudioStream *outStream, *inStream;
        SDL_AudioSpec spec;
        std::vector<float> m_audio;
        std::mutex       m_mutex;
        uint8_t buf[1024];
        int m_len_ms = 0, m_audio_len = 0, m_audio_pos = 0;

        Audio();
        ~Audio();
        void get(int ms, std::vector<float> &result);

        void SDLCALL callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);
    };

} 
