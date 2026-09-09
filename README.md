# Multi-Game Macro Manager 🎮

Advanced auto-clicker/macro tool with **profile system**, **auto-switching**, and **game-specific bindings** for Windows.

## Features ✨

- **Profile System**: Create separate binding profiles for each game
- **Auto-Switch**: Automatically switch profile when target game window becomes active
- **Game Whitelist**: Macro only works for selected games, won't interfere with others
- **Global Hotkey**: Quick Enable/Disable all macros (default: `F10`)
- **Persistent Config**: Save/load profiles from JSON
- **Modern UI**: Clean interface with profile management, status indicators
- **Thread-Safe**: Proper synchronization for multi-threaded operations
- **Low-Level Hooks**: Intercept mouse buttons and keyboard with minimal latency

## Supported Triggers

**Mouse Buttons:**
- Mouse4 (Side button - forward)
- Mouse5 (Side button - backward)

**Keyboard:**
- F6, F7, F8, Caps Lock
- Customizable via profile editor

## Supported Actions

- Left Click (repeat)
- Right Click (repeat)
- Key Press (repeat)
- Customizable CPS (Clicks Per Second): 1-20

## Installation

### Requirements
- Windows 7 or later
- Visual Studio 2015+ (C++17)
- Administrator privileges (for low-level hooks)

### Build
```bash
# Clone repo
git clone https://github.com/Thunder-Zhub/multi-game-macro-manager.git
cd multi-game-macro-manager

# Open Windows/macro_windows.sln in Visual Studio
# Build Release configuration
```

### Run
```bash
# Execute the built .exe
macro_windows.exe
```

## Usage 📖

1. **Create Profile**
   - Click "New Profile"
   - Enter game name (e.g., "Minecraft", "Valorant")
   - Select target process from list

2. **Add Bindings**
   - Click "+ Add Binding" in profile
   - Choose trigger button/key
   - Choose action (click or key press)
   - Set CPS (default 10)
   - Click Save

3. **Auto-Switch Setup**
   - Enable "Auto-Switch" checkbox
   - When target game window is active, profile auto-activates
   - When game closes/loses focus, macros disable

4. **Enable/Disable All**
   - Press `F10` to toggle all macros on/off
   - Status shows in tray icon

## Configuration File

Profiles are saved in `%APPDATA%\MacroManager\profiles.json`:

```json
{
  "profiles": [
    {
      "name": "Minecraft",
      "targetExeName": "javaw.exe",
      "targetPID": 0,
      "autoSwitch": true,
      "enabled": true,
      "bindings": [
        {
          "triggerCode": 5,
          "triggerType": "MOUSE_BTN",
          "actionType": "LEFT_CLICK",
          "actionCode": 0,
          "cps": 10
        }
      ]
    }
  ],
  "globalEnabled": true,
  "globalHotkey": 121
}
```

## Architecture 🏗️

### Main Components

- **ProfileManager**: Load/save profiles from JSON
- **GameMonitor**: Watch for active window changes, auto-switch profiles
- **BindingEngine**: Execute macros with proper timing
- **HookManager**: Low-level mouse/keyboard hooks
- **MainWindow**: GUI with profile tabs, binding list

### Thread Model

- **Main Thread**: GUI and message handling
- **Hook Thread**: System-wide low-level hooks
- **Game Monitor Thread**: Watch active window
- **Fire Thread**: Execute actions at correct CPS timing

## Security & Safety

- ✅ Hooks are per-process, won't affect other applications when game inactive
- ✅ Admin check on startup
- ✅ Can disable globally with hotkey (F10)
- ✅ Whitelist-only: macros only work for selected games

## Troubleshooting 🔧

**Macros not working?**
1. Run as Administrator
2. Verify target game in process list
3. Ensure "Auto-Switch" enabled or manually select profile
4. Check that Global Enable is ON (F10)

**Hooks not installing?**
- Some antivirus may block low-level hooks
- Add to whitelist or temporarily disable AV

**Config not saving?**
- Check folder permissions: `%APPDATA%\MacroManager\`
- Ensure write access to disk

## Changelog

### v2.0 (Refactored)
- ✨ New profile system with auto-switch
- ✨ Game whitelist/selection
- ✨ Global hotkey (F10)
- 🐛 Fixed thread safety issues
- 🐛 Better cleanup on exit
- 📁 Config persistence (JSON)

### v1.0 (Original)
- Basic binding system
- Single game at a time
- Manual profile switching

## License

MIT License - Feel free to modify and redistribute

## Contributing

Pull requests welcome! Please test thoroughly before submitting.

## ⚠️ Disclaimer

This tool is for personal use and educational purposes only. Use responsibly and in compliance with game ToS. Some games may consider macro usage against their terms of service.

---

**Made with ❤️ for gamers who want to optimize their gameplay**
