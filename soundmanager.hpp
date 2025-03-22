#ifndef SOUND_MANAGER_HPP
#define SOUND_MANAGER_HPP

#include <SFML/Audio.hpp>
#include <map>
#include <string>

class SoundManager
{
public:
    SoundManager();
    ~SoundManager();

    bool loadSound(const std::string& soundName, const std::string& filename);
    void playSound(const std::string& soundName);
    void playSoundOnce(const std::string& soundName);
    bool isSoundPlaying(const std::string& soundName);
    void stopSound(const std::string& soundName);
    void stopAllSounds();

private:
    std::map<std::string, std::unique_ptr<sf::Sound>> sounds;         
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> buffers;
};

#endif
