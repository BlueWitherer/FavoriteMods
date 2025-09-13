#include "../ModItem.hpp"

#include "../FavoritesPopup.hpp"

#include "../../Events.hpp"

#include <fmt/core.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/ColorProvider.hpp>

using namespace geode::prelude;

static auto favMod = Mod::get(); // Get this mod

bool ModItem::init(
    Mod* mod,
    CCSize const& size,
    bool geodeTheme,
    bool heartIcons
) {
    m_mod = mod;
    m_geodeTheme = geodeTheme;
    m_heartIcons = heartIcons;

    auto modID = mod->getID();

    m_favorite = favMod->getSavedValue<bool>(modID);

    if (CCNode::init()) {
        auto colProvider = ColorProvider::get();

        // node background theme color
        auto bgColor = colProvider->color("geode.loader/mod-developer-item-bg");

        setID(modID);
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

        // Create main content area
        auto [widthCS, heightCS] = getScaledContentSize();

        addChild(m_backgroundSprite);

        // Mod icon sprite
        auto modIcon = createModLogo(m_mod);
        modIcon->setID("mod-icon");
        modIcon->setScale(0.5f);
        modIcon->setPosition({ 20.f, heightCS / 2.f });
        modIcon->ignoreAnchorPointForPosition(false);
        modIcon->setAnchorPoint({ 0.5, 0.5 });

        addChild(modIcon);

        // Mod name label
        auto nameLabel = CCLabelBMFont::create(m_mod->getName().c_str(), "bigFont.fnt");
        nameLabel->setID("mod-name");
        nameLabel->setScale(0.4f);
        nameLabel->setPosition({ 37.5f, (heightCS / 2.f) + 5.f });
        nameLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
        nameLabel->setAnchorPoint({ 0, 0.5 });

        addChild(nameLabel);

        // View button
        auto viewBtnSprite = ButtonSprite::create(
            "View",
            "bigFont.fnt",
            // @geode-ignore(unknown-resource)
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

        auto fOn = m_heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png"; // enabled favorite icon
        auto fOff = m_heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png"; // disabled favorite icon

        auto favBtnSprite = CCSprite::createWithSpriteFrameName(m_favorite ? fOn : fOff);
        favBtnSprite->setScale(m_heartIcons ? 0.75f : 1.f);

        // Favorite button here :)
        m_favButton = CCMenuItemSpriteExtra::create(
            favBtnSprite,
            this,
            menu_selector(ModItem::onFavorite)
        );
        m_favButton->setID("favorite-button");

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
        btnMenu->addChild(m_favButton);

        addChild(btnMenu);

        // offset to position the id's x by if showing label version
        auto idLabelOffset = 0.f;

        // Avoid showing more details if minimalist setting is on
        if (favMod->getSettingValue<bool>("minimal")) {
            idLabelOffset = 0.f; // make sure its 0 lul
        } else {
            auto devs = m_mod->getDevelopers();
            auto devLabelText = devs[0];
            auto andMore = static_cast<int>(devs.size()) - 1;

            if (andMore > 0) devLabelText += " & " + std::to_string(andMore) + " more";

            // Developers label
            auto devLabel = CCLabelBMFont::create(devLabelText.c_str(), "goldFont.fnt");
            devLabel->setID("mod-developers");
            devLabel->setPosition({ nameLabel->getScaledContentWidth() + 40.f, nameLabel->getPositionY() });
            devLabel->setScale(0.25f);
            devLabel->setAnchorPoint({ 0, 0.5 });
            devLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

            addChild(devLabel);

            // Version label
            auto versionLabel = CCLabelBMFont::create(m_mod->getVersion().toVString().c_str(), "goldFont.fnt");
            versionLabel->setID("mod-version");
            versionLabel->setPosition({ 37.5f, (heightCS / 2.f) - 5.f });
            versionLabel->setScale(0.3f);
            versionLabel->setAnchorPoint({ 0, 0.5 });
            versionLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

            idLabelOffset += 2.5f + versionLabel->getScaledContentWidth();

            addChild(versionLabel);

            // sprite for description button
            auto descBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                // @geode-ignore(unknown-resource)
                "geode.loader/message.png",
                0.875f,
                m_geodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Small
            );
            descBtnSprite->setScale(0.375f);

            // Mod description button
            auto descBtn = CCMenuItemSpriteExtra::create(
                descBtnSprite,
                this,
                menu_selector(ModItem::onModDesc)
            );
            descBtn->setID("short-description-button");

            btnMenu->addChild(descBtn);

            // @geode-ignore(unknown-resource)
            auto issueBtnSprite = CCSprite::createWithSpriteFrameName("geode.loader/info-warning.png");
            issueBtnSprite->setScale(0.5f);

            auto issueBtn = CCMenuItemSpriteExtra::create(
                issueBtnSprite,
                this,
                menu_selector(ModItem::onModIssues)
            );
            issueBtn->setID("issue-reports-button");

            btnMenu->addChild(issueBtn);
        };

        // ID label
        auto idLabel = CCLabelBMFont::create(modID.c_str(), "bigFont.fnt");
        idLabel->setID("mod-id");
        idLabel->setPosition({ 37.5f + idLabelOffset, (heightCS / 2.f) - 5.f });
        idLabel->setScale(0.25f);
        idLabel->setAnchorPoint({ 0, 0.5 });
        idLabel->setOpacity(125);
        idLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);

        addChild(idLabel);

        btnMenu->updateLayout(true);

        // for first time users of favorites
        if (auto helpTxt = firstTimeText()) {
            helpTxt->setPosition({ widthCS - (btnMenu->getScaledContentWidth() + 2.5f), heightCS / 2.f });
            addChild(helpTxt);

            log::debug("Help text added for first time users");
        } else {
            btnMenu->updateLayout(true);
        };

        auto loadProblem = m_mod->targetsOutdatedVersion();
        if (loadProblem.has_value()) {
            if (loadProblem->isOutdated() && favMod->getSettingValue<bool>("indicate-outdated")) {
                auto gdVer = m_mod->getMetadata().getGameVersion();
                auto reason = fmt::format("Outdated ({})", gdVer.value_or("Any"));

                auto modOutdated = CCLabelBMFont::create(reason.c_str(), "bigFont.fnt");
                modOutdated->setID("outdated-indicator");
                modOutdated->setScale(0.2f);
                modOutdated->setOpacity(200);
                modOutdated->setAnchorPoint({ 0, 0.5 });
                modOutdated->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                modOutdated->setPosition({ 37.5f, (heightCS / 2.f) - 12.5f });
                modOutdated->setColor(colProvider->color3b("geode.loader/mod-list-outdated-label"));

                nameLabel->setOpacity(200);

                log::info("Mod {} is outdated, uses game version {}", m_mod->getID(), gdVer);
                addChild(modOutdated);
            };
        } else if (m_mod->isEnabled() && favMod->getSettingValue<bool>("indicate-update")) {
            auto update = m_mod->checkUpdates().getFinishedValue();
            if (update->unwrapOrDefault().has_value()) {
                // @geode-ignore(unknown-resource)
                auto pendingUpdate = CCSprite::createWithSpriteFrameName("geode.loader/updates-available.png");
                pendingUpdate->setID("pending-update-indicator");
                pendingUpdate->setScale(0.375f);
                pendingUpdate->setPosition({ btnMenu->getPositionX(), btnMenu->getPositionY() + (btnMenu->getScaledContentHeight() / 2.f) });

                log::info("Mod {} has new update of version {} available", m_mod->getID(), update->unwrapOrDefault().value().toVString());
                addChild(pendingUpdate);
            } else {
                log::debug("Mod {} up-to-date", m_mod->getID());
            };
        } else if (!m_mod->isEnabled() && favMod->getSettingValue<bool>("indicate-disabled")) { // check if the user wants to show the mod is disabled
            auto modDisabled = CCLabelBMFont::create("Disabled", "bigFont.fnt");
            modDisabled->setID("disabled-indicator");
            modDisabled->setScale(0.2f);
            modDisabled->setOpacity(200);
            modDisabled->setAnchorPoint({ 0, 0.5 });
            modDisabled->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
            modDisabled->setPosition({ 37.5f, (heightCS / 2.f) - 12.5f });
            modDisabled->setColor({ 255, 65, 65 });

            nameLabel->setOpacity(200);

            log::info("Mod {} is disabled", m_mod->getID());
            addChild(modDisabled);
        };

        return true;
    } else {
        return false;
    };
};

void ModItem::onViewMod(CCObject*) {
    openInfoPopup(m_mod);
};

void ModItem::onModDesc(CCObject*) {
    if (auto alert = FLAlertLayer::create(
        m_mod->getName().c_str(),
        m_mod->getDescription().value_or("<cr>No description available.</c>"),
        "OK"
    )) alert->show();
};

void ModItem::onModIssues(CCObject*) {
    openIssueReportPopup(m_mod);
};

void ModItem::onFavorite(CCObject*) {
    // Toggle favorite status
    m_favorite = !m_favorite;

    // Save the favorite status
    favMod->setSavedValue(m_mod->getID(), m_favorite);
    log::debug("Setting {} to {}", m_mod->getID(), favMod->getSavedValue<bool>(m_mod->getID(), m_favorite) ? "favorite" : "non-favorite");

    // Update the icon
    updateFavoriteIcon();

    // Send event to refresh list
    OnFavoriteEvent().post();
};

void ModItem::updateFavoriteIcon() {
    auto modID = m_mod->getID();

    if (m_favButton) { // Make sure the favorite button has already been created
        auto fOn = m_heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";
        auto fOff = m_heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";

        auto newSprite = CCSprite::createWithSpriteFrameName(m_favorite ? fOn : fOff);
        newSprite->setScale(m_heartIcons ? 0.625f : 0.875f);

        m_favButton->setNormalImage(newSprite);
        log::info("Updated state for {} to {}", modID, m_favorite ? "favorite" : "non-favorite");
    } else {
        log::error("Favorite button not found for {}", modID);
    };
};

CCLabelBMFont* ModItem::firstTimeText() {
    // check if mod loaded before
    if (favMod->getSavedValue<bool>("already-loaded", false)
        || favMod->getSavedValue<bool>(favMod->getID())
        || !favMod->getSettingValue<bool>("minimal")) {
        return nullptr;
    } else if (m_mod->getID().compare(favMod->getID()) == 0) { // create the help text if loaded for the first time
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

ModItem* ModItem::create(
    Mod* mod,
    CCSize const& size,
    bool geodeTheme,
    bool heartIcons
) {
    auto ret = new ModItem();

    if (ret && ret->init(mod, size, geodeTheme, heartIcons)) {
        ret->autorelease();
        return ret;
    };

    CC_SAFE_DELETE(ret);
    return nullptr;
};