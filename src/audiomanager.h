#pragma once

#include <al.h>
#include <alc.h>

class CAudio;

class CAudioManager
{
public:
	CAudioManager();
	~CAudioManager();
	bool Initialise();
    void Destroy();

    bool Load(const std::string& path);
    void Play(const std::string& path, const glm::vec3& position);
    void Stop(const std::string& path);
	void Update();

private:
    ALCcontext* context;
    ALCdevice* device;
    std::unordered_map<std::string, CAudio*> m_sounds;

    static bool LoadWavHeaderFile(std::ifstream& file, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample, ALsizei& size);
    static std::vector<char> LoadWav(const std::string& filename, uint8_t& channels, int32_t& sampleRate, uint8_t& bitsPerSample);

    friend class CAudio;
};
