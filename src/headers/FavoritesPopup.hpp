#pragma once

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

class FavoritesPopup : public Popup<>, public TextInputDelegate {
protected:
    bool m_geodeTheme = false; // Make sure visuals go with geode theme
    bool m_heartIcons = false; // Heart UI mode
    bool m_usePages = false; // Use the list page system

    Mod* m_thisMod = getMod(); // Get this mod

    ScrollLayer* m_scrollLayer = nullptr;

    int p_page = 1;

    int p_itemsPerPage = 10;

    int p_totalItems = 0;
    int p_totalPages = 0;

    CCMenuItemSpriteExtra* m_pageForwardBtn = nullptr;
    CCMenuItemSpriteExtra* m_pageBackwardBtn = nullptr;

    CCLabelBMFont* m_pagesLabel = nullptr;

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

    void onPageForward(CCObject*);
    void onPageBackward(CCObject*);

    void onClearSearch(CCObject*);
    void onPromptClearAll(CCObject*);
    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);
    void onInfoButton(CCObject*);
    void onGetStats(CCObject*);
    void onModSettings(CCObject*);
public:
    static FavoritesPopup* create(bool geodeTheme = false, bool heartIcons = false);

    void onModFavoriteChanged(); // New method for ModItem to call
};