// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#include "emu.h"
#include "ram.h"

#include "bus/msx/slot/cartridge.h"

#include "bus/generic/slot.h"


/*
Emulation of memory expansions for the MSX system.

Standard 16K RAM expansions:
- Addcom ADMSM 301
- Casio OR-216
- General RJ-102
- Hitachi MP-RA01H
- Mitsubishi ML-16RAM
- National CF-2131
- Philips VU 0031
- Sanyo MRP-16
- Sony HBM-16
- Toshiba HX-M250

Standard 32K RAM expansions:
- Addcom ADMSM 302
- Pioneer PX-RA32
- Yamaha URM-01

Standard 48K RAM expansions:
- Addcom ADMSM 303
- Neos RM-48

Standard 64K RAM expansions:
- Casio OR-264
- Philips VU 0034
- Sanyo Memoire Ram 64Ko
- Sanyo MRP-64
- Sharp-Epcom HB-4100
- Sony HBM-64
- Spectravideo SVI-747
- Toshiba HX-M251
- Walther Miller WM 0587

Not supported:
- Fujitsu MB22451 - Fujitsu FM-X only, adds 16K memory and a printer port
- Yamaha SRE-01 - 60pin 32K memory expansion
- Yamaha SRM-01 - 60pin 32K memory expansion and slot expander



256KB Memmory Mapper expansions
- R. & J.Jansen Memory Mapper 256

512KB Memory Mapper expansions
- Green MSX WBM512
- Hardware Partners TM 512KB
- HSH RE-512 MM
- Padial LPE-512KBSRAM-V3
- R. & J.Jansen Memory Mapper 512
- Sony HBM-512

768KB Memory Mapper expansions
- ASCII MEM-768

1MB Memory Mapper expansions
- 8bits4ever MEGA-MAPPER 1024KB
- Digital KC MSX Memory Mapper 1024KB
- Hardware Partners TM 1M
- MK MSX Memory Mapper 1024KB
- MSX Club Gouda Memory Mapper 1MB
- Popolon MSX Memory Mapper

2MB Memory Mapper expansions
- 8bits4ever MEGA-MAPPER 2048KB
- Digital KC MSX Memory Mapper 2048KB
- MFP Extended Memory-2048K
- MK MSX Memory Mapper 2048KB
- MSX Club Gouda Memory Mapper 2MB

4MB Memory Mapper expansions
- 8bits4ever MEGA-MAPPER 4096KB
- Digital KC MSX Memory Mapper 4096KB
- MK MSX Memory Mapper 4096KB
- MSX Club Gouda Memory Mapper 4MB
- Padial LPE-4MB-V42KP
- T.N.S. Addram
- Tecnobytes Double RAM (MegaRam not supported)

Not supported memory mappers:
- 8bits4ever SD-512
- RBSC Carnivore2
- Classic PC 16MB Expansion Memory - 4 x 4MB memory mapper
- GR8NET
- MSX Cartridge Shop MegaFlashROM SCC+ SD
- Padial LPE-4FMB-V8SKP - 4MB memory mapper + MegaRam mode
- Playsonic - 4MB memory mapper, up to 16MB through extra register
- Popolon Musical Memory Mapper - 1MB memory mapper + SN76489AN
- Stichting CODE MCR-025 - 256KB memory mapper + RAM Disk + Printer buffer
- Stichting CODE MCR-051 - 512KB memory mapper + RAM Disk + Printer buffer
- Stichting CODE MCR-076 - 768KB memory mapper + RAM Disk + Printer buffer
- Stichting CODE MCR-102 - 1MB memory mapper + RAM Disk + Printer buffer
- Stichting CODE MCR-204 - 2MB memory mapper + RAM Disk + Printer buffer
*/

DECLARE_DEVICE_TYPE(MSX_CART_16K_RAM,      msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_32K_RAM,      msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_48K_RAM,      msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_64K_RAM,      msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_256K_MM_RAM,  msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_512K_MM_RAM,  msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_768K_MM_RAM,  msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_1024K_MM_RAM, msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_2048K_MM_RAM, msx_cart_interface)
DECLARE_DEVICE_TYPE(MSX_CART_4096K_MM_RAM, msx_cart_interface)

void msx_cart_ram_register_options(device_slot_interface &device)
{
	device.option_add("ram16k", MSX_CART_16K_RAM);
	device.option_add("ram32k", MSX_CART_32K_RAM);
	device.option_add("ram48k", MSX_CART_48K_RAM);
	device.option_add("ram64k", MSX_CART_64K_RAM);
	device.option_add("mm256k", MSX_CART_256K_MM_RAM);
	device.option_add("mm512k", MSX_CART_512K_MM_RAM);
	device.option_add("mm768k", MSX_CART_768K_MM_RAM);
	device.option_add("mm1024k", MSX_CART_1024K_MM_RAM);
	device.option_add("mm2048k", MSX_CART_2048K_MM_RAM);
	device.option_add("mm4096k", MSX_CART_4096K_MM_RAM);
}

namespace {

class msx_cart_base_ram_device : public device_t, public msx_cart_interface
{
protected:
	msx_cart_base_ram_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, u32 ram_size)
		: device_t(mconfig, type, tag, owner, clock)
		, msx_cart_interface(mconfig, *this)
		, m_ram_size(ram_size)
	{ }

	virtual void device_start() override
	{
		m_ram = std::make_unique<u8[]>(m_ram_size);
		save_pointer(NAME(m_ram), m_ram_size);
	}

	u8 *ram_data()
	{
		return m_ram.get();
	}

	u32 get_ram_size() const
	{
		return m_ram_size;
	}

private:
	std::unique_ptr<u8[]> m_ram;
	const u32 m_ram_size;
};


class msx_cart_16k_ram_device : public msx_cart_base_ram_device
{
public:
	msx_cart_16k_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_ram_device(mconfig, MSX_CART_16K_RAM, tag, owner, clock, 16 * 1024)
	{ }

protected:
	virtual void device_start() override
	{
		msx_cart_base_ram_device::device_start();
		page(2)->install_ram(0x8000, 0xbfff, ram_data());
	}
};

class msx_cart_32k_ram_device : public msx_cart_base_ram_device
{
public:
	msx_cart_32k_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_ram_device(mconfig, MSX_CART_32K_RAM, tag, owner, clock, 32 * 1024)
	{ }

protected:
	virtual void device_start() override
	{
		msx_cart_base_ram_device::device_start();
		page(0)->install_ram(0x0000, 0x3fff, ram_data());
		page(1)->install_ram(0x4000, 0x7fff, ram_data() + 0x4000);
	}
};

class msx_cart_48k_ram_device : public msx_cart_base_ram_device
{
public:
	msx_cart_48k_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_ram_device(mconfig, MSX_CART_48K_RAM, tag, owner, clock, 48 * 1024)
	{ }

protected:
	virtual void device_start() override
	{
		msx_cart_base_ram_device::device_start();
		page(0)->install_ram(0x0000, 0x3fff, ram_data());
		page(1)->install_ram(0x4000, 0x7fff, ram_data() + 0x4000);
		page(2)->install_ram(0x8000, 0xbfff, ram_data() + 0x8000);
	}
};

class msx_cart_64k_ram_device : public msx_cart_base_ram_device
{
public:
	msx_cart_64k_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_ram_device(mconfig, MSX_CART_64K_RAM, tag, owner, clock, 64 * 1024)
	{ }

protected:
	virtual void device_start() override
	{
		msx_cart_base_ram_device::device_start();
		page(0)->install_ram(0x0000, 0x3fff, ram_data());
		page(1)->install_ram(0x4000, 0x7fff, ram_data() + 0x4000);
		page(2)->install_ram(0x8000, 0xbfff, ram_data() + 0x8000);
		page(3)->install_ram(0xc000, 0xffff, ram_data() + 0xc000);
	}
};


class msx_cart_base_mm_ram_device : public msx_cart_base_ram_device
{
protected:
	msx_cart_base_mm_ram_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, u32 ram_size)
		: msx_cart_base_ram_device(mconfig, type, tag, owner, clock, ram_size)
		, m_rambank(*this, "rambank%u", 0U)
		, m_bank_mask(0)
	{ }

	virtual void device_start() override;

	template <int Bank> void bank_w(u8 data);

private:
	memory_bank_array_creator<4> m_rambank;
	u8 m_bank_mask;
};

void msx_cart_base_mm_ram_device::device_start()
{
	msx_cart_base_ram_device::device_start();

	m_bank_mask = device_generic_cart_interface::map_non_power_of_two(
			unsigned(get_ram_size() / 0x4000),
			[this] (unsigned entry, unsigned page)
			{
				for (int i = 0; i < 4; i++)
					m_rambank[i]->configure_entry(entry, ram_data() + 0x4000 * page);
			}
	);

	// The MSX system allows multiple devices to react to I/O, so we use taps.
	io_space().install_write_tap(0xfc, 0xfc, "bank0", [this] (offs_t, u8& data, u8){ this->bank_w<0>(data); });
	io_space().install_write_tap(0xfd, 0xfd, "bank1", [this] (offs_t, u8& data, u8){ this->bank_w<1>(data); });
	io_space().install_write_tap(0xfe, 0xfe, "bank2", [this] (offs_t, u8& data, u8){ this->bank_w<2>(data); });
	io_space().install_write_tap(0xff, 0xff, "bank3", [this] (offs_t, u8& data, u8){ this->bank_w<3>(data); });

	page(0)->install_readwrite_bank(0x0000, 0x3fff, m_rambank[0]);
	page(1)->install_readwrite_bank(0x4000, 0x7fff, m_rambank[1]);
	page(2)->install_readwrite_bank(0x8000, 0xbfff, m_rambank[2]);
	page(3)->install_readwrite_bank(0xc000, 0xffff, m_rambank[3]);

	m_rambank[0]->set_entry(3);
	m_rambank[1]->set_entry(2);
	m_rambank[2]->set_entry(1);
	m_rambank[3]->set_entry(0);
}

template <int Bank>
void msx_cart_base_mm_ram_device::bank_w(u8 data)
{
	m_rambank[Bank]->set_entry(data & m_bank_mask);
}


class msx_cart_256k_mm_ram_device : public msx_cart_base_mm_ram_device
{
public:
	msx_cart_256k_mm_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_mm_ram_device(mconfig, MSX_CART_256K_MM_RAM, tag, owner, clock, 256 * 1024)
	{ }
};

class msx_cart_512k_mm_ram_device : public msx_cart_base_mm_ram_device
{
public:
	msx_cart_512k_mm_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_mm_ram_device(mconfig, MSX_CART_512K_MM_RAM, tag, owner, clock, 512 * 1024)
	{ }
};

class msx_cart_768k_mm_ram_device : public msx_cart_base_mm_ram_device
{
public:
	msx_cart_768k_mm_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_mm_ram_device(mconfig, MSX_CART_768K_MM_RAM, tag, owner, clock, 768 * 1024)
	{ }
};

class msx_cart_1024k_mm_ram_device : public msx_cart_base_mm_ram_device
{
public:
	msx_cart_1024k_mm_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_mm_ram_device(mconfig, MSX_CART_1024K_MM_RAM, tag, owner, clock, 1024 * 1024)
	{ }
};

class msx_cart_2048k_mm_ram_device : public msx_cart_base_mm_ram_device
{
public:
	msx_cart_2048k_mm_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_mm_ram_device(mconfig, MSX_CART_2048K_MM_RAM, tag, owner, clock, 2048 * 1024)
	{ }
};

class msx_cart_4096k_mm_ram_device : public msx_cart_base_mm_ram_device
{
public:
	msx_cart_4096k_mm_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: msx_cart_base_mm_ram_device(mconfig, MSX_CART_4096K_MM_RAM, tag, owner, clock, 4096 * 1024)
	{ }
};

} // anonymous namespace


DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_16K_RAM,      msx_cart_interface, msx_cart_16k_ram_device,      "msx_cart_16k_ram",      "Generic MSX 16K RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_32K_RAM,      msx_cart_interface, msx_cart_32k_ram_device,      "msx_cart_32k_ram",      "Generic MSX 32K RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_48K_RAM,      msx_cart_interface, msx_cart_48k_ram_device,      "msx_cart_48k_ram",      "Generic MSX 48K RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_64K_RAM,      msx_cart_interface, msx_cart_64k_ram_device,      "msx_cart_64k_ram",      "Generic MSX 64K RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_256K_MM_RAM,  msx_cart_interface, msx_cart_256k_mm_ram_device,  "msx_cart_256k_mm_ram",  "Generic MSX 256K MM RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_512K_MM_RAM,  msx_cart_interface, msx_cart_512k_mm_ram_device,  "msx_cart_512k_mm_ram",  "Generic MSX 512K MM RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_768K_MM_RAM,  msx_cart_interface, msx_cart_768k_mm_ram_device,  "msx_cart_768k_mm_ram",  "Generic MSX 768K MM RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_1024K_MM_RAM, msx_cart_interface, msx_cart_1024k_mm_ram_device, "msx_cart_1024k_mm_ram", "Generic MSX 1024K MM RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_2048K_MM_RAM, msx_cart_interface, msx_cart_2048k_mm_ram_device, "msx_cart_2048k_mm_ram", "Generic MSX 2048K MM RAM Expansion")
DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_4096K_MM_RAM, msx_cart_interface, msx_cart_4096k_mm_ram_device, "msx_cart_4096k_mm_ram", "Generic MSX 4096K MM RAM Expansion")
