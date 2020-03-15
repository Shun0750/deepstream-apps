#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "gstnvdsmeta.h"
#ifndef PLATFORM_TEGRA
#include "gst-nvmessage.h"
#endif

int main (int argc, char *argv[])
{
    g_printerr ("Initializeing app...\n");
    gst_init (&argc, &argv);

    GstElement *source;
    source = gst_element_factory_make ("autovideosrc", "source");
    g_object_set(G_OBJECT (source), "location", argv[0], NULL);

}