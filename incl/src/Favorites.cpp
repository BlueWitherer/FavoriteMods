#include "../Favorites.hpp"

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

namespace favorites {
    Mod* m_thisMod = Mod::get();

    bool isFavorite(std::string modId) {
        return m_thisMod->getSavedValue<bool>(modId);
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