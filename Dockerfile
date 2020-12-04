FROM wiiuenv/devkitppc:20200810

COPY --from=wiiuenv/libiosuhax:20200812 /artifacts $DEVKITPRO
COPY --from=devkitpro/devkitarm:20200730 $DEVKITPRO/devkitARM $DEVKITPRO/devkitARM

ENV DEVKITARM=/opt/devkitpro/devkitARM

WORKDIR project