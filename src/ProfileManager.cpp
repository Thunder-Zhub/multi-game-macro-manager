#include "ProfileManager.h"
#include <fstream>
#include <tlhelp32.h>
#pragma comment(lib, "shell32.lib")

ProfileManager::ProfileManager() : m_currentProfileIndex(-1) {
    m_configPath = GetAppDataPath() + "\\profiles.json";
    m_config.globalEnabled = true;
    m_config.globalHotkey = VK_F10; // Default: F10
}

ProfileManager::~ProfileManager() {
    SaveConfig();
}

bool ProfileManager::LoadConfig() {
    std::ifstream file(m_configPath);
    if (!file.is_open()) {
        // First time - create default config
        return SaveConfig();
    }
    
    Json::CharReaderBuilder builder;
    Json::Value obj;
    std::string errs;
    
    if (!Json::parseFromStream(builder, file, &obj, &errs)) {
        return false;
    }
    file.close();
    
    m_config = GlobalConfig::fromJson(obj);
    if (!m_config.profiles.empty()) {
        m_currentProfileIndex = 0;
    }
    return true;
}

bool ProfileManager::SaveConfig() {
    std::string path = GetAppDataPath();
    if (path.empty()) return false;
    
    std::ofstream file(m_configPath);
    if (!file.is_open()) return false;
    
    Json::Value obj = m_config.toJson();
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
    jsonWriter->write(obj, &file);
    file.close();
    return true;
}

int ProfileManager::FindProfileByName(const std::wstring& name) {
    for (size_t i = 0; i < m_config.profiles.size(); i++) {
        if (m_config.profiles[i].name == name) return (int)i;
    }
    return -1;
}

Profile* ProfileManager::GetCurrentProfile() {
    if (m_currentProfileIndex >= 0 && m_currentProfileIndex < (int)m_config.profiles.size()) {
        return &m_config.profiles[m_currentProfileIndex];
    }
    return nullptr;
}

bool ProfileManager::SetCurrentProfile(int index) {
    if (index >= 0 && index < (int)m_config.profiles.size()) {
        m_currentProfileIndex = index;
        m_config.profiles[index].enabled = true;
        // Disable all other profiles
        for (size_t i = 0; i < m_config.profiles.size(); i++) {
            if ((int)i != index) m_config.profiles[i].enabled = false;
        }
        return true;
    }
    return false;
}

bool ProfileManager::AddProfile(const Profile& profile) {
    m_config.profiles.push_back(profile);
    if (m_currentProfileIndex == -1) m_currentProfileIndex = 0;
    return SaveConfig();
}

bool ProfileManager::RemoveProfile(int index) {
    if (index >= 0 && index < (int)m_config.profiles.size()) {
        m_config.profiles.erase(m_config.profiles.begin() + index);
        if (m_currentProfileIndex >= (int)m_config.profiles.size()) {
            m_currentProfileIndex = (int)m_config.profiles.size() - 1;
        }
        return SaveConfig();
    }
    return false;
}

bool ProfileManager::UpdateProfile(int index, const Profile& profile) {
    if (index >= 0 && index < (int)m_config.profiles.size()) {
        m_config.profiles[index] = profile;
        return SaveConfig();
    }
    return false;
}

std::vector<ProfileManager::ProcessEntry> ProfileManager::EnumRunningProcesses() {
    std::vector<ProcessEntry> result;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return result;
    
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(snap, &pe)) {
        do {
            struct EnumData { DWORD pid; HWND found; } ed = { pe.th32ProcessID, NULL };
            EnumWindows([](HWND h, LPARAM lp) -> BOOL {
                EnumData* d = (EnumData*)lp;
                DWORD pid;
                GetWindowThreadProcessId(h, &pid);
                if (pid == d->pid && IsWindowVisible(h) && GetWindowTextLength(h) > 0) {
                    d->found = h;
                    return FALSE;
                }
                return TRUE;
            }, (LPARAM)&ed);
            
            if (ed.found) {
                result.push_back({ pe.th32ProcessID, pe.szExeFile });
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return result;
}
