#pragma once

#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class FavoriteEvent : public Event {
public:
    FavoriteEvent() = default;
};

class FavoriteEventFilter : public EventFilter<FavoriteEvent> {
public:
    using Callback = ListenerResult(FavoriteEvent*);

    ListenerResult handle(Function<Callback>& fn, FavoriteEvent* ev) {
        return fn(ev);
    };

    FavoriteEventFilter() = default;
    FavoriteEventFilter(CCNode*) {};
};