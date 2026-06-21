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
@.\micromamba clean --all -y
@.\micromamba config set channel_priority strict
@echo Checking for existing mcxtrace environment
@.\micromamba env list > tmpfile
@setlocal

@set FOUND=N
@set MATCH=""
@for /f %%i in ('@findstr mcxtrace tmpfile') do (
  @if /I "%%i"=="mcxtrace" (
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
@echo Removing mcxtrace env
@.\micromamba env remove -n mcxtrace -y

:INSTALL
@echo Starting installation of McXtrace and dependencies
@.\micromamba.exe create -n mcxtrace mcxtrace -c conda-forge -c nodefaults -y

@echo Creating desktop shortcuts to new environment

@echo start %USERPROFILE%\AppData\Roaming\mamba\condabin\micromamba activate mcxtrace > %USERPROFILE%\Desktop\mcxtrace-shell-conda.bat

@echo call %USERPROFILE%\AppData\Roaming\mamba\condabin\micromamba activate mcxtrace > %USERPROFILE%\Desktop\mxgui-conda.bat
@echo mxgui >> %USERPROFILE%\Desktop\mxgui-conda.bat

:END
@echo End of script
@pause
@endlocal
