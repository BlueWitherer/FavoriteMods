#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Mod list item
class FavoritesItem final : public CCNode {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    FavoritesItem();
    ~FavoritesItem();

    CCLabelBMFont* firstTimeText();

    bool init(Mod* mod, CCSize const& size, bool geodeTheme, bool heartIcons);

public:
    static FavoritesItem* create(Mod* mod, CCSize const& size, bool geodeTheme = false, bool heartIcons = false);
};