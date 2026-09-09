# API Documentation 📚

## Overview

This document provides technical details for developers who want to extend or modify the Macro Manager.

## Core Classes

### ProfileManager

Handles loading, saving, and managing profiles.

```cpp
class ProfileManager {
public:
    bool LoadConfig();                      // Load profiles from disk
    bool SaveConfig();                      // Save to %APPDATA%\MacroManager\profiles.json
    GlobalConfig& GetConfig();              // Get current config
    Profile* GetCurrentProfile();           // Get active profile
    bool SetCurrentProfile(int index);      // Switch to profile
    int FindProfileByName(const std::wstring& name);
    bool AddProfile(const Profile& profile);
    bool RemoveProfile(int index);
    bool UpdateProfile(int index, const Profile& profile);
    std::vector<ProcessEntry> EnumRunningProcesses();  // List running apps
};
```

### HookManager

Manages low-level mouse and keyboard hooks.

```cpp
class HookManager {
public:
    bool InstallHooks(HINSTANCE hInst);    // Install system hooks
    void UninstallHooks();                  // Remove hooks
    void SetBindings(std::vector<Binding>* bindings);  // Set active bindings
    void SetGlobalEnabled(bool enabled);   // Master enable/disable
    bool IsGlobalEnabled() const;
    void SetTargetPID(DWORD pid);          // Set target process
    DWORD GetTargetPID() const;
};
```

### MainWindow

GUI implementation and event handling.

```cpp
class MainWindow {
public:
    bool Create(HINSTANCE hInst);          // Create main window
    void Show(int nCmdShow);               // Show window
    int Run();                              // Message loop
    
private:
    void RefreshProfileTabs();              // Update profile tabs
    void RefreshBindingList();              // Update binding list
    void OnSelectTarget();                  // Handle target selection
    void OnAddBinding();                    // Handle add binding
    void OnProfileChanged(int idx);         // Handle profile switch
};
```

## Data Structures

### Binding

```cpp
struct Binding {
    TriggerType triggerType;    // TRIG_MOUSE_BTN or TRIG_KEY
    int triggerCode;            // VK code (e.g., XBUTTON1)
    ActionType actionType;      // ACT_LEFT_CLICK, ACT_RIGHT_CLICK, ACT_KEY_PRESS
    int actionCode;             // VK code when actionType == ACT_KEY_PRESS
    int cps;                    // 1-20 clicks per second
    bool isHeld;                // Runtime: button currently held?
    UINT_PTR timerId;           // Internal timer ID
    std::wstring label;         // Display label
    
    Json::Value toJson() const; // Serialize to JSON
    static Binding fromJson(const Json::Value&);  // Deserialize
};
```

### Profile

```cpp
struct Profile {
    std::wstring name;          // "Minecraft", "Valorant", etc.
    std::wstring targetExeName; // "javaw.exe", "VALORANT.exe"
    DWORD targetPID;            // Current process ID
    bool autoSwitch;            // Auto-activate when game is foreground
    bool enabled;               // Profile is active
    std::vector<Binding> bindings;  // 0-5 bindings per profile
    
    Json::Value toJson() const;
    static Profile fromJson(const Json::Value&);
};
```

### GlobalConfig

```cpp
struct GlobalConfig {
    std::vector<Profile> profiles;  // All profiles
    bool globalEnabled;             // Master enable/disable
    int globalHotkey;               // VK code for F10 toggle
    
    Json::Value toJson() const;
    static GlobalConfig fromJson(const Json::Value&);
};
```

## Configuration File Format

### Location
`%APPDATA%\MacroManager\profiles.json`

### Example

```json
{
  "globalEnabled": true,
  "globalHotkey": 121,
  "profiles": [
    {
      "name": "Minecraft",
      "targetExeName": "javaw.exe",
      "targetPID": 0,
      "autoSwitch": true,
      "enabled": true,
      "bindings": [
        {
          "triggerType": "MOUSE",
          "triggerCode": 5,
          "actionType": 0,
          "actionCode": 0,
          "cps": 10
        }
      ]
    }
  ]
}
```

## Hook Implementation

### Low-Level Mouse Hook

```cpp
LRESULT CALLBACK LowLevelMouseProc(
    int nCode,          // HC_ACTION, HC_NOREMOVE, etc.
    WPARAM wParam,      // WM_XBUTTONDOWN, WM_XBUTTONUP
    LPARAM lParam       // MSLLHOOKSTRUCT*
)
```

- Intercepts Mouse4/Mouse5 button presses
- Sets `isHeld` flag for affected bindings
- Triggers timer for repeated action firing

### Low-Level Keyboard Hook

```cpp
LRESULT CALLBACK LowLevelKeyboardProc(
    int nCode,          // HC_ACTION, HC_NOREMOVE, etc.
    WPARAM wParam,      // WM_KEYDOWN, WM_KEYUP
    LPARAM lParam       // KBDLLHOOKSTRUCT*
)
```

- Intercepts configured trigger keys (F6-F8, Caps Lock)
- Sets `isHeld` flag for affected bindings
- Triggers timer for repeated action firing

### Action Firing

When a binding's timer fires (every `1000/cps` milliseconds):

```cpp
void FireAction(Binding& b) {
    INPUT inputs[2] = {};
    
    if (b.actionType == ACT_LEFT_CLICK) {
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    }
    // Similar for RIGHT_CLICK and KEY_PRESS
    
    SendInput(2, inputs, sizeof(INPUT));
}
```

## Thread Safety

### Protected Data

- **m_bindings** - Protected by `m_bindingsMutex`
- **m_targetPID** - Atomic (no lock needed)
- **m_globalEnabled** - Atomic (no lock needed)

### Thread Synchronization

```cpp
// In HookManager::MouseProc()
std::lock_guard<std::mutex> lock(m_bindingsMutex);
for (auto& b : *m_bindings) {
    // Safe access to bindings
}
```

## Extension Points

### Adding New Trigger Types

1. Add to `TriggerType` enum in `common.h`
2. Add hook handler in `HookManager`
3. Update UI trigger selection dialog
4. Update JSON serialization

### Adding New Action Types

1. Add to `ActionType` enum in `common.h`
2. Implement in `HookManager::FireAction()`
3. Update UI action selection dialog
4. Add to `ActionToString()` helper

### Custom CPS Logic

Modify `HookManager::StartBindingTimer()`:

```cpp
int intervalMs = 1000 / (b.cps > 0 ? b.cps : 1);
SetTimer(hwnd, b.timerId, intervalMs, StaticTimerProc);
```

## Building with Custom Features

### Step 1: Modify Header

```cpp
// common.h
enum ActionType {
    ACT_LEFT_CLICK,
    ACT_RIGHT_CLICK,
    ACT_KEY_PRESS,
    ACT_MOUSE_MOVE   // New!
};
```

### Step 2: Implement Action

```cpp
// HookManager.cpp
void HookManager::FireAction(Binding& b) {
    // ...
    else if (b.actionType == ACT_MOUSE_MOVE) {
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dx = 100;   // Move relative
        input.mi.dy = 100;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        SendInput(1, &input, sizeof(INPUT));
    }
}
```

### Step 3: Update JSON

```cpp
// common.cpp
Json::Value Binding::toJson() const {
    // ...
    obj["actionType"] = (int)actionType;  // 0, 1, 2, 3 (new)
}
```

## Performance Considerations

### Hook Latency

- Low-level hooks: <1ms latency
- Global hooks: May increase system latency if not efficient
- Recommended: Keep hook handlers minimal

### Memory Usage

- Per-binding timer: ~24 bytes
- Per-profile: ~200-500 bytes (depending on binding count)
- Typical usage: <5MB RAM

### CPU Usage

- Idle: <0.1%
- Active firing at 10 CPS: ~0.5% single core
- Active firing at 20 CPS: ~1-2% single core

## Debugging Tips

### Enable Console Output

```cpp
// main.cpp
#include <iostream>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

int WINAPI WinMain(...) {
    // Allocate console
    AllocConsole();
    _setmode(_fileno(stdout), _O_U8TEXT);
    
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    
    std::cout << "Debug output here" << std::endl;
}
```

### Log Hook Events

```cpp
// HookManager.cpp
LRESULT HookManager::MouseProc(...) {
    OutputDebugStringW(L"Mouse hook fired\n");
    // ...
}
```

### Verify JSON

```cpp
// Test JSON parsing
Json::Value root;
std::string errors;
if (!Json::parseFromStream(builder, ifs, &root, &errors)) {
    MessageBoxW(NULL, std::wstring(errors.begin(), errors.end()).c_str(), L"JSON Error", MB_OK);
}
```

## Known Limitations

1. **Max 5 bindings per profile** - Prevents UI clutter
2. **No mouse position tracking** - Can only trigger on button press
3. **No conditional logic** - All actions fire on trigger, no "if-then"
4. **Single trigger per binding** - Can't combine keys (e.g., Ctrl+S)
5. **No recording/playback** - Must manually configure each binding

## Future Enhancements

- [ ] Recording/playback macro sequences
- [ ] Conditional logic (if-then-else)
- [ ] Mouse movement actions
- [ ] Customizable hotkeys
- [ ] Profile import/export
- [ ] Tray icon with quick access
- [ ] Multiple simultaneous triggers
- [ ] GUI profile editor (drag-drop)

---

**For questions or contributions, visit GitHub!**
