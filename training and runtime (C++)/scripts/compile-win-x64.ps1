#prerequisites:
#    OpenCV 4.x must be compiled (e.g. via vcpkg) and its static libs put in $opencvBaseDir
#    VS 2022 Community C++ tools must be installed

# execute these commands only once:
#Import-Module "$env:ProgramFiles\Microsoft Visual Studio\2022\Community\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
#Enter-VsDevShell -VsInstallPath "$env:ProgramFiles\Microsoft Visual Studio\2022\Community" -DevCmdArguments '-arch=x64'

$opencvBaseDir = "$env:ProgramFiles/vcpkg/packages/opencv4_x64-windows-static/"
$outDir = "../bin/" #"../bin/win-x64/"

$includeDirs = (Get-ChildItem -Path "../src/CoreLib/" -Recurse -Directory | Resolve-Path -Relative) | ForEach-Object {"-I $_/"}
$includeDirs += "-I ../src/ -I ../src/CoreLib/ -I '$opencvBaseDir/include/'"
$includeDirs = @($includeDirs) -join " "

$libs = "libcmt.lib", "libvcruntime.lib", "libucrt.lib", "gdi32.lib", "Advapi32.lib", "Comdlg32.lib"
$libs += Get-ChildItem -Path "$opencvBaseDir/lib/" -Filter *.lib | ForEach-Object {"'$opencvBaseDir/lib/$_'"}
$libs = @($libs) -join " "

New-Item -ItemType Directory -Force -Path $outDir

###### compile Test.exe
$appName = "Test.exe"
$cFile = "../src/ViolaJones/Test/Test.cpp"
$params = 
   "/Ox /std:c++20 /EHsc /MT",
   $includeDirs, 
   $cFile,
   "/link",
   $libs,
   "/out:$outDir/$appName"

$params = @($params) -join " "
Write-Output "building $appName..." 
Invoke-Expression ("cl " + $params)
Remove-Item -Path "Test.obj" -Force

###### compile Train.exe
$appName = "Train.exe"
$cFile = "../src/ViolaJones/Train/Train.cpp"
$params = 
   "/Ox /std:c++20 /EHsc /MT",
   $includeDirs, 
   $cFile,
   "/link",
   $libs,
   "/out:$outDir/$appName"

$params = @($params) -join " "
Write-Output "building $appName..." 
Invoke-Expression ("cl " + $params)
Remove-Item -Path "Train.obj" -Force
