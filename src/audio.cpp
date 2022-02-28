#include "audio.h"
#include "openal.h"
#include "audiomanager.h"

CAudio::CAudio(const std::string& path, bool loop, int pitch, float gain, const glm::vec3& position, const glm::vec3& velocity)
{
    auto buffer = CAudioManager::LoadWav(path, m_channels, m_sampleRate, m_bitsPerSample);

    if(buffer.empty())
    {
        std::cerr << "ERROR: Could not load wav" << std::endl;
        return;
    }

    alCall(alGenBuffers, 1, &m_buffer);

    ALenum format;
    if(m_channels == 1 && m_bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if(m_channels == 1 && m_bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if(m_channels == 2 && m_bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if(m_channels == 2 && m_bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else {
        std::cerr
                << "ERROR: unrecognised wave format: "
                << m_channels << " channels, "
                << m_bitsPerSample << " bps" << std::endl;
        return;
    }

    alCall(alBufferData, m_buffer, format, buffer.data(), buffer.size(), m_sampleRate);

    alCall(alGenSources, 1, &m_source);
    alCall(alSourcef, m_source, AL_PITCH, pitch);
    alCall(alSourcef, m_source, AL_GAIN, gain);
    alCall(alSource3f, m_source, AL_POSITION, position.x, position.y, position.z);
    alCall(alSource3f, m_source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    alCall(alSourcei, m_source, AL_LOOPING, loop);
    alCall(alSourcei, m_source, AL_BUFFER, m_buffer);

    m_state = AL_INITIAL;
}

CAudio::~CAudio()
{
    alCall(alDeleteBuffers, 1, &m_buffer);
    alCall(alDeleteSources, 1, &m_source);
}

void CAudio::Play()
{
    alCall(alSourcePlay, m_source);
    m_state = AL_PLAYING;
}

void CAudio::Stop()
{
    alCall(alSourceStop, m_source);
    m_state = AL_STOPPED;
}

void CAudio::Pause()
{
    alCall(alSourcePause, m_source);
    m_state = AL_PAUSED;
}

void CAudio::Update()
{
    alCall(alGetSourcei, m_source, AL_SOURCE_STATE, &m_state);
}

void CAudio::SetPosition(const glm::vec3& position) const
{
    alCall(alSource3f, m_source, AL_POSITION, position.x, position.y, position.z);
}

void CAudio::SetVelocity(const glm::vec3& velocity) const
{
    alCall(alSource3f, m_source, AL_POSITION, velocity.x, velocity.y, velocity.z);
}

void CAudio::SetLooping(bool loop) const
{
    alCall(alSourcei, m_source, AL_LOOPING, loop);
}

void CAudio::SetPitch(int pitch) const
{
    alCall(alSourcef, m_source, AL_PITCH, pitch);
}

void CAudio::SetGain(float gain) const
{
    alCall(alSourcef, m_source, AL_GAIN, gain);
}

uint8_t CAudio::GetChannels() const
{
    return m_channels;
}

int32_t CAudio::GetSampleRate() const
{
    return m_sampleRate;
}

uint8_t CAudio::GetBitsPerSample() const
{
    return m_bitsPerSample;
}

bool CAudio::IsPlaying() const
{
    return m_state == AL_PLAYING;
}

bool CAudio::IsPaused() const
{
    return m_state == AL_PAUSED;
}
