#include <stdlib.h>

#define FEATURE_STORE_SIZE 400

typedef struct {
    int village_size;
    int residents;
    int development;
} GameFeatureContentVillage;

typedef struct {
} ForestType;

typedef struct {
    ForestType type;
    int size;
    int rarity;
    int plant_varity;
} GameFeatureContentForest;

typedef struct {
    enum {
        GAME_FEATURE_CONTENT_VILLAGE,
        GAME_FEATURE_CONTENT_FOREST,
    } type;
    union {
        GameFeatureContentVillage village;
        GameFeatureContentForest forest;
    } var;
} GameFeatureContent;

typedef enum {
    GAME_FEATURE_VILLAGE,
    GAME_FEATURE_FOREST,
    GAME_FEATURE_VILLAGE_HOUSE,
    GAME_FEATURE_VILLAGE_HOUSE_RESIDENT,
} GameFeatureId;

typedef struct {
    GameFeatureId id;
    // Other properties
} GameFeatureType;

typedef struct _game_feature {
    GameFeatureType type;
    struct _game_feature *children;
    GameFeatureContent content;
} GameFeature;

static GameFeature FEATURE_STORE[FEATURE_STORE_SIZE];

int main() {
    GameFeature feature = {
        .type = {.id = GAME_FEATURE_FOREST},
        .children = NULL,
        .content = {.type = GAME_FEATURE_CONTENT_FOREST,
                    .var = {.forest = {.size = 100, .plant_varity = 60}}}};
}
