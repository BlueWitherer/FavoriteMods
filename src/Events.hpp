#pragma once

#include <Geode/loader/Event.hpp>

struct FavoriteEvent : geode::Event<FavoriteEvent, bool()> {
    using Event::Event;
};