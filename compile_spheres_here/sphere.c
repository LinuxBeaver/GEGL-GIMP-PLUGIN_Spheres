/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * 2023 Beaver GEGL Sphere
 */ 

/* Sphere's GEGL GRAPH (at least during development) You can test sphere
without installing it by pasting this data inside Gimp's GEGL Graph.

 
id=2 over aux=[ ref=2 
shapesize width=100 height=490
]
median-blur radius=0
id=pixelwheel
src aux=[ ref=pixelwheel gegl:gaussian-blur std-dev-x=1500
gegl:opacity value=10.00
gegl:opacity value=10.00
polar-coordinates ]
median-blur radius=0
color-overlay value=#0cff00
gaussian-blur std-dev-x=110 std-dev-y=110
id=0
gimp:layer-mode layer-mode=screen aux=[ ref=0 emboss azimuth=33 elevation=23  depth=68 ]
opacity value=10
id=1 src-in aux=[ ref=1]
id=1 src-in aux=[ ref=1]
id=1 src-in aux=[ ref=1]
id=1 src-in aux=[ ref=1]
id=1 src-in aux=[ ref=1]
median-blur radius=0
levels in-low=0.3
median-blur radius=0
hue-chroma hue=0
dropshadow x=11 y=20 opacity=0.6 radius=16 grow-radius=1

JULY 14 2023 SPHERE DOES NOT WORK IN GIMP 2.99.16. Crashes on start up
*/

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

#define LONG \
" median-blur radius=0 id=pixelwheel src aux=[ ref=pixelwheel id=container gimp:layer-mode layer-mode=replace composite-mode=clip-to-layer aux=[ ref=container gblur-1d std-dev=1500 gegl:opacity value=10 opacity value=10 median-blur radius=0 ] polar-coordinates ] color-overlay value=#0cff00 gblur-1d std-dev=110 clip-extent=false   "\
/* This is a baked in GEGL Graph.  */

#define LONG2 \
 " opacity value=10 id=1 src-in aux=[ ref=1] id=1 src-in aux=[ ref=1] id=1 src-in aux=[ ref=1] id=1 src-in aux=[ ref=1] id=1 src-in aux=[ ref=1] median-blur radius=0 levels in-low=0.3 median-blur radius=0 "\

/*  Not needed anymore. Earlier in the day of development, this filter called gaussian blur's base filter and gegl rectangle together.
 Now I found a better alternative. I'm just keeping this in case one day it becomes useful again.

property_double (shapesize, _("Size of the Sphere"), 130)
    description (_("Size of the sphere. Larger makes things slower"))
  ui_range    (60, 400)
    value_range (60, 400)
    ui_meta     ("unit", "pixel-distance")
*/

property_enum(guichange, _("Display"),
    guiendspheredropshadow, guichangeenumspheredropshadow,
    SPHERE_SHOW_DEFAULT)
  description(_("Change the GUI option"))


enum_start (guichangeenumspheredropshadow)
enum_value   (SPHERE_SHOW_DEFAULT, "default", N_("Sphere"))
enum_value   (SPHERE_SHOW_SHADOW, "shadow", N_("Show Shadow"))
enum_value   (SPHERE_SHOW_INNERGLOW, "innerglow", N_("Show Inner Glow"))
  enum_end (guiendspheredropshadow)

property_double (hue, _("Hue Rotation (0 resets)"),  0.0)
   description  (_("Rotate the hue to change the color of the sphere"))
   value_range  (-180.0, 180.0)
ui_meta ("visible", "guichange {default}")

property_enum (blendmodeswitchercolor, _("Enable/Disable HSL Color (conflicts with hue)"),
   description  (_("Please set hue to 0 (its middle) when using this. This allows a special color overlay with the hsl color blend mode."))
    GeglBlendModeTypecolorsphere, gegl_blend_mode_typecolorsphere,
    GEGL_BLEND_MODE_TYPE_NONE)
ui_meta ("visible", "guichange {default}")


property_color (coloroverlay, _("HSL Color change mode (conflicts with hue)"), "#fb0000")
    description (_("The color to paint over the sphere that uses the HSL Color blend mode. Please set hue to 0 (its middle) when using this. If the color isn't changing proper it is because hue is on a value other then 0."))
ui_meta ("visible", "guichange {default}")

enum_start (gegl_blend_mode_typecolorsphere)
  enum_value (GEGL_BLEND_MODE_TYPE_NONE, "none",
              N_("Disable"))
  enum_value (GEGL_BLEND_MODE_TYPE_HSLCOLOR,      "hslcolor",
              N_("HSL Color"))
enum_end (GeglBlendModeTypecolorsphere)


property_double (light, _("Lightness"), 0.0)
   description  (_("Lightness adjustment"))
   value_range  (-8.0, 8.0)
ui_meta ("visible", "guichange {default}")


property_double (size, _("Size"), 0.0)
    description (_("Resize the sphere"))
    value_range (-100, 100.0)
ui_meta ("visible", "guichange {default}")


property_double (azimuth, _("Azimuth"), 33.0)
    description (_("Light angle on sphere (degrees)"))
    value_range (0, 360)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")
ui_meta ("visible", "guichange {default}")


property_double (elevation, _("Elevation"), 23.0)
    description (_("Elevation angle (degrees) Rotating the brightest pixels on the sphere"))
    value_range (0, 40)
    ui_meta ("unit", "degree")
ui_meta ("visible", "guichange {default}")


property_int (depth, _("Depth"), 45)
    description (_("Filter width"))
    value_range (20, 100)
ui_meta ("visible", "guichange {default}")


property_int (egg, _("Egg Transformation"), 110)
    description (_("Transition the sphere into an egg"))
    value_range (110, 160)
ui_meta ("visible", "guichange {default}")


property_double (shadowx, _("Shadow X"), 20.0)
  description   (_("Horizontal shadow offset"))
  ui_range      (-40.0, 40.0)
  ui_steps      (1, 10)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "x")
ui_meta ("visible", "guichange {shadow}")

property_double (shadowy, _("Shadow Y"), 20.0)
  description   (_("Vertical shadow offset"))
  ui_range      (-40.0, 40.0)
  ui_steps      (1, 10)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "y")
ui_meta ("visible", "guichange {shadow}")

property_double (shadowradius, _("Shadow Blur radius"), 10.0)
  value_range   (0.0, G_MAXDOUBLE)
  ui_range      (0.0, 100.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
ui_meta ("visible", "guichange {shadow}")


property_double (shadowgrowradius, _("Shadow Grow radius"), 0.0)
  value_range   (0.0, 40.0)
  ui_range      (0.0, 40.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the shadow before blurring"))
ui_meta ("visible", "guichange {shadow}")

property_color  (shadowcolor, _("Shadow Color"), "black")
  description   (_("The shadow's color (defaults to 'black')"))
ui_meta ("visible", "guichange {shadow}")


property_double (shadowopacity, _("Opacity/ENABLE DROP SHADOW"), 0.0)
  description   (_("Sliding this entirely down will disable the drop shadow.)"))
  value_range   (0.0, 1.5)
  ui_steps      (0.01, 0.10)
ui_meta ("visible", "guichange {shadow}")


property_file_path(image, _("Image File Overlay for Sphere"), "")
    description (_("Source image file path (png, jpg, raw, svg, bmp, tif, ...)"))
ui_meta ("visible", "guichange {default}")

property_double (desaturate, _("Desaturate (for image overlay)"), 1.0)
   description  (_("Desaturate the sphere for a image file overlay"))
   value_range  (0.0, 1.0)
ui_meta ("visible", "guichange {default}")

property_double (huerotate, _("Hue Rotation for image file overlay (0 resets)"),  0.0)
   description  (_("Rotate the hue to change the color of image file overlay"))
   value_range  (-180.0, 180.0)
ui_meta ("visible", "guichange {default}")


property_double (in_low, _("Levels - Low input"), 0.0)
    description ( _("Levels setting - Input luminance level to become lowest output"))
   ui_range    (0.0, 0.6)
ui_meta ("visible", "guichange {default}")



property_double (in_high, _("Levels - High input"), 1.0)
    description (_("Input luminance level to become white"))
    ui_range    (0.5, 1.0)
   value_range  (0.5, 1.0)
    ui_meta     ("role", "output-extent")
ui_meta ("visible", "guichange {default}")


property_double (out_low, _("Low output"), 0.0)
    description (_("Lowest luminance level in output"))
    ui_range    (0.0, 0.5)
   value_range  (0.0, 0.5)
ui_meta ("visible", "guichange {default}")
    ui_meta     ("role", "output-extent")

property_double (out_high, _("High output"), 1.0)
    description (_("Levels setting - Highest luminance level in output"))
    ui_range    (0.5, 1.0)
   value_range  (0.5, 1.0)
ui_meta ("visible", "guichange {default}")

property_boolean (innerglow, _("Enable Inner Glow"), FALSE)
  description    (_("Whether or not Inner Glow should be enabled/disabled."))
ui_meta ("visible", "guichange {innerglow}")

property_double (innerglowx, _("X"), 11.7)
  description   (_("Horizontal shadow offset"))
  ui_range      (-20.0, 20.0)
  value_range   (-20.0, 20.0)
  ui_steps      (1, 2)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "x")
ui_meta ("visible", "guichange {innerglow}")

property_double (innerglowy, _("Y"), 11.7)
  description   (_("Vertical shadow offset"))
  ui_range      (-20.0, 20.0)
  value_range   (-20.0, 20.0)
  ui_steps      (1, 2)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "y")
ui_meta ("visible", "guichange {innerglow}")



property_double (innerglowradius, _("Blur radius"), 18)
  value_range   (0.0, 60.0)
  ui_range      (0.0, 40.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
ui_meta ("visible", "guichange {innerglow}")


property_double (innerglowgrowradius, _("Grow radius"), 4.0)
  value_range   (1, 10.0)
  ui_range      (1, 10.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the shadow before blurring; a negative value will contract the shadow instead"))
ui_meta ("visible", "guichange {innerglow}")

property_double (innerglowopacity, _("Opacity"), 0.8)
  value_range   (0.0, 0.9)
  ui_steps      (0.01, 0.10)
ui_meta ("visible", "guichange {innerglow}")


property_color (innerglowcolor, _("Color"), "#000000")
    description (_("The color to paint over the input"))
    ui_meta     ("role", "color-primary")
ui_meta ("visible", "guichange {innerglow}")
    ui_meta     ("role", "output-extent")


property_double  (innerglowfix, _("Median to fix non-effected pixels on edges"), 65)
  value_range (50, 80)
  description (_("Due to a bug I can't solve, not all pixels will be effected by inner glow. Median blur solves that problem.'"))
ui_meta ("visible", "guichange {innerglow}")


property_double (denoisedct, _("Smooth Sphere to kill banding"), 1.0)
    description (_("In graphic design the term --banding-- are unpleasant  bands of pixels that is usually seen in gradients. This denoise operation gets rid of the spheres banding. In default it is always running on 1 degree. If this was not present spheres with image file overlays would have severe banding. During development of this plugin that was the case. This operation denoise-dct resolves banding. "))
    ui_range    (1, 6)
   value_range  (1, 6)
ui_meta ("visible", "guichange {default}")

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     sphere
#define GEGL_OP_C_SOURCE sphere.c

#include "gegl-op.h"

typedef struct
{
  GeglNode *input;
  GeglNode *graph; 
  GeglNode *graph2; 
  GeglNode *replace; 
  GeglNode *zoom; 
  GeglNode *emboss;
  GeglNode *blur; 
  GeglNode *image; 
  GeglNode *burn; 
  GeglNode *nop3; 
  GeglNode *nop5; 
  GeglNode *normalimage;
  GeglNode *levels; 
  GeglNode *normalblendig; 
  GeglNode *innerglow; 
  GeglNode *nop4; 
  GeglNode *eggmode; 
  GeglNode *nop2;  
  GeglNode *shadow; 
  GeglNode *screen; 
  GeglNode *container; 
  GeglNode *vertical; 
  GeglNode *thresholdalpha; 
  GeglNode *nop; 
  GeglNode *repairgeglgraph; 
  GeglNode *crop;  
  GeglNode *huelight; 
  GeglNode *shapesize; 
  GeglNode *colorblend; 
  GeglNode *color; 
  GeglNode *nop6; 
  GeglNode *denoise; 
  GeglNode *desat; 
  GeglNode *none; 
  GeglNode *hue2; 
  GeglNode *output;
} State; 


static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);
  GeglNode *input, *output, *graph, *graph2, *replace, *hue2, *zoom, *emboss, *none, *desat, *blur, *denoise, *colorblend, *color, *nop6, *image, *nop5, *burn, *nop3, *levels, *normalblendig, *innerglow, *nop4, *eggmode, *nop2,  *shadow, *screen, *container, *vertical, *thresholdalpha, *nop, *repairgeglgraph, *crop,  *huelight, *normalimage, *shapesize;


  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



 graph   = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string",  LONG,  NULL);
                             

 graph2   = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string",  LONG2,  NULL);
                             
 huelight   = gegl_node_new_child (gegl,
                                  "operation", "gegl:hue-chroma",
                                  NULL);

 thresholdalpha   = gegl_node_new_child (gegl,
                                  "operation", "gimp:threshold-alpha", "value", 0.1,
                                  NULL);

 /*These are Gaussian Blur's base operations. It makes sense to use them in this workflow. */
 vertical   = gegl_node_new_child (gegl,
                                  "operation", "gegl:gblur-1d", "orientation", 1, "std-dev", 130.0,
                                  NULL);

 eggmode   = gegl_node_new_child (gegl,
                                  "operation", "gegl:gblur-1d", "orientation", 1, "clip-extent", 1, 
                                  NULL);


 screen   = gegl_node_new_child (gegl,
                                  "operation", "gegl:screen",
                                  NULL);

 shadow   = gegl_node_new_child (gegl,
                                  "operation", "gegl:dropshadow",
                                  NULL);


 denoise   = gegl_node_new_child (gegl,
                                  "operation", "gegl:denoise-dct",
                                  NULL);


 replace   = gegl_node_new_child (gegl,
                                  "operation", "gegl:src",
                                  NULL);


 shapesize   = gegl_node_new_child (gegl,
                                  "operation", "gegl:rectangle", "width", 150.0, "height", 110.0,
                                  NULL);

  repairgeglgraph      = gegl_node_new_child (gegl, "operation", "gegl:median-blur",
                                         "radius",       0,
                                         NULL);


 crop   = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);

 desat   = gegl_node_new_child (gegl,
                                  "operation", "gegl:saturation",
                                  NULL);


 blur   = gegl_node_new_child (gegl,
                                  "operation", "gegl:gaussian-blur", "std-dev-x", 1.0, "std-dev-y", 1.0,
                                  NULL);

 emboss   = gegl_node_new_child (gegl,
                                  "operation", "gegl:emboss",
                                  NULL);



 container   = gegl_node_new_child (gegl,
                                  "operation", "gegl:src",
                                  NULL);

 hue2   = gegl_node_new_child (gegl,
                                  "operation", "gegl:hue-chroma",
                                  NULL);
                                
 nop   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

 nop2   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

 nop3   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

 nop4   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

 nop5   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

 zoom   = gegl_node_new_child (gegl,
                                  "operation", "gegl:lens-distortion",
                                  NULL);
 /*This is Gimp's burn blend mode labelled 33, One day in the future it might be 32 or 34 or something else */
burn = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 33, "opacity", 0.65, "composite-mode", 0, NULL); 

 /*This is Gimp's HSL COLOR blend mode labelled 39, One day in the future it might be 38 or 40 or something else */
colorblend = gegl_node_new_child (gegl,
                                  "operation", "gimp:layer-mode", "layer-mode", 39,  "composite-mode", 2, "blend-space", 2, NULL); 

/*
 colorblend   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);*/

 color   = gegl_node_new_child (gegl,
                                  "operation", "gegl:color",
                                  NULL);

 nop6   = gegl_node_new_child (gegl,
                                  "operation", "gegl:nop",
                                  NULL);

 none   = gegl_node_new_child (gegl,
                                  "operation", "gegl:dst", 
                                  NULL);


 normalimage   = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);


 image   = gegl_node_new_child (gegl,
                                  "operation", "gegl:layer",
                                  NULL);

 levels   = gegl_node_new_child (gegl,
                                  "operation", "gegl:levels",
                                  NULL);

 innerglow   = gegl_node_new_child (gegl,
                                  "operation", "lb:innerglow", 
                                  NULL);


 normalblendig   = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);

/* Not needed anymore
   gegl_operation_meta_redirect (operation, "shapesize", vertical, "std-dev");
   gegl_operation_meta_redirect (operation, "shapesize", shapesize, "height"); */

  gegl_operation_meta_redirect (operation, "in_high", levels, "in-high");
  gegl_operation_meta_redirect (operation, "out_high", levels, "out-high");
  gegl_operation_meta_redirect (operation, "out_low", levels, "out-low");
  gegl_operation_meta_redirect (operation, "in_low", levels, "in-low");
  gegl_operation_meta_redirect (operation, "image", image, "src");
  gegl_operation_meta_redirect (operation, "egg", eggmode, "std-dev");
  gegl_operation_meta_redirect (operation, "size", zoom, "zoom");
  gegl_operation_meta_redirect (operation, "desaturate", desat, "scale");
  gegl_operation_meta_redirect (operation, "hue", huelight, "hue");
  gegl_operation_meta_redirect (operation, "light", huelight, "lightness");
  gegl_operation_meta_redirect (operation, "azimuth", emboss, "azimuth");
  gegl_operation_meta_redirect (operation, "elevation", emboss, "elevation");
  gegl_operation_meta_redirect (operation, "depth", emboss, "depth");
  gegl_operation_meta_redirect (operation, "shadowx", shadow, "x");
  gegl_operation_meta_redirect (operation, "shadowy", shadow, "y");
  gegl_operation_meta_redirect (operation, "shadowradius", shadow, "radius");
  gegl_operation_meta_redirect (operation, "shadowgrowradius", shadow, "grow-radius");
  gegl_operation_meta_redirect (operation, "shadowcolor", shadow, "color");
  gegl_operation_meta_redirect (operation, "shadowopacity", shadow, "opacity");
  gegl_operation_meta_redirect (operation, "innerglowx", innerglow, "x");
  gegl_operation_meta_redirect (operation, "innerglowy", innerglow, "y");
  gegl_operation_meta_redirect (operation, "innerglowradius", innerglow, "radius");
  gegl_operation_meta_redirect (operation, "innerglowgrowradius", innerglow, "grow-radius");
  gegl_operation_meta_redirect (operation, "innerglowcolor", innerglow, "value2");
  gegl_operation_meta_redirect (operation, "innerglowopacity", innerglow, "opacity");
  gegl_operation_meta_redirect (operation, "innerglowfix", innerglow, "fixoutline");
  gegl_operation_meta_redirect (operation, "coloroverlay", color, "value");
  gegl_operation_meta_redirect (operation, "huerotate", hue2, "hue");
  gegl_operation_meta_redirect (operation, "denoisedct", denoise, "sigma");

 /* Now save points to the various gegl nodes so we can rewire them in
   * update_graph() later
   */
  State *state = g_malloc0 (sizeof (State));
  state->input = input;
  state->replace = replace;
  state->vertical = vertical;
  state->thresholdalpha = thresholdalpha;
  state->nop = nop;
  state->container = container;
  state->crop = crop;
  state->zoom = zoom;
  state->image = image;
  state->normalimage = normalimage;
  state->nop3 = nop3;
  state->hue2 = hue2;
  state->burn = burn;
  state->denoise = denoise;
  state->levels = levels;
  state->shadow = shadow;
  state->eggmode = eggmode;
  state->graph = graph;
  state->graph2 = graph2;
  state->nop4 = nop4;
  state->nop6 = nop6;
  state->colorblend = colorblend;
  state->color = color;
  state->nop5 = nop5;
  state->nop2 = nop2;
  state->emboss = emboss;
  state->innerglow = innerglow;
  state->repairgeglgraph = repairgeglgraph;
  state->blur = blur;
  state->none = none;
  state->desat = desat;
  state->screen = screen;
  state->huelight = huelight;
  state->normalblendig = normalblendig;
  state->shapesize = shapesize;
  state->output = output;
  o->user_data = state;
}

static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  GeglNode *colorblend = state->none; /* the default */
  switch (o->blendmodeswitchercolor) {
    case GEGL_BLEND_MODE_TYPE_NONE: colorblend = state->none; break;
    case GEGL_BLEND_MODE_TYPE_HSLCOLOR: colorblend = state->colorblend; break;
default: colorblend = state->none;
}
                          /*This is an example of a very complex GEGL Graph*/
  if (o->innerglow)
  {
  gegl_node_link_many (state->input, state->replace, state->vertical, state->thresholdalpha, state->nop, state->container, state->crop, state->zoom, state->huelight, state->desat, state->nop3, state->burn, state->levels, state->denoise, state->nop4, state->normalblendig,  state->shadow,  state->output, NULL);
 /*container is the replace blend mode and it is containing most of sphere's graph, the exception being image overlay, resizing, levels adjustments, inner glow and shadow*/
  gegl_node_connect (state->container, "aux", colorblend, "output");
  gegl_node_link_many (state->nop, state->graph, state->eggmode, state->nop2,  state->screen, state->graph2, state->repairgeglgraph, colorblend,  NULL);
 /*colorblend is HSL Color and it is connecting to color overlay.*/
  gegl_node_connect (colorblend, "aux", state->color, "output");
 /*Image file upload is being blended with burn blend mode. There is also a blur that by default applies to image file overlay*/
  gegl_node_connect (state->burn, "aux", state->blur, "output");
  gegl_node_link_many (state->nop3, state->normalimage, state->blur,  NULL);
  gegl_node_connect (state->normalimage, "aux", state->hue2, "output");
  gegl_node_link_many (state->image, state->hue2, NULL);
 /*GEGL Rectangle in a container all by itself due to a bug graphs have. Rectangle is needed for this plugin to work.*/
  gegl_node_connect (state->replace, "aux", state->shapesize, "output");
 /*Emboss being fused with the screen blend mode*/
  gegl_node_connect (state->screen, "aux", state->emboss, "output");
  gegl_node_link_many (state->nop2, state->emboss,  NULL);
 /*Inner Glow inside the normal blend mode. Exclusive to this top graph. Will not be found in its alternative.*/
  gegl_node_connect (state->normalblendig, "aux", state->innerglow, "output");
  gegl_node_link_many (state->nop4, state->innerglow,  NULL);

  }
else
  {
  gegl_node_link_many (state->input, state->replace, state->vertical, state->thresholdalpha, state->nop, state->container, state->crop, state->zoom,  state->huelight, state->desat, state->nop3, state->burn, state->levels, state->nop4, state->denoise, state->shadow,  state->output, NULL);
 /*container is the replace blend mode and it is containing most of sphere's graph, the exception being image overlay, resizing, levels adjustments, and shadow*/
  gegl_node_connect (state->container, "aux", colorblend, "output");
  gegl_node_link_many (state->nop, state->graph, state->eggmode, state->nop2,  state->screen, state->graph2, state->repairgeglgraph, colorblend,  NULL);
 /*colorblend is HSL Color and it is connecting to color overlay.*/
  gegl_node_connect (colorblend, "aux", state->color, "output");
 /*Image file upload is being blended with burn blend mode. There is also a blur that by default applies to image file overlay*/
  gegl_node_connect (state->burn, "aux", state->blur, "output");
  gegl_node_link_many (state->nop3, state->normalimage, state->blur,  NULL);
  gegl_node_connect (state->normalimage, "aux", state->hue2, "output");
  gegl_node_link_many (state->image, state->hue2, NULL);
 /*GEGL Rectangle in a container all by itself due to a bug graphs have. Rectangle is needed for this plugin to work.*/
  gegl_node_connect (state->replace, "aux", state->shapesize, "output");
 /*Emboss being fused with the screen blend mode*/
  gegl_node_connect (state->screen, "aux", state->emboss, "output");
  gegl_node_link_many (state->nop2, state->emboss,  NULL);
}
}

 
static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
   GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:sphere",
    "title",       _("Sphere Generator"),
    "categories",  "Artistic",
    "reference-hash", "5g0sph416a50a51170f0f25sb2ac",
    "description", _("Renders a shiny glossed sphere or egg. Hue must be at 0 if HSL Color is used and HSL Color must be disabled to use hue rotate."
                     ""),
    "gimp:menu-path", "<Image>/Filters/Render/Fun",
    "gimp:menu-label", _("Sphere..."),
    NULL);
}

#endif
