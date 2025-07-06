#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

// use alpha's geode utils to modify layer
class $nodeModify(MyModsLayer, ModsLayer) {
    void modify() {
        auto loader = Loader::get();
        auto geodeMod = loader->getLoadedMod("geode.loader");

        // check for purply geode theme
        bool isGeodeTheme = geodeMod->getSettingValue<bool>("enable-geode-theme");

        // get the actions menu
        if (auto actionsMenu = as<CCMenu*>(this->getChildByID("actions-menu"))) {
            log::debug("Actions menu found, creating favorites button...");

            // favorites button sprite
            auto favBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                "GJ_starsIcon_001.png",
                0.875f,
                (isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green)
            );
            favBtnSprite->setScale(0.8f);

            // create favorites button
            auto favBtn = CCMenuItemSpriteExtra::create(
                favBtnSprite,
                this,
                menu_selector(MyModsLayer::onFavoritesBtn)
            );
            favBtn->setID("favorites"_spr);

            actionsMenu->addChild(favBtn);
            actionsMenu->updateLayout();

            log::info("Favorites button created!");
        } else {
            log::error("Failed to find Geode mod layer actions menu");
        };
    };

    void onFavoritesBtn(CCObject*) {
        log::debug("clicky click");
        openInfoPopup(getMod());
    };
};

// use alpha's geode utils to modify item
class $nodeModify(MyModItem, ModItem) {
    void modify() {
        log::debug("ModItem layer has been modified!");
    };
};