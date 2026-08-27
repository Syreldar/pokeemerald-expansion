#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "party_menu.h"
#include "pokemon.h"
#include "test/overworld_script.h"
#include "test/test.h"

#define TEST_MENU_DIR_DOWN     1
#define TEST_MENU_DIR_UP      -1
#define TEST_MENU_DIR_RIGHT    2

static void SetTestPartySize(enum BattleTrainer trainer, u8 partySize)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[trainer][i]);

    for (u32 i = 0; i < partySize; i++)
        CreateMon(&gParties[trainer][i], SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PRESET(0));

    gPartiesCount[trainer] = partySize;
}

TEST("Full multi partner party menu stops down navigation at partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    SetTestPartySize(B_TRAINER_PARTNER, 2);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_DOWN, FALSE, 0), PARTY_SIZE + 1);
}

TEST("Full multi partner party menu allows down navigation through partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, 2);
    SetTestPartySize(B_TRAINER_PARTNER, PARTY_SIZE);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(1, TEST_MENU_DIR_DOWN, FALSE, 0), 2);
}

TEST("Full multi partner party menu wraps cancel up to partner party count")
{
    SetTestPartySize(B_TRAINER_PLAYER, PARTY_SIZE);
    SetTestPartySize(B_TRAINER_PARTNER, 2);
    gPartyMenu.layout = PARTY_LAYOUT_MULTI_FULL_PARTNER;

    EXPECT_EQ(Test_UpdatePartySelectionSingleLayout(PARTY_SIZE + 1, TEST_MENU_DIR_UP, FALSE, 0), 1);
}

TEST("Multi battle party selection allows four or five entries")
{
    u8 maxBattleEntries;

    PARAMETRIZE { maxBattleEntries = 4; }
    PARAMETRIZE { maxBattleEntries = 5; }

    EXPECT_EQ(Test_GetMultiBattleMaxEntries(maxBattleEntries), maxBattleEntries);
}

TEST("Party reduction retains four or five selected Pokemon in order")
{
    static const enum Species species[PARTY_SIZE] =
    {
        SPECIES_BULBASAUR,
        SPECIES_CHARMANDER,
        SPECIES_SQUIRTLE,
        SPECIES_CHIKORITA,
        SPECIES_CYNDAQUIL,
        SPECIES_TOTODILE,
    };
    static const u8 selectedOrder[PARTY_SIZE] = {6, 2, 5, 1, 4, 3};
    u8 selectedMonsCount;

    PARAMETRIZE { selectedMonsCount = 4; }
    PARAMETRIZE { selectedMonsCount = 5; }

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        CreateMon(&gParties[B_TRAINER_PLAYER][i], species[i], 50, 0, OTID_STRUCT_PRESET(0));
        gSelectedOrderFromParty[i] = selectedOrder[i];
    }

    VarSet(VAR_0x8000, selectedMonsCount);
    RUN_OVERWORLD_SCRIPT(reduceplayerpartytoselectedmons VAR_0x8000;);

    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], selectedMonsCount);
    for (u32 i = 0; i < selectedMonsCount; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES), species[selectedOrder[i] - 1]);
    for (u32 i = selectedMonsCount; i < PARTY_SIZE; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES), SPECIES_NONE);
}
