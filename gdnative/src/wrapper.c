#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gdnative_api_struct.gen.h>

#include "bouncy.h"
#include "camera.hpp"
#include "processing.hpp"

typedef struct camera_data_struct {
	void* camera;
	godot_pool_byte_array buffer;
	godot_vector2 size;
	unsigned long counter;
} camera_data_struct;

const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;


void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options)
{
	api = p_options->api_struct;

	// now find our extensions
	for (int i = 0; i < api->num_extensions; i++) {
		switch (api->extensions[i]->type) {
		case GDNATIVE_EXT_NATIVESCRIPT:
			nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
		break;
		default: break;
		};
	};
}

godot_arvr_interface_gdnative ar_interface;

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options)
{
	camera_delete_all();

	api = NULL;
	nativescript_api = NULL;
}

GDCALLINGCONV void *_camera_constructor(godot_object *p_instance, void *p_method_data)
{
	camera_data_struct *user_data = api->godot_alloc(sizeof(camera_data_struct));

	user_data->camera = NULL;
	api->godot_pool_byte_array_new(&user_data->buffer);
	user_data->counter = 0;

	b_println("Loading camera resources");

	return user_data;

}

GDCALLINGCONV void _camera_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
	camera_data_struct *user_data = (camera_data_struct*) p_user_data;

	if (user_data->camera) {
		camera_delete(user_data->camera);
		user_data->camera = NULL;
	}

	api->godot_pool_byte_array_destroy(&user_data->buffer);
	api->godot_free(p_user_data);
}

/******* Properties (Setters, Getters) *********/

/* This serves for setter functions that shouldn't change their values */
void _camera_do_nothing(godot_object *p_instance, void *p_method_data, void *p_user_data, godot_variant *p_arg)
{
	// Do nothing
}

godot_variant _camera_get_width(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
	godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

	api->godot_variant_new_int(&res, camera_get_width(user_data->camera));
	return res;
}

godot_variant _camera_get_height(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
	godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

	api->godot_variant_new_int(&res, camera_get_height(user_data->camera));
	return res;
}

godot_variant _camera_get_size(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
	godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

	api->godot_variant_new_vector2(&res, &user_data->size);
	return res;
}

/******* Methods *********/

godot_variant _camera_set_default(
	godot_object *p_instance,
	void *p_method_data,
	void *p_user_data,
	int p_num_args, godot_variant **p_args
) {
	int camera_id = 0, success;
	int width, height, ms;
	godot_variant res;

	if (p_num_args > 0) {
		camera_id = api->godot_variant_as_int(p_args[0]);
	}

	camera_set_default(camera_id);

	api->godot_variant_new_bool(&res, 1);
	return res;
}


godot_variant _camera_open(
	godot_object *p_instance,
	void *p_method_data,
	void *p_user_data,
	int p_num_args, godot_variant **p_args
) {
	int camera_id = -1, success;
	int width, height;
	godot_variant res;

	camera_data_struct *user_data = (camera_data_struct*) p_user_data;

	if (p_num_args > 0) {
		camera_id = api->godot_variant_as_int(p_args[0]);
	}

	if (!user_data) {
		api->godot_variant_new_bool(&res, GODOT_FALSE);
		return res;
	}

	if (user_data->camera) {
		camera_delete(user_data->camera);
	}

	user_data->camera = camera_create(camera_id);

	if (!user_data->camera) {
		api->godot_variant_new_bool(&res, GODOT_FALSE);
		return res;
	}

	width  = camera_get_width(user_data->camera);
	height = camera_get_height(user_data->camera);
	api->godot_vector2_new(&user_data->size, width, height);

	api->godot_pool_byte_array_resize(&user_data->buffer, width * height * 3);

	b_println("Opened camera");

	api->godot_variant_new_bool(&res, 1);
	return res;
}

godot_variant _camera_get_image(
	godot_object *p_instance,
	void *p_method_data,
	void *p_user_data,
	int p_num_args, godot_variant **p_args
) {
	godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

	godot_pool_byte_array_write_access* writer = api->godot_pool_byte_array_write(&user_data->buffer);
	uint8_t* data = api->godot_pool_byte_array_write_access_ptr(writer);
	int updated = camera_get_image(user_data->camera, data, &user_data->counter);

	api->godot_pool_byte_array_write_access_destroy(writer);

	if (updated) {
		api->godot_variant_new_pool_byte_array(&res, &user_data->buffer);
	} else {
		api->godot_variant_new_bool(&res, GODOT_FALSE);
	}

	return res;
}

godot_variant _camera_flip(
	godot_object *p_instance,
	void *p_method_data,
	void *p_user_data,
	int p_num_args, godot_variant **p_args
) {
	godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

	int flip_lr = 1, flip_ud = 0;

	if (p_num_args > 0) {
		flip_lr = api->godot_variant_as_bool(p_args[0]);
	}

	if (p_num_args > 1) {
		flip_ud = api->godot_variant_as_bool(p_args[1]);
	}

	camera_flip(user_data->camera, flip_lr, flip_ud);

	api->godot_variant_new_bool(&res, GODOT_TRUE);

	return res;
}


godot_variant _camera_detect(
	godot_object *p_instance,
	void *p_method_data,
	void *p_user_data,
	int p_num_args, godot_variant **p_args
) {
	godot_variant retval;
	camera_data_struct *user_data = (camera_data_struct*) p_user_data;

	/* Set default return value */
	api->godot_variant_new_bool(&retval, GODOT_FALSE);

	/* In any other case, return value is an array of the size == p_num_args */
	godot_array g_arr;
	if (p_num_args > 1) {
		api->godot_array_new(&g_arr);
	}

	/* iterate over all arguments */
	for (int idx = 0; idx < p_num_args; idx++) {
		/* process the argument if it's a string; print error otherwise */
		if (api->godot_variant_get_type(p_args[idx]) != GODOT_VARIANT_TYPE_STRING) {
			api->godot_print_warning("Invalid argument", "detect", __FILENAME__, __LINE__);
			continue;
		}
		godot_string g_str = api->godot_variant_as_string(p_args[idx]);
		godot_char_string gc_str = api->godot_string_utf8(&g_str);

		/* Return single rect if single argument, array of rects if multiple */
		region r = processing_detect_object(user_data->camera, api->godot_char_string_get_data(&gc_str));

		/* in case region is invalid, give false */
		if (r.x < 0) {
			api->godot_variant_new_bool(&retval, GODOT_FALSE);
		}
		else {
			godot_rect2 grec;
			api->godot_rect2_new(&grec, r.x, r.y, r.w, r.h);
			api->godot_variant_new_rect2(&retval, &grec);
		}

		/* if only 1 argument, return a single rect2 */
		if (p_num_args == 1) {
			return retval;
		}

		api->godot_array_append(&g_arr, &retval);
	}

	api->godot_variant_new_array(&retval, &g_arr);
	api->godot_array_destroy(&g_arr);

	return retval;
}

godot_variant _camera_compute_flow(
	godot_object *p_instance,
	void *p_method_data,
	void *p_user_data,
	int p_num_args, godot_variant **p_args
) {
	godot_variant res;
	camera_data_struct * user_data = (camera_data_struct *) p_user_data;

	if (p_num_args == 0) {
		api->godot_variant_new_bool(&res, GODOT_FALSE);
		return res;
	}

	godot_rect2 grec = api->godot_variant_as_rect2(p_args[0]);

	region r;
	godot_vector2 p = api->godot_rect2_get_position(&grec);
	godot_vector2 s = api->godot_rect2_get_size(&grec);
	r.w = api->godot_vector2_get_x(&s);
	r.h = api->godot_vector2_get_y(&s);
	r.x = api->godot_vector2_get_x(&p) - r.w / 2;
	r.y = api->godot_vector2_get_y(&p) - r.h / 2;

	flow f = processing_calculate_flow(user_data->camera, r);

	godot_vector2 gvec;
	api->godot_vector2_new(&gvec, f.x, f.y);

	api->godot_variant_new_vector2(&res, &gvec);

	return res;
}

/******* NativeScript initialization *********
**
** Putting this at the end to avoid writing prototypes.
*/

/* Helpful Macros */

#define create_godot_property(PROP_NAME, PROP_SETTER, METHOD_DATA, FREE_FUNC) { \
	godot_property_get_func get_##PROP_NAME = { .get_func = &_camera_get_##PROP_NAME, .method_data = METHOD_DATA, .free_func = FREE_FUNC }; \
	godot_property_set_func set_##PROP_NAME = { .set_func = &_camera_##PROP_SETTER,   .method_data = METHOD_DATA, .free_func = FREE_FUNC }; \
	nativescript_api->godot_nativescript_register_property(p_handle, "Camera", #PROP_NAME, &prop_attr, set_##PROP_NAME, get_##PROP_NAME); \
}

#define create_godot_property_basic(PROP_NAME, PROP_SETTER) create_godot_property(PROP_NAME, do_nothing, NULL, NULL)

#define create_godot_property_const(PROP_NAME) create_godot_property_basic(PROP_NAME, do_nothing)


#define create_godot_method(METHOD_NAME, METHOD_DATA, FREE_FUNC) { \
	godot_instance_method METHOD_NAME = { .method = &_camera_##METHOD_NAME, .method_data = METHOD_DATA, .free_func = FREE_FUNC }; \
	nativescript_api->godot_nativescript_register_method(p_handle, "Camera", #METHOD_NAME, method_attr, METHOD_NAME); \
}

#define create_godot_method_basic(METHOD_NAME) create_godot_method(METHOD_NAME, NULL, NULL)


/* Initialization code */
void GDN_EXPORT godot_nativescript_init(void *p_handle)
{
	godot_instance_create_func create   = { .create_func = &_camera_constructor, NULL, NULL };
	godot_instance_destroy_func destroy = { .destroy_func = &_camera_destructor, NULL, NULL };
	nativescript_api->godot_nativescript_register_class(p_handle, "Camera", "Reference", create, destroy);

	/* Setting properties */
	godot_property_attributes prop_attr = {
		.rset_type = GODOT_METHOD_RPC_MODE_DISABLED,
		.type = 0, // XXX
		.hint = GODOT_PROPERTY_HINT_NONE,
		// XXX: .hint_string = ,
		.usage = GODOT_PROPERTY_USAGE_STORAGE | GODOT_PROPERTY_USAGE_EDITOR,
	};

	create_godot_property_const(width);
	create_godot_property_const(height);
	create_godot_property_const(size);

	/* Setting methods */
	godot_method_attributes method_attr = { GODOT_METHOD_RPC_MODE_DISABLED };

	create_godot_method_basic(get_image);
	create_godot_method_basic(open);
	create_godot_method_basic(set_default);
	create_godot_method_basic(flip);
	create_godot_method_basic(detect);
	create_godot_method_basic(compute_flow);

	/* Initializing process */
	processing_initialize();
	b_println("GodotCV NativeScript initialized");
}
