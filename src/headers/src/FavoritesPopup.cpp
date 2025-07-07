#include "../FavoritesPopup.hpp"

#include "../ui/ModItem.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

std::string toLowercase(std::string s) {
    std::string str = s;

    for (auto& c : str) {
        c = tolower(c);
    };

    return str;
};

bool FavoritesPopup::setup() {
    setID("favorite-mods"_spr);
    setTitle("Favorite Mods");

    // geode loader
    auto loader = Loader::get();

    auto [widthCS, heightCS] = m_mainLayer->getContentSize();

    // Create main content area
    auto contentSize = CCSize{ 400.f, 290.f };
    auto scrollSize = CCSize{ 380.f, 200.5f };

    // for buttons to work
    m_overlayMenu = CCMenu::create();
    m_overlayMenu->setID("overlay-menu");
    m_overlayMenu->ignoreAnchorPointForPosition(false);
    m_overlayMenu->setPosition({ widthCS / 2.f, heightCS / 2.f });
    m_overlayMenu->setScaledContentSize(m_mainLayer->getScaledContentSize());
    m_overlayMenu->setZOrder(10);

    m_mainLayer->addChild(m_overlayMenu);

    // Background for scroll area
    auto scrollBG = CCScale9Sprite::create("square02b_001.png");
    scrollBG->setContentSize(scrollSize);
    scrollBG->setAnchorPoint({ 0.5, 0.5 });
    scrollBG->ignoreAnchorPointForPosition(false);
    scrollBG->setPosition({ contentSize.width / 2.f, (contentSize.height / 2.f) - 35.f });
    scrollBG->setColor({ 0, 0, 0 });
    scrollBG->setOpacity(100);

    m_mainLayer->addChild(scrollBG);

    // Create search input
    m_searchInput = TextInput::create(300.f, "Search Mods", "bigFont.fnt");
    m_searchInput->setPosition({ contentSize.width / 2.f - 40.f, contentSize.height - 60.f });
    m_searchInput->setDelegate(this);
    m_searchInput->setMaxCharCount(50);
    m_mainLayer->addChild(m_searchInput);

    // Create favorites only checkbox
    auto favOnlyOffSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto favOnlyOnSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    favOnlyOffSprite->setScale(0.8f);
    favOnlyOnSprite->setScale(0.8f);

    m_favoritesOnlyToggle = CCMenuItemToggler::create(
        favOnlyOffSprite,
        favOnlyOnSprite,
        this,
        menu_selector(FavoritesPopup::onFavoritesOnlyToggle)
    );
    m_favoritesOnlyToggle->setPosition({ contentSize.width - 70.f, contentSize.height - 60.f });

    // Create hide favorites checkbox
    auto hideFavOffSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto hideFavOnSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
    hideFavOffSprite->setScale(0.8f);
    hideFavOnSprite->setScale(0.8f);

    m_hideFavoritesToggle = CCMenuItemToggler::create(
        hideFavOffSprite,
        hideFavOnSprite,
        this,
        menu_selector(FavoritesPopup::onHideFavoritesToggle)
    );
    m_hideFavoritesToggle->setPosition({ contentSize.width - 30.f, contentSize.height - 60.f });

    // Create star icon above favorites only toggle
    auto starIcon = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    starIcon->setPosition({ contentSize.width - 70.f, contentSize.height - 40.f });
    starIcon->setScale(0.6f);

    // Create eye icon above hide favorites toggle
    auto eyeIcon = CCSprite::createWithSpriteFrameName("GJ_starsIcon_gray_001.png");
    eyeIcon->setPosition({ contentSize.width - 30.f, contentSize.height - 40.f });
    eyeIcon->setScale(0.6f);

    auto checkboxMenu = CCMenu::create();
    checkboxMenu->addChild(m_favoritesOnlyToggle);
    checkboxMenu->addChild(m_hideFavoritesToggle);
    checkboxMenu->setPosition(CCPointZero);
    m_mainLayer->addChild(checkboxMenu);
    m_mainLayer->addChild(starIcon);
    m_mainLayer->addChild(eyeIcon);

    // Create info button
    auto infoBtnSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoBtnSprite->setScale(0.7f);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoBtnSprite,
        this,
        menu_selector(FavoritesPopup::onInfoButton)
    );
    infoBtn->setPosition({ contentSize.width - 15.f, contentSize.height - 25.f });

    auto infoMenu = CCMenu::create();
    infoMenu->setPosition(CCPointZero);

    infoMenu->addChild(infoBtn);
    m_mainLayer->addChild(infoMenu);

    // Create "No mods found" label (initially hidden)
    m_noModsLabel = CCLabelBMFont::create("No mods found :(", "bigFont.fnt");
    m_noModsLabel->setPosition({ contentSize.width / 2.f, contentSize.height / 2.f - 35.f });
    m_noModsLabel->setScale(0.5f);
    m_noModsLabel->setColor({ 150, 150, 150 });
    m_noModsLabel->setVisible(false);
    m_mainLayer->addChild(m_noModsLabel);

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
    std::sort(allMods.begin(), allMods.end(), [this](const Mod* a, const Mod* b) -> bool {
        auto aFav = m_thisMod->getSavedValue<bool>(a->getID(), false); // Check if mod A is favorited
        auto bFav = m_thisMod->getSavedValue<bool>(b->getID(), false); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
              });

    for (Mod* mod : allMods) { // Add all mod items to scrolllayer
        m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_scrollLayer->getScaledContentWidth(), 40.f }, this));
        log::debug("Added container for mod {}", mod->getID());
    };

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();

    if (m_thisMod->getSettingValue<bool>("settings-btn")) {
        // geode mod settings popup button
        auto modSettingsBtnSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
        modSettingsBtnSprite->setScale(0.875f);

        auto modSettingsBtn = CCMenuItemSpriteExtra::create(
            modSettingsBtnSprite,
            this,
            menu_selector(FavoritesPopup::onModSettings));
        modSettingsBtn->setID("mod-settings-button");
        modSettingsBtn->setPosition({ 25.f, 25.f });

        m_overlayMenu->addChild(modSettingsBtn);
    } else {
        log::debug("Mod settings button disabled");
    };

    // button to clear favorites
    auto clearAllBtnSprite = ButtonSprite::create("Clear");
    auto clearAllBtn = CCMenuItemSpriteExtra::create(
        clearAllBtnSprite,
        this,
        menu_selector(FavoritesPopup::onPromptClearAll));
    clearAllBtn->setPosition({ widthCS / 2.f, 5.f });
    clearAllBtn->setVisible(true);
    clearAllBtn->setZOrder(3);

    m_overlayMenu->addChild(clearAllBtn);

    return true;
};

void FavoritesPopup::refreshModList() {
    // Clear and recreate scroll content
    m_scrollLayer->m_contentLayer->removeAllChildren();

    auto loader = Loader::get();
    auto allMods = loader->getAllMods();

    // Filter mods based on search text and toggle settings
    std::vector<Mod*> filteredMods;

    for (Mod* mod : allMods) {
        bool isFavorited = m_thisMod->getSavedValue<bool>(mod->getID(), false);

        bool matchesSearch = m_searchText.empty() && mod->getID() != "geode.loader" // Skip geode by default
            || toLowercase(mod->getName()).find(toLowercase(m_searchText)) != std::string::npos
            || toLowercase(mod->getID()).find(toLowercase(m_searchText)) != std::string::npos;

        bool filterEnabled = m_thisMod->getSettingValue<bool>("enabled-required"); // Filter to only show enabled mods
        bool enabledFilter = !filterEnabled || mod->isEnabled() && filterEnabled; // Show all mods, or filter through only enabled mods

        if (enabledFilter) {
            // If favorites-only is enabled, only show favorited mods
            if (m_showFavoritesOnly) {
                if (isFavorited && matchesSearch) filteredMods.push_back(mod);
            } else if (m_hideFavorites) { // If hide favorites is enabled, only show non-favorited mods
                if (!isFavorited && matchesSearch) filteredMods.push_back(mod);
            } else { // Normal mode: show all matching mods, with favorites prioritized
                if (isFavorited || matchesSearch) filteredMods.push_back(mod);
            };
        } else {
            log::error("Mod {} must be enabled to show", mod->getID());
        };
    };

    // Sort if favorited or otherwise in alphabetical order
    std::sort(filteredMods.begin(), filteredMods.end(), [this](const Mod* a, const Mod* b) -> bool {
        auto aFav = m_thisMod->getSavedValue<bool>(a->getID(), false); // Check if mod A is favorited
        auto bFav = m_thisMod->getSavedValue<bool>(b->getID(), false); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return toLowercase(a->getName()) < toLowercase(b->getName()); // Alphabetical order
              });

    for (Mod* mod : filteredMods) { // Add filtered mod items to scrolllayer
        m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_scrollLayer->getScaledContentWidth(), 40.f }, this));
        log::debug("Added container for mod {}", mod->getID());
    };

    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->scrollToTop();

    // Show/hide "No mods found" message
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
    this->refreshModList();
};

void FavoritesPopup::onModSettings(CCObject*) {
    openSettingsPopup(m_thisMod);
};

void FavoritesPopup::onFavoritesOnlyToggle(CCObject*) {
    m_showFavoritesOnly = !m_showFavoritesOnly;

    // If favorites only is turned on, turn off hide favorites
    if (m_showFavoritesOnly && m_hideFavorites) {
        m_hideFavorites = false;
        m_hideFavoritesToggle->toggle(false);
    };

    this->refreshModList();
};

void FavoritesPopup::onHideFavoritesToggle(CCObject*) {
    m_hideFavorites = !m_hideFavorites;

    // If hide favorites is turned on, turn off favorites only
    if (m_hideFavorites && m_showFavoritesOnly) {
        m_showFavoritesOnly = false;
        m_favoritesOnlyToggle->toggle(false);
    };

    this->refreshModList();
};

void FavoritesPopup::onInfoButton(CCObject*) {
    auto popup = FLAlertLayer::create(
        "Help",
        "To <cg>add a mod to your favorites</c>, search for it in the scrolling area and press the <cy>star icon</c> located to the right-hand side of the interface. You can also press it again to <cr>remove</c> it from your favorites.",
        "OK"
    );
    popup->show();
};

void FavoritesPopup::onPromptClearAll(CCObject*) {
    createQuickPopup(
        "Clear All Favorites",
        "Are you sure you want to <cr>clear all your favorite mods</c>?",
        "Cancel", "Yes",
        [this](auto, bool btn2) {
            if (btn2) this->onClearAll();
        },
        true);
};

void FavoritesPopup::onClearAll() {
    auto loader = Loader::get();
    auto allMods = loader->getAllMods();

    for (Mod* mod : allMods) {
        auto modId = mod->getID();
        if (m_thisMod->getSavedValue<bool>(modId)) m_thisMod->setSavedValue(modId, false);
    };

    this->refreshModList();

    Notification::create("Cleared all favorites", NotificationIcon::Success, 2.5f)->show();
    log::info("Cleared all favorite mods");
};

void FavoritesPopup::onModFavoriteChanged() {
    // Refresh the mod list to re-sort based on new favorite status
    this->refreshModList();
};

FavoritesPopup* FavoritesPopup::create() {
    auto ret = new FavoritesPopup();

    if (ret && ret->initAnchored(400.f, 280.f)) {
        ret->autorelease();
        return ret;
    };

    CC_SAFE_DELETE(ret);
    return nullptr;
};