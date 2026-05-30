#include "ui_connectivity.h"

#include "utils.h"
#include "ui_widgets.h"
#include "macros.h"
#include "ui_macros.h"

static GtkWidget *create_side_bar(application_t *application);
static GtkWidget *create_content(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity);
static GtkWidget *create_connectivity_workspace(ui_connectivity_t *ui_connectivity);

// Callbacks
static void on_menu_button_clicked(GtkButton *button, gpointer data);

GtkWidget *create_page_connectivity(ui_t *ui)
{
  ui_connectivity_t *ui_connectivity = malloc(sizeof(ui_connectivity_t));
  if (ui_connectivity == NULL) return NULL;

  ui_connectivity->application = ui->application;
  ui_connectivity->window = ui->window;

  ui_connectivity->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  GtkWidget *side_bar = create_side_bar(ui_connectivity->application);
  GtkWidget *content = create_content(ui_connectivity);

  gtk_box_append(GTK_BOX(ui_connectivity->container), side_bar);
  gtk_box_append(GTK_BOX(ui_connectivity->container), content);

  g_object_set_data_full(G_OBJECT(ui_connectivity->container), "ui_connectivity", ui_connectivity, free); // ownership + free

  return ui_connectivity->container;
}

static GtkWidget *create_side_bar(application_t *application)
{
  (void)application; // unused parameter

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request(box, 280, -1);

  return box;
}

static GtkWidget *create_content(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
  gtk_widget_set_hexpand(box, TRUE);
  gtk_widget_set_margin_start(box, 24);
  gtk_widget_set_margin_end(box, 24);
  gtk_widget_set_margin_top(box, 24);
  gtk_widget_set_margin_bottom(box, 24);
  //gtk_widget_set_size_request(box, 1160, -1);

  GtkWidget *header = create_connectivity_header(ui_connectivity);
  GtkWidget *workspace = create_connectivity_workspace(ui_connectivity);

  gtk_box_append(GTK_BOX(box), header);
  gtk_box_append(GTK_BOX(box), workspace);

  return box;
}

static GtkWidget *create_connectivity_header(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);

  GtkWidget *head_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);

  GtkWidget *logo = gtk_image_new_from_file("assets/icon-connectivity.svg");
  gtk_image_set_pixel_size(GTK_IMAGE(logo), 32);
  
  GtkWidget *title = gtk_label_new("Connectivity Tools");
  gtk_widget_add_css_class(title, "connectivity-title");
  gtk_widget_set_hexpand(title, TRUE);
  gtk_widget_set_halign(title, GTK_ALIGN_START);

  gtk_box_append(GTK_BOX(head_box), logo);
  gtk_box_append(GTK_BOX(head_box), title);

  GtkWidget *menu_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  const char *tools[] = {
    "Ping",
    "TraceRoute",
    "DNS Lookup",
    "ARP"
  };

  for (int i = 0; i < 4; i++)
  {
    GtkWidget *button = create_primary_button(tools[i], "connectivity-tools-button"); 
    g_object_set_data(G_OBJECT(button), "target-page", (void *)tools[i]);
    g_signal_connect(button, "clicked", G_CALLBACK(on_menu_button_clicked), ui_connectivity);

    gtk_box_append(GTK_BOX(menu_bar), button);
  }

  gtk_box_append(GTK_BOX(box), head_box);
  gtk_box_append(GTK_BOX(box), menu_bar);

  return box;
}


static GtkWidget *create_connectivity_workspace(ui_connectivity_t *ui_connectivity)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 24);

  return box;
}

static void on_menu_button_clicked(GtkButton *button, gpointer data)
{

}
