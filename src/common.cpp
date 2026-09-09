#include "common.h"
#include <shlobj.h>
#include <tlhelp32.h>
#pragma comment(lib, "shell32.lib")

// ==================== Binding JSON ====================
Json::Value Binding::toJson() const {
    Json::Value obj;
    obj["triggerType"] = (triggerType == TRIG_MOUSE_BTN) ? "MOUSE" : "KEY";
    obj["triggerCode"] = triggerCode;
    obj["actionType"] = (int)actionType;
    obj["actionCode"] = actionCode;
    obj["cps"] = cps;
    return obj;
}

Binding Binding::fromJson(const Json::Value& val) {
    Binding b = {};
    b.triggerType = (val["triggerType"].asString() == "MOUSE") ? TRIG_MOUSE_BTN : TRIG_KEY;
    b.triggerCode = val["triggerCode"].asInt();
    b.actionType = (ActionType)val["actionType"].asInt();
    b.actionCode = val["actionCode"].asInt();
    b.cps = val["cps"].asInt();
    b.isHeld = false;
    b.timerId = 0;
    return b;
}

// ==================== Profile JSON ====================
Json::Value Profile::toJson() const {
    Json::Value obj;
    obj["name"] = std::string(name.begin(), name.end());
    obj["targetExeName"] = std::string(targetExeName.begin(), targetExeName.end());
    obj["targetPID"] = (int)targetPID;
    obj["autoSwitch"] = autoSwitch;
    obj["enabled"] = enabled;
    
    Json::Value bindingsArr(Json::arrayValue);
    for (const auto& b : bindings) {
        bindingsArr.append(b.toJson());
    }
    obj["bindings"] = bindingsArr;
    return obj;
}

Profile Profile::fromJson(const Json::Value& val) {
    Profile p = {};
    p.name = std::wstring(val["name"].asString().begin(), val["name"].asString().end());
    p.targetExeName = std::wstring(val["targetExeName"].asString().begin(), 
                                    val["targetExeName"].asString().end());
    p.targetPID = (DWORD)val["targetPID"].asInt();
    p.autoSwitch = val["autoSwitch"].asBool();
    p.enabled = val["enabled"].asBool();
    
    for (const auto& bVal : val["bindings"]) {
        p.bindings.push_back(Binding::fromJson(bVal));
    }
    return p;
}

// ==================== GlobalConfig JSON ====================
Json::Value GlobalConfig::toJson() const {
    Json::Value obj;
    Json::Value profilesArr(Json::arrayValue);
    for (const auto& p : profiles) {
        profilesArr.append(p.toJson());
    }
    obj["profiles"] = profilesArr;
    obj["globalEnabled"] = globalEnabled;
    obj["globalHotkey"] = globalHotkey;
    return obj;
}

GlobalConfig GlobalConfig::fromJson(const Json::Value& val) {
    GlobalConfig cfg = {};
    cfg.globalEnabled = val["globalEnabled"].asBool();
    cfg.globalHotkey = val["globalHotkey"].asInt();
    
    for (const auto& pVal : val["profiles"]) {
        cfg.profiles.push_back(Profile::fromJson(pVal));
    }
    return cfg;
}

// ==================== Helper Functions ====================
std::wstring VkToString(int vk) {
    if (vk == XBUTTON1) return L"Mouse4";
    if (vk == XBUTTON2) return L"Mouse5";
    
    wchar_t buf[64] = {0};
    UINT scan = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    LONG lParam = scan << 16;
    if (GetKeyNameTextW(lParam, buf, 64) > 0) return std::wstring(buf);
    return L"VK_" + std::to_wstring(vk);
}

std::wstring ActionToString(const Binding& b) {
    if (b.actionType == ACT_LEFT_CLICK) return L"Left Click (Rapid)";
    if (b.actionType == ACT_RIGHT_CLICK) return L"Right Click (Rapid)";
    return L"Press " + VkToString(b.actionCode) + L" (Rapid)";
}

std::string GetAppDataPath() {
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        std::wstring wpath(path);
        std::string result(wpath.begin(), wpath.end());
        result += "\\MacroManager";
        CreateDirectoryA(result.c_str(), NULL);
        return result;
    }
    return "";
}

bool IsTargetForeground(DWORD pid) {
    if (pid == 0) return false;
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    DWORD fgPid = 0;
    GetWindowThreadProcessId(fg, &fgPid);
    return (fgPid == pid);
}

bool IsProcessRunning(DWORD pid) {
    if (pid == 0) return false;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) return false;
    
    DWORD exitCode = 0;
    BOOL result = GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);
    return result && (exitCode == STILL_ACTIVE);
}
