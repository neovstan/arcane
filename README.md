# arcane
> Multifunctional modification as an in-game module for Grand Theft Auto: San Andreas with support for SA-MP.  
> Currently under active development
## What's unique?
The modification has an exclusively remote configuration system, thanks to which hacking of the module's licensing system to the user's computer is severely limited, since in this case there is no possibility to conveniently configure and share settings with other users.
## What libraries are used?
You can see some of it in the third-party folder. Some modules are connected via **conan** (see conanfile.txt).
> [!WARNING]
> Only libraries that are represented as submodules are listed here
* **encryption**
  > Used to encrypt data using the CBC method. Referring to the crypto library
* **kthook**
  > That's a beautiful hooker. Can almost fully work with lambdas
* **mariadbpp**
  > The official C++ connector is shitty with UB and bugs, so we use this one
* **plog**
  > Just simple and uncomplicated
* **plugin-sdk**
  > plugin-sdk in CMake (you need to keep it up to date yourself)
* **sampapi**
  > SAMP-API by LUCHARE and FYP with kin4stat's changes in order to provide multiversionality
* **vmprotect-sdk**
  > Self-designed to ensure that functions are definable even if VMProtect did not apply to the final module
## Why CMake?
Because it provides great opportunities to separate modules, thus allowing to simplify the architecture of the project and not to mix parts that should not overlap and know something about each other.
## How can I build?
```
git clone https://github.com/neovstan/arcane.git
cd arcane
git submodule update --init --remote --recursive
```
Then open the project folder in CLion and set following settings of configuration.
![image](https://github.com/neovstan/arcane/assets/138371982/381b6ac6-011f-40c7-85b0-5ea54bf09de4)
![image](https://github.com/neovstan/arcane/assets/138371982/f5644422-6ba5-457d-83b2-c51e2d9cf68b)
```
-A Win32 -DQT_PATH="path_to_qt\5.15.0\msvc2019" -DASI_PLUGIN_OUTPUT_DIR="path_to_game" -DVMP_DEBUG=1
```

## 
### Available functionality
* **Shooting**
  * **Vector aimbot**
    * Smooth
    * Maximal angle (FOV)
    * Divide angle by distance
    * Minimal distance
    * Delay between target changes
    * Do not change Z-angle
    * Hold target
    * Check for obstacles
    * Check for distance
    * Ignore the dead
    * Ignore same group
    * Ignore friendly nicknames
    * Ignore same color
    * Aim only when shooting
    * Bones
    * Model groups
    * Friendly nicknames
  * **Silent aimbot**

    > [!NOTE]
    > SA-MP packages are not directly editable because game data is substituted
    * Hit rate
    * Maximal angle (FOV)
    * Divide angle by distance
    * Minimal distance
    * Step to increase hit rate
    * Delay between target changes
    * Pull camera toward enemy
    * Turn player toward enemy
    * Display triangle above enemy
    * Increase hit rate
    * Hold target
    * Check for obstacles
    * Check for distance
    * Ignore the dead
    * Ignore same group
    * Ignore friendly nicknames
    * Ignore same color
    * Bones
    * Model groups
    * Friendly nicknames
  * **Auto cbug**
    * Duration from attack to squat
    * Duration from squat to attack
    * Key
    * Auto repeat
  * **Auto shot**

    > [!NOTE]
    > Including dynamically changing the minimum required sighting radius of the weapon depending on the distance to the enemy
* **Visuals**

  * Flat box
  * Three dimensional box
  * Line
  * Name
  * Hide samp nametag
  * Bones
  * FOV
  * Framerate
* **Actor**
  * Air Walking
    * Speed
    * Acceleration time
  * Anti-Stun
    * Chance
  * Fast Run
    * Speed
### Media
* [Developer blog](https://t.me/arcanedev)
* [Website](https://arcane.su)
* [Main page of the project](https://vk.com/arcanesamp)
* [User chat](https://vk.me/join/AJQ1d0FBaxjsqxH21BTDia9L)
