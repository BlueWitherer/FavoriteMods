#include "./headers/FavoritesPopup.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

// use alpha's geode utils to modify item
class $nodeModify(MyModItem, ModItem) {
    void modify() {
        log::debug("Hooked ModItem...");
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
        auto actionsMenu = as<CCMenu*>(this->getChildByID("actions-menu"));
        if (!actionsMenu) {
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