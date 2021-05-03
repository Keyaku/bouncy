extends Node


onready var cv_camera = preload("res://scripts/native/camera.gdns").new()
onready var player = $game/player

var active: bool = false
var image = Image.new()

signal camera_ready

signal game_start
signal force

func _ready():
	# Preparing camera display
	cv_camera.set_default(0)
	cv_camera.open()
	cv_camera.flip(true, false)

	# Start
	emit_signal("camera_ready", cv_camera)
	$animation.play("greeting_blink")

func _process(_delta):
	var buffer = cv_camera.get_image()
	if not buffer:
		return
	image.create_from_data(cv_camera.width, cv_camera.height, false, Image.FORMAT_RGB8, buffer)
	$display.set_frame(image)

	if not active:
		var face = cv_camera.detect_face()

		# Update detection value in each indicator
		for child in $Detection.get_children():
			child.emit_signal("detection_update", face is Rect2)

		if face and not active:
			var position = face.position + face.size / 2
			if (position - GAME.root.size / 2).length() < 40:
				active = true
				emit_signal("game_start", position, $animation)
				$animation.play("game_start")
	else:
		var region = Rect2(player.get("position"), Vector2(70, 70))
		var force = cv_camera.compute_flow(region)
		emit_signal("force", Vector2(clamp(force.x, -100, 100), clamp(force.y, -100, 100)))
