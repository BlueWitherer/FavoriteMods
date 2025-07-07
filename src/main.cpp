#include "./headers/FavoritesPopup.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

// use alpha's geode utils to modify item
class $nodeModify(MyModPopup, ModPopup) {
    struct Fields {
        Mod* m_mod = nullptr;
    };

    void modify() {
        log::debug("Hooked ModPopup...");

        auto layer = this->getChildByType<cocos2d::CCLayer>(0);

        auto outsideMenu = CCMenu::create();
        outsideMenu->setID("outside-menu"_spr);
        outsideMenu->setAnchorPoint({ 0, 1 });
        outsideMenu->ignoreAnchorPointForPosition(false);
        outsideMenu->setScaledContentSize({ 25.f, layer->getScaledContentHeight() });
        outsideMenu->setPosition({ layer->getScaledContentWidth() + 5.f, layer->getScaledContentHeight() });

        auto outsideMenuLayout = ColumnLayout::create();
        outsideMenuLayout->setDefaultScaleLimits(0.625f, 0.875f);
        outsideMenuLayout->setAxisAlignment(AxisAlignment::End);
        outsideMenuLayout->setCrossAxisAlignment(AxisAlignment::Center);
        outsideMenuLayout->setCrossAxisLineAlignment(AxisAlignment::Center);
        outsideMenuLayout->setAxisReverse(true);
        outsideMenuLayout->setGap(7.5f);

        outsideMenu->setLayout(outsideMenuLayout);

        // Favorite button here :)
        auto favBtnSprite = CCSprite::createWithSpriteFrameName("GJ_starsIcon_gray_001.png");
        favBtnSprite->setScale(0.875f);

        auto favBtn = CCMenuItemSpriteExtra::create(
            favBtnSprite,
            this,
            menu_selector(MyModPopup::onFavorite)
        );

        outsideMenu->addChild(favBtn);

        outsideMenu->updateLayout(true);

        layer->addChild(outsideMenu);
    };

    void onFavorite(CCObject*) {
        log::info("favorite");
    };
};

// use alpha's geode utils to modify layer
class $nodeModify(MyModsLayer, ModsLayer) {
    void modify() {
        log::info("Hooked ModsLayer...");

        auto loader = Loader::get();
        auto geodeMod = loader->getLoadedMod("geode.loader");

        log::debug("Geode mod found: {}", geodeMod->getName());

        // check for purply geode theme
        bool isGeodeTheme = false;

        if (geodeMod) {
            try {
                isGeodeTheme = geodeMod->getSettingValue<bool>("enable-geode-theme");
                log::debug("Geode theme enabled: {}", isGeodeTheme);
            } catch (...) {
                log::warn("Could not get geode theme setting, defaulting to false");
                isGeodeTheme = false;
            };
        } else {
            log::error("Failed to get Geode loader");
        };

        // get the actions menu
        if (auto actionsMenu = as<CCMenu*>(this->getChildByID("actions-menu"))) {
            log::debug("Actions menu found successfully!");

            // Check if favorites button already exists
            if (actionsMenu->getChildByID("favorites"_spr)) {
                log::debug("Favorites button already exists, skipping creation");
                return;
            };

            // favorites button sprite (pls change this with a mod icon :D)
            auto favBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                "GJ_starsIcon_001.png",
                0.875f,
                (isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green)
            );

            favBtnSprite->setScale(0.8f);
            log::debug("Favorites button sprite created successfully");

            // create favorites button
            auto favBtn = CCMenuItemSpriteExtra::create(
                favBtnSprite,
                this,
                menu_selector(MyModsLayer::onFavoritesBtn)
            );

            favBtn->setID("favorites"_spr);
            log::debug("Favorites button created successfully");

            actionsMenu->addChild(favBtn);
            actionsMenu->updateLayout();

            log::info("Favorites button added to actions menu successfully!");
        } else {
            log::error("Failed to find actions menu with ID 'actions-menu'");

            // Debug: List all children to see what's available
            log::debug("Available children:");
            auto children = this->getChildren();

            if (children) {
                for (int i = 0; i < children->count(); i++) {
                    auto child = static_cast<CCNode*>(children->objectAtIndex(i));

                    if (child) {
                        auto menu = typeinfo_cast<CCMenu*>(child);

                        if (menu) {
                            log::debug("Found menu at index {}, ID: '{}'", i, child->getID());
                        } else {
                            log::debug("Found non-menu node at index {}, ID: '{}'", i, child->getID());
                        };
                    } else {
                        log::error("Node not found at index {}", (int)i);
                    };
                };
            } else {
                log::error("Nodes not found");
            };

            return;
        };
    };

    void onFavoritesBtn(CCObject*) {
        log::debug("Favorites button clicked!");

        try {
            auto popup = FavoritesPopup::create();
            if (popup) {
                popup->show();
                log::debug("Favorites popup shown successfully");
            } else {
                log::error("Failed to create favorites popup!");
                FLAlertLayer::create("Error", "Failed to create favorites popup!", "OK")->show();
            }
            // lets try catch something shall we :p
        } catch (const std::exception& e) {
            log::error("Exception while showing favorites popup: {}", e.what());
            FLAlertLayer::create("Error", "An error occurred while showing the favorites popup!", "OK")->show();
        } catch (...) {
            log::error("Unknown error occurred while showing favorites popup");
            FLAlertLayer::create("Error", "An unknown error occurred while showing the favorites popup!", "OK")->show();
        }
    };
};