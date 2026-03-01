#pragma once

#include <Geode/loader/Event.hpp>

struct FavoriteEvent final : geode::Event<FavoriteEvent, bool()> {
    using Event::Event;
};