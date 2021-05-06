extends CanvasItem

var exists: bool = false setget _set_exists

var txt_lbl: Label

# Signals
signal detection_update(detected)

func _ready():
	GAME.connect_signal(self, "detection_update", self, "_set_exists")
	txt_lbl = Label.new()
	add_child(txt_lbl)
	txt_lbl.set_position(Vector2(0, 5))
	txt_lbl.set_text(name.capitalize())

func _exit_tree():
	txt_lbl.queue_free()

func _set_exists(val):
	exists = val
	update()

func _draw():
	draw_circle(Vector2(0, 0), 5, Color.green if exists else Color.red)
