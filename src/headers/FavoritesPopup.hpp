#include <Geode/Geode.hpp>

using namespace geode::prelude;

class FavoritesPopup : public geode::Popup<> {
protected:
    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_buttonMenu = nullptr;

    bool setup() override;
public:
    static FavoritesPopup* create();

    void onAddFavorite(CCObject*);
    void onRemoveFavorite(CCObject*);
    void onRefresh(CCObject*);
    void onViewMod(CCObject*);
};