extends Sprite

var cv_camera


# Signal methods
func _on_screen_resize():
# Updating screen resolution
	OS.set_window_size(cv_camera.size)
	if GAME.root.size != OS.get_window_size():
		GAME.root.size = cv_camera.size

func _on_main_camera_ready(camera=null):
	if camera != null:
		cv_camera = camera

	if not GAME.root.is_connected("size_changed", self, "_on_screen_resize"):
		var err = GAME.connect_signal(GAME.root, "size_changed", self, "_on_screen_resize")
		if err == 0:
			_on_screen_resize()
			reset_texture()


# Methods
func reset_texture():
	texture = ImageTexture.new()
	texture.create(cv_camera.width, cv_camera.height, Image.FORMAT_RGB8, Texture.FLAG_VIDEO_SURFACE)
	region_rect = Rect2(0, 0, cv_camera.width, cv_camera.height)
	position.x = cv_camera.width >> 1
	position.y = cv_camera.height >> 1

func set_frame(img):
	if texture == null:
		reset_texture()
	texture.set_data(img)
