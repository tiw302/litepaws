#include "../include/Config.h"
#include <sstream>
#include <algorithm>

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Simple trim logic
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            
            // Trim key and value
            key.erase(line.find_last_not_of(" \t") + 1); // Not correct but simple enough for standard 'key=value'
            // Actually let's do a proper trim on both sides
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            settings[key] = value;
        }
    }
}

std::string Config::get(const std::string& key, const std::string& defaultValue) {
    if (settings.find(key) != settings.end()) {
        return settings[key];
    }
    return defaultValue;
}

int Config::getInt(const std::string& key, int defaultValue) {
    std::string val = get(key);
    if (val.empty()) return defaultValue;
    try {
        return std::stoi(val);
    } catch (...) {
        return defaultValue;
    }
}

float Config::getFloat(const std::string& key, float defaultValue) {
    std::string val = get(key);
    if (val.empty()) return defaultValue;
    try {
        return std::stof(val);
    } catch (...) {
        return defaultValue;
    }
}
