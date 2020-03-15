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

int
main (int argc, char *argv[])
{
    GMainLoop *loop = NULL;
    GstElement *pipeline = NULL, *source = NULL, *h264parser = NULL

    /* GStreamer initialization */
    g_print ("Initializeing app...\n");
    loop = g_main_loop_new (NULL, FALSE);
    gst_init (&argc, &argv);

    /* -- 1.Create GStreamer Objects -- */
    /* Create pipeline */
    pipeline = gst_pipeline_new ("pipeline");

    GstElement *source;
    source = gst_element_factory_make ("autovideosrc", "source");
    g_object_set(G_OBJECT (source), "location", argv[0], NULL);

}