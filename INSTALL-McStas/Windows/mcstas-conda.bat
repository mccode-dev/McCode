@echo off
@mkdir %USERPROFILE%\AppData\Roaming\micromamba
@cd %USERPROFILE%\AppData\Roaming\micromamba
@echo Downloading latest micromamba
@set URL=https://github.com/mamba-org/micromamba-releases/releases/latest/download/micromamba-win-64
@curl -L -o micromamba.exe %URL%
@echo Requesting init of micromamba on the shell
@.\micromamba.exe shell init --shell cmd.exe
@.\micromamba.exe shell hook -s cmd.exe
@.\micromamba config append channels conda-forge
@.\micromamba config set channel_priority strict
@.\micromamba clean --all -y
@echo Checking for existing mcstas environment
@.\micromamba env list > tmpfile
@setlocal

@set FOUND=N
@set MATCH=""
@for /f %%i in ('@findstr mcstas tmpfile') do (
  @if /I "%%i"=="mcstas" (
    @echo Found exact environment name match: %%i!
	@set FOUND=Y
	@set MATCH=%%i
  )
)

@if /I %FOUND%==N goto INSTALL

:CHOICE
@choice /C YIN /M "Press Y to remove existing env --> %MATCH%  <--, I to ignore, or N to Cancel."
@if '%ERRORLEVEL%'=='1' goto REMOVE
@if '%ERRORLEVEL%'=='2' goto INSTALL
@if '%ERRORLEVEL%'=='3' goto END

:REMOVE
@echo Removing mcstas env
@.\micromamba env remove -n mcstas -y

:INSTALL
@echo Starting installation of McStas and dependencies
@.\micromamba.exe create -n mcstas mcstas -c conda-forge -c nodefaults -y

@echo Creating desktop shortcuts to new environment

@echo start %USERPROFILE%\AppData\Roaming\mamba\condabin\micromamba activate mcstas > %USERPROFILE%\Desktop\mcstas-shell-conda.bat

@echo call %USERPROFILE%\AppData\Roaming\mamba\condabin\micromamba activate mcstas > %USERPROFILE%\Desktop\mcgui-conda.bat
@echo mcgui >> %USERPROFILE%\Desktop\mcgui-conda.bat

:END
@echo End of script
@pause
@endlocal
