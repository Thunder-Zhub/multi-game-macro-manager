# Quick Start Guide 🎮

## Installation

### 1. Download & Extract

```bash
git clone https://github.com/Thunder-Zhub/multi-game-macro-manager.git
cd multi-game-macro-manager
```

### 2. Build (See BUILD.md)

```bash
# In Visual Studio
Build → Build Solution
```

### 3. Run as Administrator ⚠️

```bash
Release/macro_windows.exe
```

## First Time Setup

### Step 1: Create a Profile

1. **Click "Select Target Game"**
2. Choose your game from the list (e.g., Minecraft, Valorant)
3. Click OK
4. A new tab appears with the game name

### Step 2: Add Your First Binding

1. **Click "+ Add Binding"** button
2. Fill in the dialog:
   - **Trigger**: Mouse4 (or F6/F7/F8/Caps Lock)
   - **Action**: Left Click (Rapid)
   - **CPS**: 10 (clicks per second)
3. **Click Save**

### Step 3: Test It

1. **Bring your game window to foreground** (Alt+Tab)
2. **Press and hold the trigger button** (Mouse4)
3. You should see rapid clicks in the game
4. **Release the button** → clicks stop immediately

## Using Profiles

### Multiple Games

```
Profile Tabs:
[Minecraft] [Valorant] [Osu!]
     ↑
  Click to switch profiles
```

Each profile has:
- Different target game
- Its own set of bindings
- Enable/disable independently
- Auto-switch when game is active

### Save & Load

- Profiles auto-save to: `%APPDATA%\MacroManager\profiles.json`
- Next launch, all profiles automatically load
- No manual export/import needed

## Global Controls

### Master Toggle (F10)

Press **F10** anytime to:
- ✅ **ENABLE ALL** macros (green button)
- ❌ **DISABLE ALL** macros (red button)
- Useful for quick disable when needed

### Status Indicator

```
Status Bar:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Profile: Minecraft | Global: ENABLED (F10 to toggle)
```

## Binding Guide

### Trigger Options

| Trigger | Type | Best For |
|---------|------|----------|
| Mouse4 | Mouse Side Button | Default clicker |
| Mouse5 | Mouse Side Button | Alternate action |
| F6-F8 | Keyboard | Extra key binding |
| Caps Lock | Keyboard | Map existing hotkey |

### Action Options

| Action | Effect | CPS Range |
|--------|--------|----------|
| Left Click | Hold = rapid left clicks | 1-20 |
| Right Click | Hold = rapid right clicks | 1-20 |
| Key Press | Hold = rapid key presses | 1-20 |

### CPS (Clicks Per Second)

```
CPS  │ Speed    │ Best For
─────┼──────────┼──────────────────
5-7  │ Slow     │ Breaking blocks (Minecraft)
10   │ Medium   │ Most games (default)
15   │ Fast     │ Faster clicking games
20   │ Maximum  │ Ultra-fast clicking
```

## Common Setups

### Minecraft Auto-Clicker

```
Profile: Minecraft
Binding:
  Trigger: Mouse4
  Action: Left Click (Rapid)
  CPS: 10

Usage: Hold Mouse4 to auto-break blocks
```

### Osu! Stream Clicking

```
Profile: Osu!
Binding 1:
  Trigger: Mouse4
  Action: Left Click (Rapid)
  CPS: 20

Binding 2:
  Trigger: Mouse5
  Action: Left Click (Rapid)
  CPS: 20

Usage: Alternate Mouse4/Mouse5 for streams
```

### Valorant Ability Spam

```
Profile: Valorant
Binding:
  Trigger: F6
  Action: Key Press (E) - Ability key
  CPS: 15

Usage: Hold F6 to spam abilities
```

## Safety Features

✅ **Game-Specific Only**
- Macros ONLY work in target game window
- No accidental clicks in browser/Discord

✅ **Global Kill Switch**
- Press F10 to disable everything instantly
- Useful if something goes wrong

✅ **Maximum 5 Bindings**
- Per profile limit prevents complexity
- Easy to manage and troubleshoot

✅ **No Duplicate Triggers**
- Can't accidentally create conflicting bindings

## Keyboard Shortcut Reference

| Shortcut | Action |
|----------|--------|
| F10 | Toggle all macros ON/OFF |
| Mouse4 | Trigger binding (if configured) |
| Mouse5 | Trigger binding (if configured) |
| F6-F8 | Trigger binding (if configured) |

## Troubleshooting

### "Macros not working"

1. ✅ Check **Global Enable** is ON (green button)
2. ✅ Verify **target game window is active** (foreground)
3. ✅ Check **binding is in the list** and has correct CPS
4. ✅ **Run as Administrator** - this is critical!
5. ✅ Try a simple test: create binding for Mouse4 → Left Click

### "Hooks failed to install"

- **Solution**: Run the .exe as Administrator
- Some antivirus may block hooks - add to whitelist

### "Profiles won't save"

- Check folder exists: `%APPDATA%\MacroManager\`
- Check folder has write permission
- Try running as Administrator

### "Game crashes when using macros"

- **Some games detect macro input and crash**
- Try lower CPS (e.g., 5-8 instead of 20)
- Some games explicitly forbid macros in ToS
- Consider anti-cheat detection

## Performance Tips

- Lower CPS for less CPU usage
- Disable unused profiles
- Keep max 3 active profiles
- Use F10 when not playing

## Terms of Service ⚠️

**Important**: Check your game's Terms of Service!

- Some games **prohibit macros/aimbots**
- Using macros may result in **account ban**
- **Use responsibly** and at your own risk
- This tool is for **single-player/private servers only**

Read before use:
- Valorant: Uses Vanguard anti-cheat (may detect)
- Minecraft: Generally OK for single-player
- Osu!: Generally OK but check local rules
- Online multiplayer: Check ToS carefully

## Getting Help

1. **Check README.md** for overview
2. **Check BUILD.md** for build issues
3. **GitHub Issues** - report bugs with:
   - What you were trying to do
   - Error message (if any)
   - Binding configuration
   - Windows version
4. **Discord/Forum** - community support

## Next Steps

✅ Create your first profile
✅ Add a test binding
✅ Test with your game
✅ Adjust CPS to your preference
✅ Create profiles for other games
✅ Use F10 to toggle when needed

---

**Enjoy your optimized gameplay! 🎮**

For issues, feature requests, or contributions → GitHub Issues
