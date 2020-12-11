# AromaInstaller

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once)
docker build . -t aromainstaller-builder

# make 
docker run -it --rm -v ${PWD}:/project aromainstaller-builder make

# make clean
docker run -it --rm -v ${PWD}:/project aromainstaller-builder make clean
```