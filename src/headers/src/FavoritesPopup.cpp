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
    setID("favorite-mods"_spr);
    setTitle("Favorite Mods");

    // geode loader
    auto loader = Loader::get();

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
        auto aFav = getMod()->getSavedValue<bool>(a->getID(), false); // Check if mod A is favorited
        auto bFav = getMod()->getSavedValue<bool>(b->getID(), false); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
              });

    for (Mod* mod : allMods) { // Add all mod items to scrolllayer
        m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_scrollLayer->getScaledContentWidth(), 40.f }, this));
        log::debug("Added container for mod {}", mod->getID());
    };

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();

    return true;
};

void FavoritesPopup::refreshModList() {
    // Clear and recreate scroll content
    m_scrollLayer->m_contentLayer->removeAllChildren();

    auto loader = Loader::get();
    auto allMods = loader->getAllMods();

    // Sort if favorited or otherwise in alphabetical order
    std::sort(allMods.begin(), allMods.end(), [](const Mod* a, const Mod* b) -> bool {
        auto aFav = getMod()->getSavedValue<bool>(a->getID(), false); // Check if mod A is favorited
        auto bFav = getMod()->getSavedValue<bool>(b->getID(), false); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
    });

    for (Mod* mod : allMods) { // Add all mod items to scrolllayer
        m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_scrollLayer->getScaledContentWidth(), 40.f }, this));
        log::debug("Added container for mod {}", mod->getID());
    };

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();
};

void FavoritesPopup::onModFavoriteChanged() {
    // Refresh the mod list to re-sort based on new favorite status
    this->refreshModList();
}

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