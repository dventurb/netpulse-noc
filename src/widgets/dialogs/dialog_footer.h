#ifndef DIALOG_FOOTER_H
#define DIALOG_FOOTER_H 

#include <gtk/gtk.h>

#include "dialog.h"

GtkWidget *dialog_footer_new(GtkWidget *dialog, dialog_action_t action);

#endif
