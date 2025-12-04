# v1.2.2
- Bumped Geode loader support to `v4.10.0`
- Fixed crash caused by mod update checker
- Several fixes

# v1.2.1
- Bumped Geode loader support to `v4.9.0`
- Internal fixes

# v1.2.0
- Bumped Geode loader support to `v4.8.0`
- Mod cells will now show if that mod is disabled, has an update, or targets an outdated Geometry Dash version
  - Added toggle setting `Mod Disabled`
  - Added toggle setting `Update Pending`
  - Added toggle setting `Outdated Game Version`
- Switched to event-based updates for refreshing mod list when toggling favorites
- Internal tweaks

# v1.1.2
- Optimized favorites pop-up & mod list UI
- Tweaked mod logo loader in mod list
  - Fixed Geode loader mod logo failing to load
- Removed API

# v1.1.1
- Added toggle setting `Geode Mod Pop-up`
- Fixed Geode loader mod not appearing in favorites list if added to favorites
- Moved toggle setting *`Hearts`* under *`Accessibility`* category in mod settings
- Tweaked favorites UI on Geode mod pop-up

# v1.1.0
- Added favorite button to Geode mod pop-up for all mods
- Other tweaks

# v1.0.9
- Bumped Geode loader support to `v4.7.0`
- Save data will no longer be created for mods that were never favorited
- Updated mod metadata
- Other tweaks

# v1.0.8
- Fixed scroll sizing issue in mod list
- Slightly adjusted positioning of UI in favorites pop-up
- Renamed toggle setting *`Only List Enabled Mods`* to `Enabled Mods Only`
- Tweaked some mod setting descriptions

# v1.0.7
- Fixed first-time help text positioning
- Added more help text for pagination

# v1.0.6
- Removed top gap in favorites pop-up
- Minor UI tweaks

# v1.0.5
- Fixed empty gap in mod list
- Adjusted count text in mod stats pop-up
- Added issue report button to mod list item if *`Minimalist List`* is disabled
- Fixed general UI rescaling
- Other tweaks

# v1.0.4
- Added pagination to mod list
  - Added toggle setting `List Pages`
  - Added number setting `Mods Per Page`
- Added toggle setting `Hearts`
  - Hearts can now optionally be displayed instead of stars for favorites
- Added ability to search for mods by short description
- Added optional mod description button in mod list item
- Added Geode theme compatibility to *View* button in mod list item
- Set default value for setting *`Only List Enabled Mods`* to `true`
- Removed toggle setting *`Error Alerts`*
- Added stats button to pop-up
- Other tweaks

# v1.0.3
- Added toggle setting `Minimalist List`
  - Added optional developer label to mod list item
  - Version label in mod list item will only appear if *`Minimalist List`* is disabled
- Fixed coloring for version and ID label in mod list item
- Fixed enabled-only filter on mod list
- Adjusted positioning for mod settings button on pop-up
- Adjusted positioning for clear search button and search box on pop-up
- Added guide text for first-time users on *Favorite Mods* mod list item
- Added and tweaked more node IDs

# v1.0.2
- Fixed touch priority issue with scrolling UI
- Fixed texture size issue with Clear button
- Fixed visual issue with mod list item background UI
- Added mod API

# v1.0.1
- Added clear search button
- Added full Geode theme compatibility
- Set default value for setting *`Clear Search on Refresh`* to `false`

# v1.0.0
- Initial release