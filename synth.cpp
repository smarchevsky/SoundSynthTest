#include "synth.h"

enum ElementType {
    Tone,
    Octave,
    Pause,
    Invalid
};

// static const int toneMap[7] { 0, 2, 4, 5, 7, 9, 11 };
#define INVALID_ELEMENT std::numeric_limits<int>().max()
#define PAUSE std::numeric_limits<int>().min()

static const int symbolToTone(char symbol)
{
    switch (symbol) {
    case 'a':
        return 0;
    case 'b':
        return 2;
    case 'c':
        return 3;
    case 'd':
        return 5;
    case 'e':
        return 7;
    case 'f':
        return 8;
    case 'g':
        return 10;
    default:
        return INVALID_ELEMENT;
    }
}

static const int symbolToOctaveOffset(char symbol)
{
    if (symbol >= '0' && symbol <= '9')
        return (symbol - '0') * 12;
    return INVALID_ELEMENT;
}

float toneToFreq(int tone) { return powf(2.f, tone / 12.f) * 55; }

void SimpleSynthPlayer::parse(const std::string& music)
{
    std::vector<int> tones;
    int currentTone = INVALID_ELEMENT;

    for (int symbolIndex = 0; symbolIndex < music.size(); ++symbolIndex) {
        const char symbol = music[symbolIndex];

        if (symbol == '_') {
            currentTone = INVALID_ELEMENT;
            tones.push_back(PAUSE);
            continue;
        }

        int tone = symbolToTone(symbol);
        if (tone != INVALID_ELEMENT) {
            currentTone = tone;
            continue;
        }

        if (symbol == '#' && currentTone != INVALID_ELEMENT)
            currentTone++;

        int octaveOffset = symbolToOctaveOffset(symbol);
        if (octaveOffset != INVALID_ELEMENT) {
            if (currentTone != INVALID_ELEMENT) {
                tones.push_back(octaveOffset + currentTone);
                currentTone = INVALID_ELEMENT;
            }
            // it is tone
            continue;
        }
    }

    m_frequencies.clear();
    for (const auto& t : tones) {
        if (t != PAUSE)
            m_frequencies.push_back(toneToFreq(t));
        else
            m_frequencies.push_back(0);
    }
}

float SimpleSynthPlayer::update()
{
    m_playPosition = glm::mod(m_playPosition + m_sampleRateInv * m_noteSpeed, (double)m_frequencies.size());
    float currentFreq = m_frequencies[(int)m_playPosition];

    m_currentVol += ((currentFreq != 0) - m_currentVol) * m_sampleRateInv * 500.f;

    if (currentFreq)
        m_currentFreq = currentFreq;

    return m_osc.genSquare(m_currentFreq * m_sampleRateInv) * m_currentVol * 0.5f;
}
