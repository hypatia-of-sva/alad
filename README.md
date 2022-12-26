# alad

alad - glad-like OpenAL-loader by Hypatia of Sva

Usage:

Initalize with

        aladLoadAL(NULL);

for default initialization, or with

        aladLoadAL(my_alGetProcAddress);

where my_alGetProcAddress is a loader function of type LPALGETPROCADDRESS for custom initalization.
Default initialization will pull in OpenAL32.dll / soft_oal.dll on Windows or libopenal.so / libopenal.so.1 on Unix respectively.
Make sure one of these dynamic libraries are on path for LoadLibraryA / dlopen, change the code below in _alad_open or provide your own function loader.

Uodate ALC pointers to those loaded with a specific ALCdevice with

        aladUpdateALCPointers(device);

Unload the library loaded with default initalization with

        aladTerminate();


ALAD will attempt to load all function pointers in all extensions, the ones not available will be NULL, you have to check all functions you want to use for that.
The ALC_INVALID_VALUE-Error-State of alcGetProcAddress is not being checked.
If a functionality you expect to be present isn't working, you might want to check the spelling of the string in the internal _alad_load_alc_functions function.



Copyright notice from the original OpenAL header alext.h:

 * OpenAL cross platform audio library
 * Copyright (C) 2008 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html

 
