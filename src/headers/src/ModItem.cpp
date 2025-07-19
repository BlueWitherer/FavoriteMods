#include "../ModItem.hpp"

#include "../FavoritesPopup.hpp"

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/ColorProvider.hpp>

using namespace geode::prelude;

bool ModItem::init(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme, bool heartIcons) {
    m_mod = mod;
    m_parentPopup = parentPopup;
    m_geodeTheme = geodeTheme;
    m_heartIcons = heartIcons;

    m_favorite = m_thisMod->getSavedValue<bool>(m_mod->getID(), false);

    if (CCNode::init()) {
        // node background theme color
        ccColor4B bgColor = ColorProvider::get()->color("geode.loader/mod-developer-item-bg");

        setID(m_mod->getID());
        setAnchorPoint({ 0, 1 });
        setContentSize(size);

        // Background for mod item
        m_backgroundSprite = CCScale9Sprite::create("square02b_001.png");
        m_backgroundSprite->setID("background");
        m_backgroundSprite->setScale(0.5f);
        m_backgroundSprite->setContentSize({ size.width * 2.f, size.height * 2.f });
        m_backgroundSprite->setAnchorPoint({ 0.5, 0.5 });
        m_backgroundSprite->ignoreAnchorPointForPosition(false);
        m_backgroundSprite->setPosition({ getScaledContentWidth() / 2.f, getScaledContentHeight() / 2.f });
        m_backgroundSprite->setColor(to3B(bgColor));
        m_backgroundSprite->setOpacity(bgColor.a);

        addChild(m_backgroundSprite);

        // Mod icon sprite
        auto modIcon = geode::createModLogo(m_mod->getPackagePath());
        modIcon->setID("mod-icon");
        modIcon->setScale(0.5f);
        modIcon->setPosition({ 20.f, getScaledContentHeight() / 2.f });
        modIcon->ignoreAnchorPointForPosition(false);
        modIcon->setAnchorPoint({ 0.5, 0.5 });

        addChild(modIcon);

        // Mod name label
        auto nameLabel = CCLabelBMFont::create(m_mod->getName().c_str(), "bigFont.fnt");
        nameLabel->setID("mod-name");
        nameLabel->setPosition({ 37.5f, 25.f });
        nameLabel->setScale(0.4f);
        nameLabel->setAnchorPoint({ 0, 0.5 });
        nameLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

        addChild(nameLabel);

        // View button
        auto viewBtnSprite = ButtonSprite::create(
            "View",
            "bigFont.fnt",
            m_geodeTheme ? "geode.loader/GE_button_05.png" : "GJ_button_01.png",
            0.75f
        );
        viewBtnSprite->setScale(0.75f);

        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewBtnSprite,
            this,
            menu_selector(ModItem::onViewMod)
        );
        viewBtn->setID("view-button");

        auto on = m_heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";
        auto off = m_heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";

        auto favBtnSprite = CCSprite::createWithSpriteFrameName(m_favorite ? on : off);
        favBtnSprite->setScale(m_heartIcons ? 0.75f : 1.f);

        // Favorite button here :)
        m_favButton = CCMenuItemSpriteExtra::create(
            favBtnSprite,
            this,
            menu_selector(ModItem::onFavorite)
        );
        m_favButton->setID("favorite");

        // Layout to automatically position buttons on button menu
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
        btnMenu->setPosition({ getScaledContentWidth() - 10.f, getScaledContentHeight() / 2.f });
        btnMenu->setScaledContentSize({ getScaledContentWidth() * 0.375f, getScaledContentHeight() - 10.f });
        btnMenu->setLayout(btnMenuLayout);

        // add the previous buttons
        btnMenu->addChild(viewBtn);
        btnMenu->addChild(m_favButton);

        addChild(btnMenu);

        // offset to position the id's x by if showing label version
        auto idLabelOffset = 0.f;

        // Avoid showing more details if minimalist setting is on
        if (m_thisMod->getSettingValue<bool>("minimal")) {
            idLabelOffset = 0.f; // make sure its 0 lul
        } else {
            auto devs = m_mod->getDevelopers();
            auto devLabelText = devs[0];
            int andMore = as<int>(devs.size()) - 1;

            if (andMore > 0) devLabelText += " & " + std::to_string(andMore) + " more";

            // Developers label
            auto devLabel = CCLabelBMFont::create(devLabelText.c_str(), "goldFont.fnt");
            devLabel->setID("mod-developers");
            devLabel->setPosition({ nameLabel->getScaledContentWidth() + 40.f, 25.f });
            devLabel->setScale(0.25f);
            devLabel->setAnchorPoint({ 0, 0.5 });
            devLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

            addChild(devLabel);

            // Version label
            auto versionLabel = CCLabelBMFont::create(m_mod->getVersion().toVString().c_str(), "goldFont.fnt");
            versionLabel->setID("mod-version");
            versionLabel->setPosition({ 37.5f, 15.f });
            versionLabel->setScale(0.3f);
            versionLabel->setAnchorPoint({ 0, 0.5 });
            versionLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

            idLabelOffset += 2.5f + versionLabel->getScaledContentWidth();

            addChild(versionLabel);

            // sprite for description button
            auto descBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                "geode.loader/message.png",
                0.875f,
                m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Medium
            );
            descBtnSprite->setScale(0.375f);

            // Mod description button
            auto descBtn = CCMenuItemSpriteExtra::create(
                descBtnSprite,
                this,
                menu_selector(ModItem::onModDesc)
            );
            descBtn->setID("mod-description-button");

            btnMenu->addChild(descBtn);

            auto issueBtnSprite = CCSprite::createWithSpriteFrameName("geode.loader/info-warning.png");
            issueBtnSprite->setScale(0.5f);

            auto issueBtn = CCMenuItemSpriteExtra::create(
                issueBtnSprite,
                this,
                menu_selector(ModItem::onModIssues)
            );
            issueBtn->setID("mod-issues-button");

            btnMenu->addChild(issueBtn);
        };

        // ID label
        auto idLabel = CCLabelBMFont::create(m_mod->getID().c_str(), "bigFont.fnt");
        idLabel->setID("mod-id");
        idLabel->setPosition({ 37.5f + idLabelOffset, 15.f });
        idLabel->setScale(0.25f);
        idLabel->setAnchorPoint({ 0, 0.5 });
        idLabel->setOpacity(125);
        idLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

        addChild(idLabel);

        // for first time users of favorites
        if (auto helpTxt = firstTimeText()) {
            helpTxt->setPosition({ getScaledContentWidth() - (btnMenu->getScaledContentWidth() + 2.5f), getScaledContentHeight() / 2.f });
            addChild(helpTxt);

            log::debug("Help text added for first time users");
        };

        btnMenu->updateLayout(true);

        return true;
    } else {
        return false;
    };
};

void ModItem::onViewMod(CCObject*) {
    openInfoPopup(m_mod);
};

void ModItem::onModDesc(CCObject*) {
    FLAlertLayer::create(
        m_mod->getName().c_str(),
        m_mod->getDescription().value_or("<cr>No description available.</c>").c_str(),
        "OK"
    )->show();
};

void ModItem::onModIssues(CCObject*) {
    openIssueReportPopup(m_mod);
};

void ModItem::onFavorite(CCObject*) {
    // Toggle favorite status
    m_favorite = !m_favorite;

    // Save the favorite status
    m_thisMod->setSavedValue<bool>(m_mod->getID(), m_favorite);

    // Update the icon
    updateFavoriteIcon();

    // Notify parent popup to refresh
    if (m_parentPopup) m_parentPopup->onModFavoriteChanged();
};

void ModItem::updateFavoriteIcon() {
    if (m_favButton) { // Make sure the favorite button has already been created
        auto on = m_heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";
        auto off = m_heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";

        auto newSprite = CCSprite::createWithSpriteFrameName(m_favorite ? on : off);
        newSprite->setScale(m_heartIcons ? 0.625f : 0.875f);

        m_favButton->setNormalImage(newSprite);
        log::info("Updated state for {} to {}", m_mod->getID(), m_favorite ? "favorite" : "non-favorite");
    } else {
        log::error("Favorite button not found for {}", m_mod->getID());
    };
};

CCLabelBMFont* ModItem::firstTimeText() {
    // check if mod loaded before
    if (m_thisMod->getSavedValue<bool>("already-loaded", false)
        || !m_thisMod->getSavedValue<bool>(m_thisMod->getID(), false)
        || !m_thisMod->getSettingValue<bool>("minimal")) {
        return nullptr;
    } else if (m_mod->getID().compare(m_thisMod->getID()) == 0) { // create the help text if loaded for the first time
        log::info("Mod loaded for the first time, creating help text...");

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

ModItem* ModItem::create(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme, bool heartIcons) {
    auto ret = new ModItem();

    if (!ret || !ret->init(mod, size, parentPopup, geodeTheme, heartIcons)) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    };

    ret->autorelease();
    return ret;
};