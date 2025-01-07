
#include "os.h"
#define num_of_layers 5
#define mask 0x1FF
#define shift 9

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    uint64_t phys_ad = pt << 12;
    uint64_t pte;
    uint64_t *current_table;

    for (int layer = num_of_layers - 1; layer > 0; layer--) {
        current_table = (uint64_t *)phys_to_virt(phys_ad);
        if (!current_table) return;

        pte = (vpn >> (shift * layer)) & mask;

        if (!(current_table[pte] & 1)) {
            if (ppn == NO_MAPPING) return;
            current_table[pte] = (alloc_page_frame() << 12) | 1;
        }

        phys_ad = current_table[pte] & ~0xFFF;
    }

    current_table = (uint64_t *)phys_to_virt(phys_ad);
    if (!current_table) return;

    pte = vpn & mask;
    current_table[pte] = (ppn == NO_MAPPING) ? 0 : (ppn << 12) | 1;
}


uint64_t page_table_query(uint64_t pt, uint64_t vpn) {
    uint64_t phys_ad = pt << 12;
    uint64_t pte;
    uint64_t *current_table;

    for (int layer = num_of_layers - 1; layer >= 0; layer--) {
        current_table = (uint64_t *)phys_to_virt(phys_ad);
        if (!current_table) return NO_MAPPING;

        pte = (vpn >> (shift * layer)) & mask;

        if (!(current_table[pte] & 1)) return NO_MAPPING;

        if (layer == 0) return current_table[pte] >> 12;

        phys_ad = current_table[pte] & ~0xFFF;
    }

    return NO_MAPPING;
}