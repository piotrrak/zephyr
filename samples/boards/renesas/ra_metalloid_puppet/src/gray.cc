/*
 * Copyright (c) 2024 Piotr Rak
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zutils.hh"

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <chrono>

namespace z {

bool spawn_blinky_gray_task();

template <typename R_, typename P_>
auto sleep_for(std::chrono::duration<R_, P_> ival) {
	namespace t = std::chrono;
        auto in_ms = t::duration_cast<t::milliseconds>(ival);

        // FIXME: no clocks, we're sloooow... DIVed by fixup
        constexpr int FUP = 10;
	return k_msleep(in_ms.count() / FUP);
}

template <typename Ty_> requires std::is_array_v<Ty_>
inline constexpr std::size_t array_size(Ty_& a) { return std::extent_v<decltype(a)>; }

template <unsigned N_Bits_>
constexpr unsigned BIT_MASK = (0b1U << N_Bits_) - 1U;

}

namespace {

using namespace std::literals::chrono_literals;


// Returns next Gray's code for n element c-array.
//
// It starts counting from 0 and maintains it's own counter (state) across calls
//
// NB: Not thread safe
auto next_grays_for = [nat_bin_count = 0U]
	<unsigned N_Bits_> (auto (&) [N_Bits_]) mutable {
   		unsigned next_gray = nat_bin_count ^ (nat_bin_count >> 1U);

		++nat_bin_count &= z::BIT_MASK<N_Bits_>; // counter mod 2^NBits_ -1;

		return next_gray;
};


const gpio_dt_spec color_leds[] = {
#define GIMME_LED(color) GPIO_DT_SPEC_GET(DT_ALIAS(led_##color), gpios)
	GIMME_LED(blue),
	GIMME_LED(green),
	GIMME_LED(red),
#undef GIMME_LED
        };

constexpr auto LED_COUNT = std::extent_v<decltype(color_leds)>;

void blinky_gray_task(void*, void*, void*) {

        // TODO: Put in thread some day

	printf("about to toggle led \n");
        for (unsigned prev_gray = 0U, gray = next_grays_for(color_leds) ;; ) {
                for (unsigned i=0U; i < LED_COUNT; ++i) {

                    	auto ith_bit_of = [i](unsigned v) { return (v >> i) &1U; };

                	const bool wont_toggle = (ith_bit_of(gray) == ith_bit_of(prev_gray));

			bool ok = wont_toggle or [&]() noexcept {
				auto ok = z::check_<gpio_pin_toggle_dt>(&color_leds[i]);

				printf("Toggling led %u\n", i);

				return ok;
                        }();

			if (not ok) goto quit_blinky;
                }

                prev_gray = std::exchange(gray, next_grays_for(color_leds));

		z::sleep_for(1s);
	}
quit_blinky:
        printk("We're DAED, finally...");
}

constexpr int PRIORITY =  7;
constexpr unsigned STACKSIZE = 1024;

K_THREAD_STACK_DEFINE(thread_blink_stack_area, STACKSIZE);
static struct k_thread thread_blink_data;

}

namespace z {

bool spawn_blinky_gray_task() noexcept {

	if (not gpio_is_ready_dt(color_leds)) {
		printf("Error: %s device is not ready\n", color_leds->port->name);
        	return false;
        }

	k_thread_create(&thread_blink_data, thread_blink_stack_area,
			K_THREAD_STACK_SIZEOF(thread_blink_stack_area),
			blinky_gray_task, NULL, NULL, NULL,
			PRIORITY, 0, K_FOREVER);
	k_thread_name_set(&thread_blink_data, "thread_blink_gray");

        printf("Created");
        for (auto& led: color_leds) {
		auto success = z::check_<gpio_pin_configure_dt>(&led, GPIO_OUTPUT_ACTIVE);

		if (!success) return false;
        }

        printf("Set");
	k_thread_start(&thread_blink_data);
        printf("Started");
        return true;
}

}
