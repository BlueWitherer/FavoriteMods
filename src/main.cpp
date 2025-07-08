#include "./headers/FavoritesPopup.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

auto getThisMod = getMod();

// use alpha's geode utils to modify layer
class $nodeModify(MyModsLayer, ModsLayer) {
    struct Fields {
        bool m_isGeodeTheme = false; // check for geode theme
    };

    void modify() {
        log::info("Hooked ModsLayer...");

        // geode mod loader
        auto loader = Loader::get();
        auto geodeMod = loader->getLoadedMod("geode.loader");

        log::debug("Geode mod found: {}", geodeMod->getName());

        if (geodeMod) {
            try {
                m_fields->m_isGeodeTheme = geodeMod->getSettingValue<bool>("enable-geode-theme");
                log::debug("Geode theme enabled: {}", m_fields->m_isGeodeTheme);
            } catch (...) {
                log::warn("Could not get geode theme setting, defaulting to false");
                m_fields->m_isGeodeTheme = false;
            };
        } else {
            log::error("Failed to get Geode loader");
        };

        if (getThisMod->hasSavedValue(getThisMod->getID())) {
            log::debug("User has loaded the mod before");
        } else {
            log::info("User has loaded this mod for the first time!");
            getThisMod->setSavedValue(getThisMod->getID(), true);
        };

        // get the actions menu
        if (auto actionsMenu = as<CCMenu*>(getChildByID("actions-menu"))) {
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
                (m_fields->m_isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green)
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
        };
    };

    void onFavoritesBtn(CCObject*) {
        log::debug("Favorites button clicked!");
        bool alerts = getThisMod->getSettingValue<bool>("alerts");

        try {
            auto popup = FavoritesPopup::create(m_fields->m_isGeodeTheme);

            if (popup) {
                popup->show();
                log::debug("Favorites popup shown successfully");
            } else {
                log::error("Failed to create favorites popup!");
                if (alerts) FLAlertLayer::create("Error", "Failed to create favorites popup!", "OK")->show();
            };
        } catch (const std::exception& e) { // lets try catch something shall we :p
            log::error("Exception while showing favorites popup: {}", e.what());
            if (alerts) FLAlertLayer::create("Error", "An error occurred while showing the favorites popup!", "OK")->show();
        } catch (...) {
            log::error("Unknown error occurred while showing favorites popup");
            if (alerts) FLAlertLayer::create("Error", "An unknown error occurred while showing the favorites popup!", "OK")->show();
        };
    };
};