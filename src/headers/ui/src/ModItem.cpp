#include "../ModItem.hpp"
#include "../../FavoritesPopup.hpp"

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/ColorProvider.hpp>

using namespace geode::prelude;

bool ModItem::init(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme) {
    m_mod = mod;
    m_parentPopup = parentPopup;
    m_geodeTheme = geodeTheme;

    // Check if this mod is already favorited
    isFavorite = m_thisMod->getSavedValue<bool>(m_mod->getID(), false);

    if (CCNode::init()) {
        ccColor4B bgColor = ColorProvider::get()->color("geode.loader/mod-developer-item-bg");

        setID(m_mod->getID());
        setAnchorPoint({ 0, 1 });
        setContentSize(size);

        // Background for mod item
        m_backgroundSprite = CCScale9Sprite::create("square02b_001.png");
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
        modIcon->setScale(0.5f);
        modIcon->setPosition({ 20.f, getScaledContentHeight() / 2.f });
        modIcon->ignoreAnchorPointForPosition(false);
        modIcon->setAnchorPoint({ 0.5, 0.5 });

        addChild(modIcon);

        // Mod name label
        auto nameLabel = CCLabelBMFont::create(m_mod->getName().c_str(), "bigFont.fnt");
        nameLabel->setPosition({ 37.5f, 25.f });
        nameLabel->setScale(0.4f);
        nameLabel->setAnchorPoint({ 0.f, 0.5f });

        addChild(nameLabel);

        // Version label
        auto versionLabel = CCLabelBMFont::create(m_mod->getVersion().toVString().c_str(), "goldFont.fnt");
        versionLabel->setPosition({ 37.5f, 15.f });
        versionLabel->setScale(0.3f);
        versionLabel->setAnchorPoint({ 0.f, 0.5f });
        versionLabel->setColor({ 200, 200, 200 });

        addChild(versionLabel);

        // ID label
        auto idLabel = CCLabelBMFont::create(m_mod->getID().c_str(), "bigFont.fnt");
        idLabel->setPosition({ versionLabel->getScaledContentWidth() + 40.f, 15.f });
        idLabel->setScale(0.25f);
        idLabel->setAnchorPoint({ 0.f, 0.5f });
        idLabel->setColor({ 125, 125, 125 });
        idLabel->setOpacity(125);

        addChild(idLabel);

        // View button
        auto viewBtnSprite = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", 0.875f);
        viewBtnSprite->setScale(0.75f);

        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewBtnSprite,
            this,
            menu_selector(ModItem::onViewMod)
        );

        // Favorite button here :)
        auto favBtnSprite = CCSprite::createWithSpriteFrameName(isFavorite ? "GJ_starsIcon_001.png" : "GJ_starsIcon_gray_001.png");
        favBtnSprite->setScale(0.875f);

        m_favButton = CCMenuItemSpriteExtra::create(
            favBtnSprite,
            this,
            menu_selector(ModItem::onFavorite)
        );

        // Create menu for buttons
        auto btnMenu = CCMenu::create();
        btnMenu->setAnchorPoint({ 1, 0.5 });
        btnMenu->setPosition({ getScaledContentWidth() - 10.f, getScaledContentHeight() / 2.f });
        btnMenu->setScaledContentSize({ getScaledContentWidth() * 0.375f, getScaledContentHeight() - 10.f });

        // Layout to automatically position buttons on menu
        auto btnMenuLayout = RowLayout::create();
        btnMenuLayout->setDefaultScaleLimits(0.625f, 0.875f);
        btnMenuLayout->setAxisAlignment(AxisAlignment::End);
        btnMenuLayout->setCrossAxisAlignment(AxisAlignment::Center);
        btnMenuLayout->setCrossAxisLineAlignment(AxisAlignment::Center);
        btnMenuLayout->setAxisReverse(true);
        btnMenuLayout->setGap(7.5f);

        btnMenu->setLayout(btnMenuLayout);

        btnMenu->addChild(viewBtn);
        btnMenu->addChild(m_favButton);

        addChild(btnMenu);

        btnMenu->updateLayout();

        return true;
    } else {
        return false;
    };
};

void ModItem::onViewMod(CCObject*) {
    openInfoPopup(m_mod);
};

void ModItem::onFavorite(CCObject*) {
    // Toggle favorite status
    isFavorite = !isFavorite;

    // Save the favorite status
    m_thisMod->setSavedValue<bool>(m_mod->getID(), isFavorite);

    // Update the icon
    updateFavoriteIcon();

    // Notify parent popup to refresh
    if (m_parentPopup) m_parentPopup->onModFavoriteChanged();
};

void ModItem::updateFavoriteIcon() {
    if (m_favButton) {
        auto newSprite = CCSprite::createWithSpriteFrameName(
            isFavorite ? "GJ_starsIcon_001.png" : "GJ_starsIcon_gray_001.png"
        );
        newSprite->setScale(0.875f);

        m_favButton->setNormalImage(newSprite);
    } else {
        log::error("Favorite button not found");
    };
};

ModItem* ModItem::create(Mod* mod, CCSize const& size, FavoritesPopup* parentPopup, bool geodeTheme) {
    auto ret = new ModItem();

    if (!ret || !ret->init(mod, size, parentPopup, geodeTheme)) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    };

    ret->autorelease();
    return ret;
};