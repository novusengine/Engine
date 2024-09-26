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
    void PlaySoundFileCommand(const std::string& fileName, f32 volume, bool isLooping = false, bool endAllOtherAudio = false);
    void PauseSoundFile();
    void ResumeSoundFile();
    void RestartSoundFile();
    void EndLooping();
    void SetVolume(f32 volume);

    void SetAudioFile(const fs::path& filePath);
    const std::string& GetFileName() { return _fileName; }

private:
    ma_engine* _audioEngine = nullptr;
    ma_sound* _sound = nullptr;
    std::string _audioFile;
    std::string _fileName;
};