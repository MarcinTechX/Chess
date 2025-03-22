#include "SoundManager.hpp"
#include <SFML/Audio.hpp>
#include <iostream>

SoundManager::SoundManager() {};

SoundManager::~SoundManager() {}

bool SoundManager::loadSound(const std::string& soundName, const std::string& filename)
{
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filename))
    {
        std::cout << "Failed to load sound: " << filename << std::endl;
        return false;
    }

    buffers[soundName] = std::move(buffer);
    auto sound = std::make_unique<sf::Sound>(*buffers[soundName]);
    sounds[soundName] = std::move(sound);

    return true;
}

void SoundManager::playSound(const std::string& soundName)
{
    auto it = sounds.find(soundName);
    if (it != sounds.end())
    {
        it->second->play();  
    }
}

void SoundManager::playSoundOnce(const std::string& soundName)
{
    auto it = sounds.find(soundName);  
    if (it != sounds.end()) 
    {
        if (it->second->getStatus() != sf::SoundSource::Status::Playing)
        {
            it->second->play();  
        }
    }
}

bool SoundManager::isSoundPlaying(const std::string& soundName)
{
    auto it = sounds.find(soundName);  
    if (it != sounds.end())
    {
        return it->second->getStatus() == sf::SoundSource::Status::Playing;  
    }
    return false;  
}

void SoundManager::stopSound(const std::string& soundName)
{
    auto it = sounds.find(soundName);  
    if (it != sounds.end())
    {
        it->second->stop();  
    }
}

void SoundManager::stopAllSounds()
{
    for (auto& sound : sounds) 
    {
        sound.second->stop();  
    }
}



