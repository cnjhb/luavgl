#include "lugl.h"
#include "private.h"

typedef struct lugl_disp_s {
  lv_disp_t *disp;
} lugl_disp_t;

static lugl_disp_t *lugl_check_disp(lua_State *L, int idx)
{
  lugl_disp_t *v = luaL_checkudata(L, idx, "lv_disp");

  return v;
}

static lv_disp_t *lugl_to_disp(lua_State *L, int idx)
{
  return lugl_check_disp(L, idx)->disp;
}

static int lugl_disp_get(lua_State *L, lv_disp_t *disp)
{
  /* check if already exists */
  lua_pushlightuserdata(L, disp);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    /* create new disp userdata and add to registry forever */
    lua_pop(L, 1);
    lugl_disp_t *d = lua_newuserdata(L, sizeof(lugl_disp_t));
    d->disp = disp;

    luaL_getmetatable(L, "lv_disp");
    lua_setmetatable(L, -2);

    lua_pushlightuserdata(L, disp);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
  }

  return 1;
}

static int lugl_disp_get_default(lua_State *L)
{
  lv_disp_t *disp = lv_disp_get_default();
  return lugl_disp_get(L, disp);
}

static int lugl_disp_get_scr_act(lua_State *L)
{
  lv_disp_t *disp = lugl_to_disp(L, 1);
  lv_obj_t *obj = lv_disp_get_scr_act(disp);
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    lugl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

static int lugl_disp_get_scr_prev(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lv_obj_t *obj = lv_disp_get_scr_prev(d->disp);
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    lugl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

/**
 * lugl.disp.load_scr(obj, {
 *    anim = "over_left",
 *    time = 1000,
 *    delay = 100,
 * })
 */
static int lugl_disp_load_scr(lua_State *L)
{
  lv_obj_t *obj = lugl_to_obj(L, 1);
  if (!lua_istable(L, 2)) {
    lv_disp_load_scr(obj);
    return 0;
  }

  /* has parameter table */

  lv_scr_load_anim_t anim = LV_SCR_LOAD_ANIM_NONE;
  uint32_t time = 0;
  uint32_t delay = 0;
  bool auto_del = false;

  const char *str;
  lua_getfield(L, 2, "anim");
  str = lua_tostring(L, -1);
  if (str == NULL || strcmp(str, "none") == 0) {
    ; /* use default */
  } else if (strcmp(str, "over_left") == 0) {
    anim = LV_SCR_LOAD_ANIM_OVER_LEFT;
  } else if (strcmp(str, "over_right") == 0) {
    anim = LV_SCR_LOAD_ANIM_OVER_RIGHT;
  } else if (strcmp(str, "over_top") == 0) {
    anim = LV_SCR_LOAD_ANIM_OVER_TOP;
  } else if (strcmp(str, "over_botto") == 0) {
    anim = LV_SCR_LOAD_ANIM_OVER_BOTTOM;
  } else if (strcmp(str, "move_left") == 0) {
    anim = LV_SCR_LOAD_ANIM_MOVE_LEFT;
  } else if (strcmp(str, "move_right") == 0) {
    anim = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
  } else if (strcmp(str, "move_top") == 0) {
    anim = LV_SCR_LOAD_ANIM_MOVE_TOP;
  } else if (strcmp(str, "move_botto") == 0) {
    anim = LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
  } else if (strcmp(str, "fade_in") == 0) {
    anim = LV_SCR_LOAD_ANIM_FADE_IN;
  } else if (strcmp(str, "fade_on") == 0) {
    anim = LV_SCR_LOAD_ANIM_FADE_ON;
  } else if (strcmp(str, "fade_out") == 0) {
    anim = LV_SCR_LOAD_ANIM_FADE_OUT;
  } else if (strcmp(str, "out_left") == 0) {
    anim = LV_SCR_LOAD_ANIM_OUT_LEFT;
  } else if (strcmp(str, "out_right") == 0) {
    anim = LV_SCR_LOAD_ANIM_OUT_RIGHT;
  } else if (strcmp(str, "out_top") == 0) {
    anim = LV_SCR_LOAD_ANIM_OUT_TOP;
  } else if (strcmp(str, "out_bottom") == 0) {
    anim = LV_SCR_LOAD_ANIM_OUT_BOTTOM;
  }
  lua_pop(L, 1);

  lua_getfield(L, 2, "time");
  time = lugl_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "delay");
  delay = lugl_tointeger(L, -1);
  lua_pop(L, 1);

  lua_getfield(L, 2, "auto_del");
  auto_del = lugl_tointeger(L, -1);
  lua_pop(L, 1);

  lv_scr_load_anim(obj, anim, time, delay, auto_del);
  return 0;
}

static int lugl_disp_get_layer_top(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lv_obj_t *obj = lv_disp_get_layer_top(d->disp);
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    lugl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

static int lugl_disp_get_layer_sys(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lv_obj_t *obj = lv_disp_get_layer_sys(d->disp);
  if (obj == NULL) {
    lua_pushnil(L);
    return 1;
  }

  /* registry[obj] */
  lua_pushlightuserdata(L, obj);
  lua_rawget(L, LUA_REGISTRYINDEX);

  if (lua_isnoneornil(L, -1)) {
    lugl_add_lobj(L, obj)->lua_created = false;
  }

  return 1;
}

static int lugl_disp_set_bg_color(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lv_color_t c = lugl_tocolor(L, 2);

  lv_disp_set_bg_color(d->disp, c);

  return 0;
}

static int lugl_disp_set_bg_image(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  const char *img = lugl_toimgsrc(L, 2);

  lv_disp_set_bg_image(d->disp, img);

  return 0;
}

static int lugl_disp_set_bg_opa(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lv_opa_t opa = lugl_tointeger(L, 2);
  lv_disp_set_bg_opa(d->disp, opa);

  return 0;
}

static int lugl_disp_get_chroma_key_color(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lv_color_t c = lv_disp_get_chroma_key_color(d->disp);
  lua_pushinteger(L, c.full);

  return 1;
}

static int lugl_disp_get_next(lua_State *L)
{
  lv_disp_t *disp = NULL;
  if (!lua_isnoneornil(L, 1)) {
    disp = lugl_check_disp(L, 1)->disp;
  }

  disp = lv_disp_get_next(disp);
  if (disp == NULL) {
    lua_pushnil(L);
    return 1;
  }

  return lugl_disp_get(L, disp);
}

static int lugl_disp_get_res(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  lua_pushinteger(L, lv_disp_get_hor_res(d->disp));
  lua_pushinteger(L, lv_disp_get_ver_res(d->disp));
  return 2;
}

static int lugl_disp_set_rotation(lua_State *L)
{
  lugl_disp_t *d = lugl_check_disp(L, 1);
  uint32_t r = lua_tointeger(L, 2);

  lv_disp_rot_t rot;
  if (r == 0)
    rot = LV_DISP_ROT_NONE;
  else if (r == 90)
    rot = LV_DISP_ROT_90;
  else if (r == 180)
    rot = LV_DISP_ROT_180;
  else if (r == 270)
    rot = LV_DISP_ROT_270;
  else {
    return luaL_argerror(L, 2, "invalid rotation value");
  }

  lv_disp_set_rotation(d->disp, rot);
  return 0;
}

/**
 * lugl.disp lib
 */
static const luaL_Reg disp_lib[] = {
    {"get_default",  lugl_disp_get_default },
    {"get_scr_act",  lugl_disp_get_scr_act },
    {"get_scr_prev", lugl_disp_get_scr_prev},
    {"get_next",     lugl_disp_get_next    },
    {"load_scr",     lugl_disp_load_scr    },

    {NULL,           NULL                  },
};

/*
** methods for disp handles
*/
static const luaL_Reg disp_methods[] = {
    {"get_layer_top",        lugl_disp_get_layer_top       },
    {"get_layer_sys",        lugl_disp_get_layer_sys       },
    {"set_bg_color",         lugl_disp_set_bg_color        },
    {"set_bg_image",         lugl_disp_set_bg_image        },
    {"set_bg_opa",           lugl_disp_set_bg_opa          },
    {"get_chroma_key_color", lugl_disp_get_chroma_key_color},
    {"get_next",             lugl_disp_get_next            },
    {"set_rotation",         lugl_disp_set_rotation        },
    {"get_res",              lugl_disp_get_res             },

    {NULL,                   NULL                          },
};

static const luaL_Reg disp_meta[] = {
    {"__index", NULL}, /* place holder */

    {NULL,      NULL}
};

static void lugl_disp_init(lua_State *L)
{
  /* create lv_fs metatable */
  luaL_newmetatable(L, "lv_disp");
  luaL_setfuncs(L, disp_meta, 0);

  luaL_newlib(L, disp_methods);
  lua_setfield(L, -2, "__index");

  lua_pop(L, 1); /* pop metatable */

  /* lugl.disp lib */
  luaL_newlib(L, disp_lib);
  lua_setfield(L, -2, "disp");
}
