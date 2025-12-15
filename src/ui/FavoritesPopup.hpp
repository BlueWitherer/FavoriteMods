#pragma once

#include "../Events.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Favorites menu
class FavoritesPopup : public Popup<>, public TextInputDelegate {
protected:
    class Impl;
    std::unique_ptr<Impl> m_impl;

    EventListener<OnFavoriteEventFilter> m_listener = {
        [this]() { return OnFavoritesChanged(); },
        OnFavoriteEventFilter()
    };

    FavoritesPopup();
    virtual ~FavoritesPopup();

    bool setup() override;
    bool init(
        float width,
        float height,
        bool geodeTheme = false,
        bool heartIcons = false
    );

    void textChanged(CCTextInputNode* input) override;

    ListenerResult OnFavoritesChanged();
    void refreshModList(bool clearSearch = false);

    void loadModList(std::vector<Mod*> const& allMods);
    void onClearAll(); // Clear all favorites

    void onPageNext(CCObject*);
    void onPagePrevious(CCObject*);

    void onClearSearch(CCObject*);
    void onPromptClearAll(CCObject*);
    void onFavoritesOnlyToggle(CCObject*);
    void onHideFavoritesToggle(CCObject*);
    void onInfoButton(CCObject*);
    void onGetStats(CCObject*);
    void onModSettings(CCObject*);

public:
    static FavoritesPopup* create(
        bool geodeTheme = false,
        bool heartIcons = false
    );

    void onModFavoriteChanged(); // New method for ModItem to call
};