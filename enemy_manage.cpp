#include "enemy_manage.h"
#include "globals.h"
void EnemyManage::spawn_enemies() {
    // Create enemies, incrementing their amount every time a new one is created
    enemies.clear();

    for (size_t row = 0; row < current_level.rows; ++row) {
        for (size_t column = 0; column < current_level.columns; ++column) {
            if (char cell = get_level_cell(row, column); cell == ENEMY) {
                // Instantiate and add an enemy to the level
                enemies.push_back({
                        {static_cast<float>(column), static_cast<float>(row)},
                        true
                });

                set_level_cell(row, column, AIR);
            }
        }
    }
}

void EnemyManage::update_enemies() {
    for (auto &enemy : enemies) {
        // Find the enemy's next x
        float next_x = enemy.get_pos().x;
        next_x += (enemy.is_looking_right() ? ENEMY_MOVEMENT_SPEED : -ENEMY_MOVEMENT_SPEED);

        // If its next position collides with a wall, turn around
        if (is_colliding({next_x, enemy.get_pos().y}, WALL)) {
            enemy.set_looking_right(!enemy.is_looking_right());
        }
        // Otherwise, keep moving
        else {
            enemy.set_pos(Vector2{next_x, enemy.get_pos().y});
        }
    }
}

// Custom is_colliding function for enemies
bool EnemyManage::is_colliding_with_enemies(const Vector2 pos) const {
    Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (auto &enemy : enemies) {
        const Rectangle enemy_hitbox = { enemy.get_pos().x, enemy.get_pos().y, 1.0f, 1.0f};
        if (CheckCollisionRecs(entity_hitbox, enemy_hitbox)) {
            return true;
        }
    }
    return false;
}

void EnemyManage::remove_colliding_enemy(const Vector2 pos) {
    const Rectangle entity_hitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        // Erase a colliding enemy
        if (const Rectangle enemy_hitbox = {(float) it->get_pos().x, (float) it->get_pos().y, 1.0f, 1.0f}; CheckCollisionRecs(entity_hitbox, enemy_hitbox)) {
            enemies.erase(it);

            // Call the function again to remove any remaining colliding enemies
            // Cannot continue as calling erase on a vector invalidates current iterators
            remove_colliding_enemy(pos);
            return;
        }
    }
}