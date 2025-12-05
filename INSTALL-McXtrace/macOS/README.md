# Installation of McXtrace 3.5.40 on macOS 

## Supported macOS releases
* macOS 11-15 (Big Sur and later, fully supported python tool set. Supported on both Intel and Apple Silicon,
  via separate installation bundles.

## Steps to perform

* Download the package:
  [McXtrace 3.5.40 for macOS](https://download.mcxtrace.org/mcxtrace-3.5.40/macOS/mcxtrace-3.5.40-macOS-conda.tar.gz)
 and unpack it (e.g. double-clicking should work).

* Open the relevant folder for your local processor

* Drag the McXtrace-3.5.40.app to /Applications and right-click + open to start the app:<br/>
![](screenshots/1_open-mcxtrace-from-Applications.png?raw=true)

* Depending on your macOS version, security settings may initially prevent the app from opening, example from macOS 15 Sequoia:
  - Initial warning that "McXtrace-3.5.40" was not opened<br/>
  ![](screenshots/2_mcxtrace-not-opened.png?raw=true)
  - Next, go to System Preferences, Privacy and Security and select to
  "Open Anyway"<br/>
  ![](screenshots/3_mcxtrace-settings-open-anyway.png?raw=true)
  - You will then receive yet another warning<br/>
  ![](screenshots/4_mcxtrace-open-anyway.png?raw=true)
  - And finally give your password for installation to proceed<br/>
  ![](screenshots/5_admin-password.png?raw=true)

* :warning: In case the last warning still does not allow to open the application, you may issue the following command in a Terminal: `sudo xattr -dr com.apple.quarantine /Applications/McXtrace-3.5.40.app`

* McXtrace 3.5.40 macOS app bundles are fully based on conda-forge and will "self-inject" all dependencies on first launch. Please follow any on-screen instructions given.
  
* In case you have trouble accessing instrument files in certain areas
  of your disk, please give the McXtrace bundle "Full Disk Access"
  through your macOS Settings app

* In case of issues installing / using this app bundle, please contact mcxtrace-users@mcxtrace.org
