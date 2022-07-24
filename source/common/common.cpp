// clang-format off
#include "common.h"

systemXMLInformation systemXMLHashInformation[] = {
        {WII_U_MENU_JAP,                  0x0005001010040000L, "2645065A42D18D390C78543E3C4FE7E1D1957A63", "5E5C707E6DAF82393E93971BE98BE3B12204932A"},
        {WII_U_MENU_USA,                  0x0005001010040100L, "124562D41A02C7112DDD5F9A8F0EE5DF97E23471", "DC0F9941E99C629625419F444B5A5B177A67309F"},
        {WII_U_MENU_EUR,                  0x0005001010040200L, "F06041A4E5B3F899E748F1BAEB524DE058809F1D", "A0273C466DE15F33EC161BCD908B5BFE359FE6E0"},
        {HEALTH_SAFETY_JPN,               0x000500101004E000L, "066D672824128713F0A7D156142A68B998080148", "2849DE91560F6667FE7415F89FC916BE3A27DE75"},
        {HEALTH_SAFETY_USA,               0x000500101004E100L, "0EBCA1DFC0AB7A6A7FE8FB5EAF23179621B726A1", "83CF5B1CE0B64C51D15B1EFCAD659063790EB590"},
        {HEALTH_SAFETY_EUR,               0x000500101004E200L, "DE46EC3E9B823ABA6CB0638D0C4CDEEF9C793BDD", "ED59630448EC6946F3E51618DA3681EC3A84D391"},
        {MAX_SYSTEM_XML_DEFAULT_TITLE_ID, 0,                   {'\0'},                                     {'\0'}},
};

appInformation supportedApps[] = {
        {0x000500101004E000L, "Health and Safety Information [JPN]", false, {'\0'}, "9D34DDD91604D781FDB0727AC75021833304964C", "F6EBF7BC8AE3AF3BB8A42E0CF3FDA051278AEB03", "D5BABA20526524977009F7EDE25182D8E41CEFD8", "2CF358E1F51932D305911A6836ED37DB0F94ABE4"}, //v129
        {0x000500101004E100L, "Health and Safety Information [USA]", false, {'\0'}, "045734666A36C7EF0258A740855886EBDB20D59B", "F6EBF7BC8AE3AF3BB8A42E0CF3FDA051278AEB03", "5249DA6B75FEFADEBFBB18ECC93CC109FA8AA630", "C53C219FB8F777F9AB8F430D6BE4BC034D5638BA"}, //v129
        {0x000500101004E200L, "Health and Safety Information [PAL]", false, {'\0'}, "130A76F8B36B36D43B88BBC74393D9AFD9CFD2A4", "F6EBF7BC8AE3AF3BB8A42E0CF3FDA051278AEB03", "87749A8D3EE8694225423953DCF04B01F8DA2F15", "4A29A60E5FBDAF410B7C22ECAEBDDBF29D1A874E"}, //v129
        {0,                   nullptr,                               false, {'\0'}, {'\0'},                                     {'\0'}},
};

gList_t GameList[] = {
        {0x0005001010040000L, "Wii U Menu [JPN]",                                           MCP_REGION_JAPAN},
        {0x0005001010040100L, "Wii U Menu [USA]",                                           MCP_REGION_USA},
        {0x0005001010040200L, "Wii U Menu [PAL]",                                           MCP_REGION_EUROPE},

        {0x000500101004E000L, "Health and Safety Information [JPN]",                        MCP_REGION_JAPAN},
        {0x000500101004E100L, "Health and Safety Information [USA]",                        MCP_REGION_USA},
        {0x000500101004E200L, "Health and Safety Information [PAL]",                        MCP_REGION_EUROPE},

        {0x0005000010179A00L, "Kawashima: Motto Nou wo Kitaeru Otona no DS Training [JPN]", MCP_REGION_JAPAN},
        {0x0005000010179B00L, "Brain Age: Train Your Brain in Minutes a Day! [USA]",        MCP_REGION_USA},
        {0x0005000010179C00L, "Dr. Kawashima's Brain Training [PAL]",                       MCP_REGION_EUROPE},

        {0x0005000010179D00L, "Catch! Touch! Yoshi! [JPN]",                                 MCP_REGION_JAPAN},
        {0x0005000010179E00L, "Yoshi Touch & Go [USA]",                                     MCP_REGION_USA},
        {0x0005000010179F00L, "Yoshi Touch & Go [PAL]",                                     MCP_REGION_EUROPE},

        {0x0005000010195600L, "Mario Kart DS [JPN]",                                        MCP_REGION_JAPAN},
        {0x0005000010195700L, "Mario Kart DS [USA]",                                        MCP_REGION_USA},
        {0x0005000010195800L, "Mario Kart DS [PAL]",                                        MCP_REGION_EUROPE},

        {0x0005000010195900L, "New Super Mario Bros. [JPN]",                                MCP_REGION_JAPAN},
        {0x0005000010195A00L, "New Super Mario Bros. [USA]",                                MCP_REGION_USA},
        {0x0005000010195B00L, "New Super Mario Bros. [PAL]",                                MCP_REGION_EUROPE},

        {0x0005000010198800L, "Yoshi's Island DS [JPN]",                                    MCP_REGION_JAPAN},
        {0x0005000010198900L, "Yoshi's Island DS [USA]",                                    MCP_REGION_USA},
        {0x0005000010198A00L, "Yoshi's Island DS [PAL]",                                    MCP_REGION_EUROPE},

        {0x0005000010198B00L, "Yawaraka Atama Juku [JPN]",                                  MCP_REGION_JAPAN},
        {0x0005000010198C00L, "Big Brain Academy [USA]",                                    MCP_REGION_USA},
        {0x0005000010198D00L, "Big Brain Academy [PAL]",                                    MCP_REGION_EUROPE},

        {0x00050000101A1E00L, "Sawaru: Made in Wario [JPN]",                                MCP_REGION_JAPAN},
        {0x00050000101A1F00L, "WarioWare: Touched! [USA]",                                  MCP_REGION_USA},
        {0x00050000101A2000L, "WarioWare: Touched! [PAL]",                                  MCP_REGION_EUROPE},

        {0x00050000101A2100L, "Mario & Luigi RPG 2x2 [JPN]",                                MCP_REGION_JAPAN},
        {0x00050000101A2200L, "Mario & Luigi: Partners in Time [USA]",                      MCP_REGION_USA},
        {0x00050000101A2300L, "Mario & Luigi: Partners in Time [PAL]",                      MCP_REGION_EUROPE},

        {0x00050000101A5200L, "Donkey Kong: Jungle Climber [JPN]",                          MCP_REGION_JAPAN},
        {0x00050000101A5300L, "DK: Jungle Climber [USA]",                                   MCP_REGION_USA},
        {0x00050000101A5400L, "Donkey Kong: Jungle Climber [PAL]",                          MCP_REGION_EUROPE},

        {0x00050000101A5500L, "Hoshi no Kirby: Sanjou! Dorocche Dan [JPN]",                 MCP_REGION_JAPAN},
        {0x00050000101A5600L, "Kirby: Squeak Squad [USA]",                                  MCP_REGION_USA},
        {0x00050000101A5700L, "Kirby: Mouse Attack [PAL]",                                  MCP_REGION_EUROPE},

        {0x00050000101ABD00L, "Kaitou Wario the Seven [JPN]",                               MCP_REGION_JAPAN},
        {0x00050000101ABE00L, "Wario: Master of Disguise [USA]",                            MCP_REGION_USA},
        {0x00050000101ABF00L, "Wario: Master of Disguise [PAL]",                            MCP_REGION_EUROPE},

        {0x00050000101AC000L, "Star Fox Command [JPN]",                                     MCP_REGION_JAPAN},
        {0x00050000101AC100L, "Star Fox Command [USA]",                                     MCP_REGION_USA},
        {0x00050000101AC200L, "Star Fox Command [PAL]",                                     MCP_REGION_EUROPE},

        {0x00050000101B8800L, "Touch! Kirby's Magic Paintbrush [JPN]",                      MCP_REGION_JAPAN},
        {0x00050000101B8900L, "Kirby: Canvas Curse [USA]",                                  MCP_REGION_USA},
        {0x00050000101B8A00L, "Kirby: Power Paintbrush [PAL]",                              MCP_REGION_EUROPE},

        {0x00050000101B8B00L, "Zelda no Densetsu: Daichi no Kiteki [JPN]",                  MCP_REGION_JAPAN},
        {0x00050000101B8C00L, "The Legend of Zelda: Spirit Tracks [USA]",                   MCP_REGION_USA},
        {0x00050000101B8D00L, "The Legend of Zelda: Spirit Tracks [PAL]",                   MCP_REGION_EUROPE},

        {0x00050000101C3300L, "Super Mario 64 DS [JPN]",                                    MCP_REGION_JAPAN},
        {0x00050000101C3400L, "Super Mario 64 DS [USA]",                                    MCP_REGION_USA},
        {0x00050000101C3500L, "Super Mario 64 DS [PAL]",                                    MCP_REGION_EUROPE},

        {0x00050000101C3600L, "Zelda no Densetsu: Mugen no Sunadokei [JPN]",                MCP_REGION_JAPAN},
        {0x00050000101C3700L, "The Legend of Zelda: Phantom Hourglass [USA]",               MCP_REGION_USA},
        {0x00050000101C3800L, "The Legend of Zelda: Phantom Hourglass [PAL]",               MCP_REGION_EUROPE},

        {0x00050000101C8600L, "Atsumete! Kirby [JPN]",                                      MCP_REGION_JAPAN},
        {0x00050000101C8700L, "Kirby Mass Attack [USA]",                                    MCP_REGION_USA},
        {0x00050000101C8800L, "Kirby Mass Attack [PAL]",                                    MCP_REGION_EUROPE},

        {0x00050000101CC200L, "Pokemon Ranger [JPN]",                                       MCP_REGION_JAPAN},
        {0x00050000101CC300L, "Pokemon Ranger [USA]",                                       MCP_REGION_USA},
        {0x00050000101CC400L, "Pokemon Ranger [PAL]",                                       MCP_REGION_EUROPE},

        {0x00050000101D1F00L, "Oideyo Doubutsu no Mori [JPN]",                              MCP_REGION_JAPAN},
        {0x00050000101D2000L, "Animal Crossing: Wild World [USA]",                          MCP_REGION_USA},
        {0x00050000101D2100L, "Animal Crossing: Wild World [PAL]",                          MCP_REGION_EUROPE},

        {0x00050000101E0C00L, "Pokemon Fushigi no Dungeon: Sora no Tankentai [JPN]",        MCP_REGION_JAPAN},
        {0x00050000101E0D00L, "Pokemon Mystery Dungeon: Explorers of Sky [USA]",            MCP_REGION_USA},
        {0x00050000101E0E00L, "Pokemon Mystery Dungeon: Explorers of Sky [PAL]",            MCP_REGION_EUROPE},

        {0x00050000101E0F00L, "Pokemon Ranger: Batonnage [JPN]",                            MCP_REGION_JAPAN},
        {0x00050000101E1000L, "Pokemon Ranger: Shadows of Almia [USA]",                     MCP_REGION_USA},
        {0x00050000101E1100L, "Pokemon Ranger: Shadows of Almia [PAL]",                     MCP_REGION_EUROPE},

        {0x00050000101E6F00L, "Pokemon Ranger: Hikari no Kiseki [JPN]",                     MCP_REGION_JAPAN},
        {0x00050000101E7000L, "Pokemon Ranger: Guardian Signs [USA]",                       MCP_REGION_USA},
        {0x00050000101E7100L, "Pokemon Ranger: Guardian Signs [PAL]",                       MCP_REGION_EUROPE},
        {0,                   "",                                                           MCP_REGION_JAPAN},
};
