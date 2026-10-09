# Modern Wait Menu

[![License](https://img.shields.io/badge/License-Source--Available-orange.svg)](LICENSE.md)
![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)
![Game](https://img.shields.io/badge/Game-Skyrim_SE/VR-blue.svg)
[![Release](https://img.shields.io/badge/Release-2.2.0-blue.svg)](https://www.nexusmods.com/skyrimspecialedition/mods/117661)

Modern Wait Menu is a complete overhaul and redesign of Skyrim's Wait & Rest Menu, inspired by modern UI elements in games like Cyberpunk 2077 and The Witcher 3.

This repository contains everything for this mod, including the DLL source code and the Flash files. To **download and play** the mod, get it on [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/117661).

> [!NOTE]
> **A Note from the Author:**
> I do not believe in elitism in programming. If you have any questions about my code, licensing or anything else, please do not hesitate to reach out, I am always happy to help! You can contact me through the Nexus Mods comments or on my Discord server (see [Contact](#contact) below). Remember, we all start somewhere, and there are no wrong questions.


## Features

<p align="center">
  <img src="source/Media/Wait_Menu_Preview.png" alt="Modern Wait Menu in-game screenshot" width="1000">
</p>

- Complete redesign of the wait menu
- Dynamic weather indicator, compatible with all weather mods
- Current time and destination time while selecting
- Full controller support
- Waiting up to 32 days
- Available in English and German

The full feature list is on the [Nexus Mods page](https://www.nexusmods.com/skyrimspecialedition/mods/117661).


## Building from Source

This repository always contains the **complete working environment**: all source files, dependencies (as submodules), and scripts. If you have the tools below, you can build everything yourself.


### Requirements

| Tool | Used for | Needed |
| --- | --- | --- |
| Git or GitHub Desktop | Cloning the repository | Always |
| .NET SDK and [dotnet-script](https://github.com/dotnet-script/dotnet-script) | Running `pack_mod.csx` to package the mod | For packaging |
| Visual Studio 2026 (version 18) with the **Desktop development with C++** workload | Building the DLL | Only if the repository contains a DLL |
| CMake | Building the DLL | Only if the repository contains a DLL |
| Python | Building the DLL | Only if the repository contains a DLL |
| Adobe Flash CS6 | Editing the `.fla` menu files | Only to edit the menus |


### Steps

1. **Clone** the repository.
2. **Run `update_submodules.bat` right after cloning.** This downloads the dependencies. Nothing will build without it.
3. **Build the DLL:** open the `source\DLL` folder in Visual Studio and let CMake run. With all requirements installed, it should build without further setup.
4. **Package the mod:** run `pack_mod.csx` with dotnet-script.


### About the Flash files

The compiled menus (`.swf`) are already included, so you do **not** need Flash to build or package the mod. You only need Adobe Flash CS6 to edit the `.fla` and `.as` source files. Adobe no longer sells it, so I unfortunately cannot give a setup guide for this part.

If something does not work, ask me. I am happy to help, and your feedback helps me improve this guide.


## Contact

My official channels are:

- **Nexus Mods:** [comments on the mod page](https://www.nexusmods.com/skyrimspecialedition/mods/117661?tab=posts)
- **Discord:** [my Discord server](https://discord.gg/c99MyyPzhJ)

Questions, permission requests, and everything else go there. GitHub issues are for **bug reports only**.


## Credits

- [HeavyBurns](https://www.nexusmods.com/skyrimspecialedition/users/91502233) for the idea and the artwork. Check out his [YouTube channel](https://www.youtube.com/@HeavyBurns/videos) for more Bethesda-related content.


## License

This project is **source-available, not open source**. Copyright © 2026 **Fallen01135**. All rights reserved.

In short: you may look at the code, learn from it, and make patches, translations and add-ons that require this mod. You may not re-upload the mod, distribute the DLL, sell it, or port it. The full terms in [LICENSE.md](LICENSE.md) are what counts.

Bug fixes, optimizations, and feature suggestions are welcome as **Pull Requests**. Please do not publish your own builds or releases.

If the project is ever abandoned, it becomes open source under the MPL-2.0 after a defined process. The details are in the [LICENSE.md](LICENSE.md).

This project uses third-party libraries under their own licenses. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
