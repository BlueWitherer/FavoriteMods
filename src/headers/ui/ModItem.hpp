#pragma once

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

class FavoritesPopup; // Forward declaration

class ModItem : public CCNode {
protected:
    bool m_geodeTheme = false; // Make sure visuals go with geode theme

    Mod* m_mod; // Fetched mod
    Mod* m_thisMod = getMod(); // Get this mod

    CCMenuItemSpriteExtra* m_favButton = nullptr;
    FavoritesPopup* m_parentPopup = nullptr;
    CCScale9Sprite* m_backgroundSprite = nullptr;

    bool isFavorite = false;

    void onViewMod(CCObject*);
    void onFavorite(CCObject*);

    CCLabelBMFont* firstTimeText();
    void updateFavoriteIcon();

    bool init(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup = nullptr, bool geodeTheme = false);
public:
    static ModItem* create(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup = nullptr, bool geodeTheme = false);
};