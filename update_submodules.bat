@echo off
echo Updating Library Submodules...

git submodule update --init --recursive
git submodule update --remote --merge

echo Alle Submodule sind jetzt auf dem neuesten Stand!
pause