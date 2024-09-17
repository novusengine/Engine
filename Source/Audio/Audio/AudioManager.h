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
    void PlaySoundFile(const std::string& filepath, f32 volume, bool isFromAssetBrowser = false);
    void PauseSoundFile();
    void ResumeSoundFile();
    void RestartSoundFile();
    void SetVolume(f32 volume);

private:
    ma_engine* _audioEngine = nullptr;
    ma_sound* _sound = nullptr;
};