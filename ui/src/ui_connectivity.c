#include "ui_connectivity.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"

static GtkWidget *create_connectivity_side_bar(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_content(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_menu_bar(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_page_ping(ui_connectivity_t *ui_connectivity);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);
static void on_sidebar_button_clicked(GtkButton *button, gpointer data);

GtkWidget *create_page_connectivity(ui_t *ui)
{
  ui_connectivity_t *ui_connectivity = malloc(sizeof(ui_connectivity_t));
  if (ui_connectivity == NULL) return NULL;

  ui_connectivity->application = ui->application;
  ui_connectivity->window = ui->window;

  ui_connectivity->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  ui_connectivity->stack = gtk_stack_new();

  GtkWidget *side_bar = create_connectivity_side_bar(ui_connectivity);

  GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  GtkWidget *header = create_connectivity_header(ui_connectivity);

  gtk_stack_add_named(GTK_STACK(ui_connectivity->stack), create_page_ping(ui_connectivity), "Ping");

  gtk_box_append(GTK_BOX(right_box), header);
  gtk_box_append(GTK_BOX(right_box), ui_connectivity->stack);

  gtk_box_append(GTK_BOX(ui_connectivity->container), side_bar);
  gtk_box_append(GTK_BOX(ui_connectivity->container), right_box);

  gtk_stack_set_visible_child_name(GTK_STACK(ui_connectivity->stack), "Ping");

  g_object_set_data_full(G_OBJECT(ui_connectivity->container), "ui_connectivity", ui_connectivity, free); // ownership + free

  return ui_connectivity->container;
}

static GtkWidget *create_connectivity_side_bar(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  GtkWidget *label = gtk_label_new("NETWORK TOOLS");
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_add_css_class(label, "connectivity-sidebar-label");
  gtk_box_append(GTK_BOX(box), label);

  const char *tools[] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  const char *icons[] = {
    "assets/icon-ping-active.svg",
    "assets/icon-traceroute.svg",
    "assets/icon-dns-lookup.svg",
    "assets/icon-arp-table.svg"
  };

  for (int i = 0; i < 4; i++)
  {
    ui_connectivity->sidebar_buttons[i] = create_primary_button(tools[i], icons[i],"connectivity-sidebar-button"); 
    g_object_set_data(G_OBJECT(ui_connectivity->sidebar_buttons[i]), "target-page", (void *)tools[i]);
    g_signal_connect(ui_connectivity->sidebar_buttons[i], "clicked", G_CALLBACK(on_sidebar_button_clicked), ui_connectivity);

    gtk_box_append(GTK_BOX(box), ui_connectivity->sidebar_buttons[i]);
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->sidebar_buttons[0]), TRUE);

  return box;
}

static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_size_request(box, -1, 117);

  GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_widget_set_margin_top(title_box, 24);
  gtk_widget_set_margin_start(title_box, 32);

  GtkWidget *logo = gtk_image_new_from_file("assets/icon-connectivity.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(logo), 32);
  
  GtkWidget *title = gtk_label_new("Connectivity Tools");
  gtk_widget_add_css_class(title, "connectivity-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  gtk_box_append(GTK_BOX(title_box), logo);
  gtk_box_append(GTK_BOX(title_box), title);

  GtkWidget *menu_bar = create_menu_bar(ui_connectivity);
  gtk_widget_set_margin_start(menu_bar, 32);

  gtk_box_append(GTK_BOX(box), title_box);
  gtk_box_append(GTK_BOX(box), menu_bar);

  return box;
}

static GtkWidget *create_menu_bar(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24);

  const char *tools[] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  for (int i = 0; i < 4; i++)
  {
    ui_connectivity->buttons[i] = create_primary_button(tools[i], NULL, "connectivity-tools-button"); 
    g_object_set_data(G_OBJECT(ui_connectivity->buttons[i]), "target-page", (void *)tools[i]);
    g_signal_connect(ui_connectivity->buttons[i], "clicked", G_CALLBACK(on_menu_button_clicked), ui_connectivity);

    gtk_box_append(GTK_BOX(box), ui_connectivity->buttons[i]);
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->buttons[0]), TRUE);

  return box;
}

static GtkWidget *create_page_ping(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  return container;
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{
  ui_connectivity_t *ui_connectivity = (ui_connectivity_t *)data;
  
  const char *label = g_object_get_data(G_OBJECT(button), "target-page");

  gtk_stack_set_visible_child_name(GTK_STACK(ui_connectivity->stack), label);

  for (int i = 0; i < 4; i++)
  {
    if (ui_connectivity->buttons[i] != GTK_WIDGET(button))
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->buttons[i]), FALSE);
    else
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->buttons[i]), TRUE);
  }
}

static void on_sidebar_button_clicked(GtkButton *button, gpointer data)
{
  ui_connectivity_t *ui_connectivity = (ui_connectivity_t *)data;
  
  const char *label = g_object_get_data(G_OBJECT(button), "target-page");

  gtk_stack_set_visible_child_name(GTK_STACK(ui_connectivity->stack), label);

  const char *icons[] = {
    "assets/icon-ping.svg",
    "assets/icon-traceroute.svg",
    "assets/icon-dns-lookup.svg",
    "assets/icon-arp-table.svg"
  };

  const char *icons_active[] = {
    "assets/icon-ping-active.svg",
    "assets/icon-traceroute-active.svg",
    "assets/icon-dns-lookup-active.svg",
    "assets/icon-arp-table-active.svg"
  };

  for (int i = 0; i < 4; i++)
  {
    if (ui_connectivity->sidebar_buttons[i] != GTK_WIDGET(button))
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->sidebar_buttons[i]), FALSE);
      GtkWidget *box = gtk_widget_get_first_child(ui_connectivity->sidebar_buttons[i]);
      GtkWidget *image = gtk_widget_get_first_child(box);
      gtk_image_set_from_file(GTK_IMAGE(image), icons[i]);
    }
    else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui_connectivity->sidebar_buttons[i]), TRUE);
      GtkWidget *box = gtk_widget_get_first_child(ui_connectivity->sidebar_buttons[i]);
      GtkWidget *image = gtk_widget_get_first_child(box);
      gtk_image_set_from_file(GTK_IMAGE(image), icons_active[i]);
    }
  }
}
