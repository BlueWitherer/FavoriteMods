#include "../FavoritesPopup.hpp"

#include "../ui/ModItem.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;

std::string toLowercase(std::string str) {
    std::string newStr = str;

    for (auto& c : newStr) {
        c = tolower(c);
    };

    return newStr;
};

bool FavoritesPopup::setup() {
    log::debug("FavoritesPopup::setup() called");

    auto loader = Loader::get();

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    setID("favorite-mods"_spr);
    setTitle("Favorite Mods");

    // Create main content area
    auto contentSize = CCSize{ 400.f, 280.f };
    auto scrollSize = CCSize{ 380.f, 231.5f };

    // Background for scroll area
    auto scrollBG = CCScale9Sprite::create("square02b_001.png");
    scrollBG->setContentSize(scrollSize);
    scrollBG->setAnchorPoint({ 0.5, 0.5 });
    scrollBG->ignoreAnchorPointForPosition(false);
    scrollBG->setPosition({ contentSize.width / 2.f, (contentSize.height / 2.f) - 12.5f });
    scrollBG->setColor({ 0, 0, 0 });
    scrollBG->setOpacity(100);

    m_mainLayer->addChild(scrollBG);

    // Create scroll layer
    m_scrollLayer = ScrollLayer::create({ scrollSize.width - 12.5f, scrollSize.height - 12.5f });
    m_scrollLayer->setAnchorPoint({ 0.5, 0.5 });
    m_scrollLayer->ignoreAnchorPointForPosition(false);
    m_scrollLayer->setPosition(scrollBG->getPosition());
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
        ->setAxisAlignment(AxisAlignment::End) // seriously why is this end at top but start at bottom?
        ->setAxisReverse(true) // haha wtf is top reverse but bottom isnt LMAO
        ->setAutoGrowAxis(360.f)
        ->setGap(5.f)
    );

    m_mainLayer->addChild(m_scrollLayer);

    auto allMods = loader->getAllMods();

    // Sort if favorited or otherwise in alphabetical order
    std::sort(allMods.begin(), allMods.end(), [](const Mod* a, const Mod* b) -> bool {
        auto aFav = getMod()->getSavedValue<bool>(a->getID()); // Check if mod A is favorited
        auto bFav = getMod()->getSavedValue<bool>(b->getID()); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
              });

    for (Mod* mod : allMods) { // Add all mod items to scrolllayer
        m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_scrollLayer->getScaledContentWidth(), 40.f }));
        log::debug("Added container for mod {}", mod->getID());
    };

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();

    return true;
};

void FavoritesPopup::onAddFavorite(CCObject*) {
    FLAlertLayer::create("Add Favorite", "This would add a mod to your favorites!", "OK")->show();
};

void FavoritesPopup::onRemoveFavorite(CCObject*) {
    FLAlertLayer::create("Remove Favorite", "This would remove the selected mod from favorites!", "OK")->show();
};

void FavoritesPopup::onRefresh(CCObject*) {
    // Clear and recreate scroll content
    m_scrollLayer->m_contentLayer->removeAllChildren();

    // Show confirmation
    Notification::create("Favorites list refreshed!", NotificationIcon::Success)->show();
};

FavoritesPopup* FavoritesPopup::create() {
    log::debug("FavoritesPopup::create() called");

    auto ret = new FavoritesPopup();
    if (ret && ret->initAnchored(400.f, 280.f)) {
        ret->autorelease();
        log::debug("FavoritesPopup created successfully");
        return ret;
    };

    log::error("Failed to create FavoritesPopup!");
    CC_SAFE_DELETE(ret);
    return nullptr;
};

void FavoritesPopup::onViewMod(CCObject*) {
    FLAlertLayer::create("View Mod", "This would open the mod details!", "OK")->show();
};