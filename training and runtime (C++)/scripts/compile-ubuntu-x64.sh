#prerequisites:
#   sudo apt install libopencv-dev  #OpenCV 4.x

outDir="../bin/" #"../bin/linux-x64/"

includeDirs=$(find "../src/CoreLib/" -type d -printf '-I ../src/CoreLib/%P ')
includeDirs+=" -I /usr/include/opencv4/ -I ../src/ -I ../src/CoreLib/"

libs="-l:libc.so "
libs+="-l:libopencv_core.so -l:libopencv_imgcodecs.so -l:libopencv_highgui.so -l:libopencv_imgproc.so -l:libopencv_videoio.so "

noWarnings=" -Wno-deprecated-declarations  -Wno-unused-result -Wno-format-security -Wno-logical-op-parentheses "
noWarnings+="-Wno-deprecated-anon-enum-enum-conversion -Wno-inline-new-delete -Wno-new-returns-null -Wno-switch -Wno-deprecated-volatile"

mkdir -p $outDir

###### compile Test.o
appName="Test.o"
cFile="../src/ViolaJones/Test/Test.cpp"
params=" -O3 -std=c++20 "
params+="$noWarnings "
params+="$includeDirs "
params+="$cFile "
params+="$libs "
params+="-o $outDir/$appName "

echo "building $appName..."
clang++ $params

###### compile Train.o
appName="Train.o"
cFile="../src/ViolaJones/Train/Train.cpp"
params=" -O3 -std=c++20 "
params+="$noWarnings "
params+="$includeDirs "
params+="$cFile "
params+="$libs "
params+="-o $outDir/$appName "

echo "building $appName..."
clang++ $params
