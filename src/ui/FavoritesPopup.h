#pragma once

#include <Geode/Geode.hpp>

// Favorites menu
class FavoritesPopup final : public geode::Popup, public TextInputDelegate {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    static FavoritesPopup* s_inst;

protected:
    FavoritesPopup();
    ~FavoritesPopup();

    void textChanged(CCTextInputNode* input) override;

    void onFavoritesOnlyToggle(cocos2d::CCObject*);
    void onHideFavoritesToggle(cocos2d::CCObject*);

    void onClose(CCObject* sender) override;
    void onExit() override;
    void cleanup() override;

    bool init(bool geodeTheme, bool heartIcons);

public:
    static FavoritesPopup* create(bool geodeTheme = false, bool heartIcons = false);
    static FavoritesPopup* get() noexcept;

    void refreshList();
};