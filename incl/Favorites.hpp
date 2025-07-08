#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace favorites {
    extern Mod* m_thisMod; // Favorite Mods

    // Check to see if a mod is included in the user's favorites
    bool isFavorite(
        std::string modId // Your mod's ID
    );

    // Get the amount of mods the player has in their favorites
    int favoritesCount();
};