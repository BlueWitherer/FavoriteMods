#include "../FavoritesPopup.hpp"

#include "../ModItem.hpp"
#include "../../Events.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

namespace str = utils::string;

static auto favMod = Mod::get(); // Get this mod
static auto loader = Loader::get();

class FavoritesPopup::Impl final {
public:
    bool m_geodeTheme = false; // Make sure visuals go with geode theme
    bool m_heartIcons = false; // Heart UI mode
    bool m_usePages = false; // Use the list page system

    Ref<ScrollLayer> m_scrollLayer = nullptr;

    int p_page = 1;

    int p_itemsPerPage = 10;

    int p_totalItems = 0;
    int p_totalPages = 0;

    CCMenuItemSpriteExtra* m_pageNextBtn = nullptr;
    CCMenuItemSpriteExtra* m_pagePreviousBtn = nullptr;

    CCLabelBMFont* m_pagesLabel = nullptr;

    TextInput* m_searchInput = nullptr;
    CCLabelBMFont* m_noModsLabel = nullptr;

    std::string m_searchText = "";

    CCMenuItemToggler* m_favoritesOnlyToggle = nullptr;
    CCMenuItemToggler* m_hideFavoritesToggle = nullptr;

    bool m_showFavoritesOnly = false;
    bool m_hideFavorites = false;
};

FavoritesPopup::FavoritesPopup() {
    m_impl = std::make_unique<Impl>();
};

FavoritesPopup::~FavoritesPopup() {};

bool FavoritesPopup::init(
    float width,
    float height,
    bool geodeTheme,
    bool heartIcons
) {
    m_impl->m_geodeTheme = geodeTheme;
    m_impl->m_heartIcons = heartIcons;

    m_impl->m_usePages = favMod->getSettingValue<bool>("pages");

    m_impl->p_page = 1;

    m_impl->p_itemsPerPage = static_cast<int>(favMod->getSettingValue<int64_t>("pages-count"));

    if (!Popup<>::initAnchored(width, height, m_impl->m_geodeTheme ? "geode.loader/GE_square01.png" : "GJ_square01.png")) return false;

    setCloseButtonSpr(
        CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/close.png",
            0.875f,
            m_impl->m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green
        )
    );

    return true;
};

bool FavoritesPopup::setup() {
    setID("favorite-mods-popup"_spr);
    setTitle("Favorite Mods");

    // Create main content area
    auto const [widthCS, heightCS] = m_mainLayer->getScaledContentSize();

    // Create search input
    m_impl->m_searchInput = TextInput::create(widthCS - 125.f, "Search Mods", "bigFont.fnt");
    m_impl->m_searchInput->setID("search-box");
    m_impl->m_searchInput->setPosition({ widthCS / 2.f - 15.f, heightCS - 50.f });
    m_impl->m_searchInput->setDelegate(this);
    m_impl->m_searchInput->setMaxCharCount(50);

    m_mainLayer->addChild(m_impl->m_searchInput);

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
    searchClearBtn->setPosition({ 26.5f, m_impl->m_searchInput->getPositionY() });

    m_buttonMenu->addChild(searchClearBtn);

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
    m_impl->m_favoritesOnlyToggle = CCMenuItemToggler::create(
        favOnlyOffSprite,
        favOnlyOnSprite,
        this,
        menu_selector(FavoritesPopup::onFavoritesOnlyToggle)
    );
    m_impl->m_favoritesOnlyToggle->setID("show-favorites-button");
    m_impl->m_favoritesOnlyToggle->setPosition({ widthCS - 60.f, heightCS - 50.f });

    checkboxMenu->addChild(m_impl->m_favoritesOnlyToggle);

    auto hideFavOffSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    auto hideFavOnSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

    hideFavOffSprite->setScale(0.8f);
    hideFavOnSprite->setScale(0.8f);

    // Create hide favorites checkbox
    m_impl->m_hideFavoritesToggle = CCMenuItemToggler::create(
        hideFavOffSprite,
        hideFavOnSprite,
        this,
        menu_selector(FavoritesPopup::onHideFavoritesToggle)
    );
    m_impl->m_hideFavoritesToggle->setID("hide-favorites-button");
    m_impl->m_hideFavoritesToggle->setPosition({ widthCS - 25.f, heightCS - 50.f });

    checkboxMenu->addChild(m_impl->m_hideFavoritesToggle);

    auto const fOn = m_impl->m_heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png"; // enabled favorite icon
    auto const fOff = m_impl->m_heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png"; // disabled favorite icon

    // Create icon above favorites only toggle
    auto starOnIcon = CCSprite::createWithSpriteFrameName(fOn);
    starOnIcon->setID("show-favorites-icon");
    starOnIcon->setPosition({ m_impl->m_favoritesOnlyToggle->getPositionX(), m_impl->m_favoritesOnlyToggle->getPositionY() + 20.f });
    starOnIcon->setScale(m_impl->m_heartIcons ? 0.375f : 0.625f);

    // Create icon above hide favorites toggle
    auto starOffIcon = CCSprite::createWithSpriteFrameName(fOff);
    starOffIcon->setID("hide-favorites-icon");
    starOffIcon->setPosition({ m_impl->m_hideFavoritesToggle->getPositionX(), m_impl->m_hideFavoritesToggle->getPositionY() + 20.f });
    starOffIcon->setScale(m_impl->m_heartIcons ? 0.375f : 0.625f);

    m_mainLayer->addChild(starOnIcon);
    m_mainLayer->addChild(starOffIcon);

    m_impl->m_noModsLabel = CCLabelBMFont::create("No mods found! :(", "bigFont.fnt");
    m_impl->m_noModsLabel->setID("no-mods-label");
    m_impl->m_noModsLabel->setPosition({ widthCS / 2.f, heightCS / 2.f - 30.f });
    m_impl->m_noModsLabel->setScale(0.5f);
    m_impl->m_noModsLabel->setColor({ 125, 125, 125 });
    m_impl->m_noModsLabel->setVisible(false);

    m_mainLayer->addChild(m_impl->m_noModsLabel);

    // Create scroll size
    auto const scrollSize = CCSize{ widthCS - 17.5f, heightCS - 80.f };

    // bg for scroll layer
    auto scrollBG = CCScale9Sprite::create("square02b_001.png");
    scrollBG->setContentSize(scrollSize);
    scrollBG->setAnchorPoint({ 0.5, 0.5 });
    scrollBG->setPosition({ widthCS / 2.f, (heightCS / 2.f) - 30.f });
    scrollBG->setColor({ 0, 0, 0 });
    scrollBG->setOpacity(100);

    m_mainLayer->addChild(scrollBG);

    // Create layout for scroll layer
    auto scrollLayerLayout = ColumnLayout::create()
        ->setAxisAlignment(AxisAlignment::End) // seriously why is this end at top but start at bottom?
        ->setAxisReverse(true) // haha wtf is top reverse but bottom isnt LMAO
        ->setAutoGrowAxis(scrollSize.height - 12.5f)
        ->setGrowCrossAxis(false)
        ->setGap(5.f);

    // Create scroll layer
    m_impl->m_scrollLayer = ScrollLayer::create({ scrollSize.width - 10.f, scrollSize.height - 10.f });
    m_impl->m_scrollLayer->setID("mod-list");
    m_impl->m_scrollLayer->setAnchorPoint({ 0.5, 0.5 });
    m_impl->m_scrollLayer->setPosition({ 13.5f, 15.f }); // ???

    m_impl->m_scrollLayer->m_contentLayer->setLayout(scrollLayerLayout);

    m_mainLayer->addChild(m_impl->m_scrollLayer);

    m_impl->m_scrollLayer->m_contentLayer->updateLayout(true);
    m_impl->m_scrollLayer->scrollToTop();

    if (favMod->getSettingValue<bool>("settings-btn")) {
        // geode mod settings popup button
        auto modSettingsBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/settings.png",
            1.f,
            m_impl->m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
            CircleBaseSize::Medium
        );
        modSettingsBtnSprite->setScale(0.625f);

        auto modSettingsBtn = CCMenuItemSpriteExtra::create(
            modSettingsBtnSprite,
            this,
            menu_selector(FavoritesPopup::onModSettings)
        );
        modSettingsBtn->setID("options-button");
        modSettingsBtn->setPosition({ 2.5f, 2.5f });

        m_buttonMenu->addChild(modSettingsBtn);
    } else {
        log::debug("Mod settings button disabled");
    };

    // clear favs button sprite
    auto clearAllBtnSprite = ButtonSprite::create(
        "Clear",
        "bigFont.fnt",
        m_impl->m_geodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png",
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

    m_buttonMenu->addChild(clearAllBtn);

    // favorites stats button sprite
    auto statsBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
        "geode.loader/changelog.png",
        0.75f,
        m_impl->m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
        CircleBaseSize::Small
    );
    statsBtnSprite->setScale(0.5f);

    // button to view funny favorites stats
    auto statsBtn = CCMenuItemSpriteExtra::create(
        statsBtnSprite,
        this,
        menu_selector(FavoritesPopup::onGetStats)
    );
    statsBtn->setID("statistics-button");
    statsBtn->setPosition({ widthCS - 1.25f, 1.25f });
    statsBtn->setZOrder(3);

    m_buttonMenu->addChild(statsBtn);

    if (m_impl->m_usePages) { // if the player has pages enabled
        constexpr auto pageBtnSpriteName = "GJ_arrow_02_001.png"; // same for both buttons

        auto pageNextBtnSprite = CCSprite::createWithSpriteFrameName(pageBtnSpriteName);
        pageNextBtnSprite->setScale(0.875f);
        pageNextBtnSprite->setFlipX(true);

        // create go next page button
        m_impl->m_pageNextBtn = CCMenuItemSpriteExtra::create(
            pageNextBtnSprite,
            this,
            menu_selector(FavoritesPopup::onPageNext)
        );
        m_impl->m_pageNextBtn->setID("page-next-button");
        m_impl->m_pageNextBtn->setPosition({ widthCS + 17.5f , m_impl->m_scrollLayer->getPositionY() });
        m_impl->m_pageNextBtn->setVisible(true);

        auto pagePreviousBtnSprite = CCSprite::createWithSpriteFrameName(pageBtnSpriteName);
        pagePreviousBtnSprite->setScale(0.875f);
        pagePreviousBtnSprite->setFlipX(false);

        // create go previous page button
        m_impl->m_pagePreviousBtn = CCMenuItemSpriteExtra::create(
            pagePreviousBtnSprite,
            this,
            menu_selector(FavoritesPopup::onPagePrevious)
        );
        m_impl->m_pagePreviousBtn->setID("page-previous-button");
        m_impl->m_pagePreviousBtn->setPosition({ -17.5f , m_impl->m_scrollLayer->getPositionY() });
        m_impl->m_pagePreviousBtn->setVisible(false);

        m_buttonMenu->addChild(m_impl->m_pageNextBtn);
        m_buttonMenu->addChild(m_impl->m_pagePreviousBtn);

        // create pages label
        m_impl->m_pagesLabel = CCLabelBMFont::create("Loading pages...", "goldFont.fnt");
        m_impl->m_pagesLabel->setID("pages-label");
        m_impl->m_pagesLabel->setAnchorPoint({ 0, 1 });
        m_impl->m_pagesLabel->setPosition({ favMod->getSettingValue<bool>("settings-btn") ? 20.f : 0.f, -1.25f });
        m_impl->m_pagesLabel->setScale(0.375f);

        m_mainLayer->addChild(m_impl->m_pagesLabel);

        log::info("Page buttons created");
    } else {
        log::warn("Skipping page buttons");
    };

    // Create info button
    auto infoBtnSprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoBtnSprite->setScale(0.75f);

    auto infoBtn = CCMenuItemSpriteExtra::create(
        infoBtnSprite,
        this,
        menu_selector(FavoritesPopup::onInfoButton)
    );
    infoBtn->setID("info-button");
    infoBtn->setPosition({ widthCS - 15.f, heightCS - 15.f });

    m_buttonMenu->addChild(infoBtn);

    refreshModList(true);

    favMod->setSavedValue("already-loaded", true);

    return true;
};

void FavoritesPopup::loadModList(std::vector<Mod*> const& allMods) {
    for (auto const& mod : allMods) { // Add all mod items to scrolllayer
        m_impl->m_scrollLayer->m_contentLayer->addChild(ModItem::create(mod, { m_impl->m_scrollLayer->getScaledContentWidth(), 37.5f }, m_impl->m_geodeTheme, m_impl->m_heartIcons));
        log::debug("Processed list item for mod {}", mod->getID());
    };
};

ListenerResult FavoritesPopup::OnFavoritesChanged() {
    refreshModList(false);
    return ListenerResult::Propagate;
};

void FavoritesPopup::refreshModList(bool clearSearch) {
    // Clear and recreate scroll content
    m_impl->m_scrollLayer->m_contentLayer->removeAllChildrenWithCleanup(true);

    if (clearSearch && favMod->getSettingValue<bool>("refresh-clear-search")) m_impl->m_searchText = "";

    m_impl->m_searchInput->setString(m_impl->m_searchText, false);

    // Filtered mods based on search text and toggle settings
    std::vector<Mod*> filteredMods;
    auto enabledOnly = favMod->getSettingValue<bool>("enabled-only");

    for (auto const& mod : loader->getAllMods()) {
        auto const modID = mod->getID();
        bool isFavorited = favMod->getSavedValue<bool>(modID);

        // if the player only wants to show enabled mods or just everything
        bool list = enabledOnly ? mod->isOrWillBeEnabled() : true;
        if (m_impl->m_searchText.empty()) list = isFavorited || (modID != "geode.loader"); // if search text is empty, don't show geode loader mod unless its favorited

        if (list) {
            auto empty = std::string::npos; // dry code xd

            log::debug("{} {} a favorite", modID, isFavorited ? "is" : "is not");

            // evil bool >:3
            bool matchesSearch = m_impl->m_searchText.empty() // show all mods if search text is empty
                || str::toLower(mod->getName()).find(string::toLower(m_impl->m_searchText)) != empty // search via name
                || str::toLower(mod->getDescription().value_or(mod->getName())).find(string::toLower(m_impl->m_searchText)) != empty // search via description
                || str::toLower(mod->getID()).find(string::toLower(m_impl->m_searchText)) != empty; // search via id

            // If favorites-only is enabled, only show favorited mods
            if (m_impl->m_showFavoritesOnly) {
                if (isFavorited && matchesSearch) filteredMods.push_back(mod);
            } else if (m_impl->m_hideFavorites) { // If hide favorites is enabled, only show non-favorited mods
                if (!isFavorited && matchesSearch) filteredMods.push_back(mod);
            } else { // show all matching mods, with favorites prioritized
                if (matchesSearch) filteredMods.push_back(mod);
            };
        } else { // if geode or disabled skip it
            log::warn("Skipping listing mod {}", modID);
        };
    };

    // Sort if favorited or otherwise in alphabetical order
    std::sort(filteredMods.begin(), filteredMods.end(), [this](const Mod* a, const Mod* b) -> bool {
        auto aFav = favMod->getSavedValue<bool>(a->getID()); // Check if mod A is favorited
        auto bFav = favMod->getSavedValue<bool>(b->getID()); // Check if mod B is favorited

        if (aFav != bFav) return aFav > bFav; // Favorited mods first

        return str::toLower(a->getName()) < str::toLower(b->getName()); // Alphabetical order
              });

    if (m_impl->m_usePages) { // load every set amount of mods in separate pages
        m_impl->p_totalItems = static_cast<int>(filteredMods.size());
        m_impl->p_totalPages = static_cast<int>(std::ceil(static_cast<float>(m_impl->p_totalItems) / static_cast<float>(m_impl->p_itemsPerPage)));

        log::info("Loading page {} of {}", m_impl->p_page, m_impl->p_totalPages);

        if (m_impl->p_page >= m_impl->p_totalPages) { // if at the last page
            log::debug("Reached last page");

            m_impl->p_page = m_impl->p_totalPages;

            if (m_impl->m_pageNextBtn) m_impl->m_pageNextBtn->setVisible(false);
            if (m_impl->m_pagePreviousBtn) m_impl->m_pagePreviousBtn->setVisible(m_impl->p_totalPages > 1); // if there's more than 1 page, show this button
        } else if (m_impl->p_page <= 1) { // if at the first page
            log::debug("Reached first page");

            m_impl->p_page = 1;

            if (m_impl->m_pageNextBtn) m_impl->m_pageNextBtn->setVisible(m_impl->p_totalPages > 1); // if there's more than 1 page, show this button
            if (m_impl->m_pagePreviousBtn) m_impl->m_pagePreviousBtn->setVisible(false);
        } else { // if somewhere in the middle
            if (m_impl->m_pageNextBtn) m_impl->m_pageNextBtn->setVisible(true);
            if (m_impl->m_pagePreviousBtn) m_impl->m_pagePreviousBtn->setVisible(true);
        };

        int startIndex = (m_impl->p_page - 1) * m_impl->p_itemsPerPage; // show this mod first
        int endIndex = std::min(startIndex + m_impl->p_itemsPerPage, m_impl->p_totalItems); // show this mod last

        if (startIndex >= 0 && startIndex < m_impl->p_totalItems) { // make sure the mods exist
            loadModList(std::vector<Mod*>(filteredMods.begin() + startIndex, filteredMods.begin() + endIndex));

            if (m_impl->m_pagesLabel) { // make sure the page label exists
                auto pageText = fmt::format("Page {}/{}, Total {} Mods", m_impl->p_page, m_impl->p_totalPages, m_impl->p_totalItems);
                m_impl->m_pagesLabel->setCString(pageText.c_str());
            } else {
                log::error("Couldn't find page count label");
            };
        } else {
            log::error("Invalid start index for filtered mods vector: {}", startIndex);
        };
    } else { // or screw it just load 'em all!
        log::debug("Loading all mods");
        loadModList(filteredMods);
    };

    m_impl->m_scrollLayer->m_contentLayer->updateLayout(true);
    if (favMod->getSettingValue<bool>("auto-scroll")) m_impl->m_scrollLayer->scrollToTop();

    // Toggle "No mods found" message
    if (filteredMods.empty()) {
        m_impl->m_noModsLabel->setVisible(true);
        m_impl->m_scrollLayer->setVisible(false);
    } else {
        m_impl->m_noModsLabel->setVisible(false);
        m_impl->m_scrollLayer->setVisible(true);
    };
};

void FavoritesPopup::textChanged(CCTextInputNode* input) {
    m_impl->m_searchText = input->getString();
    refreshModList(false);
};

void FavoritesPopup::onClearSearch(CCObject*) {
    m_impl->m_searchText = "";
    m_impl->m_searchInput->setString(m_impl->m_searchText, true);

    log::debug("Cleared search box");
};

void FavoritesPopup::onModSettings(CCObject*) {
    log::debug("Opening mod settings popup");
    openSettingsPopup(favMod);
};

void FavoritesPopup::onFavoritesOnlyToggle(CCObject*) {
    m_impl->m_showFavoritesOnly = !m_impl->m_showFavoritesOnly;

    // If favorites only is turned on, turn off hide favorites
    if (m_impl->m_showFavoritesOnly && m_impl->m_hideFavorites) {
        m_impl->m_hideFavorites = false;
        m_impl->m_hideFavoritesToggle->toggle(false);
    } else {
        log::debug("Hide favorites mode already off");
    };

    refreshModList(true);
};

void FavoritesPopup::onHideFavoritesToggle(CCObject*) {
    m_impl->m_hideFavorites = !m_impl->m_hideFavorites;

    // If hide favorites is turned on, turn off favorites only
    if (m_impl->m_hideFavorites && m_impl->m_showFavoritesOnly) {
        m_impl->m_showFavoritesOnly = false;
        m_impl->m_favoritesOnlyToggle->toggle(false);
    } else {
        log::debug("Favorites only mode already off");
    };

    refreshModList(true);
};

void FavoritesPopup::onPageNext(CCObject*) {
    if (m_impl->p_page < m_impl->p_totalPages) m_impl->p_page++; // turn the page next
    if (m_impl->p_page >= m_impl->p_totalPages) m_impl->p_page = m_impl->p_totalPages; // if max pages just stay at 1

    refreshModList(true);
};

void FavoritesPopup::onPagePrevious(CCObject*) {
    if (m_impl->p_page > 1) m_impl->p_page--; // turn the page previous
    if (m_impl->p_page <= 1) m_impl->p_page = 1; // if max pages just stay at 1

    refreshModList(true);
};

void FavoritesPopup::onInfoButton(CCObject*) {
    // lol
    auto body = fmt::format(
        "To <cg>add a mod to your favorites</c>, search for it in the scrolling area and press the <cy>{} button</c> located to the right-hand side of the listed mod. You can also press it again to <cr>remove it</c> from your favorites.{}",
        m_impl->m_heartIcons ? "heart" : "star",
        m_impl->m_usePages ? fmt::format(
            "\n\nYou can also use the <cc>page buttons</c> to navigate through your mods if you have more than {} favorites.",
            m_impl->p_itemsPerPage
        ) : ""
    );

    if (auto alert = FLAlertLayer::create(
        "Help",
        body.c_str(),
        "OK"
    )) alert->show();
};

void FavoritesPopup::onPromptClearAll(CCObject*) {
    createQuickPopup(
        "Clear Favorites",
        "Are you sure you want to <cr>clear your favorites</c>?",
        "Cancel", "Yes",
        [this](auto, bool btn2) {
            if (btn2) onClearAll();
        }, true
    );
};

void FavoritesPopup::onClearAll() {
    // Turn off favorite from every mod
    for (auto const& mod : loader->getAllMods()) {
        auto const modID = mod->getID(); // get the mod id
        if (favMod->hasSavedValue(modID)) favMod->setSavedValue(modID, false); // prevent creating more saves
    };

    refreshModList(true);

    Notification::create("Cleared all favorites", NotificationIcon::Success, 2.5f)->show();
    log::info("Cleared all favorite mods");
};

void FavoritesPopup::onGetStats(CCObject*) {
    int favorite = 0; // favorited mods
    int enabled = 0; // all enabled mods
    int total = 0; // all installed mods

    for (auto const& mod : loader->getAllMods()) { // gotta count 'em all!
        total++;

        if (favMod->getSavedValue<bool>(mod->getID())) favorite++;
        if (mod->isEnabled()) enabled++;
    };

    auto const faveCount = fmt::format("<cy>{}</c> Favorites", favorite);
    auto const enableCount = fmt::format("<cg>{}</c> Enabled, <cr>{}</c> Disabled", enabled, total - enabled);
    auto const totalCount = fmt::format("<cj>{}</c> Installed", total);

    auto const body = fmt::format("{}\n\nYour Mods\n{}\n{}", faveCount, enableCount, totalCount);

    if (auto alert = FLAlertLayer::create(
        "Statistics",
        body.c_str(),
        "OK"
    )) alert->show();
};

FavoritesPopup* FavoritesPopup::create(
    bool geodeTheme,
    bool heartIcons
) {
    auto ret = new FavoritesPopup();
    if (ret->init(400.f, 280.f, geodeTheme, heartIcons)) {
        ret->autorelease();
        return ret;
    };

    CC_SAFE_DELETE(ret);
    return nullptr;
};