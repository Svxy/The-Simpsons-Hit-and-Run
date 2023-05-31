@echo off
cd..
cd..
cd..
P4 -c autobuild sync game/... > game\build\guard\logs\p4sync.txt 2>&1
cd game/
cd build/
cd guard/