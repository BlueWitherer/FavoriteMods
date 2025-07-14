#include "./headers/FavoritesPopup.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

// it's modding time :3
auto getThisMod = getMod();

// use alpha's geode utils to modify layer
class $nodeModify(MyModsLayer, ModsLayer) {
    struct Fields {
        bool m_isGeodeTheme = false; // setting for geode theme
        bool m_isHeartIcons = false; // if the player wants heart buttons
    };

    void modify() {
        log::info("Hooked ModsLayer...");

        // geode mod loader
        auto loader = Loader::get();

        // get the geode mod
        if (auto geodeMod = loader->getLoadedMod("geode.loader")) {
            log::debug("Geode mod found: {}", geodeMod->getName());
            m_fields->m_isGeodeTheme = geodeMod->getSettingValue<bool>("enable-geode-theme");
            log::debug("Geode theme enabled: {}", m_fields->m_isGeodeTheme);
        } else {
            log::error("Failed to get Geode loader");
        };

        // check if the player wants hearts instead
        m_fields->m_isHeartIcons = getThisMod->getSettingValue<bool>("hearts");

        // showcase itself when the player loads for the first time
        if (getThisMod->hasSavedValue("already-loaded")) {
            log::debug("User has loaded the mod before");
        } else {
            log::info("User has loaded this mod for the first time!");

            getThisMod->setSavedValue("already-loaded", false);
            getThisMod->setSavedValue(getThisMod->getID(), true);
        };

        // get the actions menu
        if (auto actionsMenu = as<CCMenu*>(getChildByID("actions-menu"))) {
            log::debug("Actions menu found successfully!");

            // favorites button sprite
            auto favBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                m_fields->m_isHeartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png",
                m_fields->m_isHeartIcons ? 1.f : 0.875f,
                m_fields->m_isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Medium
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
            actionsMenu->updateLayout(true);

            log::info("Favorites button added to actions menu successfully!");
        } else {
            log::error("Failed to find actions menu with ID 'actions-menu'");
        };
    };

    void onFavoritesBtn(CCObject*) {
        log::debug("Favorites button clicked!");

        // create the favorites pop-up
        if (auto popup = FavoritesPopup::create(m_fields->m_isGeodeTheme, m_fields->m_isHeartIcons)) {
            popup->show();
            log::debug("Favorites popup shown successfully");
        } else {
            log::error("Failed to create favorites popup!");
        };
    };
};