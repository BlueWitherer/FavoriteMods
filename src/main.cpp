#include "ui/FavoritesPopup.hpp"
#include "Events.hpp"

#include <Geode/Geode.hpp>

#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>

using namespace geode::prelude;

// it's modding time :3
static auto favMod = Mod::get();
static auto loader = Loader::get();

static const ZStringView urlGeode = "https://geode-sdk.org/mods/";

$on_game(Loaded) {
    // showcase itself when the player loads for the first time
    if (!favMod->hasSavedValue("already-loaded")) {
        log::info("User has loaded this mod for the first time!");

        favMod->setSavedValue("already-loaded", false);
        favMod->setSavedValue(GEODE_MOD_ID, true);
    };
};

class $nodeModify(FavoritesModsLayer, ModsLayer) {
    struct Fields {
        bool m_isGeodeTheme = false; // setting for geode theme
        bool m_isHeartIcons = false; // if the player wants heart buttons
    };

    void modify() {
        auto f = m_fields.self();

        // get geode's mod
        if (auto geodeMod = loader->getLoadedMod("geode.loader")) {
            f->m_isGeodeTheme = geodeMod->getSettingValue<bool>("enable-geode-theme");
            log::debug("Geode theme enabled: {}", f->m_isGeodeTheme);
        };

        // check if the player wants hearts instead
        f->m_isHeartIcons = favMod->getSettingValue<bool>("hearts");

        // get the actions menu
        if (auto actionsMenu = typeinfo_cast<CCMenu*>(getChildByID("actions-menu"))) {
            log::debug("Actions menu found successfully!");

            // favorites button sprite
            auto favBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
                f->m_isHeartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png",
                f->m_isHeartIcons ? 1.f : 0.875f,
                f->m_isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green
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
            actionsMenu->updateLayout();

            log::info("Favorites button added to actions menu successfully");
        } else {
            log::error("Failed to find actions menu in Geode mods layer with ID 'actions-menu'");
        };
    };

    void onFavoritesBtn(CCObject*) {
        auto f = m_fields.self();

        // create and show the favorites pop-up
        if (auto popup = FavoritesPopup::create(f->m_isGeodeTheme, f->m_isHeartIcons)) popup->show();
    };
};

class $nodeModify(FavoritesModPopup, ModPopup) {
    struct Fields {
        std::string m_modID;

        CCMenuItemToggler* m_favButton = nullptr;
    };

    void modify() {
        if (favMod->getSettingValue<bool>("geode-popup")) {
            log::info("Creating favorite button on Geode mod popup");

            // check for the mod url button
            if (auto modPageBtn = getChildByIDRecursive("mod-online-page-button")) {
                log::debug("Mod page button found");

                // get the url from the user object
                if (auto url = typeinfo_cast<CCString*>(modPageBtn->getUserObject("url"))) {
                    log::debug("URL string object found in mod page button");

                    std::string urlStr = url->getCString();

                    if (utils::string::startsWith(urlStr, urlGeode)) {
                        log::debug("URL found, creating favorites UI");

                        // extract mod id from url
                        auto const thisModID = urlStr.erase(0, urlGeode.size());

                        auto favMenuLayout = RowLayout::create()
                            ->setAxisAlignment(AxisAlignment::Start)
                            ->setAutoScale(false)
                            ->setAutoGrowAxis(0.f)
                            ->setGap(2.5f);

                        auto favMenu = CCMenu::create();
                        favMenu->setID("menu"_spr);
                        favMenu->setPosition({ 0, -1.f });
                        favMenu->setAnchorPoint({ 0, 1 });

                        favMenu->setLayout(favMenuLayout);

                        // find this mod if it's installed
                        if (auto mod = loader->getInstalledMod(thisModID)) {
                            auto f = m_fields.self();

                            f->m_modID = mod->getID();

                            auto isFavorite = favMod->getSavedValue<bool>(f->m_modID);
                            log::debug("Creating favorites menu for mod {}, {} favorites", f->m_modID, isFavorite ? "already in" : "not added to");

                            auto heartIcons = favMod->getSettingValue<bool>("hearts");

                            auto on = heartIcons ? "gj_heartOn_001.png" : "GJ_starsIcon_001.png";
                            auto off = heartIcons ? "gj_heartOff_001.png" : "GJ_starsIcon_gray_001.png";

                            auto favButtonOnSpr = CCSprite::createWithSpriteFrameName(on);
                            favButtonOnSpr->setScale(heartIcons ? 0.375f : 0.5f);

                            auto favButtonOffSpr = CCSprite::createWithSpriteFrameName(off);
                            favButtonOffSpr->setScale(heartIcons ? 0.375f : 0.5f);

                            // Favorite button here :)
                            f->m_favButton = CCMenuItemToggler::create(
                                favButtonOnSpr,
                                favButtonOffSpr,
                                this,
                                menu_selector(FavoritesModPopup::onToggleFavorite)
                            );
                            f->m_favButton->setID("favorite-btn");

                            f->m_favButton->toggle(!isFavorite); // works like this for some reason

                            favMenu->addChild(f->m_favButton);

                            auto favLabel = CCLabelBMFont::create("Favorite", "bigFont.fnt");
                            favLabel->setID("favorite-label");
                            favLabel->setAlignment(kCCTextAlignmentLeft);
                            favLabel->setAnchorPoint({ 0, 0.5 });
                            favLabel->setScale(0.375f);

                            favMenu->addChild(favLabel);
                        } else {
                            log::warn("'{}' must be installed to be favorited", thisModID);

                            auto favLabel = CCLabelBMFont::create("Install this mod to add it to your favorites!", "bigFont.fnt");
                            favLabel->setID("favorite-label");
                            favLabel->setOpacity(200);
                            favLabel->setColor({ 200, 200, 200 });
                            favLabel->setAlignment(kCCTextAlignmentLeft);
                            favLabel->setAnchorPoint({ 0, 0.5 });
                            favLabel->setScale(0.25f);

                            favMenu->addChild(favLabel);
                        };

                        favMenu->updateLayout();

                        // try to get the actual popup layer
                        auto popup = reinterpret_cast<FLAlertLayer*>(this);
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

    void onToggleFavorite(CCObject*) {
        auto f = m_fields.self();

        if (f->m_favButton) {
            auto toFavorite = f->m_favButton->isToggled();
            log::debug("({}) {} favorites", f->m_modID, toFavorite ? "Adding to" : "Removing from");

            // Save the favorite status
            favMod->setSavedValue(f->m_modID, toFavorite);
            log::info("{} now {} favorites", f->m_modID, favMod->getSavedValue<bool>(f->m_modID) ? "on" : "off");

            FavoriteEvent().send();
        } else {
            log::error("Couldn't get favorite button");
        };
    };
};