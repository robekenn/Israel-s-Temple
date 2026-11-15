#include "../Game/Campaign/campaign.h"
#include "../Game/MapLoader/MapLoader.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void AssertDate(const Campaign *campaign, int year, int month, int day, bool twilight)
{
    ServiceDate date = CampaignDate(campaign);
    assert(date.year == year && date.month == month && date.day == day && date.twilight == twilight);
}

static void FinishWatch(Campaign *campaign)
{
    ItemType held = ITEM_NONE;
    const RiteStep *step;
    while ((step = CampaignStep(campaign)))
    {
        ItemType result;
        if (step->required != ITEM_KEEP && held != step->required)
        {
            assert(!CampaignPerform(campaign, step->station, held, &result));
            assert(CampaignStep(campaign) == step);
            assert(held != ITEM_PREPARED_OFFERING && held != ITEM_CENSER_COAL && held != ITEM_BREAD);
            assert(step->required != ITEM_PREPARED_OFFERING && step->required != ITEM_CENSER_COAL &&
                   step->required != ITEM_BREAD && step->required != ITEM_WOOD);
            held = step->required;
        }
        assert(!CampaignPerform(campaign, -1, held, &result));
        assert(CampaignPerform(campaign, step->station, held, &result));
        held = result;

        Campaign loaded;
        assert(CampaignRestore(&loaded, campaign->shift, campaign->duty, campaign->step, campaign->freeWatch));
        assert(loaded.washed == campaign->washed);
        assert(loaded.fireTended == campaign->fireTended);
        assert(loaded.incenseCoals == campaign->incenseCoals);
        assert(loaded.incenseLit == campaign->incenseLit);
        assert(loaded.lampsTrimmed == campaign->lampsTrimmed);
        assert(loaded.lampsOiled == campaign->lampsOiled);
        assert(loaded.lampsLit == campaign->lampsLit);
        assert(loaded.breadSet == campaign->breadSet);
    }
    assert(campaign->washed && campaign->fireTended && campaign->lampsLit && campaign->incenseLit);
}

static void CheckStoryAndFreeService(void)
{
    Campaign campaign;
    assert(CampaignRestore(&campaign, 0, 0, 0, -1));
    assert(!CampaignAdvanceShift(&campaign) && !CampaignEnterFreeService(&campaign));
    assert(!CampaignRestore(&campaign, 99, 0, 0, -1));
    assert(!CampaignRestore(&campaign, 0, 0, 1, -1));
    assert(!CampaignRestore(&campaign, 0, -1, 0, -1));
    assert(!CampaignRestore(&campaign, 0, 0, 0, 0));
    static const int dates[][4] = {
        {1, 12, 2, false}, {1, 12, 2, true}, {1, 12, 8, false}, {2, 1, 1, false},
        {2, 1, 15, false}, {2, 3, 6, false}, {2, 7, 15, true}
    };
    for (int i = 0; i < CampaignShiftCount(); i++)
    {
        AssertDate(&campaign, dates[i][0], dates[i][1], dates[i][2], dates[i][3]);
        FinishWatch(&campaign);
        if (i+1 < CampaignShiftCount())
            assert(CampaignAdvanceShift(&campaign));
    }
    assert(CampaignFinished(&campaign));
    assert(!CampaignAdvanceShift(&campaign));
    assert(CampaignEnterFreeService(&campaign));
    AssertDate(&campaign, 2, 7, 16, false);
    assert(!CampaignFinished(&campaign));
    FinishWatch(&campaign);
    assert(CampaignAdvanceShift(&campaign));
    AssertDate(&campaign, 2, 7, 16, true);
    assert(CampaignDutyCount(&campaign) == 4);
    FinishWatch(&campaign);
    assert(CampaignAdvanceShift(&campaign));
    AssertDate(&campaign, 2, 7, 17, false);
}

static void CheckCalendarRollover(void)
{
    Campaign campaign;
    int lastShift = CampaignShiftCount() - 1;
    assert(CalendarMonthLength(1) == 30 && CalendarMonthLength(2) == 29);
    assert(CampaignRestore(&campaign, lastShift, 0, 0, 29));
    AssertDate(&campaign, 2, 7, 30, true);
    FinishWatch(&campaign);
    assert(CampaignAdvanceShift(&campaign));
    AssertDate(&campaign, 2, 8, 1, false);
    assert(strcmp(CalendarOccasion(CampaignDate(&campaign)), "New Moon") == 0);
    assert(CampaignDutyCount(&campaign) > 4);

    assert(CampaignRestore(&campaign, lastShift, 0, 0, 58));
    AssertDate(&campaign, 2, 8, 15, false);
    assert(CampaignRestore(&campaign, lastShift, 0, 0, 48));
    assert(CampaignDate(&campaign).weekday == 7);
    assert(CampaignDutyCount(&campaign) == 6);

    assert(CampaignRestore(&campaign, lastShift, 0, 0, 323));
    AssertDate(&campaign, 2, 12, 29, true);
    FinishWatch(&campaign);
    assert(CampaignAdvanceShift(&campaign));
    AssertDate(&campaign, 3, 1, 1, false);
    assert(strcmp(CalendarOccasion(CampaignDate(&campaign)), "New Moon") == 0);
}

int main(void)
{
    CheckStoryAndFreeService();
    CheckCalendarRollover();
    puts("Full campaign, checkpoint reconstruction, free service, and calendar rollovers passed.");
    return 0;
}
