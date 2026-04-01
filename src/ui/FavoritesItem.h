#pragma once

#include <Geode/Geode.hpp>

// Mod list item
class FavoritesItem final : public cocos2d::CCNode {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    using Callback = geode::Function<void()>;

protected:
    FavoritesItem();
    ~FavoritesItem();

    cocos2d::CCLabelBMFont* firstTimeText();

    bool init(geode::Mod* mod, cocos2d::CCSize const& size, bool geodeTheme, bool heartIcons);

public:
    static FavoritesItem* create(geode::Mod* mod, cocos2d::CCSize const& size, bool geodeTheme = false, bool heartIcons = false);

    void setFavoriteCallback(Callback&& cb);
};