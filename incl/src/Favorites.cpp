#include "../Favorites.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace favorites {
    bool isFavorite(std::string modID) {
        return m_thisMod->getSavedValue<bool>(modID);
    };

    int favoritesCount() {
        int count = 0;
        auto loader = Loader::get();

        for (Mod* mod : loader->getAllMods()) {
            if (m_thisMod->getSavedValue<bool>(mod->getID())) count++;
        };

        return count;
    };
};