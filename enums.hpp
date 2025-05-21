#pragma once
enum class Offset {
    profession = 0x24,
    statistics = 0x2C,
    money = 0x34,
    experience = 0x7C,
};

enum class Professions {
    fighter,
    mage,
    priest,
    thief,
    bishop,
    samurai,
    lord,
    ninja
};

enum class Statistics {
    strength,
    iq,
    piety,
    vitality,
    agility,
    luck
};

enum class MinMaxValues : uint64_t {
    maxClass = 7,
    minStats = 1,
    maxStats = 18,
    numOfStats = 5,
    maxMoneyExperience = 26155415535
};