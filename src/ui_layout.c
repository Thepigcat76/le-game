#include "../include/ui_layout.h"

void layout_text(Layout *layout, TextComponent text) {
    GuiComponent *component = &layout->components[layout->components_amount++];
    component->type = COMPONENT_TEXT;
    component->var.text = text;
}

void layout_button(Layout *layout, ButtonComponent button) {
    GuiComponent *component = &layout->components[layout->components_amount++];
    component->type = COMPONENT_BUTTON;
    component->var.button = button;
}

void layout_spacing(Layout *layout, SpacingComponent spacing) {
    GuiComponent *component = &layout->components[layout->components_amount++];
    component->type = COMPONENT_SPACING;
    component->var.spacing = spacing;
}
