#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <fstream>
#include <iostream>

/**
 * Singleton configuration manager.
 * Handles loading and retrieving settings from .ini files.
 */
class Config {
public:
    static Config& getInstance();

    // Load settings from a flat key=value file
    void load(const std::string& filename);

    // Getters with fallback defaults
    std::string get(const std::string& key, const std::string& defaultValue = "");
    int getInt(const std::string& key, int defaultValue = 0);
    float getFloat(const std::string& key, float defaultValue = 0.0f);

private:
    Config() = default;
    std::map<std::string, std::string> settings;
};

#endif // CONFIG_H
