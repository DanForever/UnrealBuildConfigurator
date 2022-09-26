![GitHub Workflow Status](https://img.shields.io/github/workflow/status/DanForever/UnrealBuildConfigurator/Verify%20Build) ![GitHub all releases](https://img.shields.io/github/downloads/DanForever/UnrealBuildConfigurator/total) ![GitHub](https://img.shields.io/github/license/DanForever/UnrealBuildConfigurator)
# UnrealBuildConfigurator
 Utility to conveniently modify settings in the Unreal BuildConfiguration.xml

##  Introduction
 Unity builds are a great tool for speeding up compilation times, but they can also hide some compilation errors (such as a forgotten `#include` statement). This can be a problem if you have a build server that compiles your project with unity builds disabled - you may find that every now and then, you CI/CD throws up a compilation error that doesn't happen for you locally. At which point you have to disable unity builds locally, rebuild the project, fix the error, submit the fix, and finally turn unity builds back on.

This happens often enough that you know what to do, but you don't quite remember the location of the xml file you need to edit or the exact syntax of the tag you need to insert.

That's where this tool comes in. It's a convenience utility that can automate the entire process.

## Usage
```
UnrealBuildConfigurator.exe [options] --file [path-to-uproject-or-buildconfiguration.xml]
```
Currently you can specify the uproject or the path to the xml directly. When specifying the uproject, the program currently assumes a "non-foreign directory setup", and so looks for a `/Engine` folder in the parent directory of the uproject file.
If there are other directory configurations you want supported, please open a feature request.

### Options
```
--unitybuild / --no-unitybuild
--incredibuild / --no-incredibuild
--fastbuild / --no-fastbuild
--sndbs / --no-sndbs
```
These are all the options that can currently be toggled by the program. Any elements in the BuildConfiguration.xml file that are not associated with any of the specified options will remain unchanged.

If there is an option that you would like to add, please open a feature request.

## References
https://docs.unrealengine.com/5.0/en-US/build-configuration-for-unreal-engine/
