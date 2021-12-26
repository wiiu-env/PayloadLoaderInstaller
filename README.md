# PayloadLoader Installer
A (hopefully) userfriendly and safe installer to inject a ["payload.elf-loader"](https://github.com/wiiu-env/PayloadFromRPX) into the Health and Safety application via [FailST](https://maschell.github.io/homebrew/2020/12/02/failst.html).

To be able to do a coldboot setup you need launch this installer via the Health & Safety injection itself.


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