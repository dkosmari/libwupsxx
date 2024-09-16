/*
 * libwupsxx - A C++ wrapper for libwups.
 *
 * Copyright (C) 2024  Daniel K. O.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef WUPSXX_BUTTON_COMBO_HPP
#define WUPSXX_BUTTON_COMBO_HPP

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <padscore/wpad.h>
#include <vpad/input.h>


struct WPADStatus;


namespace wups::utils {


    struct button_combo;


    namespace detail {

        struct button_state_16 {
            std::uint16_t hold    = 0;
            std::uint16_t trigger = 0;
            std::uint16_t release = 0;
        };

        struct button_state_32 {
            std::uint32_t hold    = 0;
            std::uint32_t trigger = 0;
            std::uint32_t release = 0;
        };

    } // namespace wups::utils::detail


    namespace vpad {

        struct button_set {
            // combination of VPAD_BUTTON_* values.
            std::uint32_t buttons = 0;

            constexpr
            button_set() noexcept = default;

            template<std::same_as<VPADButtons>... Bs>
            constexpr
            button_set(Bs... bs)
                noexcept :
                buttons{ (std::uint32_t{bs} | ...) }
            {}

            explicit
            button_set(const std::vector<std::string_view>& args);
        };


        std::string
        to_string(const button_set& bs);


        std::string
        to_glyph(const button_set& bs, bool prefix = true);


        // Call this from your VPADRead() hook.
        // Return true if no error.
        bool update(VPADChan channel, const VPADStatus& status) noexcept;


        [[nodiscard]]
        bool triggered(VPADChan channel, const button_combo& combo) noexcept;

    } // namespace wups::utils::vpad


    namespace wpad {

        namespace core {

            struct button_set {
                // combination of WPAD_BUTTON_*
                std::uint16_t buttons = 0;

                constexpr
                button_set() noexcept = default;

                template<std::same_as<WPADButton>... Bs>
                constexpr
                button_set(Bs... bs)
                    noexcept :
                    buttons{ (std::uint16_t{bs} | ...) }
                {}

            };


            std::string
            to_string(const button_set& bs);


            std::string
            to_glyph(const button_set& bs);

        } // namespace wups::utils::wpad::core


        namespace nunchuk {

            struct button_set {
                // combination of WPAD_NUNCHUK_BUTTON_Z and WPAD_NUNCHUK_BUTTON_C
                std::uint16_t buttons = 0;

                constexpr
                button_set() noexcept = default;

                template<std::same_as<WPADNunchukButton>... Bs>
                constexpr
                button_set(Bs... bs)
                    noexcept :
                    buttons{ (std::uint16_t{bs} | ...) }
                {}
            };


            std::string
            to_string(const button_set& bs);


            std::string
            to_glyph(const button_set& bs);

        } // namespace wups::utils::wpad::nunchuk


        namespace classic {

            struct button_set {
                // combination of WPAD_CLASSIC_BUTTON_*
                std::uint16_t buttons = 0;

                constexpr
                button_set() noexcept = default;

                template<std::same_as<WPADClassicButton>... Bs>
                constexpr
                button_set(Bs... bs)
                    noexcept :
                    buttons{ (std::uint16_t{bs} | ...) }
                {}
            };


            std::string
            to_string(const button_set& bs);


            std::string
            to_glyph(const button_set& bs);

        } // namespace wups::utils::wpad::classic


        namespace pro {

            struct button_set {
                // combination of WPAD_PRO_BUTTON_*
                std::uint32_t buttons = 0;

                constexpr
                button_set() noexcept = default;

                template<std::same_as<WPADProButton>... Bs>
                constexpr
                button_set(Bs... bs)
                    noexcept :
                    buttons{ (std::uint32_t{bs} | ...) }
                {}
            };


            std::string
            to_string(const button_set& bs);


            std::string
            to_glyph(const button_set& bs);

        } // namespace wups::utils::wpad::pro


        using ext_button_set = std::variant<std::monostate,
                                            nunchuk::button_set,
                                            classic::button_set,
                                            pro::button_set>;


        struct button_set {
            core::button_set core;
            ext_button_set ext;

            constexpr
            button_set() noexcept = default;

            constexpr
            button_set(const core::button_set& core,
                       const ext_button_set& ext)
                noexcept :
                core{core},
                ext{ext}
            {}

            button_set(const std::vector<std::string_view>& args);
        };


        std::string
        to_string(const button_set& bs);

        std::string
        to_glyph(const button_set& bs, bool prefix = true);


        // Call this from your WPADRead() hook.
        // Return true if not error.
        bool update(WPADChan channel, const WPADStatus* status) noexcept;


        [[nodiscard]]
        bool triggered(WPADChan channel, const button_combo& combo) noexcept;


        struct core_button_state    : detail::button_state_16 {};
        struct nunchuk_button_state : detail::button_state_16 {};
        struct classic_button_state : detail::button_state_16 {};
        struct pro_button_state     : detail::button_state_32 {};

        using ext_button_state = std::variant<std::monostate,
                                              nunchuk_button_state,
                                              classic_button_state,
                                              pro_button_state>;

        struct button_state {
            core_button_state core;
            ext_button_state  ext;
        };


        // Retrieve the button state as it's tracked internally to detect combos.
        [[nodiscard]]
        const button_state& get_button_state(WPADChan channel);


    } // namespace wups::utils::wpad


    struct button_combo : std::variant<std::monostate,
                                       vpad::button_set,
                                       wpad::button_set> {

        using parent = std::variant<std::monostate, vpad::button_set, wpad::button_set>;

        constexpr
        button_combo() noexcept = default;

        // inherit constructors
        using parent::parent;

        explicit
        button_combo(const std::string& str);

    };


    std::string to_string(const button_combo& bc);

    std::string to_glyph(const button_combo& bc, bool prefix = true);

} // namespace wups::utils

#endif
