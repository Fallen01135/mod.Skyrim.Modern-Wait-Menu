# Modern Wait Menu

[![License](https://img.shields.io/badge/License-Proprietary-red.svg)](https://github.com/Fallen01135/mod.Skyrim.Modern-Wait-Menu/blob/main/LICENSE)
![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)
![Game](https://img.shields.io/badge/Game-Skyrim_SE/VR-blue.svg)
[![Release](https://img.shields.io/badge/Release-2.1.1-blue.svg)](https://www.nexusmods.com/skyrimspecialedition/mods/117661)

Modern Wait Menu is a complete overhaul and redesign of Skyrims Wait & Rest Menu inspired by modern UI elements in games like Cyberpunk 2077 and The Witcher 3.
This Repository contains all the source code for this mod, including the dll source code, as well as flash elements.

The release version you can download on [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/117661).

> [!Note]
> **A Note from the Author:**
> I do not believe in elitism in programming. If you have any questions about my code, licensing or anything else, please do not hesitate to reach out, I am always happy to help! You can contact me via Nexus Mods, Discord, GitHub, or any other platform where I am active. Remember, we all start somewhere, and there are no wrong questions.

---

## 🛠 Features
- Complete UI redesign to Skyrims wait menu
- Dynamic Weather indicator (compatible with all weather mods)
- 'Current Time' to 'Destination Time' text when selecting a time
- Current in game date displayed in the UI
- New hoverable cursor indicator
- Full Controller support (Left stick, D-Pad left/right and LB/RB to set the wait time)
- Day waiting up to 32 days.
- Customizable Date String via ini
- Available in English and German

---

## 📂 Repository Structure
- `.data\DLL\src` and `.data\DLL\Include`: Contain all C++ headers and source files for the SKSE Plugin.
- `.data\Flash`: Contains all the .fla and .as files for the Menus
- `Interface\`: Compiled menu files in .swf format.
- `Interface\Translations`: SKSE Translation files for the mod.
-  `SKSE\`: The compiled .dll and the .ini file for it.
- `.data\DLL\xmake.lua`: The Project build configuration file.
- `.data\DLL\Generate Project.bat`: Project build automation script, clears cache as well as building the Visual Studio Solutions.
- `update_submodules.bat`: Helper script to keep dependencies up to date.
- `pack_mod.csx`: C# script for automate mod packaging.

---

## 💻 Developer Guide (Building from Source)

This repository contains the full source code for transparency and review. The project utilizes a custom xmake and .NET build system. If you wish to contribute via Pull Requests, please contact me directly for build setup instructions.

> [!Note]
> I might add a detailed step-by-step guide at some point. However, I refuse to write a guide that can only be followed by an elitist group. If I create a guide, it will be designed so that anyone can understand it, even new programmers without extensive experience.

---

## 📜 License & Terms of Use

This project is **Proprietary**. All rights reserved by **Fallen011[35]** (2026).
To check the license and what I permit here: [License](https://github.com/Fallen01135/mod.Skyrim.Modern-Wait-Menu/blob/main/LICENSE)

For bug fixes, optimizations, or feature suggestions, please submit a **Pull Request** or reach out directly. See the full [License](https://github.com/Fallen01135/mod.Skyrim.Modern-Wait-Menu/blob/main/LICENSE) file for details.
