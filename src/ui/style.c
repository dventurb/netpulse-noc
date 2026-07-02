#include "style.h"

static const char *CSS_FILE_PATH = "styles/style.css";

void load_styles(void)
{
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, CSS_FILE_PATH);
  gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), 800);

  g_object_unref(provider);
}
