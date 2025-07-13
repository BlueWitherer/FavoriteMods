# <img src="../logo.png" width="30" alt="The mod's logo." /> Favorite Mods
Add installed Geode mods as your favorites!

### Development
###### Work with the mod directly.
You can access data from Favorite Mods by including the [`Favorites.hpp`](./Favorites.hpp) file in your code. Make sure to use the `favorites` namespace to directly access all the needed classes and values.

```cpp
#include <cheeseworks.favoritemods/incl/Favorites.hpp>

using namespace favorites;
```

Here's some sample code to check a mod to see if it is favorited.
```cpp
using namespace favorites;

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        Mod* myMod = geode::getMod();

        // Check if your mod is a player's favorite
        if (isFavorite(myMod->getID())) {
            log::info("Yay! {} is a favorite!", myMod->getName());
        } else {
            log::warn("Aw man... You should totally add {} to your favorites!", myMod->getName());
        };

        return true;
    };
};
```

You are not required to update data manually as this mod does it by itself! Happy modding!

### Users
###### Regular users of this mod.
**[📱 Learn about this mod](../)**