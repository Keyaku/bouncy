extends Sprite

func _ready():
	set("modulate", Color(1, 1, 1, 0));

func _on_root_game_start(_position, anim_node):
	print("start")
	anim_node.interpolate_property(self, "modulate", Color(1, 1, 1, 0), Color(1, 1, 1, 1), 2.0,
		Tween.TRANS_LINEAR, Tween.EASE_IN)
	anim_node.start()
