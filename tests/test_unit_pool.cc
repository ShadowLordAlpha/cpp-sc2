#include <iostream>

#include "sc2api/sc2_unit.h"

int main() {
    sc2::UnitPool pool;

    sc2::Unit* first = pool.CreateUnit(1);
    if (!first) {
        std::cerr << "CreateUnit returned null\n";
        return 1;
    }
    if (first->User() != nullptr) {
        std::cerr << "new unit user pointer must be null\n";
        return 1;
    }

    int marker = 42;
    first->SetUser(&marker);

    sc2::Unit* same = pool.CreateUnit(1);
    if (same != first) {
        std::cerr << "same tag must reuse the Unit\n";
        return 1;
    }
    if (same->User() != &marker) {
        std::cerr << "user pointer must survive CreateUnit for a living tag\n";
        return 1;
    }

    first->alliance = sc2::Unit::Alliance::Enemy;
    pool.ClearExisting();
    if (pool.GetUnit(1) != first || first->User() != &marker) {
        std::cerr << "units that leave vision without MarkDead must stay cached\n";
        return 1;
    }
    pool.SweepDead(10000);
    if (pool.GetUnit(1) != first || first->User() != &marker) {
        std::cerr << "SweepDead must not recycle units without a death event\n";
        return 1;
    }

    first->alliance = sc2::Unit::Alliance::Self;
    sc2::Unit* taken = pool.CreateUnit(1);
    if (taken != first || !taken->is_alive || taken->User() != &marker) {
        std::cerr << "mind-controlled unit must keep its tag, slot, and user pointer\n";
        return 1;
    }

    pool.MarkDead(1, 10);
    if (pool.GetUnit(1) != first || first->is_alive) {
        std::cerr << "confirmed-dead unit must stay GetUnit-able until the grace period\n";
        return 1;
    }
    pool.SweepDead(10);
    if (pool.GetUnit(1) != first) {
        std::cerr << "must not recycle on the death frame\n";
        return 1;
    }
    pool.SweepDead(10 + sc2::UnitPool::kDeadUnitCacheLoops - 1);
    if (pool.GetUnit(1) != first) {
        std::cerr << "must not recycle before kDeadUnitCacheLoops\n";
        return 1;
    }
    pool.SweepDead(10 + sc2::UnitPool::kDeadUnitCacheLoops);
    if (pool.GetUnit(1) != nullptr) {
        std::cerr << "confirmed-dead unit must leave the cache after the grace period\n";
        return 1;
    }

    sc2::Unit* recycled = pool.CreateUnit(2);
    if (recycled != first) {
        std::cerr << "CreateUnit should reuse the confirmed-dead slot\n";
        return 1;
    }
    if (!recycled->is_alive || recycled->tag != 2 || recycled->User() != nullptr) {
        std::cerr << "recycled slot must be reset for the new tag\n";
        return 1;
    }

    std::cout << "test_unit_pool succeeded.\n";
    return 0;
}
