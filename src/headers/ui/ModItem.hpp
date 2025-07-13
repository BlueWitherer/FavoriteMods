#pragma once

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

class FavoritesPopup; // Forward declaration

class ModItem : public CCNode {
protected:
    bool m_favorite = false; // If the mod is favorite

    bool m_geodeTheme = false; // Make sure visuals go with geode theme
    bool m_heartTheme = false; // Heart UI mode

    Mod* m_mod; // Fetched mod
    Mod* m_thisMod = getMod(); // Get this mod

    CCMenuItemSpriteExtra* m_favButton = nullptr;
    FavoritesPopup* m_parentPopup = nullptr;
    CCScale9Sprite* m_backgroundSprite = nullptr;

    void onViewMod(CCObject*);
    void onFavorite(CCObject*);

    CCLabelBMFont* firstTimeText();
    void updateFavoriteIcon();

    bool init(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme = false);
public:
    static ModItem* create(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme = false);
};