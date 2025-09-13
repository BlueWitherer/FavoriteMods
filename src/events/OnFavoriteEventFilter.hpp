#pragma once

#include "./OnFavoriteEvent.hpp"

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class OnFavoriteEventFilter : public EventFilter<OnFavoriteEvent> {
public:
    using Callback = ListenerResult();

    ListenerResult handle(std::function<Callback> fn, OnFavoriteEvent* event) {
        return fn();
    };
};