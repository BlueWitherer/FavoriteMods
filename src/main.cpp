#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

#include <alphalaneous.alphas_geode_utils/include/NodeModding.h>

using namespace geode::prelude;

// use alpha's geode utils to modify layer

class FavoritesPopup : public geode::Popup<> {
private:
    ScrollLayer* m_scrollLayer = nullptr;
    CCMenu* m_buttonMenu = nullptr;
    
protected:
    bool setup() override {
        log::debug("FavoritesPopup::setup() called");
        
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        log::debug("Window size: {}x{}", winSize.width, winSize.height);
        
        // Set popup size
        this->setTitle("Favorite Mods Manager");
        log::debug("Popup title set");
        
        // Create main content area
        auto contentSize = CCSize{400.f, 280.f};
        
        // Background for scroll area
        auto scrollBG = CCScale9Sprite::create("square02b_001.png");
        scrollBG->setContentSize({380.f, 200.f});
        scrollBG->setPosition({contentSize.width / 2, contentSize.height / 2});
        scrollBG->setColor({0, 0, 0});
        scrollBG->setOpacity(100);
        this->m_mainLayer->addChild(scrollBG);
        
        // Create scroll layer
        m_scrollLayer = ScrollLayer::create({360.f, 180.f});
        m_scrollLayer->setPosition({contentSize.width / 2 - 180.f, contentSize.height / 2 - 90.f});
        m_scrollLayer->m_contentLayer->setLayout(
            ColumnLayout::create()
                ->setAxisAlignment(AxisAlignment::End) // seriously why is this end at top but start at bottom?
                ->setAxisReverse(true) // haha wtf is top reverse but bottom isnt LMAO
                ->setAutoGrowAxis(360.f)
                ->setGap(5.f)
        );
        this->m_mainLayer->addChild(m_scrollLayer);
        
        // example mod items
        this->addModItem("cheesework peak mod", "v1.0.0", "GJ_starsIcon_001.png");
        //this->addModItem("Another Mod", "v2.1.3", "GJ_likesIcon_001.png");
        
        m_scrollLayer->m_contentLayer->updateLayout();
        
        // Scroll to top
        m_scrollLayer->scrollToTop();

    }
public:
    static FavoritesPopup* create() {
        log::debug("FavoritesPopup::create() called");
        
        auto ret = new FavoritesPopup();
        if (ret && ret->initAnchored(400.f, 280.f)) {
            ret->autorelease();
            log::debug("FavoritesPopup created successfully");
            return ret;
        }
        
        log::error("Failed to create FavoritesPopup!");
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    
    void onAddFavorite(CCObject*) {
        FLAlertLayer::create("Add Favorite", "This would add a mod to your favorites!", "OK")->show();
    }
    
    void onRemoveFavorite(CCObject*) {
        FLAlertLayer::create("Remove Favorite", "This would remove the selected mod from favorites!", "OK")->show();
    }
    
    void onRefresh(CCObject*) {
        // Clear and recreate scroll content
        m_scrollLayer->m_contentLayer->removeAllChildren();
        
        // Show confirmation
        Notification::create("Favorites list refreshed!", NotificationIcon::Success)->show();
    }
    
    // this contain a mod name, a version, mod icon but ive decided to use icon sprite for now
    void addModItem(const std::string& modName, const std::string& version, const std::string& iconSprite) {
        // Create container for mod item
        auto itemContainer = CCNode::create();
        itemContainer->setContentSize({350.f, 40.f});
        
        // Background for mod item
        auto itemBG = CCScale9Sprite::create("square02_001.png");
        itemBG->setContentSize({350.f, 40.f});
        itemBG->setPosition({175.f, 20.f});
        itemBG->setColor({50, 50, 50});
        itemBG->setOpacity(150);
        itemContainer->addChild(itemBG);
        
        // Mod icon
        auto modIcon = CCSprite::createWithSpriteFrameName(iconSprite.c_str());
        modIcon->setPosition({25.f, 20.f});
        modIcon->setScale(0.7f);
        itemContainer->addChild(modIcon);
        
        // Mod name label
        auto nameLabel = CCLabelBMFont::create(modName.c_str(), "bigFont.fnt");
        nameLabel->setPosition({70.f, 25.f});
        nameLabel->setScale(0.4f);
        nameLabel->setAnchorPoint({0.f, 0.5f});
        itemContainer->addChild(nameLabel);
        
        // Version label
        auto versionLabel = CCLabelBMFont::create(version.c_str(), "goldFont.fnt");
        versionLabel->setPosition({70.f, 15.f});
        versionLabel->setScale(0.3f);
        versionLabel->setAnchorPoint({0.f, 0.5f});
        versionLabel->setColor({200, 200, 200});
        itemContainer->addChild(versionLabel);
        
        // View button
        auto viewBtnSprite = ButtonSprite::create("View", "bigFont.fnt", "GJ_button_01.png", 0.5f);
        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewBtnSprite,
            this,
            menu_selector(FavoritesPopup::onViewMod)
        );
        viewBtn->setPosition({300.f, 20.f});
        
        auto btnMenu = CCMenu::create();
        btnMenu->addChild(viewBtn);
        btnMenu->setPosition({0.f, 0.f});
        itemContainer->addChild(btnMenu);
        
        // Add to scroll layer
        m_scrollLayer->m_contentLayer->addChild(itemContainer);
    }
    
    void onViewMod(CCObject*) {
        FLAlertLayer::create("View Mod", "This would open the mod details!", "OK")->show();
    }
};
// use alpha's geode utils to modify item
class $nodeModify(MyModItem, ModItem) {
    void modify() {
        log::debug("ModItem layer has been modified!");
    };
};

// use alpha's geode utils to modify layer
class $nodeModify(MyModsLayer, ModsLayer) {
    void modify() {
        log::info("Starting MyModsLayer modification...");
        
        auto loader = Loader::get();
        if (!loader) {
            log::error("Failed to get Loader instance!");
            return;
        }
        log::debug("Loader instance obtained successfully");
        
        auto geodeMod = loader->getLoadedMod("geode.loader");
        if (!geodeMod) {
            log::error("Failed to get geode.loader mod!");
            return;
        }
        log::debug("Geode mod found: {}", geodeMod->getName());

        // check for purply geode theme
        bool isGeodeTheme = false;
        try {
            isGeodeTheme = geodeMod->getSettingValue<bool>("enable-geode-theme");
            log::debug("Geode theme enabled: {}", isGeodeTheme);
        } catch (...) {
            log::warn("Could not get geode theme setting, defaulting to false");
            isGeodeTheme = false;
        }

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
                        }
                    }
                }
            }
            return;
        }
        
        log::debug("Actions menu found successfully!");

        // Check if favorites button already exists
        if (actionsMenu->getChildByID("favorites"_spr)) {
            log::debug("Favorites button already exists, skipping creation");
            return;
        }

        // favorites button sprite (pls change this with a mod icon :D)
        auto favBtnSprite = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_starsIcon_001.png",
            0.875f,
            (isGeodeTheme ? CircleBaseColor::DarkPurple : CircleBaseColor::Green)
        );
        
        if (!favBtnSprite) {
            log::error("Failed to create favorites button sprite!");
            return;
        }
        
        favBtnSprite->setScale(0.8f);
        log::debug("Favorites button sprite created successfully");

        // create favorites button
        auto favBtn = CCMenuItemSpriteExtra::create(
            favBtnSprite,
            this,
            menu_selector(MyModsLayer::onFavoritesBtn)
        );
        
        if (!favBtn) {
            log::error("Failed to create favorites button!");
            return;
        }
        
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