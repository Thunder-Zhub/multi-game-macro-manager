#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <json/json.h>

// ==================== Enums ====================
enum TriggerType { TRIG_MOUSE_BTN, TRIG_KEY };
enum ActionType { ACT_LEFT_CLICK, ACT_RIGHT_CLICK, ACT_KEY_PRESS };

// ==================== Structures ====================
struct Binding {
    TriggerType triggerType;
    int triggerCode;        // XBUTTON1/2 or VK_xxx
    ActionType actionType;
    int actionCode;         // VK_xxx when actionType == ACT_KEY_PRESS
    int cps;                // Clicks per second (1-20)
    bool isHeld;            // Runtime state: is trigger key held?
    UINT_PTR timerId;
    std::wstring label;     // Display label

    Json::Value toJson() const;
    static Binding fromJson(const Json::Value& val);
};

struct Profile {
    std::wstring name;              // Profile name (e.g., "Minecraft")
    std::wstring targetExeName;     // Target process name
    DWORD targetPID;                // Target process ID
    bool autoSwitch;                // Auto-activate when target window is foreground
    bool enabled;                   // Is this profile active?
    std::vector<Binding> bindings;  // All bindings in this profile

    Json::Value toJson() const;
    static Profile fromJson(const Json::Value& val);
};

struct GlobalConfig {
    std::vector<Profile> profiles;
    bool globalEnabled;             // Master enable/disable
    int globalHotkey;               // VK code for global toggle (default F10=121)

    Json::Value toJson() const;
    static GlobalConfig fromJson(const Json::Value& val);
};

// ==================== Helper Functions ====================
std::wstring VkToString(int vk);
std::wstring ActionToString(const Binding& b);
std::string GetAppDataPath();
bool IsTargetForeground(DWORD pid);
bool IsProcessRunning(DWORD pid);
