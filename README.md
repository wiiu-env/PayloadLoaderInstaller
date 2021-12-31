# PayloadLoader Installer
A (hopefully) userfriendly and safe installer to inject a ["payload.elf-loader"](https://github.com/wiiu-env/PayloadFromRPX) into the Health and Safety application via [FailST](https://maschell.github.io/homebrew/2020/12/02/failst.html).

To be able to do a coldboot setup you need launch this installer via the Health & Safety injection itself.

**Use the installer to uninstall the PayloadLoader, even a factory won't remove it**

**There is always the risk of bricking the console, only proceed with installing if you understand this risk**

# Features
- Inject [PayloadFromRPX](https://github.com/wiiu-env/PayloadFromRPX) into the Health & Safety application.
- Change the boot-title of the console to Health & Safety to coldboot into PayloadFromRPX.
- Various (hash)-checks to make sure the installation will be successful.
- Restore the original Health & Safety Application.

## Usage

Load the Installer with the [PayloadLoaderInstallerEnvironment](https://github.com/wiiu-env/PayloadLoaderInstallerEnvironment) or [homebrew_on_menu_plugin](https://github.com/wiiu-env/homebrew_on_menu_plugin).

- Coldbooting into the PayloadLoader can only be activated if you launch the installer from an environment that has been loaded by the PayloadLoader. This ensures the PayloadLoader is actually working properly before coldbooting into it. 
- **Launching the PayloadLoader Installer from the browser won't let change the boot title to PayloadLoader.**
- The PayloadFromRPX can only be updated when coldbooting into the Wii U Menu.
- Coldbooting can only be activated when the installer recognizes the PayloadFromRPX.
- The installer is only designed to work with a unmodifed system. **If your have modified the Health & Safety Application or system.xml in the past the installer may fail** 

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t payloadloaderinstaller-builder

# make 
docker run -it --rm -v ${PWD}:/project payloadloaderinstaller-builder make

# make clean
docker run -it --rm -v ${PWD}:/project payloadloaderinstaller-builder make clean
```

# Credits
- Maschell (FailST, Installer, PayloadFromRPX)
- rw-r-r-0644 (Installer)
- GaryOderNichts (Installer)