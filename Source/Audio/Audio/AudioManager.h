#pragma once

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>

#include <miniaudio/miniaudio.h>

class AudioManager
{
public:
    AudioManager();

    bool Init();
    void Cleanup();
    void LoadSoundFile();
    void PlaySoundFile(const std::string& filepath, float volume);
    void PauseSoundFile();
    void ResumeSoundFile();
    void RestartSoundFile();
    //void PauseAllAudio();
    //void ResumeAllAudio();
    void SetVolume(float volume);

private:
    ma_engine* _audioEngine = nullptr;
    ma_sound* _sound = nullptr;
    //std::vector<ma_sound*> _sounds = {};
};