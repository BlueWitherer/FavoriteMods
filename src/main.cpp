#include "./ui/FavoritesPopup.hpp"

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

// it's modding time :3
auto getThisMod = Mod::get();
auto getGeodeLoader = Loader::get();

// use alpha's geode utils to modify layer
class $nodeModify(FavoritesModsLayer, ModsLayer) {
    struct Fields {
        bool m_isGeodeTheme = false; // setting for geode theme
        bool m_isHeartIcons = false; // if the player wants heart buttons
    };

    void modify() {
        // get geode's mod
        if (auto geodeMod = getGeodeLoader->getLoadedMod("geode.loader")) {
            log::debug("Geode found!");
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
        if (auto actionsMenu = static_cast<CCMenu*>(getChildByID("actions-menu"))) {
            log::debug("Actions menu found successfully!");

            // favorites button sprite
            auto favBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                m_fields->m_isHeartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png",
                m_fields->m_isHeartIcons ? 1.f : 0.875f,
                m_fields->m_isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green,
                CircleBaseSize::Medium
            );
            favBtnSprite->setScale(0.8f);

            // create favorites button
            auto favBtn = CCMenuItemSpriteExtra::create(
                favBtnSprite,
                this,
                menu_selector(FavoritesModsLayer::onFavoritesBtn)
            );
            favBtn->setID("favorites-button"_spr);

            actionsMenu->addChild(favBtn);
            actionsMenu->updateLayout(true);

            log::info("Favorites button added to actions menu successfully");
        } else {
            log::error("Failed to find actions menu in Geode mods layer with ID 'actions-menu'");
        };
    };

    void onFavoritesBtn(CCObject*) {
        log::debug("Favorites button clicked!");

        // create and show the favorites pop-up
        if (auto popup = FavoritesPopup::create(m_fields->m_isGeodeTheme, m_fields->m_isHeartIcons)) {
            log::info("Favorites popup created successfully");
            popup->show();
        } else {
            Notification::create("An error occurred, please re-open this page and try again", NotificationIcon::Error, 1.25f)->show();
            log::error("Failed to create favorites popup!");
        };
    };
};

// use alpha's geode utils to modify popup
class $nodeModify(FavoritesModPopup, ModPopup) {
    struct Fields {
        std::string m_modID;
        CCMenuItemToggler* m_favButton = nullptr;
    };

    void modify() {
        if (getThisMod->getSettingValue<bool>("geode-popup")) {
            log::info("Creating favorite button on Geode mod popup");

            // check for the mod url button
            if (auto modPageBtn = getChildByIDRecursive("mod-online-page-button")) {
                log::debug("Mod page button found");

                // get the url from the user object
                if (auto url = static_cast<CCString*>(modPageBtn->getUserObject("url"))) {
                    log::debug("URL string object found in mod page button");

                    std::string urlStr(url->getCString());
                    std::string urlGeode("https://geode-sdk.org/mods/");

                    if (urlStr.find(urlGeode) == 0) {
                        log::debug("URL found, creating favorites UI");

                        // extract mod id from url
                        auto thisModID = urlStr.erase(0, urlGeode.length());

                        auto favMenuLayout = RowLayout::create()
                            ->setAxisAlignment(AxisAlignment::Start)
                            ->setAutoScale(false)
                            ->setAutoGrowAxis(0.f)
                            ->setGap(2.5f);

                        auto favMenu = CCMenu::create();
                        favMenu->setID("menu"_spr);
                        favMenu->setPosition({ 0, -1.f });
                        favMenu->ignoreAnchorPointForPosition(false);
                        favMenu->setAnchorPoint({ 0, 1 });

                        favMenu->setLayout(favMenuLayout);

                        if (auto mod = getGeodeLoader->getInstalledMod(thisModID)) {
                            m_fields->m_modID = mod->getID();

                            auto isFavorite = getThisMod->getSavedValue<bool>(m_fields->m_modID);
                            log::debug("Creating favorites menu for mod {}, {} favorites", m_fields->m_modID, isFavorite ? "already in" : "not added to");

                            auto heartIcons = getThisMod->getSettingValue<bool>("hearts");

                            auto off = heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";
                            auto on = heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";

                            auto favButtonOnSpr = CCSprite::createWithSpriteFrameName(on);
                            favButtonOnSpr->setScale(heartIcons ? 0.375f : 0.5f);

                            auto favButtonOffSpr = CCSprite::createWithSpriteFrameName(off);
                            favButtonOffSpr->setScale(heartIcons ? 0.375f : 0.5f);

                            // Favorite button here :)
                            m_fields->m_favButton = CCMenuItemToggler::create(
                                favButtonOnSpr,
                                favButtonOffSpr,
                                this,
                                menu_selector(FavoritesModPopup::onFavorite)
                            );
                            m_fields->m_favButton->setID("favorite-button");

                            m_fields->m_favButton->toggle(!isFavorite); // this for some reason

                            favMenu->addChild(m_fields->m_favButton);

                            auto favLabel = CCLabelBMFont::create("Favorite", "bigFont.fnt");
                            favLabel->setID("favorite-label");
                            favLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                            favLabel->setAnchorPoint({ 0, 0.5 });
                            favLabel->setScale(0.375f);

                            favMenu->addChild(favLabel);
                        } else {
                            log::error("Mod must be installed to be favorited");

                            auto favLabel = CCLabelBMFont::create("Install this mod to add it to your favorites!", "bigFont.fnt");
                            favLabel->setID("favorite-label");
                            favLabel->setOpacity(200);
                            favLabel->setColor({ 200, 200, 200 });
                            favLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                            favLabel->setAnchorPoint({ 0, 0.5 });
                            favLabel->setScale(0.25f);

                            favMenu->addChild(favLabel);
                        };

                        favMenu->updateLayout(true);

                        // try to get the actual popup layer
                        FLAlertLayer* popup = reinterpret_cast<FLAlertLayer*>(this);
                        if (popup->m_mainLayer) popup->m_mainLayer->addChild(favMenu);
                    } else {
                        log::error("Couldn't find Geode mod URL");
                    };
                } else {
                    log::error("Couldn't find Geode mod URL string object");
                };
            } else {
                log::error("Couldn't find Geode mod URL button");
            };
        } else {
            log::warn("Favorite button on Geode mod popup is disabled");
        };
    };

    void onFavorite(CCObject*) {
        if (m_fields->m_favButton) {
            auto toFavorite = m_fields->m_favButton->isToggled();
            log::info("({}) {} favorites", m_fields->m_modID, toFavorite ? "Adding to" : "Removing from");

            // Save the favorite status
            getThisMod->setSavedValue(m_fields->m_modID, toFavorite);
            log::debug("{} now {} favorites", m_fields->m_modID, getThisMod->getSavedValue<bool>(m_fields->m_modID) ? "in" : "off");
        } else {
            log::error("Couldn't get favorite button");
        };
    };
};