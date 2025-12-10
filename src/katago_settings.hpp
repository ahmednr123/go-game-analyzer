#ifndef GO_KATAGO_SETTINGS_H
#define GO_KATAGO_SETTINGS_H

#include "json.hpp"

using nlohmann::json;

enum class KataGoLevel {
    LEVEL_1,
    LEVEL_2,
    LEVEL_3,
    LEVEL_4,
    LEVEL_5,

    EVAL
};

inline std::string getLevelString (KataGoLevel level) {
    switch (level) {
    case KataGoLevel::LEVEL_4:
        return "LEVEL_4";
    case KataGoLevel::LEVEL_3:
        return "LEVEL_3";
    case KataGoLevel::LEVEL_2:
        return "LEVEL_2";
    case KataGoLevel::LEVEL_1:
        return "LEVEL_1";
    default:
    case KataGoLevel::LEVEL_5:
        return "LEVEL_5";
    }
}

inline KataGoLevel getLevel (int lvl) {
    switch (lvl) {
    case 4:
        return KataGoLevel::LEVEL_4;
    case 3:
        return KataGoLevel::LEVEL_3;
    case 2:
        return KataGoLevel::LEVEL_2;
    case 1:
        return KataGoLevel::LEVEL_1;
    default:
    case 5:
        return KataGoLevel::LEVEL_5;
    }
};

typedef struct {
    int max_visits;
    float temprature;
    float fpu_red_max;
} KataGoSetting;

class KataGoSettings {
    static KataGoSetting getSetting (KataGoLevel level);

public:
    static void applyDiffLevel (json& req, KataGoLevel level);
    static void applyEvaluationConfig (json& req);

    // Prevent instantiation
    KataGoSettings() = delete;
    KataGoSettings(const KataGoSettings&) = delete;
    KataGoSettings& operator=(const KataGoSettings&) = delete;
};

#endif
