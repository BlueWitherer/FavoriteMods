#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace favorites {
    extern Mod* m_thisMod = Mod::get(); // Favorite Mods

    /**
     * Check to see if a mod is included in the user's favorites
     *
     * @param modID Your mod's ID
     */
    bool isFavorite(std::string modID);

    /**
     * Get the amount of mods the player has in their favorites
     */
    int favoritesCount();
};