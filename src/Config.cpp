#include "../include/Config.h"
#include <sstream>
#include <algorithm>

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

static std::string trim(const std::string& s) {
    auto first = s.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    auto last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));
            
            if (!key.empty()) {
                settings[key] = value;
            }
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
    auto it = settings.find(key);
    if (it == settings.end()) return defaultValue;
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultValue;
    }
}

float Config::getFloat(const std::string& key, float defaultValue) {
    auto it = settings.find(key);
    if (it == settings.end()) return defaultValue;
    try {
        return std::stof(it->second);
    } catch (...) {
        return defaultValue;
    }
}
