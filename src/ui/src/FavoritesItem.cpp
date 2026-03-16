#include "../FavoritesItem.hpp"

#include "../../Events.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/ColorProvider.hpp>

using namespace geode::prelude;

static auto favMod = Mod::get();  // Get this mod

class FavoritesItem::Impl final {
public:
    Mod* mod = nullptr;  // Fetched mod

    bool favorite = false;  // If the mod is favorite

    bool geodeTheme = false;  // Make sure visuals go with Geode theme
    bool heartIcons = false;  // Hearts UI mode

    CCMenuItemSpriteExtra* favButton = nullptr;  // Favorite button
};

FavoritesItem::FavoritesItem() : m_impl(std::make_unique<Impl>()) {};
FavoritesItem::~FavoritesItem() {};

bool FavoritesItem::init(
    Mod* mod,
    CCSize const& size,
    bool geodeTheme,
    bool heartIcons) {
    m_impl->mod = mod;
    m_impl->geodeTheme = geodeTheme;
    m_impl->heartIcons = heartIcons;

    if (!mod) return false;

    auto const modID = mod->getID();

    m_impl->favorite = favMod->getSavedValue<bool>(modID);

    if (!CCNode::init()) return false;

    auto const bgColor = ColorProvider::get()->color("geode.loader/mod-developer-item-bg");

    setID(modID);
    setAnchorPoint({0, 1});
    setContentSize(size);

    auto backgroundSprite = NineSlice::create("square02b_001.png");
    backgroundSprite->setID("background");
    backgroundSprite->setContentSize(size);
    backgroundSprite->setScaleMultiplier(0.5f);
    backgroundSprite->setPosition(getScaledContentSize() / 2.f);
    backgroundSprite->setColor(to3B(bgColor));
    backgroundSprite->setOpacity(bgColor.a);

    addChild(backgroundSprite);

    auto modIcon = createModLogo(m_impl->mod);
    modIcon->setID("mod-icon");
    modIcon->setScale(0.5f);
    modIcon->setPosition({20.f, size.height / 2.f});
    modIcon->setAnchorPoint({0.5, 0.5});

    addChild(modIcon);

    auto nameLabel = CCLabelBMFont::create(m_impl->mod->getName().c_str(), "bigFont.fnt");
    nameLabel->setID("mod-name");
    nameLabel->setScale(0.4f);
    nameLabel->setPosition({37.5f, (size.height / 2.f) + 5.f});
    nameLabel->setAlignment(kCCTextAlignmentLeft);
    nameLabel->setAnchorPoint({0, 0.5});

    addChild(nameLabel);

    auto viewBtnSprite = ButtonSprite::create(
        "View",
        "bigFont.fnt",
        // @geode-ignore(unknown-resource)
        m_impl->geodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png",
        0.75f);
    viewBtnSprite->setScale(0.75f);

    auto viewBtn = CCMenuItemExt::createSpriteExtra(
        viewBtnSprite,
        [this](auto) {
            if (m_impl->mod) openInfoPopup(m_impl->mod);
        });
    viewBtn->setID("view-btn");

    auto const fOn = m_impl->heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";         // enabled favorite icon
    auto const fOff = m_impl->heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";  // disabled favorite icon

    auto favBtnSprite = CCSprite::createWithSpriteFrameName(m_impl->favorite ? fOn : fOff);
    favBtnSprite->setScale(m_impl->heartIcons ? 0.75f : 1.f);

    // fav button here :)
    auto favBtn = CCMenuItemExt::createSpriteExtra(
        favBtnSprite,
        [this](CCMenuItem* sender) {
            m_impl->favorite = !m_impl->favorite;

            favMod->setSavedValue(m_impl->mod->getID(), m_impl->favorite);
            log::trace("Setting {} to {}", m_impl->mod->getID(), favMod->getSavedValue<bool>(m_impl->mod->getID(), m_impl->favorite) ? "favorite" : "non-favorite");

            // toggle icon manually cuz cctogglerwhatever sucks balls
            if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender)) {
                auto const fOn = m_impl->heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";
                auto const fOff = m_impl->heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";

                auto newSprite = CCSprite::createWithSpriteFrameName(m_impl->favorite ? fOn : fOff);
                newSprite->setScale(m_impl->heartIcons ? 0.75f : 1.f);

                btn->setNormalImage(newSprite);
                btn->updateSprite();

                log::debug("Updated state for {} to {}", m_impl->mod->getID(), m_impl->favorite ? "favorite" : "non-favorite");
            } else {
                log::error("Favorite button not found for {}", m_impl->mod->getID());
            };

            FavoriteEvent().send();
        });
    favBtn->setID("favorite-btn");

    auto btnMenuLayout = RowLayout::create()
                             ->setDefaultScaleLimits(0.625f, 0.875f)
                             ->setAxisAlignment(AxisAlignment::End)
                             ->setCrossAxisAlignment(AxisAlignment::Center)
                             ->setCrossAxisLineAlignment(AxisAlignment::Center)
                             ->setGrowCrossAxis(false)
                             ->setAutoGrowAxis(0.f)
                             ->setAxisReverse(true)
                             ->setGap(5.f);

    auto btnMenu = CCMenu::create();
    btnMenu->setID("button-menu");
    btnMenu->setAnchorPoint({1, 0.5});
    btnMenu->setPosition({size.width - 10.f, size.height / 2.f});
    btnMenu->setContentSize({size.width * 0.375f, size.height - 10.f});
    btnMenu->setLayout(btnMenuLayout);

    btnMenu->addChild(viewBtn);
    btnMenu->addChild(favBtn);

    addChild(btnMenu);

    // offset to position the id's x by if showing label version
    auto idLabelOffset = 0.f;

    // Avoid showing more details if minimalist setting is on
    if (!favMod->getSettingValue<bool>("minimal")) {
        auto const devs = m_impl->mod->getDevelopers();
        auto const andMore = static_cast<int>(devs.size()) - 1;

        auto devLabelText = devs[0];
        if (andMore > 0) devLabelText = fmt::format("{} & {} more", devLabelText, andMore);

        auto devLabel = CCLabelBMFont::create(devLabelText.c_str(), "goldFont.fnt");
        devLabel->setID("mod-developers");
        devLabel->setPosition({nameLabel->getScaledContentWidth() + 40.f, nameLabel->getPositionY()});
        devLabel->setScale(0.25f);
        devLabel->setAnchorPoint({0, 0.5});
        devLabel->setAlignment(kCCTextAlignmentLeft);

        addChild(devLabel);

        auto versionLabel = CCLabelBMFont::create(m_impl->mod->getVersion().toVString().c_str(), "goldFont.fnt");
        versionLabel->setID("mod-version");
        versionLabel->setPosition({37.5f, (size.height / 2.f) - 5.f});
        versionLabel->setScale(0.3f);
        versionLabel->setAnchorPoint({0, 0.5});
        versionLabel->setAlignment(kCCTextAlignmentLeft);

        idLabelOffset += 2.5f + versionLabel->getScaledContentWidth();

        addChild(versionLabel);

        auto descBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
            // @geode-ignore(unknown-resource)
            "geode.loader/message.png",
            0.875f,
            m_impl->geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
            CircleBaseSize::Small);
        descBtnSprite->setScale(0.375f);

        // Mod description button
        auto descBtn = CCMenuItemExt::createSpriteExtra(
            descBtnSprite,
            [this](auto) {
                if (auto alert = FLAlertLayer::create(
                        m_impl->mod->getName().c_str(),
                        m_impl->mod->getDescription().value_or("<cr>No description available.</c>"),
                        "OK")) alert->show();
            });
        descBtn->setID("short-description-btn");

        btnMenu->addChild(descBtn);
    };

    // ID label
    auto idLabel = CCLabelBMFont::create(modID.c_str(), "bigFont.fnt");
    idLabel->setID("mod-id");
    idLabel->setPosition({37.5f + idLabelOffset, (size.height / 2.f) - 5.f});
    idLabel->setScale(0.25f);
    idLabel->setAnchorPoint({0, 0.5});
    idLabel->setOpacity(125);
    idLabel->setAlignment(kCCTextAlignmentLeft);

    addChild(idLabel);

    btnMenu->updateLayout();

    // for first time users of favorites
    if (auto helpTxt = firstTimeText()) {
        helpTxt->setPosition({size.width - (btnMenu->getScaledContentWidth() + 2.5f), size.height / 2.f});
        addChild(helpTxt);

        log::debug("Help text added for first time users");
    };

    auto const loadProblem = m_impl->mod->targetsOutdatedVersion();
    if (loadProblem.has_value()) {
        if (loadProblem->type == LoadProblem::Type::Outdated && favMod->getSettingValue<bool>("indicate-outdated")) {
            auto const gdVer = m_impl->mod->getMetadata().getGameVersion();
            auto const reason = fmt::format("Outdated ({})", gdVer.value_or("Unknown"));

            auto modOutdated = CCLabelBMFont::create(reason.c_str(), "bigFont.fnt");
            modOutdated->setID("outdated-indicator");
            modOutdated->setScale(0.2f);
            modOutdated->setOpacity(200);
            modOutdated->setAnchorPoint({0, 0.5});
            modOutdated->setAlignment(kCCTextAlignmentLeft);
            modOutdated->setPosition({37.5f, (size.height / 2.f) - 12.5f});
            modOutdated->setColor(ColorProvider::get()->color3b("geode.loader/mod-list-outdated-label"));

            nameLabel->setOpacity(200);

            log::info("Mod {} is outdated, uses game version {}", m_impl->mod->getID(), gdVer);
            addChild(modOutdated);
        };
    } else if (!m_impl->mod->isLoaded() && favMod->getSettingValue<bool>("indicate-disabled")) {  // check if the user wants to show the mod is disabled
        auto modDisabled = CCLabelBMFont::create("Disabled", "bigFont.fnt");
        modDisabled->setID("disabled-indicator");
        modDisabled->setScale(0.2f);
        modDisabled->setOpacity(200);
        modDisabled->setAnchorPoint({0, 0.5});
        modDisabled->setAlignment(kCCTextAlignmentLeft);
        modDisabled->setPosition({37.5f, (size.height / 2.f) - 12.5f});
        modDisabled->setColor({255, 65, 65});

        nameLabel->setOpacity(200);

        log::info("Mod {} is disabled", m_impl->mod->getID());
        addChild(modDisabled);
    };

    return true;
};

CCLabelBMFont* FavoritesItem::firstTimeText() {
    // check if mod loaded before
    if (favMod->getSavedValue<bool>("already-loaded", false) || favMod->getSavedValue<bool>(GEODE_MOD_ID) || !favMod->getSettingValue<bool>("minimal")) {
        return nullptr;
    } else if (m_impl->mod->getID() == GEODE_MOD_ID) {  // create the help text if loaded for the first time
        log::info("Mod loaded for the first time!");

        // Help text for first-time users
        auto help = CCLabelBMFont::create("Press to Toggle ->", "chatFont.fnt");
        help->setID("first-time-help-text");
        help->setScale(0.5f);
        help->setAnchorPoint({1, 0.5});
        help->setColor({200, 200, 200});
        help->setOpacity(200);

        return help;
    };

    return nullptr;
};

FavoritesItem* FavoritesItem::create(
    Mod* mod,
    CCSize const& size,
    bool geodeTheme,
    bool heartIcons) {
    auto ret = new FavoritesItem();
    if (ret->init(mod, size, geodeTheme, heartIcons)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};