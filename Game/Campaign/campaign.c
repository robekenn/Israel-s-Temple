#include "campaign.h"
#include "../MapLoader/MapLoader.h"
#include <stddef.h>

#define LENGTH(array) ((int)(sizeof(array) / sizeof((array)[0])))
#define STEP(at, needs, gives, effect, task, text) { at, needs, gives, effect, task, text }
#define ACT(at, task, text) STEP(at, ITEM_KEEP, ITEM_KEEP, RITE_NONE, task, text)
#define USE(at, item, task, text) STEP(at, item, ITEM_NONE, RITE_NONE, task, text)
#define DUTY(id, title, ref) static const Duty id = { title, ref, id##Steps, LENGTH(id##Steps) }

static const RiteStep washSteps[] = {
    STEP(INTERACT_LAVER, ITEM_KEEP, ITEM_KEEP, RITE_WASH,
         "Wash your hands and feet at the bronze laver.", "You are ready to serve at the altar and enter the tent.")
};
DUTY(wash, "Wash at the laver", "Exodus 30:17-21");

static const RiteStep fireSteps[] = {
    STEP(INTERACT_BRONZE_ALTAR, TOOL_ASH_BROOM, ITEM_NONE, RITE_NONE,
         "Take the ash broom from storage and clear the altar ashes.", "The old ashes are gathered. Embers remain under them."),
    STEP(INTERACT_WOOD_STACK, ITEM_KEEP, ITEM_WOOD, RITE_NONE,
         "Collect a fresh bundle from the wood stack.", "You lift a dry bundle of wood."),
    STEP(INTERACT_BRONZE_ALTAR, ITEM_WOOD, ITEM_NONE, RITE_FIRE,
         "Lay fresh wood over the embers at the bronze altar.", "The fresh wood catches. The altar fire is ready.")
};
DUTY(fire, "Tend the altar fire", "Leviticus 6:8-13");

static const RiteStep bankSteps[] = {
    STEP(INTERACT_WOOD_STACK, ITEM_KEEP, ITEM_WOOD, RITE_NONE,
         "Collect wood to strengthen the fire for twilight.", "You lift a dry bundle of wood."),
    STEP(INTERACT_BRONZE_ALTAR, ITEM_WOOD, ITEM_NONE, RITE_FIRE,
         "Feed the standing fire at the bronze altar.", "The fire will hold through the evening service.")
};
DUTY(bank, "Keep the fire burning", "Leviticus 6:12-13");

static const RiteStep dailySteps[] = {
    ACT(INTERACT_SACRIFICE_TABLE, "Inspect the yearling lamb at a preparation table.",
        "The lamb is without defect."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the lamb at the table and take the offering.", "The offering and its blood are ready for the altar."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Present the blood and lay the daily burnt offering on the bronze altar.",
        "The daily lamb is offered."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_FLOUR, "Bring flour for the daily grain offering.",
        "You present the grain portion with its oil."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_WINE, "Bring wine for the daily drink offering.",
        "The daily offering is complete.")
};
DUTY(daily, "Present the daily offering", "Numbers 28:1-8");

static const RiteStep holySteps[] = {
    STEP(INTERACT_MENORAH, TOOL_BLADE, ITEM_NONE, RITE_TRIM,
         "Take the blade to the lampstand and trim the wicks.", "The trimmed wicks are ready for fresh oil."),
    STEP(INTERACT_MENORAH, ITEM_OIL, ITEM_NONE, RITE_OIL,
         "Bring olive oil to the lampstand.", "You refill the cups with olive oil."),
    STEP(INTERACT_BRONZE_ALTAR, ITEM_CENSER, ITEM_CENSER_COAL, RITE_NONE,
         "Bring the censer to the bronze altar and collect a coal.", "The censer holds a live coal."),
    STEP(INTERACT_MENORAH, ITEM_CENSER_COAL, ITEM_CENSER_COAL, RITE_LAMPS,
         "Carry the coal into the Holy Place and light the lampstand.", "The seven lamps catch. The coal is still alight."),
    STEP(INTERACT_INCENSE_ALTAR, ITEM_CENSER_COAL, ITEM_CENSER, RITE_COALS,
         "Set the coal on the golden incense altar.", "The coal is in place. Exchange the censer for incense."),
    STEP(INTERACT_INCENSE_ALTAR, ITEM_INCENSE, ITEM_NONE, RITE_INCENSE,
         "Bring incense from storage to the golden altar.", "Incense rises in the Holy Place.")
};
DUTY(holy, "Tend the lamps and incense", "Exodus 27:20-21, 30:7-9");

static const RiteStep breadSteps[] = {
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_FLOUR, ITEM_BREAD, RITE_NONE,
         "Bring flour to a preparation table and collect twelve prepared loaves.",
         "The twelve loaves are ready to be set out."),
    STEP(INTERACT_SHOWBREAD_TABLE, ITEM_BREAD, ITEM_NONE, RITE_BREAD,
         "Set two rows of six loaves on the bread table in the Holy Place.",
         "Fresh bread is set out for the Sabbath.")
};
DUTY(bread, "Renew the bread of the Presence", "Leviticus 24:5-9");

static const RiteStep sabbathSteps[] = {
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the additional Sabbath lambs at the preparation table.",
         "Two lambs have been prepared for the Sabbath offering."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Present the additional Sabbath burnt offering at the bronze altar.", "The Sabbath lambs are offered."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_FLOUR, "Bring the grain portion for the Sabbath offering.",
        "You offer the grain portion mixed with oil."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_WINE, "Bring the Sabbath drink offering to the bronze altar.",
        "The additional Sabbath offering is complete.")
};
DUTY(sabbath, "Present the Sabbath offering", "Numbers 28:9-10");

static const RiteStep newMoonSteps[] = {
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Check and prepare the new-moon herd and flock offerings at a table.",
         "The extra burnt offerings for the new month are ready."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Present the new-moon burnt offerings at the bronze altar.", "You finish the new-moon burnt offerings."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_FLOUR, "Bring the additional new-moon grain portions.",
        "The grain portions, mixed with oil, are presented."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_WINE, "Bring the additional new-moon drink offerings.",
        "The drink offerings are presented."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the goat for the public purification offering.", "The goat is prepared for the purification offering."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Complete the new-moon purification offering at the bronze altar.", "The new-moon service is complete.")
};
DUTY(newMoon, "Mark the new moon", "Numbers 28:11-15");

static const RiteStep dedicationSteps[] = {
    ACT(INTERACT_VISITOR, "Speak to Eliab at the courtyard gate.",
        "Eliab: My household has come to present a burnt offering."),
    ACT(INTERACT_SACRIFICE_TABLE, "Inspect Eliab's animal at a preparation table.",
        "The animal is suitable. Eliab lays his hand on its head."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Receive Eliab's prepared offering at the table.", "The priests receive the blood and prepare the portions."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Apply the blood and present the whole burnt offering at the bronze altar.",
        "Eliab's offering goes up on the fire."),
    ACT(INTERACT_VISITOR, "Tell Eliab the offering is complete.",
        "Eliab: Thank you for showing my household where to go.")
};
DUTY(dedication, "Eliab's burnt offering", "Leviticus 1:3-9");

static const RiteStep thanksgivingSteps[] = {
    ACT(INTERACT_VISITOR, "Speak to Tirzah at the courtyard gate.",
        "Tirzah: My sister has recovered. We have come to give thanks."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare Tirzah's thanksgiving offering and separate the altar portion.",
         "The priests receive the blood. The fat is ready for the altar."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Apply the blood and burn the altar portion of Tirzah's offering.",
        "The reserved portion goes on the altar. The rest is for the shared meal."),
    ACT(INTERACT_VISITOR, "Tell Tirzah her family can share the meal today.",
        "Tirzah: Then I will go find her. She wanted to walk here herself.")
};
DUTY(thanksgiving, "Tirzah's thanksgiving offering", "Leviticus 3; 7:11-15");

static const RiteStep purificationSteps[] = {
    ACT(INTERACT_VISITOR, "Hear Naaman's request at the courtyard gate.",
        "Naaman: I realized what I had done without knowing. I have brought a goat."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare Naaman's purification offering from the flock.", "Naaman lays his hand on the goat before it is prepared."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Apply the blood to the horns of the bronze altar and offer the fat.",
        "The blood and altar portion are presented for Naaman."),
    ACT(INTERACT_VISITOR, "Let Naaman know the rite is complete.",
        "Naaman: I thought I would be too ashamed to come. I am glad I did.")
};
DUTY(purification, "Naaman's purification offering", "Leviticus 4:27-35");

static const RiteStep guiltSteps[] = {
    ACT(INTERACT_VISITOR, "Hear Micah's request at the courtyard gate.",
        "Micah: I kept something that was entrusted to me. I have returned it with an added fifth."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the ram Micah brought following restitution.", "You receive the ram for the reparation offering."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Apply the blood and present the altar portion of the ram.",
        "The reparation offering is presented."),
    ACT(INTERACT_VISITOR, "Tell Micah that his offering is complete.",
        "Micah: Making it right was harder than bringing the ram. I am grateful I could do both.")
};
DUTY(guilt, "Micah's reparation offering", "Leviticus 5:14-6:7; 7:1-7");

static const RiteStep grainSteps[] = {
    ACT(INTERACT_VISITOR, "Meet Hannah at the courtyard gate.",
        "Hannah: The first harvest is in. I have brought flour and oil."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_FLOUR,
        "Take flour from storage and offer the memorial portion of Hannah's gift.",
        "The memorial portion of flour, oil, and frankincense is offered."),
    ACT(INTERACT_VISITOR, "Tell Hannah her grain offering is complete.",
        "Hannah: I carried that flour all morning. I would do it again.")
};
DUTY(grain, "Hannah's grain offering", "Leviticus 2:1-3");

static const RiteStep returningSteps[] = {
    ACT(INTERACT_VISITOR, "Welcome Tirzah and her sister back to the courtyard.",
        "Tirzah: My sister made the whole walk this year. We brought another offering together."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the sisters' shared well-being offering.", "Their offering is prepared for the bronze altar."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Present the blood and altar portion of the sisters' offering.",
        "The rest of the offering is reserved for their shared meal."),
    ACT(INTERACT_VISITOR, "Send the sisters to share their meal.",
        "Tirzah's sister: I remember her telling me about this place. This time I got to see it.")
};
DUTY(returning, "The sisters' offering", "Leviticus 3; 7:11-18");

static const RiteStep feastSteps[] = {
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the additional festival burnt offerings at a table.",
         "The festival portions from the herd and flock are ready."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Present the festival burnt offerings at the bronze altar.", "The festival burnt offerings go on the fire."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_FLOUR, "Bring the festival grain portions to the bronze altar.",
        "The grain portions are presented with oil."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_WINE, "Bring the festival drink offerings to the bronze altar.",
        "The festival drink offerings are presented."),
    STEP(INTERACT_SACRIFICE_TABLE, ITEM_KEEP, ITEM_PREPARED_OFFERING, RITE_NONE,
         "Prepare the public purification goat at a preparation table.", "The purification offering is ready."),
    USE(INTERACT_BRONZE_ALTAR, ITEM_PREPARED_OFFERING,
        "Present the festival purification offering at the bronze altar.", "The communal service is complete.")
};
DUTY(feast, "Serve the festival gathering", "Numbers 28:16-31; 29:12-38");

static const Duty *const morning[] = { &wash, &fire, &daily, &holy };
static const Duty *const evening[] = { &wash, &bank, &daily, &dedication, &holy };
static const Duty *const sabbathDay[] = { &wash, &fire, &daily, &sabbath, &bread, &thanksgiving, &holy };
static const Duty *const newMonth[] = { &wash, &fire, &daily, &newMoon, &purification, &holy };
static const Duty *const unleavened[] = { &wash, &fire, &daily, &feast, &guilt, &holy };
static const Duty *const weeks[] = { &wash, &fire, &daily, &feast, &grain, &holy };
static const Duty *const booths[] = { &wash, &bank, &daily, &feast, &returning, &holy };

static const Shift shifts[] = {
    { "First Light", "Apprentice",
      "Eleazar: Your watch starts at the laver. I'll be at the gate when your work is done.",
      "Eleazar: You kept the fire and the lamps. Come back at twilight. A household is waiting.",
      NULL, morning, LENGTH(morning), false },
    { "The Evening Watch", "Apprentice",
      "Eleazar: There is another daily offering at twilight. Eliab and his household have also come.",
      "Eleazar: You made room for a family in the middle of your own work. Come back for the Sabbath.",
      "Eliab", evening, LENGTH(evening), true },
    { "Bread for the Sabbath", "Attendant",
      "Eleazar: The Sabbath adds its own offering. Set out fresh bread and make time for Tirzah.",
      "Eleazar: I saw Tirzah hurry back for her sister. The next watch is the new moon.",
      "Tirzah", sabbathDay, LENGTH(sabbathDay), false },
    { "A New Month", "Attendant",
      "Eleazar: The new month brings public offerings. Naaman is waiting at the gate for a quieter word.",
      "Eleazar: You served the gathering and heard Naaman out. Soon the pilgrims will arrive.",
      "Naaman", newMonth, LENGTH(newMonth), false },
    { "The Road from Egypt", "Priest",
      "Eleazar: Families have come for Unleavened Bread. Micah has a reparation offering of his own.",
      "Eleazar: Stay close to the gate at the next feast. People will come carrying their first harvest.",
      "Micah", unleavened, LENGTH(unleavened), false },
    { "The First Harvest", "Priest",
      "Eleazar: Hannah has brought a gift from her harvest. Receive it after the communal service.",
      "Eleazar: I remember when you asked me where the ash broom was. One more watch, then I leave you the ledger.",
      "Hannah", weeks, LENGTH(weeks), false },
    { "The Last Lamp", "Priest",
      "Eleazar: Tirzah is back, and her sister is with her. Bring the watch to a close when the lamps are lit.",
      "Eleazar: I used to wait here to see if you needed me. Tonight I waited to hand you the ledger.",
      "Tirzah", booths, LENGTH(booths), true }
};

enum { DAYS_IN_YEAR = 354, CAMPAIGN_START = 326, SABBATH_WATCH = 2, MAX_FREE_WATCH = 1000000 };

static const int watchDays[] = {
    CAMPAIGN_START, CAMPAIGN_START, CAMPAIGN_START + 6, DAYS_IN_YEAR,
    DAYS_IN_YEAR + 14, DAYS_IN_YEAR + 64, DAYS_IN_YEAR + 191
};
_Static_assert(LENGTH(watchDays) == LENGTH(shifts), "Each watch needs a calendar date");

static const Shift freeShifts[] = {
    { "Morning service", "Priest",
      "Bring the altar and lamps into readiness for the new day.",
      "The morning watch is complete. The court will open again at twilight.",
      NULL, NULL, 0, false },
    { "Evening service", "Priest",
      "Keep the altar fire and tend the lamps as the light fades.",
      "The evening watch is complete. Continue to the next morning.",
      NULL, NULL, 0, true }
};

int CampaignShiftCount(void)
{
    return LENGTH(shifts);
}

int CalendarMonthLength(int month)
{
    return month % 2 ? 30 : 29;
}

ServiceDate CampaignDate(const Campaign *campaign)
{
    int day = watchDays[campaign->shift];
    bool twilight = shifts[campaign->shift].twilight;
    if (campaign->freeWatch >= 0)
    {
        day += 1 + campaign->freeWatch / 2;
        twilight = campaign->freeWatch % 2 != 0;
    }

    ServiceDate date = { .year = 1 + day / DAYS_IN_YEAR, .month = 1,
                         .day = 1 + day % DAYS_IN_YEAR, .weekday = 1 + (day - CAMPAIGN_START) % 7,
                         .twilight = twilight };
    while (date.day > CalendarMonthLength(date.month))
    {
        date.day -= CalendarMonthLength(date.month);
        date.month++;
    }
    return date;
}

static const char *FestivalName(ServiceDate date)
{
    if (date.month == 1 && date.day >= 15 && date.day <= 21) return "Unleavened Bread";
    if (date.month == 3 && date.day == 6) return "Festival of Weeks";
    if (date.month == 7 && date.day >= 15 && date.day <= 22) return "Festival of Booths";
    return NULL;
}

const char *CalendarOccasion(ServiceDate date)
{
    const char *festival = FestivalName(date);
    if (festival) return festival;
    if (date.day == 1) return "New Moon";
    if (date.weekday == 7) return "Sabbath";
    return "Ordinary day";
}

const Shift *CampaignShift(const Campaign *campaign)
{
    if (campaign->freeWatch >= 0)
        return &freeShifts[campaign->freeWatch % 2];
    return &shifts[campaign->shift];
}

static int ComposeFreeDuties(const Campaign *campaign, const Duty **duties)
{
    ServiceDate date = CampaignDate(campaign);
    int count = 0;
    duties[count++] = &wash;
    duties[count++] = date.twilight ? &bank : &fire;
    duties[count++] = &daily;
    if (!date.twilight)
    {
        if (date.weekday == 7)
        {
            duties[count++] = &sabbath;
            duties[count++] = &bread;
        }
        if (date.day == 1) duties[count++] = &newMoon;
        if (FestivalName(date)) duties[count++] = &feast;
    }
    duties[count++] = &holy;
    return count;
}

int CampaignDutyCount(const Campaign *campaign)
{
    if (campaign->freeWatch < 0)
        return CampaignShift(campaign)->count;

    const Duty *duties[8];
    return ComposeFreeDuties(campaign, duties);
}

const Duty *CampaignDutyAt(const Campaign *campaign, int index)
{
    if (index < 0)
        return NULL;
    if (campaign->freeWatch < 0)
    {
        const Shift *shift = CampaignShift(campaign);
        return index < shift->count ? shift->duties[index] : NULL;
    }
    const Duty *duties[8];
    int count = ComposeFreeDuties(campaign, duties);
    return index < count ? duties[index] : NULL;
}

const Duty *CampaignDuty(const Campaign *campaign)
{
    return CampaignDutyAt(campaign, campaign->duty);
}

const RiteStep *CampaignStep(const Campaign *campaign)
{
    const Duty *duty = CampaignDuty(campaign);
    return duty ? &duty->steps[campaign->step] : NULL;
}

static void ApplyEffect(Campaign *campaign, RiteEffect effect)
{
    switch (effect)
    {
        case RITE_WASH: campaign->washed = true; break;
        case RITE_FIRE: campaign->fireTended = true; break;
        case RITE_COALS: campaign->incenseCoals = true; break;
        case RITE_INCENSE: campaign->incenseLit = true; break;
        case RITE_TRIM: campaign->lampsTrimmed = true; break;
        case RITE_OIL: campaign->lampsOiled = true; break;
        case RITE_LAMPS: campaign->lampsLit = true; break;
        case RITE_BREAD: campaign->breadSet = true; break;
        case RITE_NONE: break;
    }
}

bool CampaignRestore(Campaign *campaign, int shiftIndex, int dutyIndex, int stepIndex, int freeWatch)
{
    if (shiftIndex < 0 || shiftIndex >= LENGTH(shifts) || freeWatch < -1 || freeWatch > MAX_FREE_WATCH ||
        (freeWatch >= 0 && shiftIndex != LENGTH(shifts) - 1))
        return false;
    Campaign restored = { .shift = shiftIndex, .duty = dutyIndex, .step = stepIndex, .freeWatch = freeWatch };
    int count = CampaignDutyCount(&restored);
    if (dutyIndex < 0 || dutyIndex > count || stepIndex < 0)
        return false;
    const Duty *current = CampaignDuty(&restored);
    if (current ? stepIndex >= current->count : stepIndex != 0)
        return false;

    // The most recently renewed Sabbath bread remains on the table in later watches.
    restored.breadSet = shiftIndex > SABBATH_WATCH;
    for (int i = 0; i <= dutyIndex && i < count; i++)
    {
        const Duty *duty = CampaignDutyAt(&restored, i);
        int completed = i == dutyIndex ? stepIndex : duty->count;
        for (int j = 0; j < completed; j++)
            ApplyEffect(&restored, duty->steps[j].effect);
    }
    *campaign = restored;
    return true;
}

bool CampaignFinished(const Campaign *campaign)
{
    return campaign->freeWatch < 0 && campaign->shift == LENGTH(shifts) - 1 && !CampaignDuty(campaign);
}

bool CampaignEnterFreeService(Campaign *campaign)
{
    return CampaignFinished(campaign) && CampaignRestore(campaign, campaign->shift, 0, 0, 0);
}

bool CampaignAdvanceShift(Campaign *campaign)
{
    if (CampaignDuty(campaign) || CampaignFinished(campaign))
        return false;
    if (campaign->freeWatch >= 0)
        return CampaignRestore(campaign, campaign->shift, 0, 0, campaign->freeWatch + 1);
    return CampaignRestore(campaign, campaign->shift + 1, 0, 0, -1);
}

bool CampaignPerform(Campaign *campaign, int station, ItemType held, ItemType *result)
{
    const RiteStep *step = CampaignStep(campaign);
    if (!step || station != step->station || (step->required != ITEM_KEEP && step->required != held))
        return false;

    ApplyEffect(campaign, step->effect);
    *result = step->result == ITEM_KEEP ? held : step->result;
    campaign->step++;
    if (campaign->step == CampaignDuty(campaign)->count)
    {
        campaign->duty++;
        campaign->step = 0;
    }
    return true;
}
