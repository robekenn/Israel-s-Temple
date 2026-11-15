#ifndef TEMPLE_CAMPAIGN_H
#define TEMPLE_CAMPAIGN_H

#include "../Inventory/Inventory.h"

typedef enum RiteEffect {
    RITE_NONE, RITE_WASH, RITE_FIRE, RITE_COALS, RITE_INCENSE,
    RITE_TRIM, RITE_OIL, RITE_LAMPS, RITE_BREAD
} RiteEffect;

typedef struct RiteStep {
    int station;
    ItemType required;
    ItemType result;
    RiteEffect effect;
    const char *instruction;
    const char *feedback;
} RiteStep;

typedef struct Duty {
    const char *name;
    const char *reference;
    const RiteStep *steps;
    int count;
} Duty;

typedef struct Shift {
    const char *name;
    const char *rank;
    const char *opening;
    const char *closing;
    const char *visitor;
    const Duty *const *duties;
    int count;
    bool twilight;
} Shift;

typedef struct ServiceDate {
    int year;
    int month;
    int day;
    int weekday;
    bool twilight;
} ServiceDate;

typedef struct Campaign {
    int freeWatch;
    int shift;
    int duty;
    int step;
    bool washed;
    bool fireTended;
    bool incenseCoals;
    bool incenseLit;
    bool lampsTrimmed;
    bool lampsOiled;
    bool lampsLit;
    bool breadSet;
} Campaign;

int CampaignShiftCount(void);
const Shift *CampaignShift(const Campaign *campaign);
ServiceDate CampaignDate(const Campaign *campaign);
const char *CalendarOccasion(ServiceDate date);
int CalendarMonthLength(int month);
int CampaignDutyCount(const Campaign *campaign);
const Duty *CampaignDutyAt(const Campaign *campaign, int index);
const Duty *CampaignDuty(const Campaign *campaign);
const RiteStep *CampaignStep(const Campaign *campaign);
bool CampaignRestore(Campaign *campaign, int shift, int duty, int step, int freeWatch);
bool CampaignFinished(const Campaign *campaign);
bool CampaignEnterFreeService(Campaign *campaign);
bool CampaignAdvanceShift(Campaign *campaign);
bool CampaignPerform(Campaign *campaign, int station, ItemType held, ItemType *result);

#endif
