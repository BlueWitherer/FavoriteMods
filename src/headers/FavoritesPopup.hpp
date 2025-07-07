#pragma once

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

class FavoritesPopup : public Popup<>, public TextInputDelegate {
protected:
    Mod* m_thisMod = getMod(); // Get this mod
    bool m_pendingUpdate = false;

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
    void textChanged(CCTextInputNode* input) override;

    void refreshModList();
    void onClearAll(); // Clear all favorites

    void onPromptClearAll(CCObject*);
    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);
    void onInfoButton(CCObject*);
    void onModSettings(CCObject*);
public:
    static FavoritesPopup* create();

    void onModFavoriteChanged(); // New method for ModItem to call
};