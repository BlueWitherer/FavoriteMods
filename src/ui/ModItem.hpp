#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Mod list item
class ModItem : public CCNode {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    ModItem();
    virtual ~ModItem();

    void onViewMod(CCObject*);
    void onModDesc(CCObject*);
    void onFavorite(CCObject*);

    CCLabelBMFont* firstTimeText();
    void updateFavoriteIcon();

    bool init(Mod* mod, CCSize const& size, bool geodeTheme, bool heartIcons);

public:
    static ModItem* create(Mod* mod, CCSize const& size, bool geodeTheme = false, bool heartIcons = false);
};