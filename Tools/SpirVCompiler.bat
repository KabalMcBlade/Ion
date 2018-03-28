@echo off
echo *****************************
echo *   COMPILE SPIR-V SHADER   *
echo *****************************

::-Select the Vulkan platform is Bin32 (for 32 bit) or Bin (for 64 bit) only!
set VK_PLATFORM=Bin

:LOOP
if "%~1"=="" goto :END
%VULKAN_SDK%/%VK_PLATFORM%/glslangValidator.exe -V -o %~1.spv %~1
rem echo "%~1" 
shift
goto :LOOP
:END

pause

exit