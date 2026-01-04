#pragma once

#include "../Events.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

// Favorites menu
class FavoritesPopup : public Popup<>, public TextInputDelegate {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    EventListener<OnFavoriteEventFilter> m_listener = {
        [this]() { return OnFavoritesChanged(); },
        OnFavoriteEventFilter()
    };

    FavoritesPopup();
    virtual ~FavoritesPopup();

    bool setup() override;
    bool init(float width, float height, bool geodeTheme, bool heartIcons);

    void textChanged(CCTextInputNode* input) override;

    ListenerResult OnFavoritesChanged();
    void refreshModList(bool clearSearch = false);

    void loadModList(std::vector<Mod*> const& allMods);

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

public:
    static FavoritesPopup* create(
        bool geodeTheme = false,
        bool heartIcons = false
    );
};