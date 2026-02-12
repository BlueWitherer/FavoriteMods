#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Mod list item
class FavoritesItem : public CCNode {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    FavoritesItem();
    ~FavoritesItem();

    void onViewMod(CCObject*);
    void onModDesc(CCObject*);

    void onFavorite(CCObject*);

    CCLabelBMFont* firstTimeText();
    void updateFavoriteIcon();

    bool init(Mod* mod, CCSize const& size, bool geodeTheme, bool heartIcons);

public:
    static FavoritesItem* create(Mod* mod, CCSize const& size, bool geodeTheme = false, bool heartIcons = false);
};