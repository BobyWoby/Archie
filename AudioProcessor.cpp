#include "AudioProcessor.h"
#include "SDL3/SDL_audio.h"
#include "SDL3/SDL_stdinc.h"
#include "whisper.h"
#include <ggml-backend.h>
#include <mutex>

using namespace AP;

void initialize_whisper(){
    ggml_backend_load_all();

    whisper_params params;
    whisper_context_params cparams = whisper_context_default_params();

    cparams.use_gpu = params.use_gpu;
    cparams.flash_attn = params.flash_attn;

    struct whisper_context *ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);

}

void split(std::string input, std::vector<std::string> &result){
    std::string substr;
    size_t splitPos;
    while((splitPos = input.find(" ")) != std::string::npos){
        result.push_back(input.substr(0, splitPos));
        input = input.substr(splitPos, input.length());
    }
}

Audio::Audio(){
    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_RECORDING, NULL);
    SDL_GetAudioDeviceFormat(dev, &spec, NULL);
    spec.channels = 1;
    spec.freq = WHISPER_SAMPLE_RATE;
    spec.format = SDL_AUDIO_F32;

    outStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    inStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_RECORDING, &spec, [](void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount){
            Audio *audio = (Audio*)userdata;
            audio->callback(userdata, stream, additional_amount, total_amount);
            }, this);

    SDL_PauseAudioStreamDevice(outStream);
    // SDL_PauseAudioStreamDevice(inStream);
    SDL_ResumeAudioStreamDevice(inStream);
    // SDL_ResumeAudioStreamDevice(outStream);

    m_len_ms = 30 * 1000;
    m_audio_pos = 0;

    m_audio.resize(m_len_ms * WHISPER_SAMPLE_RATE / 1000);
}
Audio::~Audio(){
    SDL_DestroyAudioStream(inStream);
    SDL_DestroyAudioStream(outStream);
}
void Audio::get(int ms, std::vector<float> &result){
    result.clear();
    {
        //lock the the current audio class to this thread so multiple things don't try and access it
        std::lock_guard<std::mutex> lock(m_mutex);

        if(ms <= 0){
            ms = m_len_ms;
        }

        size_t n_samples = (WHISPER_SAMPLE_RATE * ms) / 1000;
        if(n_samples > m_audio_len){
            n_samples = m_audio_len;
        }
        result.resize(n_samples);
        int startIndex = m_audio_pos - n_samples;
        if(startIndex < 0){
            startIndex += m_audio.size();
        }
        if(startIndex + n_samples > m_audio.size()){
            const size_t sampleSplit = m_audio.size() - startIndex;

            memcpy(result.data(), &m_audio[startIndex], sampleSplit * sizeof(float));
            memcpy(result.data(), &m_audio[startIndex], sampleSplit * sizeof(float));
        }
        else{
            memcpy(result.data(), &m_audio[startIndex], n_samples * sizeof(float));
        }
        //the lock_guard class unlocks the given mutex when its destructor is called
    }
}

void SDLCALL Audio::callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount){
    if(additional_amount > 0){
        size_t n_samples = additional_amount / sizeof(float);
        Uint8 *data = SDL_stack_alloc(Uint8, additional_amount);
        if(data){
            if(SDL_GetAudioStreamAvailable(stream) != 0){
                // use the original callback here
                {
                    if(n_samples > m_audio.size()){
                        n_samples = m_audio.size();
                        data += (additional_amount  - (n_samples * sizeof(float)));
                    }
                    SDL_GetAudioStreamData(stream, data, additional_amount);
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        if (m_audio_pos + n_samples > m_audio.size()) {
                            const size_t n0 = m_audio.size() - m_audio_pos;
                            memcpy(&m_audio[m_audio_pos], data, n0 * sizeof(float));
                            memcpy(&m_audio[0], data + n0 * sizeof(float), (n_samples - n0) * sizeof(float));
                        } else {
                            memcpy(&m_audio[m_audio_pos], data, n_samples * sizeof(float));
                        }
                        m_audio_pos = (m_audio_pos + n_samples) % m_audio.size();
                        m_audio_len = std::min(m_audio_len + n_samples, m_audio.size());
                        SDL_PutAudioStreamData(outStream, data, additional_amount);
                    }
                }
            }
            SDL_stack_free(data);
        }
    }
}
