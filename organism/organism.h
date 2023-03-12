#ifndef ORGANISM_H
#define ORGANISM_H

#include <tuple>
#include <concepts>
#include <cstdint>
#include <optional>
#include <assert.h>

template <typename species_t, bool can_eat_meat, bool can_eat_plants>
requires std::equality_comparable<species_t>
class Organism
{
private:
    const species_t name;
    uint64_t vitality;

public:
    constexpr Organism(const species_t &species, uint64_t vitality) : name(species), vitality(vitality) {}
    constexpr const species_t &get_species() const { return name; }
    constexpr uint64_t get_vitality() const { return vitality; }
    constexpr bool eats_meat() const { return can_eat_meat; }
    constexpr bool eats_plants() const { return can_eat_plants; }
    constexpr void set_vitality(uint64_t new_vitality) { 
        vitality = new_vitality; 
    }
    constexpr bool is_plant() const { 
        return !can_eat_meat && !can_eat_plants;
    }
    constexpr bool is_dead() const {
        return vitality == 0;
    }
};

template <typename species_t>
using Omnivore = Organism<species_t, true, true>;

template <typename species_t>
using Herbivore = Organism<species_t, false, true>;

template <typename species_t>
using Carnivore = Organism<species_t, true, false>;

template <typename species_t>
using Plant = Organism<species_t, false, false>;

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, bool sp2_eats_m, bool sp2_eats_p>
constexpr std::tuple<Organism<species_t, sp1_eats_m, sp1_eats_p>,
                     Organism<species_t, sp2_eats_m, sp2_eats_p>,
                     std::optional<Organism<species_t, sp1_eats_m, sp1_eats_p>>>
encounter(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
          Organism<species_t, sp2_eats_m, sp2_eats_p> organism2) {

    // Plant encountering plant
    static_assert(!(organism1.is_plant() && organism2.is_plant()));

    const uint64_t vitality1 = organism1.get_vitality();
    const uint64_t vitality2 = organism2.get_vitality();
    
    // Encountering dead
    if (vitality1 == 0 || vitality2 == 0) {
        return std::make_tuple(organism1, organism2, std::nullopt);
    }

    // Mating
    if ((organism1.get_species() == organism2.get_species()) &&
        (organism1.eats_meat() == organism2.eats_meat()) &&
        (organism1.eats_plants() == organism2.eats_plants())){
        return std::make_tuple(organism1, organism2, 
                    Organism<species_t, sp1_eats_m, sp1_eats_p>(organism1.get_species(),
                            (vitality1 + vitality2) / 2));
    }

    // Fighting
    if (organism1.eats_meat() && organism2.eats_meat()) {
        if (vitality1 == vitality2) {
            organism1.set_vitality(0);
            organism2.set_vitality(0);
        } else if (vitality1 > vitality2) {
            organism1.set_vitality(vitality1 + vitality2 / 2);
            organism2.set_vitality(0);
        } else {
            organism2.set_vitality(vitality2+vitality1 / 2);
            organism1.set_vitality(0);
        }
        return std::make_tuple(organism1, organism2, std::nullopt);
    }

    // Eating plant encounters plant
    if (organism1.eats_plants() && organism2.is_plant()) {
        organism1.set_vitality(vitality1 + vitality2);
        organism2.set_vitality(0);
        return std::make_tuple(organism1, organism2, std::nullopt);
    }
    if (organism2.eats_plants() && organism1.is_plant()) {
        organism2.set_vitality(vitality1 + vitality2);
        organism1.set_vitality(0);
        return std::make_tuple(organism1, organism2, std::nullopt);
    }

    // Eating meat encounters animal
    if (organism1.eats_meat() && !organism2.is_plant() && vitality1 > vitality2) {
        organism1.set_vitality(vitality1+vitality2/2);
        organism2.set_vitality(0);
        return std::make_tuple(organism1, organism2, std::nullopt);
    } 

    if (organism2.eats_meat() && !organism1.is_plant() && vitality2 > vitality1) {
        organism2.set_vitality(vitality2 + vitality1/2);
        organism1.set_vitality(0);
        return std::make_tuple(organism1, organism2, std::nullopt);
    } 
    // Last case, eating meat encounter plant
    // Or meat eater does not have enough vitality to 
    // eat other animal
    return std::make_tuple(organism1, organism2, std::nullopt);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p,
bool sp2_eats_m, bool sp2_eats_p, typename... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1,
Organism<species_t, sp2_eats_m, sp2_eats_p> organism2, Args... args) {
    return encounter_series(std::get<0>(encounter(organism1, organism2)), args...);
}

template <typename species_t, bool sp1_eats_m, bool sp1_eats_p, typename... Args>
constexpr Organism<species_t, sp1_eats_m, sp1_eats_p>
encounter_series(Organism<species_t, sp1_eats_m, sp1_eats_p> organism1) {
    return organism1;
}



#endif /* ORGANISM_H */
