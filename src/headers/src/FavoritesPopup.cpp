#include "../FavoritesPopup.hpp"

#include "../ui/ModItem.hpp"

#include <fmt/core.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

std::string toLowercase(std::string s) {
    std::string str = s;
    for (auto& c : str) c = tolower(c);

    return str;
};

bool FavoritesPopup::init(float width, float height, bool geodeTheme, bool heartIcons) {
    m_geodeTheme = geodeTheme;
    m_heartIcons = heartIcons;

    if (Popup<>::initAnchored(width, height, m_geodeTheme ? "geode.loader/GE_square01.png" : "GJ_square01.png")) {
        setCloseButtonSpr(
            CircleButtonSprite::createWithSpriteFrameName(
                "geode.loader/close.png",
                0.875f,
                m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green
            )
        );

        return true;
    } else {
        log::error("Could not initialize favorites popup");
        return false;
    };
};

bool FavoritesPopup::setup() {
    setID("favorite-mods-popup"_spr);
    setTitle("Favorite Mods");

    auto [widthCS, heightCS] = m_mainLayer->getContentSize();

    // geode mod loader
    auto loader = Loader::get();

    // Create main content area
    auto contentSize = CCSize{ 400.f, 290.f };
    auto scrollSize = CCSize{ 380.f, 200.5f };

    // for buttons to work
    m_overlayMenu = CCMenu::create();
    m_overlayMenu->setID("overlay-menu");
    m_overlayMenu->ignoreAnchorPointForPosition(false);
    m_overlayMenu->setPosition({ widthCS / 2.f, heightCS / 2.f });
    m_overlayMenu->setScaledContentSize(m_mainLayer->getScaledContentSize());
    m_overlayMenu->setTouchPriority(-505);
    m_overlayMenu->setZOrder(10);

    m_mainLayer->addChild(m_overlayMenu);

    // Create search input
    m_searchInput = TextInput::create(265.f, "Search Mods", "bigFont.fnt");
    m_searchInput->setID("search-box");
    m_searchInput->setPosition({ contentSize.width / 2.f - 20.f, contentSize.height - 60.f });
    m_searchInput->setDelegate(this);
    m_searchInput->setMaxCharCount(50);

    m_mainLayer->addChild(m_searchInput);

    // Clear search box button sprite
    auto searchClearBtnSprite = CCSprite::createWithSpriteFrameName("GJ_longBtn07_001.png");
    searchClearBtnSprite->setScale(0.875f);

    // Create clear search box button
    auto searchClearBtn = CCMenuItemSpriteExtra::create(
        searchClearBtnSprite,
        this,
        menu_selector(FavoritesPopup::onClearSearch)
    );
    searchClearBtn->setID("clear-search-button");
    searchClearBtn->setAnchorPoint({ 0.5, 0.5 });
    searchClearBtn->ignoreAnchorPointForPosition(false);
    searchClearBtn->setPosition({ 26.5f, m_searchInput->getPositionY() });

    m_overlayMenu->addChild(searchClearBtn);

    // Menu for checkboxes
    auto checkboxMenu = CCMenu::create();
    checkboxMenu->setID("checkbox-menu");
    checkboxMenu->setPosition(CCPointZero);
    checkboxMenu->setTouchPriority(-504);

    m_mainLayer->addChild(checkboxMenu);

    auto favOnlyOffSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto favOnlyOnSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    favOnlyOffSprite->setScale(0.8f);
    favOnlyOnSprite->setScale(0.8f);

    // Create favorites only checkbox
    m_favoritesOnlyToggle = CCMenuItemToggler::create(
        favOnlyOffSprite,
        favOnlyOnSprite,
        this,
        menu_selector(FavoritesPopup::onFavoritesOnlyToggle)
    );
    m_favoritesOnlyToggle->setID("favorites-only-toggler");
    m_favoritesOnlyToggle->setPosition({ contentSize.width - 70.f, contentSize.height - 60.f });

    checkboxMenu->addChild(m_favoritesOnlyToggle);

    auto hideFavOffSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto hideFavOnSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    hideFavOffSprite->setScale(0.8f);
    hideFavOnSprite->setScale(0.8f);

    // Create hide favorites checkbox
    m_hideFavoritesToggle = CCMenuItemToggler::create(
        hideFavOffSprite,
        hideFavOnSprite,
        this,
        menu_selector(FavoritesPopup::onHideFavoritesToggle)
    );
    m_hideFavoritesToggle->setID("hide-favorites-toggler");
    m_hideFavoritesToggle->setPosition({ contentSize.width - 30.f, contentSize.height - 60.f });

    checkboxMenu->addChild(m_hideFavoritesToggle);

    // Create icon above favorites only toggle
    auto starOnIcon = CCSprite::createWithSpriteFrameName(m_heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png");
    starOnIcon->setID("favorites-icon");
    starOnIcon->setPosition({ contentSize.width - 70.f, contentSize.height - 40.f });
    starOnIcon->setScale(m_heartIcons ? 0.375f : 0.625f);

    // Create icon above hide favorites toggle
    auto starOffIcon = CCSprite::createWithSpriteFrameName(m_heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png");
    starOffIcon->setID("non-favorites-icon");
    starOffIcon->setPosition({ contentSize.width - 30.f, contentSize.height - 40.f });
    starOffIcon->setScale(m_heartIcons ? 0.375f : 0.625f);

    m_mainLayer->addChild(starOnIcon);
    m_mainLayer->addChild(starOffIcon);

    // Create info button
    auto infoBtnSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoBtnSprite->setScale(0.7f);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoBtnSprite,
        this,
        menu_selector(FavoritesPopup::onInfoButton)
    );
    infoBtn->setID("info-button");
    infoBtn->setPosition({ contentSize.width - 15.f, contentSize.height - 25.f });

    auto infoMenu = CCMenu::create();
    infoMenu->setID("info-menu");
    infoMenu->setPosition(CCPointZero);
    infoMenu->setTouchPriority(-503);

    infoMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoMenu);

    // Create "No mods found" label (initially hidden)
    m_noModsLabel = CCLabelBMFont::create("No mods found :(", "bigFont.fnt");
    m_noModsLabel->setPosition({ contentSize.width / 2.f, contentSize.height / 2.f - 35.f });
    m_noModsLabel->setScale(0.5f);
    m_noModsLabel->setColor({ 125, 125, 125 });
    m_noModsLabel->setVisible(false);

    m_mainLayer->addChild(m_noModsLabel);

    // Background for scroll layer
    auto scrollBG = CCScale9Sprite::create("square02b_001.png");
    scrollBG->setContentSize(scrollSize);
    scrollBG->setAnchorPoint({ 0.5, 0.5 });
    scrollBG->ignoreAnchorPointForPosition(false);
    scrollBG->setPosition({ contentSize.width / 2.f, (contentSize.height / 2.f) - 35.f });
    scrollBG->setColor({ 0, 0, 0 });
    scrollBG->setOpacity(100);

    m_mainLayer->addChild(scrollBG);

    // Create layout for scroll layer
    auto scrollLayerLayout = ColumnLayout::create()
        ->setAxisAlignment(AxisAlignment::End) // seriously why is this end at top but start at bottom?
        ->setAxisReverse(true) // haha wtf is top reverse but bottom isnt LMAO
        ->setAutoGrowAxis(360.f)
        ->setGap(5.f);

    // Create scroll layer
    m_scrollLayer = ScrollLayer::create({ scrollSize.width - 12.5f, scrollSize.height - 12.5f });
    m_scrollLayer->setID("scroll-layer");
    m_scrollLayer->setAnchorPoint({ 0.5, 0.5 });
    m_scrollLayer->ignoreAnchorPointForPosition(false);
    m_scrollLayer->setPosition(scrollBG->getPosition());
    m_scrollLayer->m_contentLayer->setLayout(scrollLayerLayout);

    m_mainLayer->addChild(m_scrollLayer);

    auto allMods = loader->getAllMods();

    // Sort if favorited or otherwise in alphabetical order
    std::sort(allMods.begin(), allMods.end(), [this](const Mod* a, const Mod* b) -> bool {
        auto aFav = m_thisMod->getSavedValue<bool>(a->getID(), false); // Check if mod A is favorited
        auto bFav = m_thisMod->getSavedValue<bool>(b->getID(), false); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
              });

    loadModList(allMods);

    m_scrollLayer->m_contentLayer->updateLayout(true);
    m_scrollLayer->scrollToTop();

    if (m_thisMod->getSettingValue<bool>("settings-btn")) {
        // geode mod settings popup button
        auto modSettingsBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/settings.png",
            1.f,
            m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
            CircleBaseSize::Medium
        );
        modSettingsBtnSprite->setScale(0.75f);

        auto modSettingsBtn = CCMenuItemSpriteExtra::create(
            modSettingsBtnSprite,
            this,
            menu_selector(FavoritesPopup::onModSettings)
        );
        modSettingsBtn->setID("mod-settings-button");
        modSettingsBtn->setPosition({ 2.5f, 2.5f });

        m_overlayMenu->addChild(modSettingsBtn);
    } else {
        log::debug("Mod settings button disabled");
    };

    // clear favs button sprite
    auto clearAllBtnSprite = ButtonSprite::create(
        "Clear",
        "bigFont.fnt",
        m_geodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png",
        0.875f
    );
    clearAllBtnSprite->setScale(0.75f);

    // button to clear favorites
    auto clearAllBtn = CCMenuItemSpriteExtra::create(
        clearAllBtnSprite,
        this,
        menu_selector(FavoritesPopup::onPromptClearAll)
    );
    clearAllBtn->setID("clear-favorites-button");
    clearAllBtn->setPosition({ widthCS / 2.f, 1.25f });
    clearAllBtn->setZOrder(3);

    m_overlayMenu->addChild(clearAllBtn);

    // favorites stats button sprite
    auto statsBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
        "geode.loader/changelog.png",
        0.875f,
        m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
        CircleBaseSize::Medium
    );
    statsBtnSprite->setScale(0.5f);

    // button to view funny favorites stats
    auto statsBtn = CCMenuItemSpriteExtra::create(
        statsBtnSprite,
        this,
        menu_selector(FavoritesPopup::onGetStats)
    );
    statsBtn->setID("favorites-stats-button");
    statsBtn->setPosition({ widthCS - 1.25f, 1.25f });
    statsBtn->setZOrder(3);

    m_overlayMenu->addChild(statsBtn);

    m_thisMod->setSavedValue("already-loaded", true);

    return true;
};

void FavoritesPopup::loadModList(std::vector<Mod*> allMods) {
    for (Mod* mod : allMods) { // Add all mod items to scrolllayer
        bool list = m_thisMod->getSettingValue<bool>("enabled-only") ? mod->isOrWillBeEnabled() : true;
        if (list) m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_scrollLayer->getScaledContentWidth(), 40.f }, this, m_geodeTheme, m_heartIcons));

        log::debug("{} list item for mod {}", list ? "Processed" : "Skipped", mod->getID());
    };
};

void FavoritesPopup::refreshModList(bool clearSearch) {
    // Clear and recreate scroll content
    m_scrollLayer->m_contentLayer->removeAllChildren();

    if (clearSearch && m_thisMod->getSettingValue<bool>("refresh-clear-search")) m_searchText = "";

    m_searchInput->setString(m_searchText, false);

    auto loader = Loader::get();
    auto allMods = loader->getAllMods();

    // Filtered mods based on search text and toggle settings
    std::vector<Mod*> filteredMods;

    for (Mod* mod : allMods) {
        auto empty = std::string::npos; // dry code xd
        bool isFavorited = m_thisMod->getSavedValue<bool>(mod->getID(), false);

        // evil lines >:3
        bool matchesSearch = (m_searchText.empty() && (mod->getID() != std::string("geode.loader"))) // Skip geode by default
            || toLowercase(mod->getName()).find(toLowercase(m_searchText)) != empty // search via name
            || toLowercase(mod->getDescription().value_or(mod->getName())).find(toLowercase(m_searchText)) != empty // search via description
            || toLowercase(mod->getID()).find(toLowercase(m_searchText)) != empty; // search via id

        // If favorites-only is enabled, only show favorited mods
        if (m_showFavoritesOnly) {
            if (isFavorited && matchesSearch) filteredMods.push_back(mod);
        } else if (m_hideFavorites) { // If hide favorites is enabled, only show non-favorited mods
            if (!isFavorited && matchesSearch) filteredMods.push_back(mod);
        } else { // Normal mode: show all matching mods, with favorites prioritized
            if (matchesSearch) filteredMods.push_back(mod);
        };
    };

    // Sort if favorited or otherwise in alphabetical order
    std::sort(filteredMods.begin(), filteredMods.end(), [this](const Mod* a, const Mod* b) -> bool {
        auto aFav = m_thisMod->getSavedValue<bool>(a->getID(), false); // Check if mod A is favorited
        auto bFav = m_thisMod->getSavedValue<bool>(b->getID(), false); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
              });

    loadModList(filteredMods);

    m_scrollLayer->m_contentLayer->updateLayout(true);
    if (m_thisMod->getSettingValue<bool>("auto-scroll")) m_scrollLayer->scrollToTop();

    // Toggle "No mods found" message
    if (filteredMods.empty()) {
        m_noModsLabel->setVisible(true);
        m_scrollLayer->setVisible(false);
    } else {
        m_noModsLabel->setVisible(false);
        m_scrollLayer->setVisible(true);
    };
};

void FavoritesPopup::textChanged(CCTextInputNode* input) {
    m_searchText = input->getString();
    refreshModList(false);
};

void FavoritesPopup::onClearSearch(CCObject*) {
    m_searchText = "";
    m_searchInput->setString(m_searchText, true);
    log::debug("Cleared search box");
};

void FavoritesPopup::onModSettings(CCObject*) {
    log::debug("Opening mod settings popup");
    openSettingsPopup(m_thisMod);
};

void FavoritesPopup::onFavoritesOnlyToggle(CCObject*) {
    m_showFavoritesOnly = !m_showFavoritesOnly;

    // If favorites only is turned on, turn off hide favorites
    if (m_showFavoritesOnly && m_hideFavorites) {
        m_hideFavorites = false;
        m_hideFavoritesToggle->toggle(false);
    } else {
        log::debug("Hide favorites mode already off");
    };

    refreshModList(true);
};

void FavoritesPopup::onHideFavoritesToggle(CCObject*) {
    m_hideFavorites = !m_hideFavorites;

    // If hide favorites is turned on, turn off favorites only
    if (m_hideFavorites && m_showFavoritesOnly) {
        m_showFavoritesOnly = false;
        m_favoritesOnlyToggle->toggle(false);
    } else {
        log::debug("Favorites only mode already off");
    };

    refreshModList(true);
};

void FavoritesPopup::onInfoButton(CCObject*) {
    auto body = fmt::format("To <cg>add a mod to your favorites</c>, search for it in the scrolling area and press the <cy>{} button</c> located to the right-hand side of the listed mod. You can also press it again to <cr>remove it</c> from your favorites.", m_heartIcons ? "heart" : "star");

    FLAlertLayer::create(
        "Help",
        body.c_str(),
        "OK"
    )->show();
};

void FavoritesPopup::onPromptClearAll(CCObject*) {
    createQuickPopup(
        "Clear Favorites",
        "Are you sure you want to <cr>clear all your favorite mods</c>?",
        "Cancel", "Yes",
        [this](auto, bool btn2) {
            if (btn2) onClearAll();
        },
        true);
};

void FavoritesPopup::onGetStats(CCObject*) {
    int favorite = 0; // favorited mods
    int enabled = 0; // all enabled mods
    int disabled = 0; // all disabled mods
    int total = 0; // all installed mods

    auto loader = Loader::get();

    for (Mod* mod : loader->getAllMods()) {
        total++;

        if (m_thisMod->getSavedValue<bool>(mod->getID())) favorite++;

        if (mod->isEnabled()) {
            enabled++;
        } else {
            disabled++;
        };
    };

    auto faveCount = fmt::format("<cy>{}</c> Favorites", favorite);
    auto enableCount = fmt::format("<cg>{}</c> Mods Enabled", enabled);
    auto disableCount = fmt::format("<cr>{}</c> Mods Disabled", disabled);
    auto totalCount = fmt::format("<cj>{}</c> Mods Installed", total);

    auto body = fmt::format("{}\n\n{}\n{}\n\n{}", faveCount, enableCount, disableCount, totalCount);

    FLAlertLayer::create(
        "Statistics",
        body.c_str(),
        "OK"
    )->show();
};

void FavoritesPopup::onClearAll() {
    auto loader = Loader::get();
    auto allMods = loader->getAllMods();

    // Turn off favorite from every mod
    for (Mod* mod : allMods) {
        auto modId = mod->getID(); // get the mod id
        if (m_thisMod->getSavedValue<bool>(modId, false)) m_thisMod->setSavedValue(modId, false);
    };

    refreshModList(true);

    Notification::create("Cleared all favorites", NotificationIcon::Success, 2.5f)->show();
    log::info("Cleared all favorite mods");
};

void FavoritesPopup::onModFavoriteChanged() {
    // Refresh the mod list to re-sort based on new favorite status
    refreshModList(false);
};

FavoritesPopup* FavoritesPopup::create(bool geodeTheme, bool heartIcons) {
    auto ret = new FavoritesPopup();

    if (ret && ret->init(400.f, 280.f, geodeTheme, heartIcons)) {
        ret->autorelease();
        return ret;
    };

    CC_SAFE_DELETE(ret);
    return nullptr;
};