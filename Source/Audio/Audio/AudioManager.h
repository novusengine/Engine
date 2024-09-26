#pragma once

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>

#include <filesystem>

#include <miniaudio/miniaudio.h>

namespace fs = std::filesystem;

class AudioManager
{
public:
    AudioManager();

    bool Init();
    void Cleanup();
    void PlaySoundFile(f32 volume, bool isLooping = false, bool endAllOtherAudio = false);
    void PlaySoundCommand(const std::string& fileName, f32 volume, bool isLooping = false, bool endAllOtherAudio = false);
    void StopSoundCommand();
    void PauseSoundFile();
    void ResumeSoundFile();
    void RestartSoundFile();
    void EndLooping();
    void SetVolume(f32 volume);

    void SetAudioFile(const fs::path& filePath);
    const std::string& GetFileName() { return _fileName; }

private:
    void SoundListCleanup(std::vector<ma_sound*>& list);

private:
    ma_engine* _audioEngine = nullptr;
    ma_sound* _assetBrowserSound = nullptr;
    std::vector<ma_sound*> _consoleSounds = {};
    std::string _audioFile;
    std::string _fileName;
};