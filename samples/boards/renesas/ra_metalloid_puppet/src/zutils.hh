#pragma once

#include <expected>
#include <utility>

namespace z {

template <auto ZCall_> [[nodiscard]]
std::expected<void, int> check_(auto && ...args) noexcept {
	int ret = ZCall_(std::forward<decltype(args)>(args)...);

	if (ret < 0) return std::unexpected(ret);

	return {};
}

}
