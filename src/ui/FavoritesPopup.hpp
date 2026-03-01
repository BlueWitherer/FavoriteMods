#pragma once

#include "../Events.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Favorites menu
class FavoritesPopup final : public Popup, public TextInputDelegate {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    FavoritesPopup();
    ~FavoritesPopup();

    void textChanged(CCTextInputNode* input) override;

    void onPageNext(CCObject*);
    void onPagePrevious(CCObject*);

    void onClearAll();
    void onClearSearch(CCObject*);
    void onPromptClearAll(CCObject*);

    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);

    void onInfoButton(CCObject*);
    void onGetStats(CCObject*);

    void onModSettings(CCObject*);

    bool init(bool geodeTheme, bool heartIcons);

public:
    static FavoritesPopup* create(bool geodeTheme = false, bool heartIcons = false);
};