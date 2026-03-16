#pragma once

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

    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);

    bool init(bool geodeTheme, bool heartIcons);

public:
    static FavoritesPopup* create(bool geodeTheme = false, bool heartIcons = false);
};