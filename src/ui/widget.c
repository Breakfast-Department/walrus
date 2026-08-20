#include "walrus/ui/widget.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <walrus/renderer/batch.h>
#include <walrus/renderer/renderer.h>

static void widget_add_rule(
	WrWidget* widget,
	const WrStyleRule* rule
)
{
	if (!widget || !rule)
		return;

	WrStyleRule* new_rules = realloc(widget->rules, sizeof(WrStyleRule) * (widget->rule_count + 1));
	if (!new_rules)
		return;

	widget->rules = new_rules;
	widget->rules[widget->rule_count] = *rule;
	widget->rule_count++;
}

static void widget_add_child(
	WrWidget* widget,
	WrElement* child
)
{
	if (!widget || !child)
		return;

	WrElement** new_children = realloc(widget->children, sizeof(WrElement*) * (widget->child_count + 1));
	if (!new_children)
		return;

	widget->children = new_children;
	widget->children[widget->child_count] = child;
	widget->child_count++;
}

static void widget_remove_child(
	WrWidget* widget,
	WrElement* child
)
{
	if (!widget || !child)
		return;

	unsigned int i, dst = 0;
	for (i = 0; i < widget->child_count; ++i) {
		if (widget->children[i] == child)
			continue;
		widget->children[dst++] = widget->children[i];
	}

	widget->child_count = dst;
	if (dst == 0) {
		free(widget->children);
		widget->children = NULL;
	} else {
		WrElement** shrink = realloc(widget->children, sizeof(WrElement*) * dst);
		if (shrink)
			widget->children = shrink;
	}
}

static void widget_set_parent(
	WrWidget* widget,
	WrElement* parent
)
{
	if (!widget)
		return;
	widget->parent = parent;
}

static void default_render(WrWidget* widget, WrRenderSurface* surface)
{
	if (!widget || !surface)
		return;

	WrBatch* batch = wr_batch_create();
	if (!batch)
		return;

	if (widget->style.background.type == WR_BACKGROUND_COLOR) {
		WrColor c = widget->style.background.color;
		float x = widget->style.layout.margin.left + widget->style.layout.padding.left;
		float y = widget->style.layout.margin.top + widget->style.layout.padding.top;
		float w = widget->style.layout.width;
		float h = widget->style.layout.height;
		if (w <= 0.0f) w = 100.0f;
		if (h <= 0.0f) h = 24.0f;

		float radius = widget->style.border.radius;
		if (radius > 0.0f) {
			wr_batch_rounded_rect(batch, x, y, w, h, radius, c);
		} else {
			wr_batch_rect(batch, x, y, w, h, c);
		}
	}

	WrRenderer* renderer = wr_get_renderer();
	if (renderer && renderer->draw_batch) {
		renderer->draw_batch(surface, batch);
	}

	wr_batch_destroy(batch);
}

WrWidget* wr_create_widget(const char* selector)
{
	WrWidget* w = calloc(1, sizeof(WrWidget));
	if (!w)
		return NULL;

	w->rules = NULL;
	w->rule_count = 0;
	w->parent = NULL;
	w->children = NULL;
	w->child_count = 0;

	w->render = default_render;

	w->add_rule = widget_add_rule;
	w->add_child = widget_add_child;
	w->remove_child = widget_remove_child;
	w->set_parent = widget_set_parent;

	if (selector) {
		size_t sel_len = strlen(selector) + 1;
		char* tmp = malloc(sel_len);
		if (!tmp) {
			return w;
		}
		memcpy(tmp, selector, sel_len);
		char* save = tmp;
		char* part = strtok(tmp, "/");
		unsigned int part_index = 0;
		while (part) {
			if (part_index == 0) {
				/* treat first part as id */
				/* store id in style.foreground as placeholder? Not storing for now */
			}
			part = strtok(NULL, "/");
			part_index++;
		}
		free(save);
	}

	return w;
}

void wr_widget_destroy(WrWidget* widget)
{
	if (!widget)
		return;

	free(widget->rules);
	free(widget->children);
	free(widget);
}
