#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class FavoriteEvent : public SimpleEvent<FavoriteEvent> {
public:
    FavoriteEvent() = default;
};