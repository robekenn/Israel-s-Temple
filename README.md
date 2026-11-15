# Israel's Temple

A top-down C and raylib game set at the Tabernacle. You begin as a priest learning the watch from Eleazar. Keep the altar fire, tend the lamps and incense, and meet the people who bring offerings to the courtyard.

## Play the campaign

Seven story watches take you from your first morning to the Festival of Booths. The watches include the evening service, Sabbath bread, the new moon, Unleavened Bread, and the Festival of Weeks. As the calendar advances, you serve visitors bringing burnt, thanksgiving, purification, reparation, and grain offerings.

You can carry one item at a time. Follow the gold marker to the next station or supply chest. Approach most stations from any side, and face the doorway to enter or leave the Holy Place. Read the journal for the complete duty list and the passage behind each duty. When the list is complete, return to Eleazar to hand off the watch.

After the finale, choose **Keep serving** to continue through consecutive dawn and twilight watches. Sabbath, new moon, and festival duties recur on their calendar dates. The story visits selected dates. Free service advances one day at a time, with twelve months alternating between 30 and 29 days. The calendar uses a fixed 354-day year for gameplay.

Ritual sequences are condensed for play. Public offerings that call for multiple animals are represented by batches, and sacrifice preparation happens offscreen. The journal cites the Torah passages behind the duties. [Exodus 27–30](https://www.biblegateway.com/passage/?search=Exodus%2027-30&version=NRSVUE) · [Leviticus 1–7, 23–24](https://www.biblegateway.com/passage/?search=Leviticus%201-7%3B%2023-24&version=NRSVUE) · [Numbers 28–29](https://www.biblegateway.com/passage/?search=Numbers%2028-29&version=NRSVUE)

## Controls

| Input | Action |
| --- | --- |
| WASD or arrow keys | Move |
| Shift | Run |
| E | Interact with a nearby station or person |
| J | Open or close the watch journal and calendar |
| Esc | Pause or dismiss the current panel |
| Arrow keys, then E or Enter | Choose and take an item from storage |
| Click | Choose a menu button or take an item from storage |
| Q | Close storage |

The game autosaves after each completed ritual step and watch handoff. Choose **Continue** to resume. Saving and quitting from the pause menu also records your current location and carried item.

## Build and run

Raylib 6.0 builds automatically on the first game build. You need Git, Make, and a C compiler.

| Platform | Build | Run |
| --- | --- | --- |
| Linux | `make unx` | `./Israels_Temple_linux` |
| macOS | `make mac_app` | `open Israels_Temple.app` |
| Windows with MinGW | `mingw32-make win` | `Israels_Temple.exe` |

On Debian or Ubuntu, install the desktop development libraries before building.

```sh
sudo apt install build-essential git libgl-dev libasound2-dev xorg-dev
make unx
./Israels_Temple_linux
```

Keep the `Game/sprites` and `Game/MapLoader/Maps` folders with distributions of the executable. The macOS app bundles them automatically.

Run `make test` to check all seven story watches, reconstruction of ritual state from saved progress, free service, and month and year rollover. The tests need a C compiler and the checked-in raylib headers.
