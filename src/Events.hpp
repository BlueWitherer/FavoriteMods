#pragma once

#include <Geode/loader/Event.hpp>

struct FavoriteEvent : geode::SimpleEvent<FavoriteEvent> {
    using SimpleEvent::SimpleEvent;
};