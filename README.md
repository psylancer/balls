Balls is a simple Direct3D9 based game.

###Building from Eclipse
* Run cmd.exe
* Move to your build folder, created outside of source folder
* Run: ```cmake <Path To Source With Balls Project> -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_MAKE_ARGUMENTS="-j%NUMBER_OF_PROCESSORS% -s" -DDX_INCLUDE="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" -DDX_LIB_PATH="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86"```
* Now you can add existing project into Eclipse workspace to make and run targets from it.
* CMAKE_BUILD_TYPE can be also set to Release or RelWithDebInfo
* Also you can build project via console by command: ```mingw32-make -j%NUMBER_OF_PROCESSORS% -s```
* If you running on 32-bit (x86) of Windows path ```C:\Program Files (x86)``` changed to ```C:\Program Files```

###Building using Visual Studio 2013
* Run cmd.exe
* Move to your build folder, created outside of source folder
* Run: ```cmake <Path To Source With Balls Project> -G "Visual Studio 12" -DDX_INCLUDE="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include" -DDX_LIB_PATH="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86"``` to generate VS solution
* VS solution is ready now
* Also you can build from console via MSBuild:
* ```"C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" balls.sln /maxcpucount:%NUMBER_OF_PROCESSORS% /property:Configuration=Debug /property:Platform=Win32```
* If you running on 32-bit (x86) of Windows path ```C:\Program Files (x86)``` changed to ```C:\Program Files```

cmake tool can be downloaded [here](http://www.cmake.org/download/). Version 2.8.12 and above require.
To enter only cmake command, without full path, in cmd please do not forget to add ```cmake-2.8.12.2-win32-x86\bin``` into your path environment.