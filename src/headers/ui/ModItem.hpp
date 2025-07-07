#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FavoritesPopup; // Forward declaration

class ModItem : public CCNode {
protected:
    Mod* m_mod;
    CCMenuItemSpriteExtra* m_favButton = nullptr;
    FavoritesPopup* m_parentPopup = nullptr;
    CCScale9Sprite* m_backgroundSprite = nullptr;

    bool isFavorite = false;

    bool init(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup = nullptr);
    void onViewMod(CCObject*);
    void onFavorite(CCObject*);
    void updateFavoriteIcon();
public:
    static ModItem* create(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup = nullptr);
};