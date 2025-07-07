#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FavoritesPopup : public Popup<> {
protected:
    bool m_pendingUpdate = false;

    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_buttonMenu = nullptr;

    bool setup() override;
    void refreshModList();
public:
    static FavoritesPopup* create();

    void onModFavoriteChanged(); // New method for ModItem to call
};