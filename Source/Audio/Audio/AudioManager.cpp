#include "AudioManager.h"

AudioManager::AudioManager()
{

}

bool AudioManager::Init()
{
    if (_audioEngine != nullptr)
        return false;

    ma_engine* engine = new ma_engine();
    ma_result result = ma_engine_init(NULL, engine);

    if (result != MA_SUCCESS)
    {
        delete engine;
        return false;
    }

    _audioEngine = engine;
    return true;
}

void AudioManager::Cleanup()
{
    if (_audioEngine == nullptr)
        return;

    ma_engine_uninit(_audioEngine);
    
    delete _audioEngine;
    _audioEngine = nullptr;
}

void AudioManager::LoadSoundFile()
{

}

void AudioManager::PlaySoundFile(const std::string& filepath, float volume)
{
    if (_audioEngine == nullptr)
        return;

    ma_result result;
    _sound = new ma_sound();
    
    result = ma_sound_init_from_file(_audioEngine, filepath.c_str(), 0, NULL, NULL, _sound);
    if (result != MA_SUCCESS)
        return;

    ma_sound_set_volume(_sound, volume);
    ma_sound_start(_sound);
}

void AudioManager::PauseSoundFile()
{
    ma_sound_stop(_sound);
}

void AudioManager::ResumeSoundFile()
{
    ma_sound_start(_sound);
}

void AudioManager::RestartSoundFile()
{
    ma_sound_seek_to_pcm_frame(_sound, 0);

    ma_sound_start(_sound);
}

//void AudioManager::PauseAllAudio()
//{
//
//}
//
//void AudioManager::ResumeAllAudio()
//{
//
//}

void AudioManager::SetVolume(float volume)
{
    ma_sound_set_volume(_sound, volume);
}
