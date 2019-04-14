echo %0
cd %~dp0
pause

cd 
pushd ..\x64\Debug
CD35Game.exe ..\..\testdata\TeamData.dat ..\..\testdata\TeamData.dat ..\..\testdata\Batter_Open.dat ..\..\testdata\Pitcher_Open.dat > result.txt
pause
