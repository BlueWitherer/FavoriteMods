#include "../FavoritesItem.hpp"

#include "../../Events.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/ColorProvider.hpp>

using namespace geode::prelude;

static auto favMod = Mod::get(); // Get this mod

class FavoritesItem::Impl final {
public:
    Mod* mod = nullptr; // Fetched mod

    bool favorite = false; // If the mod is favorite

    bool geodeTheme = false; // Make sure visuals go with Geode theme
    bool heartIcons = false; // Hearts UI mode

    CCMenuItemSpriteExtra* favButton = nullptr; // Favorite button

    CCScale9Sprite* backgroundSprite = nullptr; // Background theme
};

FavoritesItem::FavoritesItem() {
    m_impl = std::make_unique<Impl>();
};

FavoritesItem::~FavoritesItem() {};

bool FavoritesItem::init(
    Mod* mod,
    CCSize const& size,
    bool geodeTheme,
    bool heartIcons
) {
    m_impl->mod = mod;
    m_impl->geodeTheme = geodeTheme;
    m_impl->heartIcons = heartIcons;

    if (!mod) {
        log::error("FavoritesItem init function called with null mod");
        return false;
    };

    auto const modID = mod->getID();

    m_impl->favorite = favMod->getSavedValue<bool>(modID);

    if (!CCNode::init()) return false;

    auto const bgColor = ColorProvider::get()->color("geode.loader/mod-developer-item-bg");

    setID(modID);
    setAnchorPoint({ 0, 1 });
    setContentSize(size);

    // Background for mod item
    m_impl->backgroundSprite = CCScale9Sprite::create("square02b_001.png");
    m_impl->backgroundSprite->setID("background");
    m_impl->backgroundSprite->setScale(0.5f);
    m_impl->backgroundSprite->setContentSize({ size.width * 2.f, size.height * 2.f });
    m_impl->backgroundSprite->setAnchorPoint({ 0.5, 0.5 });
    m_impl->backgroundSprite->setPosition({ getScaledContentWidth() / 2.f, getScaledContentHeight() / 2.f });
    m_impl->backgroundSprite->setColor(to3B(bgColor));
    m_impl->backgroundSprite->setOpacity(bgColor.a);

    // Create main content area
    auto const [widthCS, heightCS] = getScaledContentSize();

    addChild(m_impl->backgroundSprite);

    // Mod icon sprite
    auto modIcon = createModLogo(m_impl->mod);
    modIcon->setID("mod-icon");
    modIcon->setScale(0.5f);
    modIcon->setPosition({ 20.f, heightCS / 2.f });
    modIcon->setAnchorPoint({ 0.5, 0.5 });

    addChild(modIcon);

    // Mod name label
    auto nameLabel = CCLabelBMFont::create(m_impl->mod->getName().c_str(), "bigFont.fnt");
    nameLabel->setID("mod-name");
    nameLabel->setScale(0.4f);
    nameLabel->setPosition({ 37.5f, (heightCS / 2.f) + 5.f });
    nameLabel->setAlignment(kCCTextAlignmentLeft);
    nameLabel->setAnchorPoint({ 0, 0.5 });

    addChild(nameLabel);

    // View button
    auto viewBtnSprite = ButtonSprite::create(
        "View",
        "bigFont.fnt",
        m_impl->geodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png",
        0.75f
    );
    viewBtnSprite->setScale(0.75f);

    auto viewBtn = CCMenuItemSpriteExtra::create(
        viewBtnSprite,
        this,
        menu_selector(FavoritesItem::onViewMod)
    );
    viewBtn->setID("view-btn");

    auto const fOn = m_impl->heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png"; // enabled favorite icon
    auto const fOff = m_impl->heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png"; // disabled favorite icon

    auto favBtnSprite = CCSprite::createWithSpriteFrameName(m_impl->favorite ? fOn : fOff);
    favBtnSprite->setScale(m_impl->heartIcons ? 0.75f : 1.f);

    // Favorite button here :)
    m_impl->favButton = CCMenuItemSpriteExtra::create(
        favBtnSprite,
        this,
        menu_selector(FavoritesItem::onFavorite)
    );
    m_impl->favButton->setID("favorite-btn");

    // Layout to automatically position buttons on right-side button menu
    auto btnMenuLayout = RowLayout::create()
        ->setDefaultScaleLimits(0.625f, 0.875f)
        ->setAxisAlignment(AxisAlignment::End)
        ->setCrossAxisAlignment(AxisAlignment::Center)
        ->setCrossAxisLineAlignment(AxisAlignment::Center)
        ->setGrowCrossAxis(false)
        ->setAutoGrowAxis(0.f)
        ->setAxisReverse(true)
        ->setGap(5.f);

    // Create menu for buttons
    auto btnMenu = CCMenu::create();
    btnMenu->setID("button-menu");
    btnMenu->setAnchorPoint({ 1, 0.5 });
    btnMenu->setPosition({ widthCS - 10.f, heightCS / 2.f });
    btnMenu->setScaledContentSize({ widthCS * 0.375f, heightCS - 10.f });
    btnMenu->setLayout(btnMenuLayout);

    // add the previous buttons
    btnMenu->addChild(viewBtn);
    btnMenu->addChild(m_impl->favButton);

    addChild(btnMenu);

    // offset to position the id's x by if showing label version
    auto idLabelOffset = 0.f;

    // Avoid showing more details if minimalist setting is on
    if (!favMod->getSettingValue<bool>("minimal")) {
        auto const devs = m_impl->mod->getDevelopers();
        auto const andMore = static_cast<int>(devs.size()) - 1;

        auto devLabelText = devs[0];
        if (andMore > 0) devLabelText = fmt::format("{} & {} more", devLabelText, andMore);

        // Developers label
        auto devLabel = CCLabelBMFont::create(devLabelText.c_str(), "goldFont.fnt");
        devLabel->setID("mod-developers");
        devLabel->setPosition({ nameLabel->getScaledContentWidth() + 40.f, nameLabel->getPositionY() });
        devLabel->setScale(0.25f);
        devLabel->setAnchorPoint({ 0, 0.5 });
        devLabel->setAlignment(kCCTextAlignmentLeft);

        addChild(devLabel);

        // Version label
        auto versionLabel = CCLabelBMFont::create(m_impl->mod->getVersion().toVString().c_str(), "goldFont.fnt");
        versionLabel->setID("mod-version");
        versionLabel->setPosition({ 37.5f, (heightCS / 2.f) - 5.f });
        versionLabel->setScale(0.3f);
        versionLabel->setAnchorPoint({ 0, 0.5 });
        versionLabel->setAlignment(kCCTextAlignmentLeft);

        idLabelOffset += 2.5f + versionLabel->getScaledContentWidth();

        addChild(versionLabel);

        // sprite for description button
        auto descBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/message.png",
            0.875f,
            m_impl->geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
            CircleBaseSize::Small
        );
        descBtnSprite->setScale(0.375f);

        // Mod description button
        auto descBtn = CCMenuItemSpriteExtra::create(
            descBtnSprite,
            this,
            menu_selector(FavoritesItem::onModDesc)
        );
        descBtn->setID("short-description-btn");

        btnMenu->addChild(descBtn);
    };

    // ID label
    auto idLabel = CCLabelBMFont::create(modID.c_str(), "bigFont.fnt");
    idLabel->setID("mod-id");
    idLabel->setPosition({ 37.5f + idLabelOffset, (heightCS / 2.f) - 5.f });
    idLabel->setScale(0.25f);
    idLabel->setAnchorPoint({ 0, 0.5 });
    idLabel->setOpacity(125);
    idLabel->setAlignment(kCCTextAlignmentLeft);

    addChild(idLabel);

    btnMenu->updateLayout();

    // for first time users of favorites
    if (auto helpTxt = firstTimeText()) {
        helpTxt->setPosition({ widthCS - (btnMenu->getScaledContentWidth() + 2.5f), heightCS / 2.f });
        addChild(helpTxt);

        log::debug("Help text added for first time users");
    } else {
        btnMenu->updateLayout();
    };

    auto const loadProblem = m_impl->mod->targetsOutdatedVersion();
    if (loadProblem.has_value()) {
        if (loadProblem->isOutdated() && favMod->getSettingValue<bool>("indicate-outdated")) {
            auto const gdVer = m_impl->mod->getMetadata().getGameVersion();
            auto const reason = fmt::format("Outdated ({})", gdVer.value_or("Unknown"));

            auto modOutdated = CCLabelBMFont::create(reason.c_str(), "bigFont.fnt");
            modOutdated->setID("outdated-indicator");
            modOutdated->setScale(0.2f);
            modOutdated->setOpacity(200);
            modOutdated->setAnchorPoint({ 0, 0.5 });
            modOutdated->setAlignment(kCCTextAlignmentLeft);
            modOutdated->setPosition({ 37.5f, (heightCS / 2.f) - 12.5f });
            modOutdated->setColor(ColorProvider::get()->color3b("geode.loader/mod-list-outdated-label"));

            nameLabel->setOpacity(200);

            log::info("Mod {} is outdated, uses game version {}", m_impl->mod->getID(), gdVer);
            addChild(modOutdated);
        };
    } else if (!m_impl->mod->isEnabled() && favMod->getSettingValue<bool>("indicate-disabled")) { // check if the user wants to show the mod is disabled
        auto modDisabled = CCLabelBMFont::create("Disabled", "bigFont.fnt");
        modDisabled->setID("disabled-indicator");
        modDisabled->setScale(0.2f);
        modDisabled->setOpacity(200);
        modDisabled->setAnchorPoint({ 0, 0.5 });
        modDisabled->setAlignment(kCCTextAlignmentLeft);
        modDisabled->setPosition({ 37.5f, (heightCS / 2.f) - 12.5f });
        modDisabled->setColor({ 255, 65, 65 });

        nameLabel->setOpacity(200);

        log::info("Mod {} is disabled", m_impl->mod->getID());
        addChild(modDisabled);
    };

    return true;
};

void FavoritesItem::onViewMod(CCObject*) {
    if (m_impl->mod) openInfoPopup(m_impl->mod);
};

void FavoritesItem::onModDesc(CCObject*) {
    if (!m_impl->mod) return;

    if (auto alert = FLAlertLayer::create(
        m_impl->mod->getName().c_str(),
        m_impl->mod->getDescription().value_or("<cr>No description available.</c>"),
        "OK"
    )) alert->show();
};

void FavoritesItem::onFavorite(CCObject*) {
    if (!m_impl->mod) return;

    // Toggle favorite status
    m_impl->favorite = !m_impl->favorite;

    // Save the favorite status
    favMod->setSavedValue(m_impl->mod->getID(), m_impl->favorite);
    log::debug("Setting {} to {}", m_impl->mod->getID(), favMod->getSavedValue<bool>(m_impl->mod->getID(), m_impl->favorite) ? "favorite" : "non-favorite");

    // Update the icon
    updateFavoriteIcon();

    // Send event to refresh list
    FavoriteEvent().send();
};

void FavoritesItem::updateFavoriteIcon() {
    if (!m_impl->mod) return;

    if (m_impl->favButton) { // Make sure the favorite button has already been created
        auto const fOn = m_impl->heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";
        auto const fOff = m_impl->heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";

        auto newSprite = CCSprite::createWithSpriteFrameName(m_impl->favorite ? fOn : fOff);
        newSprite->setScale(m_impl->heartIcons ? 0.75f : 1.f);

        m_impl->favButton->setNormalImage(newSprite);
        m_impl->favButton->updateSprite();

        log::info("Updated state for {} to {}", m_impl->mod->getID(), m_impl->favorite ? "favorite" : "non-favorite");
    } else {
        log::error("Favorite button not found for {}", m_impl->mod->getID());
    };
};

CCLabelBMFont* FavoritesItem::firstTimeText() {
    if (!m_impl->mod) return nullptr;

    // check if mod loaded before
    if (favMod->getSavedValue<bool>("already-loaded", false)
        || favMod->getSavedValue<bool>(GEODE_MOD_ID)
        || !favMod->getSettingValue<bool>("minimal")) {
        return nullptr;
    } else if (m_impl->mod->getID() == GEODE_MOD_ID) { // create the help text if loaded for the first time
        log::info("Mod loaded for the first time!");

        // Help text for first-time users
        auto help = CCLabelBMFont::create("Press to Toggle ->", "chatFont.fnt");
        help->setID("first-time-help-text");
        help->setScale(0.5f);
        help->setAnchorPoint({ 1, 0.5 });
        help->setColor({ 200, 200, 200 });
        help->setOpacity(200);

        return help;
    };

    return nullptr;
};

FavoritesItem* FavoritesItem::create(
    Mod* mod,
    CCSize const& size,
    bool geodeTheme,
    bool heartIcons
) {
    auto ret = new FavoritesItem();
    if (ret->init(mod, size, geodeTheme, heartIcons)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};