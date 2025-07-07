#include "../ModItem.hpp"

#include <Geode/ui/GeodeUI.hpp>

using namespace geode::prelude;

bool ModItem::init(Mod* mod, CCSize const& size) {
    m_mod = mod;

    if (CCNode::init()) {
        this->setID(m_mod->getID());
        this->setAnchorPoint({ 0, 1 });
        this->setContentSize(size);

        // Background for mod item
        auto itemBG = CCScale9Sprite::create("square02_001.png");
        itemBG->setContentSize(size);
        itemBG->setAnchorPoint({ 0.5, 0.5 });
        itemBG->ignoreAnchorPointForPosition(false);
        itemBG->setPosition({ this->getScaledContentWidth() / 2.f, this->getScaledContentHeight() / 2.f });
        itemBG->setColor({ 125, 125, 125 });
        itemBG->setOpacity(150);

        this->addChild(itemBG);

        // Mod icon sprite
        auto modIcon = geode::createModLogo(m_mod->getPackagePath());
        modIcon->setScale(0.38f);
        modIcon->setPosition({ 25.f, this->getScaledContentHeight() / 2.f });
        modIcon->ignoreAnchorPointForPosition(false);
        modIcon->setAnchorPoint({ 0.5, 0.5 });

        this->addChild(modIcon);

        // Mod name label
        auto nameLabel = CCLabelBMFont::create(m_mod->getName().c_str(), "bigFont.fnt");
        nameLabel->setPosition({ 37.5f, 25.f });
        nameLabel->setScale(0.4f);
        nameLabel->setAnchorPoint({ 0.f, 0.5f });

        this->addChild(nameLabel);

        // Version label
        auto versionLabel = CCLabelBMFont::create(m_mod->getVersion().toVString().c_str(), "goldFont.fnt");
        versionLabel->setPosition({ 37.5f, 15.f });
        versionLabel->setScale(0.3f);
        versionLabel->setAnchorPoint({ 0.f, 0.5f });
        versionLabel->setColor({ 200, 200, 200 });

        this->addChild(versionLabel);

        // ID label
        auto idLabel = CCLabelBMFont::create(m_mod->getID().c_str(), "bigFont.fnt");
        idLabel->setPosition({ versionLabel->getScaledContentWidth() + 40.f, 15.f });
        idLabel->setScale(0.25f);
        idLabel->setAnchorPoint({ 0.f, 0.5f });
        idLabel->setColor({ 125, 125, 125 });
        idLabel->setOpacity(125);

        this->addChild(idLabel);

        // View button
        auto viewBtnSprite = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", 0.875f);
        viewBtnSprite->setScale(0.75f);

        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewBtnSprite,
            this,
            menu_selector(ModItem::onViewMod)
        );

        // Favorite button here :)
        auto favBtnSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_gray_001.png");
        favBtnSprite->setScale(0.875f);

        auto favBtn = CCMenuItemSpriteExtra::create(
            favBtnSprite,
            this,
            menu_selector(ModItem::onFavorite)
        );

        // Create menu for buttons
        auto btnMenu = CCMenu::create();
        btnMenu->setAnchorPoint({ 1, 0.5 });
        btnMenu->setPosition({ this->getScaledContentWidth() - 10.f, this->getScaledContentHeight() / 2.f });
        btnMenu->setScaledContentSize({ this->getScaledContentWidth() * 0.375f, this->getScaledContentHeight() - 10.f });

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
        btnMenu->addChild(favBtn);

        this->addChild(btnMenu);

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
    Notification::create("Favorites are a work in progress!", NotificationIcon::Info, 2.5f)->show();
};

ModItem* ModItem::create(Mod* mod, CCSize const& size) {
    auto ret = new ModItem();

    if (!ret || !ret->init(mod, size)) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    };

    ret->autorelease();
    return ret;
};