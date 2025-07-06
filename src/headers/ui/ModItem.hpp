#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ModItem : public CCNode {
protected:
    Mod* m_mod;

    bool isFavorite = false;

    bool init(Mod* mod, CCSize const& size);
    void onViewMod(CCObject*);
    void onFavorite(CCObject*);
public:
    static ModItem* create(Mod* mod, CCSize const& size);
};