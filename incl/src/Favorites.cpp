#include "../Favorites.hpp"

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

namespace favorites {
    bool isFavorite(std::string mod) {
        auto thisMod = getMod();
        return thisMod->getSavedValue<bool>(mod);
    };
};