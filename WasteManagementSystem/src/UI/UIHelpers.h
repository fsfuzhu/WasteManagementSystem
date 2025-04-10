// UIHelpers.h
// Utility functions for common UI operations
#pragma once

#include "pch.h"

/**
 * @brief Helper functions for common UI operations
 */
namespace UIHelpers {
    /**
     * @brief Draw a colored text label
     * @param text Text to display
     * @param color Text color (ImVec4)
     */
    inline void DrawColoredText(const char* text, const ImVec4& color) {
        ImGui::TextColored(color, "%s", text);
    }

    /**
     * @brief Draw a labeled value with automatic columns
     * @param label Label text
     * @param value Value to display
     * @param format Printf-style format string
     */
    inline void DrawLabeledValue(const char* label, float value, const char* format = "%.2f") {
        ImGui::Text("%s:", label);
        ImGui::NextColumn();
        ImGui::Text(format, value);
        ImGui::NextColumn();
    }

    /**
     * @brief Draw a labeled value with automatic columns and custom color
     * @param label Label text
     * @param value Value to display
     * @param color Text color for the value
     * @param format Printf-style format string
     */
    inline void DrawLabeledColoredValue(const char* label, float value,
        const ImVec4& color, const char* format = "%.2f") {
        ImGui::Text("%s:", label);
        ImGui::NextColumn();
        ImGui::TextColored(color, format, value);
        ImGui::NextColumn();
    }

    /**
     * @brief Draw a section header with separator
     * @param text Header text
     */
    inline void DrawSectionHeader(const char* text) {
        ImGui::Separator();
        ImGui::Text("%s", text);
        ImGui::Separator();
    }

    /**
     * @brief Create a tooltip when item is hovered
     * @param text Tooltip text
     */
    inline void AddTooltip(const char* text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", text);
            ImGui::EndTooltip();
        }
    }

    /**
     * @brief Draw a progress bar with optional text overlay
     * @param fraction Progress fraction (0.0-1.0)
     * @param text Optional text to display (nullptr for automatic percentage)
     * @param size Bar size (width, height), use -1 for default
     */
    inline void DrawProgressBar(float fraction, const char* text = nullptr,
        const ImVec2& size = ImVec2(-1, 0)) {
        ImGui::ProgressBar(fraction, size, text);
    }

    /**
     * @brief Draw a button with confirmation dialog
     * @param label Button label
     * @param confirmMsg Confirmation message
     * @param callback Function to call if confirmed
     * @return True if action was confirmed and executed
     */
    inline bool DrawConfirmButton(const char* label, const char* confirmMsg,
        std::function<void()> callback) {
        if (ImGui::Button(label)) {
            ImGui::OpenPopup("Confirm Action");
        }

        bool confirmed = false;
        if (ImGui::BeginPopupModal("Confirm Action", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", confirmMsg);
            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                callback();
                confirmed = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("No", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return confirmed;
    }
}