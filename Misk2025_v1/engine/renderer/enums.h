#pragma once


enum class Loading_state {
    AWAITING_LOADING_FROM_DISK,
    LOADING_FROM_DISK,
    LOADING_COMPLETE
};

enum class Baking_state {
    AWAITING_BAKE,
    BAKE_COMPLETE
};