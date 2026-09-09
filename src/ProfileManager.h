#pragma once
#include "common.h"
#include <string>

class ProfileManager {
public:
    ProfileManager();
    ~ProfileManager();
    
    // Load config from disk
    bool LoadConfig();
    
    // Save config to disk
    bool SaveConfig();
    
    // Get/set current config
    GlobalConfig& GetConfig() { return m_config; }
    const GlobalConfig& GetConfig() const { return m_config; }
    
    // Profile management
    int FindProfileByName(const std::wstring& name);
    Profile* GetCurrentProfile();
    bool SetCurrentProfile(int index);
    bool AddProfile(const Profile& profile);
    bool RemoveProfile(int index);
    bool UpdateProfile(int index, const Profile& profile);
    
    // Get all process list
    struct ProcessEntry {
        DWORD pid;
        std::wstring exeName;
    };
    std::vector<ProcessEntry> EnumRunningProcesses();
    
private:
    GlobalConfig m_config;
    int m_currentProfileIndex;
    std::string m_configPath;
};
