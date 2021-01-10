FROM wiiuenv/devkitppc:20210101

COPY --from=wiiuenv/libiosuhax:20210109 /artifacts $DEVKITPRO
COPY --from=devkitpro/devkitarm:20200730 $DEVKITPRO/devkitARM $DEVKITPRO/devkitARM

ENV DEVKITARM=/opt/devkitpro/devkitARM

WORKDIR project