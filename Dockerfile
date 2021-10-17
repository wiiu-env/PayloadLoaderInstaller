FROM wiiuenv/devkitppc:20210920

COPY --from=wiiuenv/libiosuhax:20211008 /artifacts $DEVKITPRO
COPY --from=wiiuenv/devkitarm:20210917 $DEVKITPRO/devkitARM $DEVKITPRO/devkitARM

ENV DEVKITARM=/opt/devkitpro/devkitARM

WORKDIR project