#pragma once

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

class FavoritesPopup : public Popup<>, public TextInputDelegate {
protected:
    bool m_geodeTheme = false; // Make sure visuals go with geode theme
    bool m_heartTheme = false; // Heart UI mode

    Mod* m_thisMod = getMod(); // Get this mod

    ScrollLayer* m_scrollLayer = nullptr;

    CCMenu* m_overlayMenu = nullptr;
    TextInput* m_searchInput = nullptr;
    CCLabelBMFont* m_noModsLabel = nullptr;

    std::string m_searchText = "";

    CCMenuItemToggler* m_favoritesOnlyToggle = nullptr;
    CCMenuItemToggler* m_hideFavoritesToggle = nullptr;

    bool m_showFavoritesOnly = false;
    bool m_hideFavorites = false;

    bool setup() override;
    bool init(float width, float height, bool geodeTheme = false, bool heartIcons = false);
    void textChanged(CCTextInputNode* input) override;

    void loadModList(std::vector<Mod*> allMods);
    void refreshModList(bool clearSearch = false);
    void onClearAll(); // Clear all favorites

    void onClearSearch(CCObject*);
    void onPromptClearAll(CCObject*);
    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);
    void onInfoButton(CCObject*);
    void onModSettings(CCObject*);
public:
    static FavoritesPopup* create(bool geodeTheme = false, bool heartIcons = false);

    void onModFavoriteChanged(); // New method for ModItem to call
};