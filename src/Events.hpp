#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class OnFavoriteEvent : public Event {
public:
    OnFavoriteEvent() = default;
};

class OnFavoriteEventFilter : public EventFilter<OnFavoriteEvent> {
public:
    using Callback = ListenerResult();

    ListenerResult handle(std::function<Callback> fn, OnFavoriteEvent* event) {
        return fn();
    };
};