#pragma once

#include <al.h>
#include <alc.h>

class CAudio {
public:
    CAudio(const std::string& path, bool loop = true, int pitch = 1, float gain = 1.0f, const glm::vec3& position = {}, const glm::vec3& velocity = {});
    ~CAudio();

    void Play();
    void Stop();
    void Pause();
    void Update();

    void SetPosition(const glm::vec3& position) const;
    void SetVelocity(const glm::vec3& velocity) const;
    void SetLooping(bool loop) const;
    void SetPitch(int pitch) const;
    void SetGain(float gain) const;

    uint8_t GetChannels() const;
    int32_t GetSampleRate() const;
    uint8_t GetBitsPerSample() const;

    bool IsPlaying() const;
    bool IsPaused() const;

private:
    ALuint m_buffer;
    ALuint m_source;
    ALint m_state;
    uint8_t m_channels;
    int32_t m_sampleRate;
    uint8_t m_bitsPerSample;
};