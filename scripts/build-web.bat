@echo off
echo Setting up Emscripten environment...
call C:\raylib\emsdk\emsdk_env.bat

echo.
echo Configuring Web Build...
if not exist "build-web" mkdir build-web
cd build-web
call emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -G Ninja

echo.
echo Building...
cmake --build .

echo.
echo Build complete! Starting local web server...
echo Access game at: http://localhost:8080/raylib-game-template.html
cd raylib-game-template
python -m http.server 8080
