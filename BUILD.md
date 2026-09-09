# Build Instructions 🔨

## Prerequisites

- **Windows 7 or later** (tested on Windows 10/11)
- **Visual Studio 2015+** with C++ support (or Build Tools for Visual Studio)
- **C++17 support** required
- **Administrator privileges** (for running the built .exe)
- **jsoncpp library** (included or via package manager)

## Setup Steps

### 1. Install Dependencies

#### Option A: Using vcpkg (Recommended)
```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install jsoncpp
.\vcpkg install jsoncpp:x86-windows
.\vcpkg integrate install
```

#### Option B: Manual Installation
1. Download jsoncpp from https://github.com/open-source-parsers/jsoncpp
2. Build or use pre-built binaries
3. Add include path and lib path to project settings

### 2. Clone Repository

```bash
git clone https://github.com/Thunder-Zhub/multi-game-macro-manager.git
cd multi-game-macro-manager
```

### 3. Open in Visual Studio

```bash
# Open solution file
start macro_windows.sln
```

### 4. Configure Project

1. **Right-click project** → Properties
2. Go to **VC++ Directories**
3. Add jsoncpp include path to **Include Directories**
4. Add jsoncpp lib path to **Library Directories**
5. Go to **Linker → Input**
6. Add `jsoncpp.lib` to **Additional Dependencies**

### 5. Build

```bash
# Build in Debug mode (for testing)
Build → Build Solution (Ctrl+Shift+B)

# Or build Release (optimized)
Configuration Manager → Select "Release" → Build
```

### 6. Run

```bash
# Debug build
Debug/macro_windows.exe

# Release build
Release/macro_windows.exe
```

⚠️ **IMPORTANT: Run as Administrator** - Low-level hooks require admin privileges!

## Troubleshooting Build Issues

### "jsoncpp not found"
- Ensure jsoncpp is installed and paths are correct
- Check Project → Properties → VC++ Directories
- Rebuild the solution (Ctrl+Shift+B)

### "Linking errors"
- Make sure jsoncpp.lib is in Additional Dependencies
- Verify the library file exists in the lib path
- Try Clean Solution → Rebuild Solution

### "MIDL compilation errors"
- These can usually be ignored if final .exe builds successfully
- If blocking build, check for stray .idl files and remove them

## Directory Structure

```
macro-manager-refactored/
├── src/
│   ├── common.h/.cpp          # Shared types and helpers
│   ├── ProfileManager.h/.cpp   # Profile load/save logic
│   ├── HookManager.h/.cpp      # Low-level hooks implementation
│   ├── MainWindow.h/.cpp       # GUI implementation
│   └── main.cpp                # Entry point
├── macro_windows.vcxproj       # Project configuration
├── profiles.json.example       # Example config file
├── README.md                   # Overview
└── BUILD.md                    # This file
```

## Advanced Build Options

### Release Build with Optimizations
```bash
# Smaller file size, better performance
Configuration: Release
Platform: x86 or x64
```

### Static Linking
To statically link jsoncpp (single .exe, no DLL dependencies):
1. Build jsoncpp as static library (.lib)
2. Link against static version in project properties

### Code Analysis
```bash
Analyze → Run Code Analysis on Solution
```

## Testing the Build

1. **Run as Administrator**
   ```bash
   # Create shortcut with admin privilege
   Right-click .exe → Create Shortcut → Edit → Advanced → Check "Run as Administrator"
   ```

2. **Verify hooks are installed**
   - Launch app
   - Should see message confirming hook installation
   - No error about permissions

3. **Test with simple profile**
   - Create "Test" profile
   - Select a running game or notepad
   - Add one binding (Mouse4 → Left Click)
   - Press and hold Mouse4, should see clicks in target window

## Debugging

### Enable Debug Output
Add to main.cpp:
```cpp
#include <iostream>
#include <windows.h>
// Redirect cout to debug output
freopen("CONOUT$", "w", stdout);
```

### Debug Hooks
- Add breakpoints in HookManager.cpp
- Mouse/keyboard callbacks will trigger hooks
- Watch variables to verify trigger detection

### Profile Loading Issues
- Check `%APPDATA%\MacroManager\profiles.json` exists
- Verify JSON syntax (use json validator)
- Check file permissions (must be readable)

## Building Portable Version

For distribution without dependencies:

1. Use static linking for jsoncpp
2. Ensure no external DLLs needed
3. Create single .exe file
4. Include `profiles.json.example` for users

```bash
# Final distribution package
macro_manager_v2.0/
├── macro_windows.exe
├── profiles.json.example
└── README.txt
```

## Performance Considerations

- **Debug Build**: Slower execution, better for development
- **Release Build**: ~3-5x faster, recommended for daily use
- **Optimizations**: 
  - O2 optimization enabled for Release
  - Link-time code generation (LTCG) reduces binary size

## Support & Issues

If build fails:
1. Check all prerequisites are installed
2. Verify Visual Studio has C++ tools installed
3. Check Windows SDK is up to date
4. Post issue to GitHub with:
   - Visual Studio version
   - Error message (full)
   - Windows version
   - Steps to reproduce

---

**Happy building! 🚀**
