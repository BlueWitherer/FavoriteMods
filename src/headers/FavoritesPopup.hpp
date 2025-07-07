#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FavoritesPopup : public geode::Popup<> {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_buttonMenu = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;

    bool setup() override;
    void refreshModList();
    void onDelayedRefresh(float dt);
public:
    static FavoritesPopup* create();

    void onAddFavorite(CCObject*);
    void onRemoveFavorite(CCObject*);
    void onRefresh(CCObject*);
    void onViewMod(CCObject*);
};