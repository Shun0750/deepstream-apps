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

/* command */

/* gst-launch-1.0 v4l2src device=/dev/video0 ! 
video/x-h264,width=1920,height=960,framerate=30/1 ! 
h264parse config-interval=3 ! fakesink */

int
main (int argc, char *argv[])
{
    GMainLoop *loop;
    GstElement *pipeline, *source, *filter, *parser, *nvdewarper, *sink, *nvvideoconvert;
    GstCaps *cap;
    GstBus *bus;
    GstStateChangeReturn ret;
    GstMessage *msg;
    gboolean terminate = FALSE;
    guint source_id = 0;

    /* GStreamer initialization */
    g_print ("Initializeing app...\n");
    loop = g_main_loop_new (NULL, FALSE);
    gst_init (&argc, &argv);


    /* -- 1.Create GStreamer Objects -- */
    /* Create caps */
    cap = gst_caps_new_simple ("video/x-h264",
         "width", G_TYPE_INT, 1920,
         "height", G_TYPE_INT, 960,
         "framerate", GST_TYPE_FRACTION, 30, 1,
         NULL);

    /* Create pipeline */
    pipeline = gst_pipeline_new ("pipeline");
    source = gst_element_factory_make ("v4l2src", "source");
    filter = gst_element_factory_make ("capsfilter","filter");
    parser = gst_element_factory_make ("h264parse","parser");
    nvvideoconvert = gst_element_factory_make ("nvvideoconvert", NULL);
    nvdewarper = gst_element_factory_make ("nvdewarper", NULL);
    sink = gst_element_factory_make ("filesink", "sink");

    if (!pipeline || !source || !sink || !filter || !nvvideoconvert || !nvdewarper) {
      g_printerr ("Failed to create gst elements. Exiting.\n");
      return -1;
    }

    /* Set the properties */
    g_object_set(source, "device", argv[1], NULL);
    g_object_set(parser, "config-interval", 3, NULL);
    g_object_set(sink, "location", "sample.mp4", "sync", 1, NULL);

    source_id = atoi(argv[2]);
    g_object_set (G_OBJECT (nvdewarper),
      "config-file", "./config_dewarper.txt",
      "source-id", source_id,
      NULL);
    //g_object_set (sink, "emit-signals", TRUE, "caps", cap, NULL);

    /* Link gst elements */
    gst_bin_add_many (GST_BIN (pipeline), source, filter, parser, sink, nvvideoconvert, nvdewarper, NULL);
    if (gst_element_link (source, sink) != TRUE) {
        g_printerr ("Elements could not be linked.");
        gst_object_unref (pipeline);
    }

    /*-- Start pipeline --*/
    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (pipeline);
        return -1;
    }

  bus = gst_element_get_bus (pipeline);
  do {
    msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
        GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    GError *err;
    gchar *debug_info;

    /* Parse message */
    if (msg != NULL) {
      switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
          gst_message_parse_error (msg, &err, &debug_info);
          g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
          g_clear_error (&err);
          g_free (debug_info);
          terminate = TRUE;
          break;
        case GST_MESSAGE_EOS:
          g_print ("End-Of-Stream reached.\n");
          terminate = TRUE;
          break;
        default:
          /* We should not reach here */
          g_printerr ("Unexpected message received.\n");
          break;
      }
      gst_message_unref (msg);
    }
  } while (!terminate);

  /* Free resources */
  gst_caps_unref (cap);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  return 0;
}