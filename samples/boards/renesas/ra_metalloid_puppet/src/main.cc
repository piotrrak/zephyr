/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <chrono>
#include <expected>
#include <utility>

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */

using namespace std::literals::chrono_literals;

namespace z {

template <auto ZCall_> [[nodiscard]]
std::expected<void, int> check_(auto && ...args) noexcept {
	int ret = ZCall_(std::forward<decltype(args)>(args)...);

	if (ret < 0) return std::unexpected(ret);

	return {};
}

template <typename R_, typename P_>
auto sleep_for(std::chrono::duration<R_, P_> ival) {
	namespace t = std::chrono;
        auto in_ms = t::duration_cast<t::milliseconds>(ival);

        // FIXME: no clocks, we're sloooow... DIVed by fixup
        constexpr int FUP = 100;
	return k_msleep(in_ms.count() / FUP);
}

template <typename Ty_> requires std::is_array_v<Ty_>
inline constexpr std::size_t array_size(Ty_& a) { return std::extent_v<decltype(a)>; }

}

template <unsigned N_Bits_>
constexpr unsigned BIT_MASK = (0b1U << N_Bits_) - 1U;

namespace {

// Returns next Gray's code for n element c-array.
//
// It starts counting from 0 and maintains it's own counter (state) across calls
//
// NB: Not thread safe
auto next_grays_for = [nat_bin_count = 0U]
	<unsigned N_Bits_> (auto (&) [N_Bits_]) mutable {
   		unsigned next_gray = nat_bin_count ^ (nat_bin_count >> 1U);

		++nat_bin_count &= BIT_MASK<N_Bits_>; // counter mod 2^NBits_ -1;

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
}

int main(void)
{
	if (not gpio_is_ready_dt(color_leds)) goto quit_main;

        for (auto& led: color_leds) {
		auto success = z::check_<gpio_pin_configure_dt>(&led, GPIO_OUTPUT_ACTIVE);

		if (!success) goto quit_main;
        }

        // TODO: Put in thread some day

        for (unsigned prev_gray = 0U, gray = next_grays_for(color_leds) ;; ) {

                for (unsigned i=0U; i < LED_COUNT; ++i) {

                    	auto ith_bit_of = [i](unsigned v) { return (v >> i) &1U; };

                	const bool wont_toggle = (ith_bit_of(gray) == ith_bit_of(prev_gray));

			bool ok = wont_toggle or [&]() noexcept {
				auto ok = z::check_<gpio_pin_toggle_dt>(&color_leds[i]);

				printf("Toggling led %u\n", i);

				return ok;
                        }();

			if (not ok) goto quit_main;
                }

                prev_gray = std::exchange(gray, next_grays_for(color_leds));

		z::sleep_for(1s);
	}

quit_main:
        printk("We're DAED, finally...");
	return 0;
}
