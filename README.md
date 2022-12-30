# alad

### Overview

alad is a [glad](https://github.com/Dav1dde/glad)-like, header-only C99 [OpenAL](http://www.openal.org/)-loader library by Hypatia of Sva

Dependencies for compilation: Kernel32.dll on Windows and -ldl on Unix for the default initialization.

Usage:

Include the single header

        #include "alad.h"
        
once with

        #define ALAD_IMPLEMENTATION

to define the functions and global function pointers, and include the header without this symbol defined in all other translation units.

This will pull in all OpenAL symbols except the functions.

There are two interfaces to initialize the API, the (recomended) simplified interface and the manual interface, that gives you more options. 

### Simplified Interface

The simplified interface initializes the library first with

        aladLoadAL();

which loads in all the core functions from the shared library. This should be enough to select a device and create a context. Once you've done that, call

        aladUpdateAL();

which will load all extensions with the current context and its device. Once you've done, terminate the library with

        aladTerminate();

this will unload the shared library.

The library should be named
- OpenAL32.dll / soft_oal.dll on Windows
- libopenal.so.1 / libopenal.so on Linux/BSD
- libopenal.1.dylib / libopenal.dylib on Mac OS.

and should be on path for LoadLibraryA / dlopen.

alad will attempt to load all function pointers in all extensions as currently described in the [openal-soft](https://github.com/kcat/openal-soft) headers (repo version from 24.12.2022, commit https://github.com/kcat/openal-soft/commit/4fe6eba8c79a4c9cad91d6f6835506cde96a48c4), the ones not available will be NULL, you have to check all functions you want to use for that.
The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.


### Manual interface (not recommended)

The manual interface initializes the function pointers first with

        aladLoadALFromLoaderFunction(NULL);

for default initialization, or with

        aladLoadALFromLoaderFunction(my_alGetProcAddress);

where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization, to load all the function via alGetProcAddress.

Default initialization will pull in OpenAL32.dll / soft_oal.dll on Windows or libopenal.so.1 / libopenal.so on Unix respectively,
and libopenal.1.dylib / libopenal.dylib on Mac OS (yet untested).

Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code below in _alad_open or provide your own function loader.

The shared library will only be loaded once, you can call "aladLoadALFromLoaderFunction(NULL)" as often as you want
to reload the pointers from the loaded shared library.

If you're unsure about loading with a function loader, intialize with

        aladLoadALContextFree(AL_FALSE);

to load function pointers directly from the shared library. This only works for default initialization.
This will however not load all function pointers, but only those necessary to create a context.
Those are the core ALC functions and all Core AL functions not relating to buffers, listeners, sources,
and also not alDopplerFactor, alDopplerVelocity, alSpeedOfSound and alDistanceModel.
However, if you change the parameter to anything else (like AL_TRUE), all core AL and ALC functions will be loaded from the shared library.

If you want to use your own library, not available in default initialization, and want to load directly from it, you need to write a wrapper of type
LPALGETPROCADDRESS of the form

        my_alGetProcAddress("[al-function]") = dlsym(my_lib, "[al-function]").
        
aladLoadALContextFree is not used here.

These aladLoad functions only initialize the Core API. To load the extensions you have to update the function pointers.

You can update those via the ALCcontext* context through

        aladUpdateALPointers(context, AL_FALSE);
        aladUpdateALCPointersFromContext(context, AL_FALSE);

which will load all AL and ALC functions, including extensions, via alGetProcAddress for the specific context
by switching the current context temporarily. It will just use the current context, if the parameter is NULL.
If you replace AL_FALSE with anything else (AL_TRUE makes the most sense) this will only load the extensions,
and the core function pointers will remain unchanged


Update ALC pointers to those loaded with a specific ALCdevice* device with

        aladUpdateALCPointersFromDevice(device, AL_FALSE);

If you want to remove this reference to the device, reload them to the nonspecific pointers by calling "aladUpdateALCPointersFromContext" again.
or by calling "aladLoadALContextFree()" if you need them without reference to any context.
Calling "aladLoadALFromLoaderFunction" again won't do anything different from "aladUpdateALCPointersFromContext",
since both call alGetProcAddress and are therefore just dependent on driver state.
The last parameter again makes sure to reload the core, change the value to disable that.

Unload the library loaded with default initalization with

        aladTerminate();
        
This will not unload the function pointers or set them to NULL, but they will be probably invalid after unloading the shared library.

alad will attempt to load all function pointers in all extensions as currently described in the [openal-soft](https://github.com/kcat/openal-soft) headers (repo version from 24.12.2022, commit https://github.com/kcat/openal-soft/commit/4fe6eba8c79a4c9cad91d6f6835506cde96a48c4), the ones not available will be NULL, you have to check all functions you want to use for that.
The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.
If a functionality you expect to be present isn't working, you might want to check the spelling of the strings in the internal "_alad_load_alc_functions" function.



