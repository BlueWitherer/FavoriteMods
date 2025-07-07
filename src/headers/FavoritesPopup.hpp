#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FavoritesPopup : public Popup<>, public TextInputDelegate {
protected:
    bool m_pendingUpdate = false;

    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_buttonMenu = nullptr;
    TextInput* m_searchInput = nullptr;
    CCMenuItemToggler* m_favoritesOnlyToggle = nullptr;
    CCMenuItemToggler* m_hideFavoritesToggle = nullptr;
    CCLabelBMFont* m_noModsLabel = nullptr;
    std::string m_searchText = "";
    bool m_showFavoritesOnly = false;
    bool m_hideFavorites = false;

    bool setup() override;
    void refreshModList();
    void textChanged(CCTextInputNode* input) override;
    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);
    void onInfoButton(CCObject*);
public:
    static FavoritesPopup* create();

    void onModFavoriteChanged(); // New method for ModItem to call
};