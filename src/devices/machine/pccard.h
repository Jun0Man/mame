// license:BSD-3-Clause
// copyright-holders:smf
#ifndef MAME_MACHINE_PCCARD_H
#define MAME_MACHINE_PCCARD_H

#pragma once

class pccard_slot_device;

class device_pccard_interface : public device_interface
{
public:
	virtual uint16_t read_memory(offs_t offset, uint16_t mem_mask = ~0);
	virtual uint16_t read_reg(offs_t offset, uint16_t mem_mask = ~0);
	virtual void write_memory(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	virtual void write_reg(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	virtual ~device_pccard_interface() {}

protected:
	device_pccard_interface(const machine_config &mconfig, device_t &device);

	pccard_slot_device *const m_slot;
};

DECLARE_DEVICE_TYPE(PCCARD_SLOT, pccard_slot_device)

class pccard_slot_device : public device_t, public device_single_card_slot_interface<device_pccard_interface>
{
public:
	template <typename T>
	pccard_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, T &&opts, char const *dflt)
		: pccard_slot_device(mconfig, tag, owner, (uint32_t)0)
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	pccard_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// callbacks
	auto card_detect_cb() { return m_card_detect_cb.bind(); }
	auto battery_voltage_1_cb() { return m_battery_voltage_1_cb.bind(); }
	auto battery_voltage_2_cb() { return m_battery_voltage_2_cb.bind(); }
	auto write_protect_cb() { return m_write_protect_cb.bind(); }

	// called from card device
	DECLARE_WRITE_LINE_MEMBER( card_detect_w ) { m_card_detect_cb(state); }
	DECLARE_WRITE_LINE_MEMBER( battery_voltage_1_w ) { m_battery_voltage_1_cb(state); }
	DECLARE_WRITE_LINE_MEMBER( battery_voltage_2_w ) { m_battery_voltage_2_cb(state); }
	DECLARE_WRITE_LINE_MEMBER( write_protect_w ) { m_write_protect_cb(state); }

	uint16_t read_memory(offs_t offset, uint16_t mem_mask = ~0);
	uint16_t read_reg(offs_t offset, uint16_t mem_mask = ~0);
	void write_memory(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void write_reg(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	uint16_t read_memory_swap(offs_t offset, uint16_t mem_mask = 0xffff) { return swapendian_int16(read_memory(offset, swapendian_int16(mem_mask))); }
	uint16_t read_reg_swap(offs_t offset, uint16_t mem_mask = 0xffff) { return swapendian_int16(read_reg(offset, swapendian_int16(mem_mask))); }
	void write_memory_swap(offs_t offset, uint16_t data, uint16_t mem_mask = 0xffff) { write_memory(offset, swapendian_int16(data), swapendian_int16(mem_mask)); }
	void write_reg_swap(offs_t offset, uint16_t data, uint16_t mem_mask = 0xffff) { write_reg(offset, swapendian_int16(data), swapendian_int16(mem_mask)); }

protected:
	virtual void device_start() override;

private:
	devcb_write_line m_card_detect_cb;
	devcb_write_line m_battery_voltage_1_cb;
	devcb_write_line m_battery_voltage_2_cb;
	devcb_write_line m_write_protect_cb;

	// internal state
	device_pccard_interface *m_pccard;
};

#endif // MAME_MACHINE_PCCARD_H
