extends Sprite

var cv_camera

func _on_screen_resize():
# Updating screen resolution
	print("first")
	OS.set_window_size(cv_camera.size)
	print("second")
	if GAME.root.size != OS.get_window_size():
		GAME.root.size = cv_camera.size
	print("Update:", GAME.root.size, OS.get_window_size())

func _on_main_camera_ready(camera=null):
	if camera != null:
		cv_camera = camera

	if not GAME.root.is_connected("size_changed", self, "_on_screen_resize"):
		var err = GAME.root.connect("size_changed", self, "_on_screen_resize")
		if err != 0:
			printerr("Error ", err, ". Signal connection failed.")
		else:
			_on_screen_resize()

	# Preparing Texture
	texture = ImageTexture.new()
	texture.create(cv_camera.width, cv_camera.height, Image.FORMAT_RGB8, Texture.FLAG_VIDEO_SURFACE)
	region_rect = Rect2(0, 0, cv_camera.width, cv_camera.height)
	position.x = cv_camera.width >> 1
	position.y = cv_camera.height >> 1

func set_frame(img):
	if texture == null:
		_on_main_camera_ready()
	texture.set_data(img)
