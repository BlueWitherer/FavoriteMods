#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Mod list item
class ModListItem : public CCNode {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    ModListItem();
    virtual ~ModListItem();

    void onViewMod(CCObject*);
    void onModDesc(CCObject*);

    void onFavorite(CCObject*);

    CCLabelBMFont* firstTimeText();
    void updateFavoriteIcon();

    bool init(Mod* mod, CCSize const& size, bool geodeTheme, bool heartIcons);

public:
    static ModListItem* create(Mod* mod, CCSize const& size, bool geodeTheme = false, bool heartIcons = false);
};