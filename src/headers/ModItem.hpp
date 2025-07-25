#pragma once

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

class FavoritesPopup; // Forward declaration

class ModItem : public CCNode {
protected:
    bool m_favorite = false; // If the mod is favorite

    bool m_geodeTheme = false; // Make sure visuals go with Geode theme
    bool m_heartIcons = false; // Hearts UI mode

    Mod* m_mod; // Fetched mod
    Mod* m_thisMod = getMod(); // Get this mod

    CCMenuItemSpriteExtra* m_favButton = nullptr; // Favorite button

    FavoritesPopup* m_parentPopup = nullptr; // To access parent methods
    CCScale9Sprite* m_backgroundSprite = nullptr; // Background theme

    void onViewMod(CCObject*);
    void onModDesc(CCObject*);
    void onModIssues(CCObject*);
    void onFavorite(CCObject*);

    CCLabelBMFont* firstTimeText();
    void updateFavoriteIcon();

    bool init(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme = false, bool heartIcons = false);
public:
    static ModItem* create(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme = false, bool heartIcons = false);
};