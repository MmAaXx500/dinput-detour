# DInput Detour

DInput Detour is a tool for intercepting DirectInput calls in Windows applications using [Detours](https://github.com/microsoft/Detours).

Sometimes it's useful to analyze application behavior to improve compatibility or debug issues. DInput Detour enables logging of every parameter of the hooked functions to notice any differences or behavioral errors.

## Supported Environments

 - Windows 10
 - Wine / Proton

## Usage

There are currently no binary releases. If you want to try it, follow the [Compile from source](#compile-from-source) section.

Typically, you'll need `withdll.exe` and the `dinput-detour*.dll`s. Use the 64-bit DLL for 64-bit applications and the 32-bit DLL for 32-bit applications. After successful compilation, these are located in the `out/bin` directory.

> [!CAUTION]  
> Think twice before injecting a DLL into any process, especially games. Anti-cheat systems or other protections may detect it and potentially ban you or block access to the software. DInput Detour does not circumvent these protections in any way.

To launch an application with DInput Detour injected:

```
withdll.exe /d:dinput-detour64.dll my64bit-program.exe
```

DInput Detour will log the activity of the process under the `C:\dinput-detour-logs` directory on both Windows and Wine/Proton. Be aware that the logs grow fast if the application makes frequent calls to the DInput API.

### Steam (Windows)

Starting a Steam game on windows is straightforward. In the game's launch options field use this command.

```
X:\path\to\withdll.exe -d:X:\path\to\dinput-detour64.dll %command%
```

### Steam (Linux)

On Linux the situation is more complex because steam uses various wrappers to launch a game. In general this oneliner should cover most cases.

The following command is unreadable, but it can be copied and used in the Steam launch options. Don't forget to change the paths mentioned bellow.

```
PROTON_LOG=1 STEAM_COMPAT_MOUNTS="/path" bash -c 'args=(); withdll=0; winconv=0; for ((i=$#; i>0; i--)); do if [[ "${!i}" == "waitforexitandrun" && $withdll -eq 0 ]]; then args=("waitforexitandrun" "Z:\\path\\to\\withdll.exe" "/d:Z:\\path\\to\\dinput-detour64.dll" "${args[@]}") && withdll=1; elif [[ ${!i} == /* && ${!i} == *.exe && $winconv -eq 0 ]]; then args=("Z:${!i//\//\\\\}" "${args[@]}") && winconv=1; else args=("${!i}" "${args[@]}"); fi; done; exec "${args[@]}"' -- %command%
```

Here is a readable format.

```
PROTON_LOG=1 STEAM_COMPAT_MOUNTS="/path" bash -c '
    args=();
    withdll=0;
    winconv=0; 
    for ((i=$#; i>0; i--)); do
        if [[ "${!i}" == "waitforexitandrun" && $withdll -eq 0 ]]; then
            args=("waitforexitandrun"
                "Z:\\path\\to\\withdll.exe"
                "/d:Z:\\path\\to\\dinput-detour64.dll"
                "${args[@]}")
            withdll=1;
        elif [[ ${!i} == /* && ${!i} == *.exe && $winconv -eq 0 ]]; then
            args=("Z:${!i//\//\\\\}" "${args[@]}")
            winconv=1;
        else
            args=("${!i}" "${args[@]}");
        fi;
    done;
    exec "${args[@]}"
' -- %command%
```

`Z:\\path\\to\\withdll.exe` must be changed to the real location of the file. Use backslashes to be safe.

`/d:Z:\\path\\to\\dinput-detour64.dll` must be changed to the real location of the dll. Use backslashes to be safe.

`PROTON_LOG=1` is optional, but useful to have logs to see if something went wrong. The log is located at `~/steam-<gameid>.log`.

`STEAM_COMPAT_MOUNTS="/path"` is also optional, but required if the game, `withdll.exe`, or `dinput-detour*.dll` is outside of the steam library folder or the home folder. The path must point to the same device where the files are located. For example, / does not work for other mounted drives, it only works for the root file system.

## Compile from Source

To compile DInput-Detour from source you need to clone this repository and the included submodules.

```
git clone https://github.com/MmAaXx500/dinput-detour.git --recurse-submodules
```

If you already have the repo cloned and you want to pull the latest changes, you will need to pull the changes from this repo and update the submodules too.

```
git pull --recurse-submodules
```

At this point you are ready to compile DInput-Detour. Chose one of the methods below, or invent you own.

Regardless of the method used, build files are placed in the `out/build/<preset-name>` directory. For convenience, binaries are copied to `out/bin`.

### Visual Studio / Visual Studio Code

These combinations known to work out of the box:
 - Visual Studio + MSVC (Windows)
 - Visual Studio Code + MSVC (Windows)
 - Visual Studio Code + MinGW (Linux)

MinGW on Windows is not working right now. Use the [CLI](#cli-windows-mingw) method.

### CLI (Windows MSVC)

You need the appropriate command prompts to build x86 and x64 versions. Use `x64 Native Tools` for x64, and `x64_x86 Cross Tools` for x86. These are known to work.

In the `x64` prompt, you can build the `msvc-x64-*` presets. In the `x64_x86` prompt, the `msvc-x86-*` presets are usable.

To configure the project:

```
cmake.exe --preset msvc-x64-release
```

To build:

```
cmake.exe --build --preset msvc-x64-release
```

Now you are ready to use it. Go to [Usage](#usage).

### CLI (Windows MinGW)

Install the MinGW toolchain in your environment (with MSYS for example) and choose one of the `mingw-*` presets. Use the appropriate 64bit or 32bit environment to build the respective presets.

```
cmake --preset mingw-x64-release
cmake --build --preset mingw-x64-release
```

Now you are ready to use it. Go to [Usage](#usage).

### CLI (Linux MinGW)

Install the MinGW toolchain in your environment and choose one of the `mingw-*` presets.

```
cmake --preset mingw-x64-release
cmake --build --preset mingw-x64-release
```

Now you are ready to use it. Go to [Usage](#usage).
