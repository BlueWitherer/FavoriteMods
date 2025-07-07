#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FavoritesPopup : public Popup<> {
protected:
    bool m_pendingUpdate = false;

    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_buttonMenu = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;

    bool setup() override;
    void refreshModList();
    void onDelayedRefresh(float dt);
public:
    static FavoritesPopup* create();

    void onRefresh(CCObject*);
};