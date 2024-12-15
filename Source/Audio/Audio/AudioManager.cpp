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

void AudioManager::PlaySoundFile(f32 volume, bool isLooping, bool endAllOtherAudio)
{
    if (_audioEngine == nullptr)
        return;

    if (endAllOtherAudio && ma_sound_is_playing(_assetBrowserSound))
        ma_sound_uninit(_assetBrowserSound);

    _assetBrowserSound = new ma_sound();

    ma_result result =ma_sound_init_from_file(_audioEngine, _audioFile.c_str(), 0, NULL, NULL, _assetBrowserSound);
    if (result != MA_SUCCESS)
        return;

    ma_sound_set_volume(_assetBrowserSound, volume);
    ma_sound_set_looping(_assetBrowserSound, isLooping);
    ma_sound_start(_assetBrowserSound);
}

void AudioManager::PlaySoundCommand(const std::string& fileName, f32 volume, bool isLooping, bool endAllOtherAudio)
{
    if (_audioEngine == nullptr)
        return;

    if (endAllOtherAudio)
        SoundListCleanup(_consoleSounds);

    ma_sound* sound = new ma_sound();
    _consoleSounds.push_back(sound);

    ma_result result = ma_sound_init_from_file(_audioEngine, fileName.c_str(), 0, NULL, NULL, sound);
    if (result != MA_SUCCESS)
        return;

    ma_sound_set_volume(sound, volume);
    ma_sound_set_looping(sound, isLooping);
    ma_sound_start(sound);
}

void AudioManager::StopSoundCommand()
{
    SoundListCleanup(_consoleSounds);
}

void AudioManager::PauseSoundFile()
{
    ma_sound_stop(_assetBrowserSound);
}

void AudioManager::ResumeSoundFile()
{
    ma_sound_start(_assetBrowserSound);
}

void AudioManager::RestartSoundFile()
{
    ma_sound_seek_to_pcm_frame(_assetBrowserSound, 0);

    ma_sound_start(_assetBrowserSound);
}

void AudioManager::EndLooping()
{
    ma_sound_set_looping(_assetBrowserSound, false);
}

void AudioManager::SetVolume(f32 volume)
{
    ma_sound_set_volume(_assetBrowserSound, volume);
}

void AudioManager::SetAudioFile(const fs::path& filePath)
{
    _audioFile = filePath.string();
    _fileName = filePath.filename().string();
}

void AudioManager::SoundListCleanup(std::vector<ma_sound*>& list)
{
    if (!list.empty())
    {
        for (auto sound : list)
        {
            ma_sound_stop(sound);
            ma_sound_uninit(sound);
            delete sound;
        }

        list.clear();
        list.shrink_to_fit();
    }
}
