#pragma once

#include "../Events.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Favorites menu
class FavoritesPopup : public Popup<>, public TextInputDelegate {
protected:
    bool m_geodeTheme = false; // Make sure visuals go with geode theme
    bool m_heartIcons = false; // Heart UI mode
    bool m_usePages = false; // Use the list page system

    Ref<ScrollLayer> m_scrollLayer = nullptr;

    EventListener<OnFavoriteEventFilter> m_listener = {
        [this]() { return OnFavoritesChanged(); },
        OnFavoriteEventFilter()
    };

    int p_page = 1;

    int p_itemsPerPage = 10;

    int p_totalItems = 0;
    int p_totalPages = 0;

    Ref<CCMenuItemSpriteExtra> m_pageNextBtn = nullptr;
    Ref<CCMenuItemSpriteExtra> m_pagePreviousBtn = nullptr;

    CCLabelBMFont* m_pagesLabel = nullptr;

    CCMenu* m_overlayMenu = nullptr;

    Ref<TextInput> m_searchInput = nullptr;
    CCLabelBMFont* m_noModsLabel = nullptr;

    std::string m_searchText = "";

    Ref<CCMenuItemToggler> m_favoritesOnlyToggle = nullptr;
    Ref<CCMenuItemToggler> m_hideFavoritesToggle = nullptr;

    bool m_showFavoritesOnly = false;
    bool m_hideFavorites = false;

    bool setup() override;
    bool init(
        float width,
        float height,
        bool geodeTheme = false,
        bool heartIcons = false
    );

    void textChanged(CCTextInputNode* input) override;

    ListenerResult OnFavoritesChanged();
    void refreshModList(bool clearSearch = false);

    void loadModList(std::vector<Mod*> allMods);
    void onClearAll(); // Clear all favorites

    void onPageNext(CCObject*);
    void onPagePrevious(CCObject*);

    void onClearSearch(CCObject*);
    void onPromptClearAll(CCObject*);
    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);
    void onInfoButton(CCObject*);
    void onGetStats(CCObject*);
    void onModSettings(CCObject*);

public:
    static FavoritesPopup* create(
        bool geodeTheme = false,
        bool heartIcons = false
    );

    void onModFavoriteChanged(); // New method for ModItem to call
};