#include "../Favorites.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace favorites {
    bool isFavorite(std::string modID) {
        return m_thisMod->getSavedValue<bool>(modID, false);
    };

    int favoritesCount() {
        int count = 0;

        if (auto loader = Loader::get()) {
            for (Mod* mod : loader->getAllMods()) {
                if (m_thisMod->getSavedValue<bool>(mod->getID())) count++;
            };
        } else {
            log::error("Failed to get loader");
        };

        return count;
    };
};