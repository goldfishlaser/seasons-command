
  /*                          
 
..####...######...####....####....####...##..##...####..
.##......##......##..##..##......##..##..###.##..##.....
..####...####....######...####...##..##..##.###...####..
.....##..##......##..##......##..##..##..##..##......##.
..####...######..##..##...####....####...##..##...####..
........................................................

GNU General Public License v3.0 
Author: Melanie Allen

*/ 


// season.c — display the current Japanese micro-season (七十二候)
// Source file must be saved as UTF-8 for Japanese characters to work.
// Build: clang -Wall -O0 -g -o build/season season.c
// Usage: ./season          (uses today's date)
//        ./season 2026-03-22  (uses given date)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int month;
    int day;
    int number;               // 1–72 in traditional order (1 = ~Jan 5)
    const char *solar_term_jp;
    const char *solar_term_en;
    const char *name_jp;
    const char *name_en;
    const char *description;
} MicroSeason;

// Ordered by Gregorian calendar date.
// Season #72 (雪下出麦) begins Jan 1; traditional season #1 begins ~Jan 5.
// All start dates are approximate (±1–2 days depending on the year).
static const MicroSeason seasons[] = {
    {  1,  1, 72, "冬至", "Winter Solstice",      "雪下出麦",  "Wheat Sprouts Under Snow",        "Wheat begins to sprout beneath the snow" },
    {  1,  5,  1, "小寒", "Lesser Cold",           "芹乃栄",    "Water Parsley Flourishes",        "Water parsley grows thick along streams" },
    {  1, 10,  2, "小寒", "Lesser Cold",           "水泉動",    "Springs Thaw",                    "Frozen springs begin to flow once more" },
    {  1, 15,  3, "小寒", "Lesser Cold",           "雉始雊",    "Pheasants Call",                  "Male pheasants begin their mating calls" },
    {  1, 20,  4, "大寒", "Greater Cold",          "款冬華",    "Butterburs Bud",                  "Butterbur buds push up through frozen ground" },
    {  1, 25,  5, "大寒", "Greater Cold",          "水沢腹堅",  "Ice Thickens on Streams",         "Ice grows thick over streams and ponds" },
    {  1, 30,  6, "大寒", "Greater Cold",          "鶏始乳",    "Hens Start Laying",               "Hens begin laying eggs again" },
    {  2,  4,  7, "立春", "Start of Spring",       "東風解凍",  "East Winds Melt Ice",             "Warm easterly winds begin to thaw the ice" },
    {  2,  9,  8, "立春", "Start of Spring",       "黄鶯睍睆",  "Bush Warblers Sing",              "Bush warblers begin their song in the mountains" },
    {  2, 14,  9, "立春", "Start of Spring",       "魚上氷",    "Fish Emerge from Ice",            "Fish rise toward cracks in the ice" },
    {  2, 19, 10, "雨水", "Rain Water",            "土脉潤起",  "Soil Grows Moist",                "The soil absorbs moisture as the rains begin" },
    {  2, 24, 11, "雨水", "Rain Water",            "霞始靆",    "Mist Begins to Linger",           "Morning mist drifts and lingers over the land" },
    {  3,  1, 12, "雨水", "Rain Water",            "草木萌動",  "Plants Begin to Sprout",          "Buds and shoots begin to stir on trees and grass" },
    {  3,  6, 13, "啓蟄", "Awakening of Insects",  "蟄虫啓戸",  "Insects Awaken",                  "Hibernating insects begin to emerge from the earth" },
    {  3, 11, 14, "啓蟄", "Awakening of Insects",  "桃始笑",    "First Peach Blossoms",            "Peach trees bloom with their first flowers" },
    {  3, 16, 15, "啓蟄", "Awakening of Insects",  "菜虫化蝶",  "Caterpillars Become Butterflies", "Caterpillars transform and emerge as butterflies" },
    {  3, 21, 16, "春分", "Vernal Equinox",        "雀始巣",    "Sparrows Start Nesting",          "Sparrows begin to build their nests" },
    {  3, 26, 17, "春分", "Vernal Equinox",        "桜始開",    "First Cherry Blossoms",           "The first cherry blossoms open" },
    {  3, 31, 18, "春分", "Vernal Equinox",        "雷乃発声",  "First Thunder",                   "The first thunder of spring rumbles across the sky" },
    {  4,  5, 19, "清明", "Clear and Bright",      "玄鳥至",    "Swallows Arrive",                 "Swallows arrive from the south" },
    {  4, 10, 20, "清明", "Clear and Bright",      "鴻雁北",    "Wild Geese Fly North",            "Wild geese depart northward" },
    {  4, 15, 21, "清明", "Clear and Bright",      "虹始見",    "First Rainbows",                  "The first rainbows of the year appear" },
    {  4, 20, 22, "穀雨", "Grain Rain",            "葭始生",    "Reeds Start Growing",             "Reeds begin to grow along the water's edge" },
    {  4, 25, 23, "穀雨", "Grain Rain",            "霜止出苗",  "Frost Ends, Seedlings Grow",      "Frost disappears and rice seedlings begin to grow" },
    {  4, 30, 24, "穀雨", "Grain Rain",            "牡丹華",    "Peonies Bloom",                   "Peonies burst into full bloom" },
    {  5,  5, 25, "立夏", "Start of Summer",       "蛙始鳴",    "Frogs Start Singing",             "Frogs begin their chorus in the paddies" },
    {  5, 10, 26, "立夏", "Start of Summer",       "蚯蚓出",    "Earthworms Emerge",               "Earthworms emerge from the warming soil" },
    {  5, 15, 27, "立夏", "Start of Summer",       "竹笋生",    "Bamboo Shoots Emerge",            "Bamboo shoots push up rapidly through the ground" },
    {  5, 21, 28, "小満", "Grain Buds",            "蚕起食桑",  "Silkworms Eat Mulberry",          "Silkworms hatch and begin feeding on mulberry leaves" },
    {  5, 26, 29, "小満", "Grain Buds",            "紅花栄",    "Safflowers Bloom",                "Safflowers open in vivid orange and red" },
    {  5, 31, 30, "小満", "Grain Buds",            "麦秋至",    "Wheat Ripens",                    "Wheat reaches the peak of ripeness" },
    {  6,  6, 31, "芒種", "Grain in Ear",          "螳螂生",    "Mantises Hatch",                  "Praying mantis eggs hatch" },
    {  6, 11, 32, "芒種", "Grain in Ear",          "腐草為蛍",  "Fireflies Emerge",                "Fireflies appear, as if risen from the rotting grass" },
    {  6, 16, 33, "芒種", "Grain in Ear",          "梅子黄",    "Plums Turn Yellow",               "Plums ripen and turn from green to yellow" },
    {  6, 21, 34, "夏至", "Summer Solstice",       "乃東枯",    "Self-Heal Withers",               "The self-heal plant withers in the summer heat" },
    {  6, 26, 35, "夏至", "Summer Solstice",       "菖蒲華",    "Irises Bloom",                    "Irises open in purple and white" },
    {  7,  1, 36, "夏至", "Summer Solstice",       "半夏生",    "Crow-Dipper Sprouts",             "The crow-dipper plant spreads across damp ground" },
    {  7,  7, 37, "小暑", "Minor Heat",            "温風至",    "Warm Winds Arrive",               "Hot, humid winds begin to blow" },
    {  7, 12, 38, "小暑", "Minor Heat",            "蓮始開",    "Lotuses Begin to Bloom",          "Lotus flowers open on still ponds" },
    {  7, 17, 39, "小暑", "Minor Heat",            "鷹乃学習",  "Young Hawks Learn to Fly",        "Young hawks practice and learn to soar" },
    {  7, 23, 40, "大暑", "Major Heat",            "桐始結花",  "Paulownias Bear Seeds",           "Paulownia trees begin forming their seed pods" },
    {  7, 28, 41, "大暑", "Major Heat",            "土潤溽暑",  "Earth Drenched in Humid Heat",    "The earth is soaked and the air thick with heat" },
    {  8,  2, 42, "大暑", "Major Heat",            "大雨時行",  "Heavy Rains Fall",                "Sudden heavy rains fall from time to time" },
    {  8,  7, 43, "立秋", "Start of Autumn",       "涼風至",    "Cool Winds Arrive",               "The first cool breezes hint at autumn's coming" },
    {  8, 12, 44, "立秋", "Start of Autumn",       "寒蝉鳴",    "Evening Cicadas Sing",            "Evening cicadas sing their melancholy song" },
    {  8, 17, 45, "立秋", "Start of Autumn",       "蒙霧升降",  "Thick Fog Descends",              "Dense morning fogs roll in over the land" },
    {  8, 23, 46, "処暑", "End of Heat",           "綿柎開",    "Cotton Flowers Bloom",            "Cotton bolls open to reveal their white fibers" },
    {  8, 28, 47, "処暑", "End of Heat",           "天地始粛",  "Heaven and Earth Cool",           "The heat begins to ease as heaven and earth cool" },
    {  9,  2, 48, "処暑", "End of Heat",           "禾乃登",    "Rice Ripens",                     "Rice grains fill and ripen in the paddies" },
    {  9,  8, 49, "白露", "White Dew",             "草露白",    "Dew Glistens on Grass",           "Morning dew clings white and cold to the grass" },
    {  9, 13, 50, "白露", "White Dew",             "鶺鴒鳴",    "Wagtails Sing",                   "Wagtails sing along the water's edge" },
    {  9, 18, 51, "白露", "White Dew",             "玄鳥去",    "Swallows Depart",                 "Swallows gather and fly south for the winter" },
    {  9, 23, 52, "秋分", "Autumnal Equinox",      "雷乃収声",  "Thunder Silences",                "The last thunder of the season fades away" },
    {  9, 28, 53, "秋分", "Autumnal Equinox",      "蟄虫坏戸",  "Insects Burrow Underground",      "Insects seal themselves away underground for winter" },
    { 10,  3, 54, "秋分", "Autumnal Equinox",      "水始涸",    "Water Begins to Dry",             "Streams and pools begin to dry as autumn deepens" },
    { 10,  8, 55, "寒露", "Cold Dew",              "鴻雁来",    "Wild Geese Arrive",               "Wild geese return from the north" },
    { 10, 13, 56, "寒露", "Cold Dew",              "菊花開",    "Chrysanthemums Bloom",            "Chrysanthemums open in gold and white" },
    { 10, 18, 57, "寒露", "Cold Dew",              "蟋蟀在戸",  "Crickets Chirp at the Door",      "Crickets move close to the warmth of homes" },
    { 10, 23, 58, "霜降", "Frost's Descent",       "霜始降",    "First Frost Falls",               "The first frost of autumn settles on the ground" },
    { 10, 28, 59, "霜降", "Frost's Descent",       "霎時施",    "Light Rains Fall",                "Brief showers of cold rain fall from time to time" },
    { 11,  2, 60, "霜降", "Frost's Descent",       "楓蔦黄",    "Maple and Ivy Turn Yellow",       "Maple leaves and ivy vines blaze with autumn color" },
    { 11,  7, 61, "立冬", "Start of Winter",       "山茶始開",  "Camellias Begin to Bloom",        "Camellias open their flowers as winter begins" },
    { 11, 12, 62, "立冬", "Start of Winter",       "地始凍",    "Ground Begins to Freeze",         "The earth begins to freeze in the cold" },
    { 11, 17, 63, "立冬", "Start of Winter",       "金盞香",    "Narcissus Blooms",                "Narcissus flowers bloom with a sweet fragrance" },
    { 11, 22, 64, "小雪", "Minor Snow",            "虹蔵不見",  "Rainbows Hide",                   "Rainbows are no longer seen in the cloudy sky" },
    { 11, 27, 65, "小雪", "Minor Snow",            "朔風払葉",  "North Winds Strip the Leaves",    "Cold north winds blow the last leaves from the trees" },
    { 12,  2, 66, "小雪", "Minor Snow",            "橘始黄",    "Citrus Turns Yellow",             "Tachibana citrus ripens to yellow" },
    { 12,  7, 67, "大雪", "Major Snow",            "閉塞成冬",  "Cold Sets In",                    "The sky closes in and true winter arrives" },
    { 12, 12, 68, "大雪", "Major Snow",            "熊蟄穴",    "Bears Retreat to Dens",           "Bears find their dens and settle in for winter" },
    { 12, 17, 69, "大雪", "Major Snow",            "鱖魚群",    "Salmon Gather",                   "Salmon gather and swim upstream to spawn" },
    { 12, 22, 70, "冬至", "Winter Solstice",       "乃東生",    "Self-Heal Sprouts",               "The self-heal plant sends up new shoots in the cold" },
    { 12, 27, 71, "冬至", "Winter Solstice",       "麋角解",    "Elk Shed Antlers",                "Elk shed their antlers as the year draws to a close" },
};

#define NUM_SEASONS (int)(sizeof(seasons) / sizeof(seasons[0]))

// Returns a pointer to the current micro-season for the given month and day.
// Uses a simple linear scan; with only 72 entries this is fast enough.
static const MicroSeason *get_season(int month, int day) {
    const MicroSeason *result = &seasons[NUM_SEASONS - 1]; // safe fallback
    for (int i = 0; i < NUM_SEASONS; i++) {
        if (seasons[i].month > month ||
           (seasons[i].month == month && seasons[i].day > day)) {
            break;
        }
        result = &seasons[i];
    }
    return result;
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [YYYY-MM-DD]\n", prog);
    fprintf(stderr, "  With no argument, uses today's date.\n");
}

int main(int argc, char *argv[]) {
    int month, day, year;

    if (argc == 1) {
        // Use today's date
        time_t t = time(NULL);
        struct tm *now = localtime(&t);
        year  = now->tm_year + 1900;
        month = now->tm_mon + 1;
        day   = now->tm_mday;
    } else if (argc == 2) {
        if (sscanf(argv[1], "%d-%d-%d", &year, &month, &day) != 3 ||
            month < 1 || month > 12 || day < 1 || day > 31) {
            fprintf(stderr, "Error: invalid date '%s'\n", argv[1]);
            usage(argv[0]);
            return 1;
        }
    } else {
        usage(argv[0]);
        return 1;
    }

    const MicroSeason *s = get_season(month, day);

    printf("Season #%d of 72\n", s->number);
    printf("Solar term:  %s — %s\n", s->solar_term_jp, s->solar_term_en);
    printf("Name:        %s — %s\n", s->name_jp, s->name_en);
    printf("             %s\n", s->description);

    return 0;
}
