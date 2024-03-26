#pragma once
#include <glm/glm.hpp>

#include <numeric>
#include <string>
#include <vector>

#define LOG(x) std::cout << x << std::endl

class Osc {
public:
    Osc() { }
    float genSine(float freq)
    {
        float x = (updatedPhase(freq) - .5f) * 2.f;
        return (4.f * x) * (1.f - glm::abs(x));
    }
    float genSaw(float freq) { return updatedPhase(freq) * 2.f - 1.f; }
    float genTriangle(float freq) { return m_phase * 4.f + glm::clamp(2.f - updatedPhase(freq) * 8.f, -4.f, 0.f); }
    float genSquare(float freq) { return updatedPhase(freq) > .5f ? -1.f : 1.f; }
    static float genWhiteNoise() { return rand() / float(RAND_MAX) * 2.f - 1.f; }

private:
    float updatedPhase(float freqSampleRateScale)
    {
        m_phase = glm::fract<float>(m_phase + freqSampleRateScale);
        return m_phase;
    }

private:
    float m_phase = 0;
};

class SimpleSynthPlayer {
    Osc m_osc;
    const float m_sampleRateInv;
    double m_playPosition {};
    const float m_noteSpeed = 18.f;
    float m_currentFreq {}, m_currentVol {};
    std::vector<float> m_frequencies;

public:
    SimpleSynthPlayer(float sampleRate)
        : m_sampleRateInv(1.f / sampleRate)
    {
    }
    void parse(const std::string& music);
    float update();
};
