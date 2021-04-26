extends CanvasItem

export var text: String = ""
var exists: bool = false setget _set_exists

# Signals
signal detection_update(yesno)

func _ready():
	GAME.connect_signal(self, "detection_update", self, "_set_exists")

func _set_exists(val):
	exists = val
	update()

func _draw():
	draw_circle(Vector2(0, 0), 5, Color.green if exists else Color.red)
