/*
 * This file is part of libsidplayfp, a SID player engine.
 *
 * Copyright 2013-2015 Leandro Nini <drfiemost@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef C64SID_H
#define C64SID_H

#include "Banks/Bank.h"

#include "sidcxx11.h"

#include <stdint.h>

namespace libsidplayfp
{

/**
 * SID interface.
 */
class c64sid : public Bank
{
protected:
    c64sid(void) : lastpoke{0} {};
    virtual ~c64sid() {}

    virtual uint8_t read(uint_least8_t addr) = 0;
    virtual void write(uint_least8_t addr, uint8_t data) = 0;

    uint8_t gatetoggles;
    uint8_t synctoggles;
    uint8_t testtoggles;

public:
    virtual void reset(uint8_t volume) = 0;

    void reset() { reset(0); }

    // Bank functions
    uint8_t lastpoke[0x20];
    void poke(uint_least16_t address, uint8_t value) override
    {
        address &= 0x1f;
        if ((address==0x04) && ((value ^ lastpoke[0x04]) & 0x01)) { gatetoggles|=0x01<<(value&(0x01)); } // GATE toggle for voice 1
        if ((address==0x0b) && ((value ^ lastpoke[0x0b]) & 0x01)) { gatetoggles|=0x04<<(value&(0x01)); } // GATE toggle for voice 2
        if ((address==0x12) && ((value ^ lastpoke[0x12]) & 0x01)) { gatetoggles|=0x10<<(value&(0x01)); } // GATE toggle for voice 3

        if ((address==0x04) && ((value ^ lastpoke[0x04]) & 0x02)) { synctoggles|=0x01<<(value&(0x02)); } // SYNC toggle for voice 1
        if ((address==0x0b) && ((value ^ lastpoke[0x0b]) & 0x02)) { synctoggles|=0x04<<(value&(0x02)); } // SYNC toggle for voice 2
        if ((address==0x12) && ((value ^ lastpoke[0x12]) & 0x02)) { synctoggles|=0x10<<(value&(0x02)); } // SYNC toggle for voice 3

        if ((address==0x04) && ((value ^ lastpoke[0x04]) & 0x04)) { synctoggles|=0x01<<(value&(0x04)); } // TEST toggle for voice 1
        if ((address==0x0b) && ((value ^ lastpoke[0x0b]) & 0x04)) { synctoggles|=0x04<<(value&(0x04)); } // TEST toggle for voice 2
        if ((address==0x12) && ((value ^ lastpoke[0x12]) & 0x04)) { synctoggles|=0x10<<(value&(0x04)); } // TEST toggle for voice 3

        lastpoke[address] = value;
        write(address & 0x1f, value);
    }
    // 1  = Voice 1 GATE has toggled into LOW state
    // 2  = Voice 1 GATE has toggled into HIGH state
    // 4  = Voice 2 GATE has toggled into LOW state
    // 8  = Voice 2 GATE has toggled into HIGH state
    // 16 = Voice 3 GATE has toggled into LOW state
    // 32 = Voice 3 GATE has toggled into HIGH state
    void getToggles(uint8_t& gates, uint8_t& sync, uint8_t& test)
    {
        gates = gatetoggles;
        sync = synctoggles;
        test = testtoggles;
    }
    uint8_t peek(uint_least16_t address) override { return read(address & 0x1f); }

    virtual void GetVolumes(uint8_t &a, uint8_t &b, uint8_t &c) const = 0;
};

}

#endif // C64SID_H
